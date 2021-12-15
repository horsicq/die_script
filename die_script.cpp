/* Copyright (c) 2019-2021 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "die_script.h"

bool sort_signature_prio(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2)
{
    if((sr1.sName=="_init")&&(sr2.sName=="_init"))
    {
        return false;
    }

    if(sr1.sName=="_init")
    {
        return true;
    }
    else if(sr2.sName=="_init")
    {
        return false;
    }

    qint32 nPos1=sr1.sName.count(".");
    qint32 nPos2=sr2.sName.count(".");

    if((nPos1>1)&&(nPos2>1))
    {
        QString sPrio1=sr1.sName.section(".",nPos1-1,nPos1-1);
        QString sPrio2=sr2.sName.section(".",nPos2-1,nPos2-1);

        if((sPrio1!="")&&(sPrio2!=""))
        {
            if(sPrio1>sPrio2)
            {
                return false;
            }
            else if(sPrio1<sPrio2)
            {
                return true;
            }
            else if(sPrio1==sPrio2)
            {
                return (sr1.sName.section(".",nPos1-2,nPos1-2)<sr2.sName.section(".",nPos2-2,nPos2-2));
            }
        }
    }

    return (sr1.sName<sr2.sName);
}

bool sort_signature_name(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2)
{
    if((sr1.sName=="_init")&&(sr2.sName=="_init"))
    {
        return false;
    }

    if(sr1.sName=="_init")
    {
        return true;
    }
    else if(sr2.sName=="_init")
    {
        return false;
    }

    return (sr1.sName<sr2.sName);
}

DiE_Script::DiE_Script(QObject *pParent) : QObject(pParent)
{
    g_bIsStop=false;
    g_databaseType=DBT_UNKNOWN;
    g_pDirectoryElapsedTimer=nullptr;
    g_directoryStats={};
#ifdef QT_SCRIPTTOOLS_LIB
    pDebugger=0;
#endif
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> DiE_Script::_loadDatabasePath(QString sDatabasePath, XBinary::FT fileType)
{
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> listResult;

    QDir dir(sDatabasePath);

    QFileInfoList eil=dir.entryInfoList();

    qint32 nNumberOfFiles=eil.count();

    for(qint32 i=0;i<nNumberOfFiles;i++)
    {
        if(eil.at(i).isFile())
        {
            DiE_ScriptEngine::SIGNATURE_RECORD record={};

            record.fileType=fileType;
            record.sName=eil.at(i).fileName();
            record.sText=XBinary::readFile(eil.at(i).absoluteFilePath());
            record.sFilePath=eil.at(i).absoluteFilePath();

            listResult.append(record);
        }
    }

    std::sort(listResult.begin(),listResult.end(),sort_signature_prio);

    return listResult;
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> DiE_Script::_loadDatabaseFromZip(XZip *pZip, QList<XArchive::RECORD> *pListRecords, QString sPrefix,XBinary::FT fileType)
{
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> listResult;

    qint32 nNumberOfRecords=pListRecords->count();

    for(qint32 i=0;i<nNumberOfRecords;i++)
    {
        XArchive::RECORD zipRecord=pListRecords->at(i);

        if(((sPrefix=="")&&(!zipRecord.sFileName.contains("/")))||((zipRecord.sFileName.contains("/"))&&(zipRecord.sFileName.section("/",0,0)==sPrefix)&&(zipRecord.sFileName.section("/",1,1)!="")))
        {
            QFileInfo fi(zipRecord.sFileName);

            DiE_ScriptEngine::SIGNATURE_RECORD record={};

            record.fileType=fileType;
            record.sName=fi.fileName();
            record.sText=pZip->decompress(&zipRecord);
            record.sFilePath=zipRecord.sFileName;
            record.bReadOnly=true;

            listResult.append(record);
        }
    }

    return listResult;
}

DiE_Script::SCAN_RESULT DiE_Script::_scan(QIODevice *pDevice, XBinary::SCANID parentId, XBinary::FT fileType, SCAN_OPTIONS *pOptions, QString sSignatureFilePath, qint64 nOffset)
{
    SCAN_RESULT scanResult={};

    qint32 nCurrent=0;

    emit progressMaximumChanged(100); // TODO const
    emit progressValueChanged(nCurrent);

    XBinary::_MEMORY_MAP memoryMap=XFormats::getMemoryMap(fileType,pDevice);


    XBinary::SCANID baseId={};

    baseId.fileType=fileType;
    baseId.sUuid=XBinary::generateUUID();
    baseId.sArch=memoryMap.sArch;
    baseId.mode=memoryMap.mode;
    baseId.bIsBigEndian=memoryMap.bIsBigEndian;
    baseId.sType=memoryMap.sType;
    baseId.nOffset=nOffset;
    baseId.nSize=pDevice->size();
    baseId.filePart=XBinary::FILEPART_HEADER;

    qint32 nNumberOfSignatures=g_listSignatures.count();

    DiE_ScriptEngine::SIGNATURE_RECORD srGlobalInit;
    DiE_ScriptEngine::SIGNATURE_RECORD srInit;

    bool bGlobalInit=false;
    bool bInit=false;

    for(qint32 i=0;(i<nNumberOfSignatures)&&(!g_bIsStop);i++)
    {
        if(g_listSignatures.at(i).sName=="_init")
        {
            if(g_listSignatures.at(i).fileType==XBinary::FT_UNKNOWN)
            {
                srGlobalInit=g_listSignatures.at(i);
                bGlobalInit=true;
            }

            if(XBinary::checkFileType(g_listSignatures.at(i).fileType,fileType))
            {
                srInit=g_listSignatures.at(i);
                bInit=true;
            }

            if(bGlobalInit&&bInit)
            {
                break;
            }
        }  
    }

    DiE_ScriptEngine scriptEngine(&g_listSignatures,pDevice,fileType);
    connect(this,SIGNAL(stopEngine()),&scriptEngine,SLOT(stop()),Qt::DirectConnection);
    connect(&scriptEngine,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));
    connect(&scriptEngine,SIGNAL(infoMessage(QString)),this,SIGNAL(infoMessage(QString)));

#ifdef QT_SCRIPTTOOLS_LIB
    if(pDebugger)
    {
        pDebugger->attachTo(&scriptEngine);
    }
#endif

    if(nNumberOfSignatures)
    {
        if(bGlobalInit)
        {
            _handleError(&scriptEngine,scriptEngine.evaluate(srGlobalInit.sText,srGlobalInit.sFilePath),&srGlobalInit,&scanResult);
        }

        if(bInit)
        {
            _handleError(&scriptEngine,scriptEngine.evaluate(srInit.sText,srInit.sFilePath),&srInit,&scanResult);
        }
    }

    for(qint32 i=0;(i<nNumberOfSignatures)&&(!g_bIsStop);i++)
    {
        bool bExec=false;

        if((g_listSignatures.at(i).sName!="_init")&&(XBinary::checkFileType(g_listSignatures.at(i).fileType,fileType)))
        {
            bExec=true;
        }

        if(pOptions->sSignatureName!="")
        {
            if(pOptions->sSignatureName!=g_listSignatures.at(i).sName)
            {
                bExec=false;
            }
        }

        if(!pOptions->bDeepScan)
        {
            QString sPrefix=g_listSignatures.at(i).sName.section(".",0,0).toUpper();

            if((sPrefix=="DS")||(sPrefix=="EP"))
            {
                bExec=false;
            }
        }

        if(sSignatureFilePath!="") // TODO Check!
        {
            bExec=(sSignatureFilePath==g_listSignatures.at(i).sFilePath);
        }

        if(bExec)
        {
            scriptEngine.clearListResult();

            DiE_ScriptEngine::SIGNATURE_RECORD signatureRecord=g_listSignatures.at(i);

            QElapsedTimer scanTimer;

            if(pOptions->bDebug)
            {
                scanTimer.start();
            }

            XSCRIPTVALUE script=scriptEngine.evaluate(signatureRecord.sText,signatureRecord.sFilePath);

            if(_handleError(&scriptEngine,script,&signatureRecord,&scanResult))
            {
#ifdef QT_SCRIPTTOOLS_LIB
                if(pDebugger)
                {
                    pDebugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
                }
#endif
                XSCRIPTVALUE detect=scriptEngine.globalObject().property("detect");

                if(_handleError(&scriptEngine,detect,&signatureRecord,&scanResult))
                {
                    XSCRIPTVALUELIST valuelist;

                    valuelist << pOptions->bShowType << pOptions->bShowVersion << pOptions->bShowOptions;

                #ifdef QT_SCRIPT_LIB
                    QScriptValue result=detect.call(script,valuelist);
                #else
                    QJSValue result=detect.callWithInstance(script,valuelist);
                #endif

                    if(_handleError(&scriptEngine,result,&signatureRecord,&scanResult))
                    {
                        // TODO getResult
                        QString sResult=result.toString();

                        QList<DiE_ScriptEngine::RESULT> listDetects=scriptEngine.getListResult();

                        if((listDetects.count()==0)&&(sResult!=""))
                        {
                            listDetects.append(DiE_ScriptEngine::stringToResult(sResult,pOptions->bShowType,pOptions->bShowVersion,pOptions->bShowOptions));
//                            SCAN_STRUCT ss=getScanStructFromString(scanResult.scanHeader,&signatureRecord,sResult,pOptions);

//                            scanResult.listRecords.append(ss);
                        }

                        qint32 nNumberOfDetects=listDetects.count();

                        for(qint32 j=0;j<nNumberOfDetects;j++)
                        {
                            SCAN_STRUCT ssRecord={};

                            if(baseId.fileType==XBinary::FT_BINARY)
                            {
                                QString sPrefix=signatureRecord.sName.section(".",0,0).toUpper();

                                if(sPrefix=="COM")
                                {
                                    baseId.fileType=XBinary::FT_COM;
                                    baseId.sArch="8086";
                                    baseId.sType="EXE";
                                }
                                else if(sPrefix=="TEXT") // mb TODO not set if COM
                                {
                                    baseId.fileType=XBinary::FT_TEXT;
                                }
                            }

                            // TODO IDs
                            ssRecord.id=baseId;
                            ssRecord.parentId=parentId;

                            ssRecord.sSignature=signatureRecord.sName;
                            ssRecord.sType=listDetects.at(j).sType;
                            ssRecord.sName=listDetects.at(j).sName;
                            ssRecord.sVersion=listDetects.at(j).sVersion;
                            ssRecord.sOptions=listDetects.at(j).sOptions;
                            ssRecord.sFullString=QString("%1: %2(%3)[%4]").arg(ssRecord.sType,ssRecord.sName,ssRecord.sVersion,ssRecord.sOptions);
                            ssRecord.sResult=QString("%1(%2)[%3]").arg(ssRecord.sName,ssRecord.sVersion,ssRecord.sOptions);

                            scanResult.listRecords.append(ssRecord);
                        }
                    }
                }
            }

            if(pOptions->bDebug)
            {
                DEBUG_RECORD debugRecord={};
                debugRecord.sScript=signatureRecord.sName;
                debugRecord.nElapsedTime=scanTimer.elapsed();
            #ifdef QT_DEBUG
                qDebug("%s: %lld msec",debugRecord.sScript.toLatin1().data(),debugRecord.nElapsedTime);
            #endif
                scanResult.listDebugRecords.append(debugRecord);
            }
        }

        if((i*100)/nNumberOfSignatures>nCurrent)
        {
            nCurrent++;
            emit progressValueChanged(nCurrent);
        }
    }

//    if(scanResult.listRecords.count()==0)
//    {
//        SCAN_STRUCT ss={};

//        ss.fileType=fileType;
//        ss.sString="Unknown";

//        scanResult.listRecords.append(ss);
//    }

    g_bIsStop=false;

    emit progressValueChanged(100);

    return scanResult;
}

bool DiE_Script::_handleError(DiE_ScriptEngine *pScriptEngine, XSCRIPTVALUE scriptValue, DiE_ScriptEngine::SIGNATURE_RECORD *pSignatureRecord, DiE_Script::SCAN_RESULT *pScanResult)
{
    bool bResult=false;

    QString sErrorString;
    if(pScriptEngine->handleError(scriptValue,&sErrorString))
    {
        bResult=true;
    }
    else
    {
        ERROR_RECORD errorRecord={};
        errorRecord.sScript=pSignatureRecord->sName;
        errorRecord.sErrorString=sErrorString;

        pScanResult->listErrors.append(errorRecord);
    }

    return bResult;
}

bool DiE_Script::loadDatabase(QString sDatabasePath)
{
    // TODO Check if empty file
    this->g_sDatabasePath=sDatabasePath;

    g_databaseType=DBT_UNKNOWN;

    g_listSignatures.clear();

    QString _sDatabasePath=XBinary::convertPathName(sDatabasePath);

    if(XBinary::isFileExists(_sDatabasePath)) // Load from zip
    {
        QFile file;
        file.setFileName(_sDatabasePath);

        if(file.open(QIODevice::ReadOnly))
        {
            XZip zip(&file);

            if(zip.isValid())
            {
                QList<XArchive::RECORD> listRecords=zip.getRecords();

                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"",XBinary::FT_UNKNOWN));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"Binary",XBinary::FT_BINARY));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"COM",XBinary::FT_COM));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"JAR",XBinary::FT_JAR));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"APK",XBinary::FT_APK));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"IPA",XBinary::FT_IPA));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"DEX",XBinary::FT_DEX));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"MSDOS",XBinary::FT_MSDOS));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"LE",XBinary::FT_LE));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"LX",XBinary::FT_LX));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"NE",XBinary::FT_NE));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"PE",XBinary::FT_PE));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"ELF",XBinary::FT_ELF));
                g_listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"MACH",XBinary::FT_MACHO));

                g_databaseType=DBT_COMPRESSED;
            }

            file.close();
        }
    }
    else if(XBinary::isDirectoryExists(_sDatabasePath))
    {
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath,XBinary::FT_UNKNOWN));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"Binary",XBinary::FT_BINARY));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"COM",XBinary::FT_COM));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"JAR",XBinary::FT_JAR));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"APK",XBinary::FT_APK));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"IPA",XBinary::FT_IPA));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"DEX",XBinary::FT_IPA));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"MSDOS",XBinary::FT_MSDOS));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"LE",XBinary::FT_LE));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"LX",XBinary::FT_LX));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"NE",XBinary::FT_NE));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"PE",XBinary::FT_PE));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"ELF",XBinary::FT_ELF));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"MACH",XBinary::FT_MACHO));

        g_databaseType=DBT_FOLDER;
    }
    else
    {
        emit errorMessage(QString("%1: %2").arg(tr("Cannot load database"),sDatabasePath));
    }

    return g_listSignatures.count();
}

QString DiE_Script::getDatabasePath()
{
    return g_sDatabasePath;
}

QList<DiE_Script::SIGNATURE_STATE> DiE_Script::getSignatureStates()
{
    QList<SIGNATURE_STATE> listResult;

    QList<XBinary::FT> listFT;

    listFT.append(XBinary::FT_BINARY);
    listFT.append(XBinary::FT_COM);
    listFT.append(XBinary::FT_MSDOS);
    listFT.append(XBinary::FT_NE);
    listFT.append(XBinary::FT_LE);
    listFT.append(XBinary::FT_LX);
    listFT.append(XBinary::FT_PE);
    listFT.append(XBinary::FT_ELF);
    listFT.append(XBinary::FT_MACHO);

    int nNumberOfFileTypes=listFT.count();

    for(qint32 i=0;i<nNumberOfFileTypes;i++)
    {
        SIGNATURE_STATE state={};
        state.fileType=listFT.at(i);
        state.nNumberOfSignatures=getNumberOfSignatures(state.fileType);

        listResult.append(state);
    }

    return listResult;
}

qint32 DiE_Script::getNumberOfSignatures(XBinary::FT fileType)
{
    qint32 nResult=0;

    qint32 nNumberOfSignatures=g_listSignatures.count();

    for(qint32 i=0;(i<nNumberOfSignatures);i++)
    {
        if((g_listSignatures.at(i).sName!="_init")&&(XBinary::checkFileType(g_listSignatures.at(i).fileType,fileType)))
        {
            nResult++;
        }
    }

    return nResult;
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> *DiE_Script::getSignatures()
{
    return &g_listSignatures;
}

DiE_Script::SCAN_RESULT DiE_Script::scanFile(QString sFileName, SCAN_OPTIONS *pOptions)
{
    SCAN_RESULT scanResult={};

    QFile file;

    file.setFileName(sFileName);

    if(file.open(QIODevice::ReadOnly))
    {
        scanResult=scanDevice(&file,pOptions);

        file.close();
    }

    return scanResult;
}

DiE_Script::SCAN_RESULT DiE_Script::scanDevice(QIODevice *pDevice,SCAN_OPTIONS *pOptions)
{
    SCAN_RESULT scanResult={};

    XBinary::SCANID parentId={0};
    parentId.fileType=XBinary::FT_UNKNOWN;
    parentId.filePart=XBinary::FILEPART_HEADER;

    scan(pDevice,&scanResult,0,pDevice->size(),parentId,pOptions,true);

    return scanResult;
}

void DiE_Script::scan(QIODevice *pDevice, SCAN_RESULT *pScanResult, qint64 nOffset, qint64 nSize, XBinary::SCANID parentId, SCAN_OPTIONS *pOptions, bool bInit)
{
    QElapsedTimer *pScanTimer=nullptr;

    if(bInit)
    {
        pScanTimer=new QElapsedTimer;
        pScanTimer->start();
        pScanResult->sFileName=XBinary::getDeviceFileName(pDevice);
    }

    SubDevice sd(pDevice,nOffset,nSize);

    if(sd.open(QIODevice::ReadOnly)) // TODO if not stop
    {
        QSet<XBinary::FT> stFT=XFormats::getFileTypes(&sd,true);

        if(pOptions->fileType!=XBinary::FT_UNKNOWN)
        {
            XBinary::filterFileTypes(&stFT,pOptions->fileType);
        }

        if(pOptions->bAllTypesScan)
        {
            if( stFT.contains(XBinary::FT_PE32)||
                stFT.contains(XBinary::FT_PE64)||
                stFT.contains(XBinary::FT_LE)||
                stFT.contains(XBinary::FT_LX)||
                stFT.contains(XBinary::FT_NE))
            {
                SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_MSDOS,pOptions);

                pScanResult->listRecords.append(_scanResult.listRecords);
                pScanResult->listErrors.append(_scanResult.listErrors);
                pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
            }
        }

        if(stFT.contains(XBinary::FT_PE32))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_PE32,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_PE64))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_PE64,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_ELF32))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_ELF32,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_ELF64))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_ELF64,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_MACHO32))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_MACHO32,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_MACHO64))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_MACHO64,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_LX))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_LX,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_LE))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_LE,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_NE))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_NE,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_MSDOS))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_MSDOS,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_JAR))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_JAR,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_APK))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_APK,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_IPA))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_IPA,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else if(stFT.contains(XBinary::FT_COM)&&(stFT.size()==1))
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_COM,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);
        }
        else
        {
            SCAN_RESULT _scanResult=_scan(&sd,parentId,XBinary::FT_BINARY,pOptions);

            pScanResult->listRecords.append(_scanResult.listRecords);
            pScanResult->listErrors.append(_scanResult.listErrors);
            pScanResult->listDebugRecords.append(_scanResult.listDebugRecords);

            SCAN_RESULT _scanResultCOM=_scan(&sd,parentId,XBinary::FT_COM,pOptions);

            pScanResult->listRecords.append(_scanResultCOM.listRecords);
            pScanResult->listErrors.append(_scanResultCOM.listErrors);
            pScanResult->listDebugRecords.append(_scanResultCOM.listDebugRecords);
        }

        sd.close();
    }

    if(pScanTimer)
    {
        pScanResult->nScanTime=pScanTimer->elapsed();
    }
}

DiE_ScriptEngine::SIGNATURE_RECORD DiE_Script::getSignatureByFilePath(QString sSignatureFilePath)
{
    DiE_ScriptEngine::SIGNATURE_RECORD result={};

    qint32 nNumberOfSignatures=g_listSignatures.count();

    for(qint32 i=0;i<nNumberOfSignatures;i++)
    {
        if(g_listSignatures.at(i).sFilePath==sSignatureFilePath)
        {
            result=g_listSignatures.at(i);

            break;
        }
    }

    return result;
}

bool DiE_Script::updateSignature(QString sSignatureFilePath, QString sText)
{
    bool bResult=false;

    qint32 nNumberOfSignatures=g_listSignatures.count();

    for(qint32 i=0;i<nNumberOfSignatures;i++)
    {
        if(g_listSignatures.at(i).sFilePath==sSignatureFilePath)
        {
            if(XBinary::writeToFile(sSignatureFilePath,QByteArray().append(sText.toLatin1())))
            {
                g_listSignatures[i].sText=sText;
                bResult=true;
            }

            break;
        }
    }

    return bResult;
}

void DiE_Script::stop()
{
//    g_mutex.lock(); // TODO
    g_bIsStop=true;
//    g_mutex.unlock();

    emit stopEngine();
}

void DiE_Script::processDirectory()
{
    g_pDirectoryElapsedTimer=new QElapsedTimer;
    g_pDirectoryElapsedTimer->start();

    if(g_sDirectoryProcess!="")
    {
        QList<QString> listFileNames;

        XBinary::FFOPTIONS ffoptions={};
        ffoptions.bSubdirectories=g_scanOptionsProcess.bSubdirectories;
        ffoptions.pbIsStop=&g_bIsStop;
        ffoptions.pnNumberOfFiles=&(g_directoryStats.nTotal);
        ffoptions.pListFileNames=&listFileNames;

        XBinary::findFiles(g_sDirectoryProcess,&ffoptions);

        g_directoryStats.nTotal=listFileNames.count();

        for(qint32 i=0;(i<g_directoryStats.nTotal)&&(!g_bIsStop);i++)
        {
//            g_mutex.lock();

            g_directoryStats.nCurrent=i+1;
            g_directoryStats.sStatus=listFileNames.at(i);

            emit directoryScanFileStarted(g_directoryStats.sStatus);

            SCAN_RESULT _scanResult=scanFile(g_directoryStats.sStatus,&g_scanOptionsProcess);

            emit directoryScanResult(_scanResult);

//            g_mutex.unlock();

            // TODO a function
            QEventLoop loop;
            QTimer t;
            t.connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
            t.start(1);
            loop.exec();
        }
    }

    emit directoryScanCompleted(g_pDirectoryElapsedTimer->elapsed());
    delete g_pDirectoryElapsedTimer;
    g_pDirectoryElapsedTimer=nullptr;

    g_bIsStop=false;
}

DiE_Script::STATS DiE_Script::getStats()
{
    STATS result={};

    qint32 nNumberOfSignatures=g_listSignatures.count();

    for(qint32 i=0;i<nNumberOfSignatures;i++)
    {
        QString sText=g_listSignatures.at(i).sText;

        QString sType=XBinary::regExp("init\\(\"(.*?)\",",sText,1);

        if(sType!="")
        {
            result.mapTypes.insert(sType,result.mapTypes.value(sType,0)+1);
        }
    }

    return result;
}

DiE_Script::DBT DiE_Script::getDatabaseType()
{
    return g_databaseType;
}

bool DiE_Script::isSignaturesPresent(XBinary::FT fileType)
{
    bool bResult=false;

    qint32 nNumberOfSignatures=g_listSignatures.count();

    for(qint32 i=0;i<nNumberOfSignatures;i++)
    {
        if(g_listSignatures.at(i).fileType==fileType)
        {
            bResult=true;

            break;
        }
    }

    return bResult;
}

//QString DiE_Script::scanResultToPlainString(DiE_Script::SCAN_RESULT *pScanResult)
//{
//    QString sResult;

////    sResult+=QString("filetype: %1\n").arg(XBinary::fileTypeIdToString(pScanResult->id.fileType));
////    sResult+=QString("arch: %1\n").arg(pScanResult->id.sArch);
////    sResult+=QString("mode: %1\n").arg(XBinary::modeIdToString(pScanResult->id.mode));
////    sResult+=QString("endianess: %1\n").arg(XBinary::endiannessToString(pScanResult->id.bIsBigEndian));
////    sResult+=QString("type: %1\n").arg(pScanResult->id.sType);

//    qint32 nNumberOfRecords=pScanResult->listRecords.count();

//    for(qint32 i=0;i<nNumberOfRecords;i++)
//    {
//        sResult+=QString("  %1\n").arg(pScanResult->listRecords.at(i).sFullString);
//    }

//    return sResult;
//}

//QString DiE_Script::scanResultToJsonString(DiE_Script::SCAN_RESULT *pScanResult)
//{
//    QString sResult;

//    QJsonObject jsonResult;

////    jsonResult.insert("filetype",XBinary::fileTypeIdToString(pScanResult->id.fileType));
////    jsonResult.insert("arch",pScanResult->id.sArch);
////    jsonResult.insert("mode",XBinary::modeIdToString(pScanResult->id.mode));
////    jsonResult.insert("endianess",XBinary::endiannessToString(pScanResult->id.bIsBigEndian));
////    jsonResult.insert("type",pScanResult->id.sType);

//    QJsonArray jsArray;

//    qint32 nNumberOfRecords=pScanResult->listRecords.count();

//    for(qint32 i=0;i<nNumberOfRecords;i++)
//    {
//        QJsonObject jsRecord;

//        jsRecord.insert("type",pScanResult->listRecords.at(i).sType);
//        jsRecord.insert("name",pScanResult->listRecords.at(i).sName);
//        jsRecord.insert("version",pScanResult->listRecords.at(i).sVersion);
//        jsRecord.insert("options",pScanResult->listRecords.at(i).sOptions);
//        jsRecord.insert("string",pScanResult->listRecords.at(i).sFullString);

//        jsArray.append(jsRecord);
//    }

//    jsonResult.insert("detects",jsArray);

//    QJsonDocument saveFormat(jsonResult);

//    sResult=saveFormat.toJson(QJsonDocument::Indented).constData();

//    return sResult;
//}

//QString DiE_Script::scanResultToXmlString(DiE_Script::SCAN_RESULT *pScanResult)
//{
//    QString sResult;

//    QXmlStreamWriter xml(&sResult);

//    xml.setAutoFormatting(true);

//    xml.writeStartElement("filescan");

////    xml.writeAttribute("filetype",XBinary::fileTypeIdToString(pScanResult->id.fileType));
////    xml.writeAttribute("arch",pScanResult->id.sArch);
////    xml.writeAttribute("mode",XBinary::modeIdToString(pScanResult->id.mode));
////    xml.writeAttribute("endianess",XBinary::endiannessToString(pScanResult->id.bIsBigEndian));
////    xml.writeAttribute("type",pScanResult->id.sType);

//    qint32 nNumberOfRecords=pScanResult->listRecords.count();

//    for(qint32 i=0;i<nNumberOfRecords;i++)
//    {
//        xml.writeStartElement("detect");
//        xml.writeAttribute("type",pScanResult->listRecords.at(i).sType);
//        xml.writeAttribute("name",pScanResult->listRecords.at(i).sName);
//        xml.writeAttribute("version",pScanResult->listRecords.at(i).sVersion);
//        xml.writeAttribute("options",pScanResult->listRecords.at(i).sOptions);
//        xml.writeCharacters(pScanResult->listRecords.at(i).sFullString);
//        xml.writeEndElement();
//    }

//    xml.writeEndElement();

//    return sResult;
//}

//QString DiE_Script::scanResultToCsvString(SCAN_RESULT *pScanResult)
//{
//    QString sResult;

//    qint32 nNumberOfRecords=pScanResult->listRecords.count();

//    for(qint32 i=0;i<nNumberOfRecords;i++)
//    {
//        QString sRecord=QString("%1;%2;%3;%4;%5\n").arg(pScanResult->listRecords.at(i).sType,
//                                                        pScanResult->listRecords.at(i).sName,
//                                                        pScanResult->listRecords.at(i).sVersion,
//                                                        pScanResult->listRecords.at(i).sOptions,
//                                                        pScanResult->listRecords.at(i).sFullString);

//        sResult+=sRecord;
//    }

//    return sResult;
//}

//QString DiE_Script::scanResultToTsvString(SCAN_RESULT *pScanResult)
//{
//    QString sResult;

//    qint32 nNumberOfRecords=pScanResult->listRecords.count();

//    for(qint32 i=0;i<nNumberOfRecords;i++)
//    {
//        QString sRecord=QString("%1\t%2\t%3\t%4\t%5\n").arg(pScanResult->listRecords.at(i).sType,
//                                                            pScanResult->listRecords.at(i).sName,
//                                                            pScanResult->listRecords.at(i).sVersion,
//                                                            pScanResult->listRecords.at(i).sOptions,
//                                                            pScanResult->listRecords.at(i).sFullString);

//        sResult+=sRecord;
//    }

//    return sResult;
//}

QString DiE_Script::getErrorsString(DiE_Script::SCAN_RESULT *pScanResult)
{
    QString sResult;

    qint32 nNumberOfErrors=pScanResult->listErrors.count();

    for(qint32 i=0;i<nNumberOfErrors;i++)
    {
        sResult+=QString("%1: %2\n").arg(pScanResult->listErrors.at(i).sScript,pScanResult->listErrors.at(i).sErrorString);
    }

    return sResult;
}

void DiE_Script::setProcessDirectory(QString sDirectory, SCAN_OPTIONS scanOptions)
{
    g_sDirectoryProcess=sDirectory;
    g_scanOptionsProcess=scanOptions;
}

DiE_Script::DIRECTORYSTATS DiE_Script::getCurrentDirectoryStats()
{
    if(g_pDirectoryElapsedTimer)
    {
        if(g_pDirectoryElapsedTimer->isValid())
        {
            g_directoryStats.nElapsed=g_pDirectoryElapsedTimer->elapsed();
        }
        else
        {
            g_directoryStats.nElapsed=0;
        }
    }

    return g_directoryStats;
}

QList<XBinary::SCANSTRUCT> DiE_Script::convert(QList<SCAN_STRUCT> *pListScanStructs)
{
    QList<XBinary::SCANSTRUCT> listResult;

    qint32 nNumberOfRecords=pListScanStructs->count();

    for(qint32 i=0;i<nNumberOfRecords;i++)
    {
        XBinary::SCANSTRUCT record={};

        record.bIsHeuristic=pListScanStructs->at(i).bIsHeuristic;
        record.id=pListScanStructs->at(i).id;
        record.parentId=pListScanStructs->at(i).parentId;
        record.sType=pListScanStructs->at(i).sType;
        record.sName=pListScanStructs->at(i).sName;
        record.sVersion=pListScanStructs->at(i).sVersion;
        record.sInfo=pListScanStructs->at(i).sOptions;
        record.varInfo=pListScanStructs->at(i).sSignature;

    #ifdef QT_GUI_LIB
        QString _sName=record.sName.toLower();

        // TODO more
        if(     (_sName=="installer")||
                (_sName=="sfx"))
        {
            record.colText=QColor(Qt::blue);
        }
        else if((_sName=="protector")||
                (_sName=="apk obfuscator")||
                (_sName=="jar obfuscator")||
                (_sName==".net obfuscator")||
                (_sName==".net compressor")||
                (_sName=="dongle protection")||
                (_sName=="joiner")||
                (_sName=="packer"))
        {
            record.colText=QColor(Qt::red);
        }
        else if((_sName=="pe tool")||
                (_sName=="apk tool"))
        {
            record.colText=QColor(Qt::green);
        }
        else if((_sName=="operation system")||
                (_sName=="virtual machine"))
        {
            record.colText=QColor(Qt::darkYellow);
        }
        else if(_sName=="signtool")
        {
            record.colText=QColor(Qt::darkMagenta);
        }
        else if(_sName=="language")
        {
            record.colText=QColor(Qt::darkCyan);
        }
        else
        {
            record.colText=QApplication::palette().text().color();
        }
    #endif

        QString _sType=record.sType;

        if      (_sType=="archive")         _sType=tr("Archive");
        else if (_sType=="compiler")        _sType=tr("Compiler");
        else if (_sType=="cryptor")         _sType=tr("Cryptor");
        else if (_sType=="certificate")     _sType=tr("Certificate");
        else if (_sType=="converter")       _sType=tr("Converter");
        else if (_sType=="database")        _sType=tr("Database");
        else if (_sType=="debug data")      _sType=tr("Debug data");
        else if (_sType=="format")          _sType=tr("Format");
        else if (_sType=="image")           _sType=tr("Image");
        else if (_sType=="installer")       _sType=tr("Installer");
        else if (_sType=="installer data")  _sType=tr("Installer data");
        else if (_sType=="joiner")          _sType=tr("Joiner");
        else if (_sType=="language")        _sType=tr("Language");
        else if (_sType=="library")         _sType=tr("Library");
        else if (_sType=="linker")          _sType=tr("Linker");
        else if (_sType=="overlay")         _sType=tr("Overlay");
        else if (_sType=="packer")          _sType=tr("Packer");
        else if (_sType=="player")          _sType=tr("Player");
        else if (_sType=="protection")      _sType=tr("Protection");
        else if (_sType=="protector")       _sType=tr("Protector");
        else if (_sType=="protector data")  _sType=tr("Protector data");
        else if (_sType=="sfx")             _sType=QString("SFX");
        else if (_sType=="source code")     _sType=tr("Source code");
        else if (_sType=="stub")            _sType=tr("Stub");
        else if (_sType=="tool")            _sType=tr("Tool");
        else if (_sType==".net compressor") _sType=QString(".NET %1").arg(tr("compressor"));
        else if (_sType==".net obfuscator") _sType=QString(".NET %1").arg(tr("obfuscator"));
        else if (_sType=="dos extender")    _sType=QString("DOS %1").arg(tr("extender"));

        record.sType=_sType;

        listResult.append(record);
    }

    return listResult;
}

#ifdef QT_SCRIPTTOOLS_LIB
void DiE_Script::setDebugger(QScriptEngineDebugger *pDebugger)
{
    this->pDebugger=pDebugger;
}
#endif

#ifdef QT_SCRIPTTOOLS_LIB
void DiE_Script::removeDebugger()
{
    this->pDebugger=nullptr;
}
#endif
