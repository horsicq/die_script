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
#include "die_scriptengine.h"

DiE_ScriptEngine::DiE_ScriptEngine(QList<DiE_ScriptEngine::SIGNATURE_RECORD> *pSignaturesList, QIODevice *pDevice, XBinary::FT fileType)
{
    this->pSignaturesList=pSignaturesList;

    _addFunction(_includeScript,"includeScript");

    if(fileType==XBinary::FT_BINARY)
    {
        pBinary=new XBinary(pDevice);
        pBinaryScript=new Binary_Script(pBinary);
        _addClass(pBinaryScript,"Binary");
    }
    else if(fileType==XBinary::FT_PE)
    {
        XPE *pPE=new XPE(pDevice);
        pBinaryScript=new PE_Script(pPE);
        _addClass(pBinaryScript,"PE");
        pBinary=pPE;
    }
    else if(fileType==XBinary::FT_ELF)
    {
        XELF *pELF=new XELF(pDevice);
        pBinaryScript=new ELF_Script(pELF);
        _addClass(pBinaryScript,"ELF");
        pBinary=pELF;
    }
    else if(fileType==XBinary::FT_MACH)
    {
        XMACH *pMACH=new XMACH(pDevice);
        pBinaryScript=new MACH_Script(pMACH);
        _addClass(pBinaryScript,"MACH");
        pBinary=pMACH;
    }
    else if(fileType==XBinary::FT_MSDOS)
    {
        XMSDOS *pXMSDOS=new XMSDOS(pDevice);
        pBinaryScript=new MSDOS_Script(pXMSDOS);
        _addClass(pBinaryScript,"MSDOS");
        pBinary=pXMSDOS;
    }
    else if(fileType==XBinary::FT_TEXT)
    {
        XBinary *pText=new XBinary(pDevice);
        pBinaryScript=new Text_Script(pText);
        _addClass(pBinaryScript,"Text");
        pBinary=pText;
    }
}

DiE_ScriptEngine::~DiE_ScriptEngine()
{
    delete pBinaryScript;
}

bool DiE_ScriptEngine::handleError(QScriptValue value, QString *psErrorString)
{
    bool bResult=true;

    if(value.isError())
    {
        *psErrorString=QString("%1: %2").arg(value.property("lineNumber").toInt32()).arg(value.toString());

        bResult=false;
    }

    return bResult;
}

QScriptValue DiE_ScriptEngine::_includeScript(QScriptContext *context, QScriptEngine *engine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine=static_cast<DiE_ScriptEngine *>(engine);

    if(pScriptEngine)
    {
        QString sName=context->argument(0).toString();

        int nCount=pScriptEngine->pSignaturesList->count();

        for(int i=0;i<nCount;i++)
        {
            if(pScriptEngine->pSignaturesList->at(i).fileType==XBinary::FT_UNKNOWN)
            {
                if(pScriptEngine->pSignaturesList->at(i).sName==sName)
                {
                    engine->currentContext()->setActivationObject(engine->currentContext()->parentContext()->activationObject());
                    result=engine->evaluate(pScriptEngine->pSignaturesList->at(i).sText);

                    break;
                }
            }
        }
    }

    return result;
}

void DiE_ScriptEngine::_addFunction(QScriptEngine::FunctionSignature function, QString sFunctionName)
{
    QScriptValue func=this->newFunction(function);
    this->globalObject().setProperty(sFunctionName,func);
}

void DiE_ScriptEngine::_addClass(QObject *pClass, QString sClassName)
{
    QScriptValue objectWnd=this->newQObject(pClass);
    this->globalObject().setProperty(sClassName, objectWnd);
}
