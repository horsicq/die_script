// copyright (c) 2019-2020 hors<horsicq@gmail.com>
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
#ifndef DIE_SCRIPTENGINE_H
#define DIE_SCRIPTENGINE_H

#include <QScriptEngine>
#include <QIODevice>
#include "xbinary.h"
#include "binary_script.h"
#include "elf_script.h"
#include "mach_script.h"
#include "msdos_script.h"
#include "pe_script.h"

class DiE_ScriptEngine : public QScriptEngine
{
    Q_OBJECT

public:
    struct SIGNATURE_RECORD
    {
        XBinary::FT fileType;
        QString sName;
        QString sFilePath;
        QString sText;
    };

    DiE_ScriptEngine(QList<SIGNATURE_RECORD> *pSignaturesList,QIODevice *pDevice,XBinary::FT fileType);
    ~DiE_ScriptEngine();
    bool handleError(QScriptValue value,QString *psErrorString);
    static bool isSignatureTypeValid(XBinary::FT ftSignature, XBinary::FT ftTarget);

public slots:
    void stop();

private:
    static QScriptValue _includeScript(QScriptContext *context, QScriptEngine *engine);
    void _addFunction(FunctionSignature function,QString sFunctionName);
    void _addClass(QObject *pClass,QString sClassName);

private:
    QList<SIGNATURE_RECORD> *pSignaturesList;
    XBinary *pBinary;
    Binary_Script *pBinaryScript;
};

#endif // DIE_SCRIPTENGINE_H
