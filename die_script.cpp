// copyright (c) 2019 hors<horsicq@gmail.com>
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

bool sort_signature(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2)
{
    if(sr1.sName=="_init")
    {
        return true;
    }
    else if(sr2.sName=="_init")
    {
        return false;
    }

    QString sPrio1=sr1.sName.section(".",1,1);
    QString sPrio2=sr2.sName.section(".",1,1);

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
            return (sr1.sName.section(".",0,0)<sr2.sName.section(".",0,0));
        }
    }

    return (sr1.sName<sr2.sName);
}

DiE_Script::DiE_Script(QObject *parent) : QObject(parent)
{

}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> DiE_Script::_loadDatabase(QString sDatabasePath, DiE_ScriptEngine::FT fileType)
{
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> listResult;

    QDir dir(sDatabasePath);

    QFileInfoList eil=dir.entryInfoList();

    int nCount=eil.count();

    for(int i=0;i<nCount; i++)
    {
        if(eil.at(i).isFile())
        {
            DiE_ScriptEngine::SIGNATURE_RECORD record={};

            record.fileType=fileType;
            record.sName=eil.at(i).fileName();
            record.sText=XBinary::readFile(eil.at(i).absoluteFilePath());

            listResult.append(record);
        }
    }

    std::sort(listResult.begin(),listResult.end(),sort_signature);

    return listResult;
}

DiE_Script::SCAN_RESULT DiE_Script::_scan(QIODevice *pDevice, DiE_ScriptEngine::FT fileType, SCAN_OPTIONS *pOptions)
{
    SCAN_RESULT scanResult={};

    scanResult.sFileName=XBinary::getDeviceFileName(pDevice);

    int nCount=listSignatures.count();

    DiE_ScriptEngine::SIGNATURE_RECORD srGlobalInit;
    DiE_ScriptEngine::SIGNATURE_RECORD srInit;

    bool bGlobalInit=false;
    bool bInit=false;

    for(int i=0; i<nCount; i++)
    {
        if(listSignatures.at(i).sName=="_init")
        {
            if(listSignatures.at(i).fileType==DiE_ScriptEngine::FT_GENERIC)
            {
                srGlobalInit=listSignatures.at(i);
                bGlobalInit=true;
            }

            if(listSignatures.at(i).fileType==fileType)
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

    if(nCount)
    {
        if(bGlobalInit)
        {
            _handleError(&scriptEngine,scriptEngine.evaluate(srGlobalInit.sText),&srGlobalInit,&scanResult);
        }

        if(bInit)
        {
            _handleError(&scriptEngine,scriptEngine.evaluate(srInit.sText),&srInit,&scanResult);
        }
    }

    for(int i=0;i<nCount;i++)
    {
        bool bExec=false;

        if((listSignatures.at(i).fileType==fileType)&&(listSignatures.at(i).sName!="_init"))
        {
            bExec=true;
        }

        if(!pOptions->bDeepScan)
        {
            QString sPrefix=listSignatures.at(i).sName.section(".",0,0).toUpper();

            if((sPrefix=="DS")||(sPrefix=="EP"))
            {
                bExec=false;
            }
        }

        if(bExec)
        {
            QElapsedTimer scanTimer;

            if(pOptions->bDebug)
            {
                scanTimer.start();
            }

            DiE_ScriptEngine::SIGNATURE_RECORD signatureRecord=listSignatures.at(i);


        #ifdef QT_DEBUG
            if(pOptions->bDebug)
            {
                if(signatureRecord.sName=="_NET Reactor.2.sg")
                {
                    qDebug("%s:",signatureRecord.sName.toLatin1().data());
                }
                qDebug("%s:",signatureRecord.sName.toLatin1().data());
            }

        #endif

            QScriptValue script=scriptEngine.evaluate(signatureRecord.sText);

            if(_handleError(&scriptEngine,script,&signatureRecord,&scanResult))
            {
                QScriptValue detect=scriptEngine.globalObject().property("detect");

                if(_handleError(&scriptEngine,detect,&signatureRecord,&scanResult))
                {
                    QScriptValueList valuelist;

                    valuelist << true << pOptions->bShowVersion << pOptions->bShowOptions;

                    QScriptValue result=detect.call(script,valuelist);

                    if(_handleError(&scriptEngine,result,&signatureRecord,&scanResult))
                    {
                        QString sResult=result.toString();

                        if(sResult!="")
                        {
                            SCAN_STRUCT ss=getScanStructFromString(pDevice,fileType,sResult);

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
//            #ifdef QT_DEBUG
//                qDebug("%s: %d msec",debugRecord.sScript.toLatin1().data(),debugRecord.nElapsedTime);
//            #endif
                scanResult.listDebugRecords.append(debugRecord);
            }
        }
    }

    if(scanResult.listRecords.count()==0)
    {
        SCAN_STRUCT ss={};

        ss.fileType=fileType;
        ss.sString="Unknown";

        scanResult.listRecords.append(ss);
    }

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

DiE_Script::SCAN_STRUCT DiE_Script::getScanStructFromString(QIODevice *pDevice,DiE_ScriptEngine::FT fileType, QString sString)
{
    SCAN_STRUCT result={};

    result.fileType=fileType;

    if(fileType==DiE_ScriptEngine::FT_PE)
    {
        if(XPE::is64(pDevice))
        {
            result.fileType=DiE_ScriptEngine::FT_PE64;
        }
        else
        {
            result.fileType=DiE_ScriptEngine::FT_PE32;
        }
    }
    else if(fileType==DiE_ScriptEngine::FT_ELF)
    {
        if(XELF::is64(pDevice))
        {
            result.fileType=DiE_ScriptEngine::FT_ELF64;
        }
        else
        {
            result.fileType=DiE_ScriptEngine::FT_ELF32;
        }
    }
    else if(fileType==DiE_ScriptEngine::FT_MACH)
    {
        if(XMACH::is64(pDevice))
        {
            result.fileType=DiE_ScriptEngine::FT_MACH64;
        }
        else
        {
            result.fileType=DiE_ScriptEngine::FT_MACH32;
        }
    }

    result.sType=sString.section(": ",0,0);
    result.sString=sString.section(": ",1,-1);

    // TODO

    return result;
}

bool DiE_Script::loadDatabase(QString sDatabasePath)
{
    listSignatures.clear();

    listSignatures.append(_loadDatabase(sDatabasePath,DiE_ScriptEngine::FT_GENERIC));
    listSignatures.append(_loadDatabase(sDatabasePath+QDir::separator()+"Binary",DiE_ScriptEngine::FT_BINARY));
    listSignatures.append(_loadDatabase(sDatabasePath+QDir::separator()+"Text",DiE_ScriptEngine::FT_TEXT));
    listSignatures.append(_loadDatabase(sDatabasePath+QDir::separator()+"MSDOS",DiE_ScriptEngine::FT_MSDOS));
    listSignatures.append(_loadDatabase(sDatabasePath+QDir::separator()+"PE",DiE_ScriptEngine::FT_PE));
    listSignatures.append(_loadDatabase(sDatabasePath+QDir::separator()+"ELF",DiE_ScriptEngine::FT_ELF));
    listSignatures.append(_loadDatabase(sDatabasePath+QDir::separator()+"MACH",DiE_ScriptEngine::FT_MACH));

    return listSignatures.count();
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

    if(stFT.contains(XBinary::FT_PE32)||stFT.contains(XBinary::FT_PE64))
    {
        scanResult=_scan(pDevice,DiE_ScriptEngine::FT_PE,pOptions);
    }
    else if(stFT.contains(XBinary::FT_ELF32)||stFT.contains(XBinary::FT_ELF64))
    {
        scanResult=_scan(pDevice,DiE_ScriptEngine::FT_ELF,pOptions);
    }
    else if(stFT.contains(XBinary::FT_MACH32)||stFT.contains(XBinary::FT_MACH64))
    {
        scanResult=_scan(pDevice,DiE_ScriptEngine::FT_MACH,pOptions);
    }
    else if(stFT.contains(XBinary::FT_MSDOS))
    {
        scanResult=_scan(pDevice,DiE_ScriptEngine::FT_MSDOS,pOptions);
    }
    else
    {
        if(XBinary::isPlainTextType(pDevice))
        {
            scanResult=_scan(pDevice,DiE_ScriptEngine::FT_TEXT,pOptions);
        }
        else
        {
            scanResult=_scan(pDevice,DiE_ScriptEngine::FT_BINARY,pOptions);
        }
    }

    scanResult.nScanTime=scanTimer.elapsed();

    return scanResult;
}
