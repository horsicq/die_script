// copyright (c) 2019-2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
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

    int nPos1=sr1.sName.count(".");
    int nPos2=sr2.sName.count(".");

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
    bIsStop=false;
    databaseType=DBT_UNKNOWN;
#ifdef QT_SCRIPTTOOLS_LIB
    pDebugger=0;
#endif
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> DiE_Script::_loadDatabasePath(QString sDatabasePath, XBinary::FT fileType)
{
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> listResult;

    QDir dir(sDatabasePath);

    QFileInfoList eil=dir.entryInfoList();

    int nNumberOfFiles=eil.count();

    for(int i=0;i<nNumberOfFiles;i++)
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

    int nNumberOfRecords=pListRecords->count();

    for(int i=0;i<nNumberOfRecords;i++)
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

DiE_Script::SCAN_RESULT DiE_Script::_scan(QIODevice *pDevice, XBinary::FT fileType, SCAN_OPTIONS *pOptions,QString sSignatureFilePath)
{
    SCAN_RESULT scanResult={};

    qint32 nCurrent=0;

    emit progressMaximumChanged(100); // TODO const
    emit progressValueChanged(nCurrent);

    scanResult.sFileName=XBinary::getDeviceFileName(pDevice);
    scanResult.scanHeader.fileType=fileType;

    XBinary::_MEMORY_MAP memoryMap=XFormats::getMemoryMap(fileType,pDevice);

    scanResult.scanHeader.sArch=memoryMap.sArch;
    scanResult.scanHeader.mode=memoryMap.mode;
    scanResult.scanHeader.bIsBigEndian=memoryMap.bIsBigEndian;
    scanResult.scanHeader.sType=memoryMap.sType;

    int nNumberOfSignatures=listSignatures.count();

    DiE_ScriptEngine::SIGNATURE_RECORD srGlobalInit;
    DiE_ScriptEngine::SIGNATURE_RECORD srInit;

    bool bGlobalInit=false;
    bool bInit=false;

    for(int i=0;(i<nNumberOfSignatures)&&(!bIsStop);i++)
    {
        if(listSignatures.at(i).sName=="_init")
        {
            if(listSignatures.at(i).fileType==XBinary::FT_UNKNOWN)
            {
                srGlobalInit=listSignatures.at(i);
                bGlobalInit=true;
            }

            if(XBinary::checkFileType(listSignatures.at(i).fileType,fileType))
            {
                srInit=listSignatures.at(i);
                bInit=true;
            }

            if(bGlobalInit&&bInit)
            {
                break;
            }
        }  
    }

    DiE_ScriptEngine scriptEngine(&listSignatures,pDevice,fileType);
    connect(this,SIGNAL(stopEngine()),&scriptEngine,SLOT(stop()),Qt::DirectConnection);

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

    for(int i=0;(i<nNumberOfSignatures)&&(!bIsStop);i++)
    {
        bool bExec=false;

        if((listSignatures.at(i).sName!="_init")&&(XBinary::checkFileType(listSignatures.at(i).fileType,fileType)))
        {
            bExec=true;
        }

        if(pOptions->sSignatureName!="")
        {
            if(pOptions->sSignatureName!=listSignatures.at(i).sName)
            {
                bExec=false;
            }
        }

        if(!pOptions->bDeepScan)
        {
            QString sPrefix=listSignatures.at(i).sName.section(".",0,0).toUpper();

            if((sPrefix=="DS")||(sPrefix=="EP"))
            {
                bExec=false;
            }
        }

        if(sSignatureFilePath!="") // TODO Check!
        {
            bExec=(sSignatureFilePath==listSignatures.at(i).sFilePath);
        }

        if(bExec)
        {
            DiE_ScriptEngine::SIGNATURE_RECORD signatureRecord=listSignatures.at(i);

            QElapsedTimer scanTimer;

            if(pOptions->bDebug)
            {
                scanTimer.start();
            }

            QScriptValue script=scriptEngine.evaluate(signatureRecord.sText,signatureRecord.sFilePath);

            if(_handleError(&scriptEngine,script,&signatureRecord,&scanResult))
            {
#ifdef QT_SCRIPTTOOLS_LIB
                if(pDebugger)
                {
                    pDebugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
                }
#endif
                QScriptValue detect=scriptEngine.globalObject().property("detect");

                if(_handleError(&scriptEngine,detect,&signatureRecord,&scanResult))
                {
                    QScriptValueList valuelist;

                    valuelist << pOptions->bShowType << pOptions->bShowVersion << pOptions->bShowOptions;

                    QScriptValue result=detect.call(script,valuelist);

                    if(_handleError(&scriptEngine,result,&signatureRecord,&scanResult))
                    {
                        QString sResult=result.toString();

                        if(sResult!="")
                        {
                            SCAN_STRUCT ss=getScanStructFromString(scanResult.scanHeader,&signatureRecord,sResult,pOptions);

                            scanResult.listRecords.append(ss);
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

    bIsStop=false;

    emit progressValueChanged(100);

    return scanResult;
}

bool DiE_Script::_handleError(DiE_ScriptEngine *pScriptEngine, QScriptValue scriptValue, DiE_ScriptEngine::SIGNATURE_RECORD *pSignatureRecord, DiE_Script::SCAN_RESULT *pScanResult)
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

DiE_Script::SCAN_STRUCT DiE_Script::getScanStructFromString(SCAN_HEADER scanHeader, DiE_ScriptEngine::SIGNATURE_RECORD *pSignatureRecord, QString sString, SCAN_OPTIONS *pScanOptions)
{
    SCAN_STRUCT result={};

    if(scanHeader.fileType==XBinary::FT_BINARY)
    {
        QString sPrefix=pSignatureRecord->sName.section(".",0,0).toUpper();

        if(sPrefix=="COM")
        {
            result.fileType=XBinary::FT_COM;
        }
        else if(sPrefix=="TEXT") // mb TODO not set if COM
        {
            result.fileType=XBinary::FT_TEXT;
        }
    }

    result.scanHeader=scanHeader;
    result.sSignature=pSignatureRecord->sName;

    result.sFullString=sString;

    if(pScanOptions->bShowType)
    {
        result.sType=sString.section(": ",0,0);

        QString _sType=result.sType.toLower();

        if      (_sType=="compiler")        result.sType=tr("Compiler");
        else if (_sType=="cryptor")         result.sType=tr("Cryptor");
        else if (_sType=="certificate")     result.sType=tr("Certificate");
        else if (_sType=="converter")       result.sType=tr("Converter");
        else if (_sType=="database")        result.sType=tr("Database");
        else if (_sType=="debug data")      result.sType=tr("Debug data");
        else if (_sType=="format")          result.sType=tr("Format");
        else if (_sType=="image")           result.sType=tr("Image");
        else if (_sType=="installer")       result.sType=tr("Installer");
        else if (_sType=="installer data")  result.sType=tr("Installer data");
        else if (_sType=="joiner")          result.sType=tr("Joiner");
        else if (_sType=="language")        result.sType=tr("Language");
        else if (_sType=="library")         result.sType=tr("Library");
        else if (_sType=="linker")          result.sType=tr("Linker");
        else if (_sType=="packer")          result.sType=tr("Packer");
        else if (_sType=="protector")       result.sType=tr("Protector");
        else if (_sType=="protector data")  result.sType=tr("Protector data");
        else if (_sType=="source code")     result.sType=tr("Source code");
        else if (_sType=="stub")            result.sType=tr("Stub");
        else if (_sType=="tool")            result.sType=tr("Tool");
        else if (_sType==".net compressor") result.sType=QString(".NET %1").arg(tr("compressor"));
        else if (_sType==".net obfuscator") result.sType=QString(".NET %1").arg(tr("obfuscator"));
        else if (_sType=="dos extender")    result.sType=QString("DOS %1").arg(tr("extender"));

        result.sResult=sString.section(": ",1,-1);
    }
    else
    {
        result.sResult=sString;
    }

    QString _sString=result.sResult;

    if(pScanOptions->bShowOptions)
    {
        if(_sString.count("[")==1)
        {
            result.sName=_sString.section("[",0,0);
            result.sOptions=_sString.section("[",1,-1).section("]",0,0);
            _sString=_sString.section("[",0,0);
        }
    }
    // TODO
    // Check
    if(pScanOptions->bShowVersion)
    {
        if(_sString.count("(")==1)
        {
            result.sVersion=_sString.section("(",1,-1).section(")",0,0);
            result.sName=_sString.section("(",0,0);
        }
    }

    return result;
}

bool DiE_Script::loadDatabase(QString sDatabasePath)
{
    // TODO Check if empty file
    this->g_sDatabasePath=sDatabasePath;

    databaseType=DBT_UNKNOWN;

    listSignatures.clear();

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

                listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"",XBinary::FT_UNKNOWN));
                listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"Binary",XBinary::FT_BINARY));
                listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"MSDOS",XBinary::FT_MSDOS));
                listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"PE",XBinary::FT_PE));
                listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"ELF",XBinary::FT_ELF));
                listSignatures.append(_loadDatabaseFromZip(&zip,&listRecords,"MACH",XBinary::FT_MACHO));

                databaseType=DBT_COMPRESSED;
            }

            file.close();
        }
    }
    else if(XBinary::isDirectoryExists(_sDatabasePath))
    {
        listSignatures.append(_loadDatabasePath(_sDatabasePath,XBinary::FT_UNKNOWN));
        listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"Binary",XBinary::FT_BINARY));
        listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"MSDOS",XBinary::FT_MSDOS));
        listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"PE",XBinary::FT_PE));
        listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"ELF",XBinary::FT_ELF));
        listSignatures.append(_loadDatabasePath(_sDatabasePath+QDir::separator()+"MACH",XBinary::FT_MACHO));

        databaseType=DBT_FOLDER;
    }

    return listSignatures.count();
}

QString DiE_Script::getDatabasePath()
{
    return g_sDatabasePath;
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> *DiE_Script::getSignatures()
{
    return &listSignatures;
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

    QElapsedTimer scanTimer;
    scanTimer.start();

    QSet<XBinary::FT> stFT=XBinary::getFileTypes(pDevice);

    if(pOptions->fileType!=XBinary::FT_UNKNOWN)
    {
        XBinary::filterFileTypes(&stFT,pOptions->fileType);
    }

    scanResult.bIsValidType=true;

    if(stFT.contains(XBinary::FT_PE32))
    {
        scanResult=_scan(pDevice,XBinary::FT_PE32,pOptions);
    }
    else if(stFT.contains(XBinary::FT_PE64))
    {
        scanResult=_scan(pDevice,XBinary::FT_PE64,pOptions);
    }
    else if(stFT.contains(XBinary::FT_ELF32))
    {
        scanResult=_scan(pDevice,XBinary::FT_ELF32,pOptions);
    }
    else if(stFT.contains(XBinary::FT_ELF64))
    {
        scanResult=_scan(pDevice,XBinary::FT_ELF64,pOptions);
    }
    else if(stFT.contains(XBinary::FT_MACHO32))
    {
        scanResult=_scan(pDevice,XBinary::FT_MACHO32,pOptions);
    }
    else if(stFT.contains(XBinary::FT_MACHO64))
    {
        scanResult=_scan(pDevice,XBinary::FT_MACHO64,pOptions);
    }
    else if(stFT.contains(XBinary::FT_MSDOS))
    {
        scanResult=_scan(pDevice,XBinary::FT_MSDOS,pOptions);
    }
    else if(stFT.contains(XBinary::FT_BINARY))
    {
        scanResult=_scan(pDevice,XBinary::FT_BINARY,pOptions);
    }
    else
    {
        scanResult.bIsValidType=false;
    }

    scanResult.nScanTime=scanTimer.elapsed();

    return scanResult;
}

DiE_ScriptEngine::SIGNATURE_RECORD DiE_Script::getSignatureByFilePath(QString sSignatureFilePath)
{
    DiE_ScriptEngine::SIGNATURE_RECORD result={};

    int nNumberOfSignatures=listSignatures.count();

    for(int i=0;i<nNumberOfSignatures;i++)
    {
        if(listSignatures.at(i).sFilePath==sSignatureFilePath)
        {
            result=listSignatures.at(i);

            break;
        }
    }

    return result;
}

bool DiE_Script::updateSignature(QString sSignatureFilePath, QString sText)
{
    bool bResult=false;

    int nNumberOfSignatures=listSignatures.count();

    for(int i=0;i<nNumberOfSignatures;i++)
    {
        if(listSignatures.at(i).sFilePath==sSignatureFilePath)
        {
            if(XBinary::writeToFile(sSignatureFilePath,QByteArray().append(sText.toLatin1())))
            {
                listSignatures[i].sText=sText;
                bResult=true;
            }

            break;
        }
    }

    return bResult;
}

void DiE_Script::stop()
{
    bIsStop=true;
    emit stopEngine();
}

DiE_Script::STATS DiE_Script::getStats()
{
    STATS result={};

    int nNumberOfSignatures=listSignatures.count();

    for(int i=0;i<nNumberOfSignatures;i++)
    {
        QString sText=listSignatures.at(i).sText;

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
    return databaseType;
}

bool DiE_Script::isSignaturesPresent(XBinary::FT fileType)
{
    bool bResult=false;

    int nNumberOfSignatures=listSignatures.count();

    for(int i=0;i<nNumberOfSignatures;i++)
    {
        if(listSignatures.at(i).fileType==fileType)
        {
            bResult=true;

            break;
        }
    }

    return bResult;
}

QString DiE_Script::scanResultToPlainString(DiE_Script::SCAN_RESULT *pScanResult)
{
    QString sResult;

    sResult+=QString("filetype: %1\n").arg(XBinary::fileTypeIdToString(pScanResult->scanHeader.fileType));
    sResult+=QString("arch: %1\n").arg(pScanResult->scanHeader.sArch);
    sResult+=QString("mode: %1\n").arg(XBinary::modeIdToString(pScanResult->scanHeader.mode));
    sResult+=QString("endianess: %1\n").arg(XBinary::endiannessToString(pScanResult->scanHeader.bIsBigEndian));
    sResult+=QString("type: %1\n").arg(pScanResult->scanHeader.sType);

    int nNumberOfRecords=pScanResult->listRecords.count();

    for(int i=0;i<nNumberOfRecords;i++)
    {
        sResult+=QString("  %1\n").arg(pScanResult->listRecords.at(i).sFullString);
    }

    return sResult;
}

QString DiE_Script::scanResultToJsonString(DiE_Script::SCAN_RESULT *pScanResult)
{
    QString sResult;

    QJsonObject jsonResult;

    jsonResult.insert("filetype",XBinary::fileTypeIdToString(pScanResult->scanHeader.fileType));
    jsonResult.insert("arch",pScanResult->scanHeader.sArch);
    jsonResult.insert("mode",XBinary::modeIdToString(pScanResult->scanHeader.mode));
    jsonResult.insert("endianess",XBinary::endiannessToString(pScanResult->scanHeader.bIsBigEndian));
    jsonResult.insert("type",pScanResult->scanHeader.sType);

    QJsonArray jsArray;

    int nNumberOfRecords=pScanResult->listRecords.count();

    for(int i=0;i<nNumberOfRecords;i++)
    {
        QJsonObject jsRecord;

        jsRecord.insert("type",pScanResult->listRecords.at(i).sType);
        jsRecord.insert("name",pScanResult->listRecords.at(i).sName);
        jsRecord.insert("version",pScanResult->listRecords.at(i).sVersion);
        jsRecord.insert("options",pScanResult->listRecords.at(i).sOptions);
        jsRecord.insert("string",pScanResult->listRecords.at(i).sFullString);

        jsArray.append(jsRecord);
    }

    jsonResult.insert("detects",jsArray);

    QJsonDocument saveFormat(jsonResult);

    sResult=saveFormat.toJson(QJsonDocument::Indented).constData();

    return sResult;
}

QString DiE_Script::scanResultToXmlString(DiE_Script::SCAN_RESULT *pScanResult)
{
    QString sResult;

    QXmlStreamWriter xml(&sResult);

    xml.setAutoFormatting(true);

    xml.writeStartElement("filescan");

    xml.writeAttribute("filetype",XBinary::fileTypeIdToString(pScanResult->scanHeader.fileType));
    xml.writeAttribute("arch",pScanResult->scanHeader.sArch);
    xml.writeAttribute("mode",XBinary::modeIdToString(pScanResult->scanHeader.mode));
    xml.writeAttribute("endianess",XBinary::endiannessToString(pScanResult->scanHeader.bIsBigEndian));
    xml.writeAttribute("type",pScanResult->scanHeader.sType);

    int nNumberOfRecords=pScanResult->listRecords.count();

    for(int i=0;i<nNumberOfRecords;i++)
    {
        xml.writeStartElement("detect");
        xml.writeAttribute("type",pScanResult->listRecords.at(i).sType);
        xml.writeAttribute("name",pScanResult->listRecords.at(i).sName);
        xml.writeAttribute("version",pScanResult->listRecords.at(i).sVersion);
        xml.writeAttribute("options",pScanResult->listRecords.at(i).sOptions);
        xml.writeCharacters(pScanResult->listRecords.at(i).sFullString);
        xml.writeEndElement();
    }

    xml.writeEndElement();

    return sResult;
}

QString DiE_Script::scanResultToCsvString(SCAN_RESULT *pScanResult)
{
    QString sResult;

    int nNumberOfRecords=pScanResult->listRecords.count();

    for(int i=0;i<nNumberOfRecords;i++)
    {
        QString sRecord=QString("%1;%2;%3;%4;%5\n").arg(pScanResult->listRecords.at(i).sType,
                                                        pScanResult->listRecords.at(i).sName,
                                                        pScanResult->listRecords.at(i).sVersion,
                                                        pScanResult->listRecords.at(i).sOptions,
                                                        pScanResult->listRecords.at(i).sFullString);

        sResult+=sRecord;
    }

    return sResult;
}

QString DiE_Script::scanResultToTsvString(SCAN_RESULT *pScanResult)
{
    QString sResult;

    int nNumberOfRecords=pScanResult->listRecords.count();

    for(int i=0;i<nNumberOfRecords;i++)
    {
        QString sRecord=QString("%1\t%2\t%3\t%4\t%5\n").arg(pScanResult->listRecords.at(i).sType,
                                                            pScanResult->listRecords.at(i).sName,
                                                            pScanResult->listRecords.at(i).sVersion,
                                                            pScanResult->listRecords.at(i).sOptions,
                                                            pScanResult->listRecords.at(i).sFullString);

        sResult+=sRecord;
    }

    return sResult;
}

QString DiE_Script::getErrorsString(DiE_Script::SCAN_RESULT *pScanResult)
{
    QString sResult;

    int nNumberOfErrors=pScanResult->listErrors.count();

    for(int i=0;i<nNumberOfErrors;i++)
    {
        sResult+=QString("%1: %2\n").arg(pScanResult->listErrors.at(i).sScript,pScanResult->listErrors.at(i).sErrorString);
    }

    return sResult;
}

#ifdef QT_SCRIPTTOOLS_LIB
void DiE_Script::setDebugger(QScriptEngineDebugger *pDebugger)
{
    this->pDebugger=pDebugger;
}
#endif
