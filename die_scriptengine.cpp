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
#include "die_scriptengine.h"

DiE_ScriptEngine::DiE_ScriptEngine(QList<DiE_ScriptEngine::SIGNATURE_RECORD> *pSignaturesList, QIODevice *pDevice, XBinary::FT fileType)
{
    this->g_pSignaturesList=pSignaturesList;

    // TODO _log function
    _addFunction(_includeScript,"includeScript");
    _addFunction(_log,"_log");

    g_pBinary=0;
    g_pBinaryScript=0;

    if(XBinary::checkFileType(XBinary::FT_BINARY,fileType))
    {
        g_pBinary=new XBinary(pDevice);
        g_pBinaryScript=new Binary_Script(g_pBinary);
        _addClass(g_pBinaryScript,"Binary");
    }

    if(XBinary::checkFileType(XBinary::FT_PE,fileType))
    {
        XPE *pPE=new XPE(pDevice);
        g_pBinaryScript=new PE_Script(pPE);
        _addClass(g_pBinaryScript,"PE");
        g_pBinary=pPE;
    }
    else if(XBinary::checkFileType(XBinary::FT_ELF,fileType))
    {
        XELF *pELF=new XELF(pDevice);
        g_pBinaryScript=new ELF_Script(pELF);
        _addClass(g_pBinaryScript,"ELF");
        g_pBinary=pELF;
    }
    else if(XBinary::checkFileType(XBinary::FT_MACHO,fileType))
    {
        XMACH *pMACH=new XMACH(pDevice);
        g_pBinaryScript=new MACH_Script(pMACH);
        _addClass(g_pBinaryScript,"MACH");
        g_pBinary=pMACH;
    }
    else if(XBinary::checkFileType(XBinary::FT_NE,fileType))
    {
        XNE *pNE=new XNE(pDevice);
        g_pBinaryScript=new NE_Script(pNE);
        _addClass(g_pBinaryScript,"NE");
        g_pBinary=pNE;
    }
    else if(XBinary::checkFileType(XBinary::FT_LE,fileType))
    {
        XLE *pLE=new XLE(pDevice);
        g_pBinaryScript=new LE_Script(pLE);
        _addClass(g_pBinaryScript,"LE");
        g_pBinary=pLE;
    }
    else if(XBinary::checkFileType(XBinary::FT_MSDOS,fileType))
    {
        XMSDOS *pXMSDOS=new XMSDOS(pDevice);
        g_pBinaryScript=new MSDOS_Script(pXMSDOS);
        _addClass(g_pBinaryScript,"MSDOS");
        g_pBinary=pXMSDOS;
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
    delete g_pBinary;
    delete g_pBinaryScript;
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

void DiE_ScriptEngine::stop()
{
    if(g_pBinary)
    {
        g_pBinary->setSearchProcessEnable(false);
    }
}

QScriptValue DiE_ScriptEngine::_includeScript(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine=static_cast<DiE_ScriptEngine *>(pEngine);

    // TODO error, cannot find signature

    if(pScriptEngine)
    {
        QString sName=pContext->argument(0).toString();

        qint32 nNumberOfSignatures=pScriptEngine->g_pSignaturesList->count();

        for(qint32 i=0;i<nNumberOfSignatures;i++)
        {
            if(pScriptEngine->g_pSignaturesList->at(i).fileType==XBinary::FT_UNKNOWN)
            {
                if(pScriptEngine->g_pSignaturesList->at(i).sName==sName)
                {
                    pEngine->currentContext()->setActivationObject(pEngine->currentContext()->parentContext()->activationObject());
                    result=pEngine->evaluate(pScriptEngine->g_pSignaturesList->at(i).sText);

                    break;
                }
            }
        }
    }

    return result;
}

QScriptValue DiE_ScriptEngine::_log(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine=static_cast<DiE_ScriptEngine *>(pEngine);

    if(pScriptEngine)
    {
        QString sText=pContext->argument(0).toString();

        pScriptEngine->emitInfoMessage(sText);
    }

    return result;
}

void DiE_ScriptEngine::_addFunction(QScriptEngine::FunctionSignature function, QString sFunctionName)
{
    XSCRIPTVALUE func=this->newFunction(function);
    this->globalObject().setProperty(sFunctionName,func);
}

void DiE_ScriptEngine::_addClass(QObject *pClass, QString sClassName)
{
    XSCRIPTVALUE objectWnd=this->newQObject(pClass);
    this->globalObject().setProperty(sClassName, objectWnd);
}

void DiE_ScriptEngine::emitErrorMessage(QString sErrorMessage)
{
    emit errorMessage(sErrorMessage);
}

void DiE_ScriptEngine::emitInfoMessage(QString sInfoMessage)
{
    emit infoMessage(sInfoMessage);
}
