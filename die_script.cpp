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

DiE_Script::DiE_Script(QObject *parent) : QObject(parent)
{

}

void DiE_Script::_loadDatabase(QString sDatabasePath, DiE_ScriptEngine::STYPE stype)
{
    QDir dir(sDatabasePath);

    QFileInfoList eil=dir.entryInfoList();

    int nCount=eil.count();

    for(int i=0;i<nCount; i++)
    {
        if(eil.at(i).isFile())
        {
            DiE_ScriptEngine::SIGNATURE_RECORD record={};

            record.stype=stype;
            record.sName=eil.at(i).fileName();
            record.sText=XBinary::readFile(eil.at(i).absoluteFilePath());

            listSignatures.append(record);
        }
    }

    // TODO Sort
}

DiE_Script::SCAN_RESULT DiE_Script::_scan(QIODevice *pDevice, DiE_ScriptEngine::STYPE stype,SCAN_OPTIONS *pOptions)
{
    SCAN_RESULT scanResult={};

    int nCount=listSignatures.count();

    DiE_ScriptEngine::SIGNATURE_RECORD srGlobalInit;
    DiE_ScriptEngine::SIGNATURE_RECORD srInit;

    bool bGlobalInit=false;
    bool bInit=false;

    for(int i=0; i<nCount; i++)
    {
        if(listSignatures.at(i).sName=="_init")
        {
            if(listSignatures.at(i).stype==DiE_ScriptEngine::STYPE_GENERIC)
            {
                srGlobalInit=listSignatures.at(i);
                bGlobalInit=true;
            }

            if(listSignatures.at(i).stype==stype)
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

    DiE_ScriptEngine scriptEngine(&listSignatures,pDevice,stype);

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
        if((listSignatures.at(i).stype==stype)&&(listSignatures.at(i).sName!="_init"))
        {
            QElapsedTimer scanTimer;

            if(pOptions->bDebug)
            {
                scanTimer.start();
            }

            QString sInfo=listSignatures.at(i).sName;

            DiE_ScriptEngine::SIGNATURE_RECORD signatureRecord=listSignatures.at(i);

            QScriptValue script=scriptEngine.evaluate(signatureRecord.sText);

            if(_handleError(&scriptEngine,script,&signatureRecord,&scanResult))
            {
                QScriptValue detect=scriptEngine.globalObject().property("detect");

                if(_handleError(&scriptEngine,detect,&signatureRecord,&scanResult))
                {
                    QScriptValueList valuelist;

                    valuelist << true << (pOptions->bShowVersion) << (pOptions->bShowOptions);

                    QScriptValue result=detect.call(script,valuelist);

                    if(_handleError(&scriptEngine,result,&signatureRecord,&scanResult))
                    {
                        QString sResult=result.toString();

                        if(sResult!="")
                        {
                            qDebug(sResult.toLatin1().data());
                        }
                    }
                }
            }

            if(pOptions->bDebug)
            {
            #ifdef QT_DEBUG
                qint64 nElapsed=scanTimer.elapsed();
                if(nElapsed>5)
                {
                    qDebug("Elapsed time(%s): %d msec",sInfo.toLatin1().data(),nElapsed);
                }

            #endif
            }
        }
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

        qDebug(errorRecord.sErrorString.toLatin1().data());
    }

    return bResult;
}

bool DiE_Script::loadDatabase(QString sDatabasePath)
{
    listSignatures.clear();

    _loadDatabase(sDatabasePath,DiE_ScriptEngine::STYPE_GENERIC);
    _loadDatabase(sDatabasePath+QDir::separator()+"Binary",DiE_ScriptEngine::STYPE_BINARY);
    _loadDatabase(sDatabasePath+QDir::separator()+"Text",DiE_ScriptEngine::STYPE_TEXT);
    _loadDatabase(sDatabasePath+QDir::separator()+"MSDOS",DiE_ScriptEngine::STYPE_MSDOS);
    _loadDatabase(sDatabasePath+QDir::separator()+"PE",DiE_ScriptEngine::STYPE_PE);
    _loadDatabase(sDatabasePath+QDir::separator()+"ELF",DiE_ScriptEngine::STYPE_ELF);
    _loadDatabase(sDatabasePath+QDir::separator()+"MACH",DiE_ScriptEngine::STYPE_MACH);

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
        scanResult=_scan(pDevice,DiE_ScriptEngine::STYPE_PE,pOptions);
    }
    else if(stFT.contains(XBinary::FT_ELF32)||stFT.contains(XBinary::FT_ELF64))
    {
        scanResult=_scan(pDevice,DiE_ScriptEngine::STYPE_ELF,pOptions);
    }
    else if(stFT.contains(XBinary::FT_MACH32)||stFT.contains(XBinary::FT_MACH64))
    {
        scanResult=_scan(pDevice,DiE_ScriptEngine::STYPE_MACH,pOptions);
    }
    else if(stFT.contains(XBinary::FT_MSDOS))
    {
        scanResult=_scan(pDevice,DiE_ScriptEngine::STYPE_MSDOS,pOptions);
    }
    else
    {
        if(XBinary::isPlainTextType(pDevice))
        {
            scanResult=_scan(pDevice,DiE_ScriptEngine::STYPE_TEXT,pOptions);
        }
        else
        {
            scanResult=_scan(pDevice,DiE_ScriptEngine::STYPE_BINARY,pOptions);
        }
    }

    scanResult.nScanTime=scanTimer.elapsed();

#ifdef QT_DEBUG
    qDebug("Elapsed time: %d msec",scanResult.nScanTime);
#endif

    return scanResult;
}
