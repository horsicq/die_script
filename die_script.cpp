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
}

void DiE_Script::_scan(QIODevice *pDevice, DiE_ScriptEngine::STYPE stype)
{
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

    for(int i=0;i<nCount;i++)
    {
        if((listSignatures.at(i).stype==stype)&&(listSignatures.at(i).sName!="_init"))
        {
            QString sInfo=listSignatures.at(i).sName;

            DiE_ScriptEngine scriptEngine(&listSignatures,pDevice,stype);

            if(srGlobalInit.sText!="")
            {
                scriptEngine.handleError(scriptEngine.evaluate(srGlobalInit.sText),"_init");
            }

            if(srInit.sText!="")
            {
                scriptEngine.handleError(scriptEngine.evaluate(srInit.sText),"_init");
            }

            QScriptValue script=scriptEngine.evaluate(listSignatures.at(i).sText);

            if(scriptEngine.handleError(script,sInfo))
            {
                QScriptValue detect=scriptEngine.globalObject().property("detect");

                if(scriptEngine.handleError(detect,sInfo))
                {
                    QScriptValueList valuelist;

                    valuelist <<true<<true<<true;

                    QScriptValue result=detect.call(script,valuelist);

                    if(scriptEngine.handleError(result,sInfo))
                    {

                    }
                }
            }
        }
    }
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

bool DiE_Script::scanFile(QString sFileName)
{
    QFile file;

    file.setFileName(sFileName);

    if(file.open(QIODevice::ReadOnly))
    {
        QSet<XBinary::FT> stFT=XBinary::getFileTypes(&file);

        if(stFT.contains(XBinary::FT_PE32)||stFT.contains(XBinary::FT_PE64))
        {
            _scan(&file,DiE_ScriptEngine::STYPE_PE);
        }
        else if(stFT.contains(XBinary::FT_ELF32)||stFT.contains(XBinary::FT_ELF64))
        {
            _scan(&file,DiE_ScriptEngine::STYPE_ELF);
        }
        else if(stFT.contains(XBinary::FT_MACH32)||stFT.contains(XBinary::FT_MACH64))
        {
            _scan(&file,DiE_ScriptEngine::STYPE_MACH);
        }
        else if(stFT.contains(XBinary::FT_MSDOS))
        {
            _scan(&file,DiE_ScriptEngine::STYPE_MSDOS);
        }
        else
        {
            if(XBinary::isPlainTextType(&file))
            {
                _scan(&file,DiE_ScriptEngine::STYPE_TEXT);
            }
            else
            {
                _scan(&file,DiE_ScriptEngine::STYPE_BINARY);
            }
        }

        file.close();
    }

    return false;
}
