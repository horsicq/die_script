/* Copyright (c) 2019-2022 hors<horsicq@gmail.com>
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
#include "die_scriptengine.h"

DiE_ScriptEngine::DiE_ScriptEngine(QList<DiE_ScriptEngine::SIGNATURE_RECORD> *pSignaturesList, QIODevice *pDevice, XBinary::FT fileType)
{
    this->g_pSignaturesList=pSignaturesList;

#ifdef QT_SCRIPT_LIB
    _addFunction(includeScript,"includeScript");
    _addFunction(_log,"_log");
    _addFunction(_setResult,"_setResult");
#else
    connect(&g_globalScript,SIGNAL(includeScriptSignal(QString)),this,SLOT(includeScriptSlot(QString)),Qt::DirectConnection);
    connect(&g_globalScript,SIGNAL(_logSignal(QString)),this,SLOT(_logSlot(QString)),Qt::DirectConnection);
    connect(&g_globalScript,SIGNAL(_setResultSignal(QString,QString,QString,QString)),this,SLOT(_setResultSlot(QString,QString,QString,QString)),Qt::DirectConnection);

    QJSValue valueGlobalScript=newQObject(&g_globalScript);
    globalObject().setProperty("includeScript",valueGlobalScript.property("includeScript"));
    globalObject().setProperty("_log",valueGlobalScript.property("_log"));
    globalObject().setProperty("_setResult",valueGlobalScript.property("_setResult"));
#endif

    g_pBinary=0;
    g_pBinaryScript=0;

    g_pExtra=0;
    g_pExtraScript=0;

    g_pBinary=new XBinary(pDevice); // TODO Check memory leak!
    g_pBinaryScript=new Binary_Script(g_pBinary); // TODO Check memory leak!
    _addClass(g_pBinaryScript,"Binary");

    if(XBinary::checkFileType(XBinary::FT_COM,fileType))
    {
        XCOM *pCOM=new XCOM(pDevice);
        g_pExtraScript=new COM_Script(pCOM);
        _addClass(g_pExtraScript,"COM");
        g_pExtra=pCOM;
    }
    else if(XBinary::checkFileType(XBinary::FT_PE,fileType))
    {
        XPE *pPE=new XPE(pDevice);
        g_pExtraScript=new PE_Script(pPE);
        _addClass(g_pExtraScript,"PE");
        g_pExtra=pPE;
    }
    else if(XBinary::checkFileType(XBinary::FT_ELF,fileType))
    {
        XELF *pELF=new XELF(pDevice);
        g_pExtraScript=new ELF_Script(pELF);
        _addClass(g_pExtraScript,"ELF");
        g_pExtra=pELF;
    }
    else if(XBinary::checkFileType(XBinary::FT_MACHO,fileType))
    {
        XMACH *pMACH=new XMACH(pDevice);
        g_pExtraScript=new MACH_Script(pMACH);
        _addClass(g_pExtraScript,"MACH");
        g_pExtra=pMACH;
    }
    else if(XBinary::checkFileType(XBinary::FT_NE,fileType))
    {
        XNE *pNE=new XNE(pDevice);
        g_pExtraScript=new NE_Script(pNE);
        _addClass(g_pExtraScript,"NE");
        g_pExtra=pNE;
    }
    else if(XBinary::checkFileType(XBinary::FT_LE,fileType))
    {
        XLE *pLE=new XLE(pDevice);
        g_pExtraScript=new LE_Script(pLE);
        _addClass(g_pExtraScript,"LE");
        g_pExtra=pLE;
    }
    else if(XBinary::checkFileType(XBinary::FT_LX,fileType))
    {
        XLE *pLE=new XLE(pDevice);
        g_pExtraScript=new LX_Script(pLE);
        _addClass(g_pExtraScript,"LX");
        g_pExtra=pLE;
    }
    else if(XBinary::checkFileType(XBinary::FT_MSDOS,fileType))
    {
        XMSDOS *pXMSDOS=new XMSDOS(pDevice);
        g_pExtraScript=new MSDOS_Script(pXMSDOS);
        _addClass(g_pExtraScript,"MSDOS");
        g_pExtra=pXMSDOS;
    }
//    else if(XBinary::checkFileType(XBinary::FT_JAR,fileType))
//    {
//        XZip *pZIP=new XZip(pDevice);
//        g_pBinaryScript=new JAR_Script(pZIP);
//        _addClass(g_pBinaryScript,"JAR");
//        g_pBinary=pZIP;
//    }
//    else if(XBinary::checkFileType(XBinary::FT_APK,fileType))
//    {
//        XZip *pZIP=new XZip(pDevice);
//        g_pBinaryScript=new APK_Script(pZIP);
//        _addClass(g_pBinaryScript,"APK");
//        g_pBinary=pZIP;
//    }
//    else if(XBinary::checkFileType(XBinary::FT_IPA,fileType))
//    {
//        XZip *pZIP=new XZip(pDevice);
//        g_pBinaryScript=new IPA_Script(pZIP);
//        _addClass(g_pBinaryScript,"IPA");
//        g_pBinary=pZIP;
//    }
//    if(XBinary::checkFileType(XBinary::FT_DEX,fileType))
//    {
//        XDEX *pDEX=new XDEX(pDevice);
//        g_pBinaryScript=new DEX_Script(pDEX);
//        _addClass(g_pBinaryScript,"DEX");
//        g_pBinary=pDEX;
//    }
}

DiE_ScriptEngine::~DiE_ScriptEngine()
{
    if(g_pBinary)           delete g_pBinary;
    if(g_pExtra)            delete g_pExtra;
    if(g_pBinaryScript)     delete g_pBinaryScript;
    if(g_pExtraScript)      delete g_pExtraScript;
}

bool DiE_ScriptEngine::handleError(XSCRIPTVALUE value, QString *psErrorString)
{
    bool bResult=true;

    if(value.isError())
    {
        // TODO Check more information
        *psErrorString=QString("%1: %2").arg(value.property("lineNumber").toString(),value.toString());

        bResult=false;
    }

    return bResult;
}

QList<DiE_ScriptEngine::RESULT> DiE_ScriptEngine::getListResult()
{
    return g_listResult;
}

void DiE_ScriptEngine::clearListResult()
{
    g_listResult.clear();
}

void DiE_ScriptEngine::stop()
{
    if(g_pBinary)
    {
        g_pBinary->setSearchProcessEnable(false);
    }
}
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::includeScript(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine=static_cast<DiE_ScriptEngine *>(pEngine);

    if(pScriptEngine)
    {
        pEngine->currentContext()->setActivationObject(pEngine->currentContext()->parentContext()->activationObject());

        QString sScript=pContext->argument(0).toString();

        pScriptEngine->includeScriptSlot(sScript);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_log(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine=static_cast<DiE_ScriptEngine *>(pEngine);

    if(pScriptEngine)
    {
        QString sText=pContext->argument(0).toString();

        pScriptEngine->_logSlot(sText);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_setResult(QScriptContext *pContext,QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine=static_cast<DiE_ScriptEngine *>(pEngine);

    if(pScriptEngine)
    {
        QString sType=pContext->argument(0).toString();
        QString sName=pContext->argument(1).toString();
        QString sVersion=pContext->argument(2).toString();
        QString sOptions=pContext->argument(3).toString();

        pScriptEngine->_setResultSlot(sType,sName,sVersion,sOptions);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
void DiE_ScriptEngine::_addFunction(QScriptEngine::FunctionSignature function, QString sFunctionName)
{
    QScriptValue func=this->newFunction(function);
    this->globalObject().setProperty(sFunctionName,func);
}
#endif

void DiE_ScriptEngine::_addClass(QObject *pClass, QString sClassName)
{
    XSCRIPTVALUE objectWnd=this->newQObject(pClass);
    this->globalObject().setProperty(sClassName, objectWnd);
}

void DiE_ScriptEngine::includeScriptSlot(QString sScript)
{
// TODO error, cannot find signature

    qint32 nNumberOfSignatures=g_pSignaturesList->count();

    for(qint32 i=0;i<nNumberOfSignatures;i++)
    {
        if(g_pSignaturesList->at(i).fileType==XBinary::FT_UNKNOWN)
        {
            if(g_pSignaturesList->at(i).sName==sScript)
            {
                // TODO error
                evaluate(g_pSignaturesList->at(i).sText);

                break;
            }
        }
    }
}

void DiE_ScriptEngine::_logSlot(QString sText)
{
    emit infoMessage(sText);
}

void DiE_ScriptEngine::_setResultSlot(QString sType, QString sName, QString sVersion, QString sOptions)
{
    RESULT record={};
    record.sType=sType;
    record.sName=sName;
    record.sVersion=sVersion;
    record.sOptions=sOptions;

    g_listResult.append(record);
}

DiE_ScriptEngine::RESULT DiE_ScriptEngine::stringToResult(QString sString, bool bShowType, bool bShowVersion, bool bShowOptions)
{
    RESULT result={};

    if(bShowType)
    {
        result.sType=sString.section(": ",0,0);
        sString=sString.section(": ",1,-1);
    }

    QString _sString=sString;

    if(bShowOptions)
    {
        if(_sString.count("[")==1)
        {
            result.sName=_sString.section("[",0,0);
            result.sOptions=_sString.section("[",1,-1).section("]",0,0);
            _sString=_sString.section("[",0,0);
        }
    }

    if(bShowVersion)
    {
        if(_sString.count("(")==1)
        {
            result.sVersion=_sString.section("(",1,-1).section(")",0,0);
            result.sName=_sString.section("(",0,0);
        }
    }

    return result;
}
