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
#ifndef DIE_SCRIPTENGINE_H
#define DIE_SCRIPTENGINE_H

#include <QIODevice>
#include "xbinary.h"
#include "global_script.h"
#include "binary_script.h"
#include "com_script.h"
#include "elf_script.h"
#include "mach_script.h"
#include "msdos_script.h"
#include "pe_script.h"
#include "ne_script.h"
#include "le_script.h"
#include "lx_script.h"
//#include "jar_script.h"
//#include "apk_script.h"
//#include "ipa_script.h"
//#include "dex_script.h"

#ifdef QT_SCRIPT_LIB
#include <QScriptEngine>
#else
#include <QJSEngine>
#include "global_script.h"
#endif

#ifdef QT_SCRIPT_LIB
#define XSCRIPTENGINE QScriptEngine
#define XSCRIPTVALUE QScriptValue
#define XSCRIPTVALUELIST QScriptValueList
#else
#define XSCRIPTENGINE QJSEngine
#define XSCRIPTVALUE QJSValue
#define XSCRIPTVALUELIST QJSValueList
#endif

class DiE_ScriptEngine : public XSCRIPTENGINE
{
    Q_OBJECT

public:
    struct SIGNATURE_RECORD
    {
        XBinary::FT fileType;
        QString sName;
        QString sFilePath;
        QString sText;
        bool bReadOnly;
    };

    struct RESULT
    {
        QString sType;
        QString sName;
        QString sVersion;
        QString sOptions;
    };

    DiE_ScriptEngine(QList<SIGNATURE_RECORD> *pSignaturesList,QIODevice *pDevice,XBinary::FT fileType,XBinary::PDSTRUCT *pPdStruct);
    ~DiE_ScriptEngine();
    bool handleError(XSCRIPTVALUE value,QString *psErrorString);
    QList<RESULT> getListResult();
    void clearListResult();
    static RESULT stringToResult(QString sString,bool bShowType,bool bShowVersion,bool bShowOptions);

public slots:
    void stop();

private:
#ifdef QT_SCRIPT_LIB
    static QScriptValue includeScript(QScriptContext *pContext,QScriptEngine *pEngine);
    static QScriptValue _log(QScriptContext *pContext,QScriptEngine *pEngine);
    static QScriptValue _setResult(QScriptContext *pContext,QScriptEngine *pEngine);
    void _addFunction(FunctionSignature function,QString sFunctionName);
#endif
    void _addClass(QObject *pClass,QString sClassName);

private slots:
    void includeScriptSlot(QString sScript);
    void _logSlot(QString sText);
    void _setResultSlot(QString sType,QString sName,QString sVersion,QString sOptions);

signals:
    void errorMessage(QString sErrorMessage);
    void infoMessage(QString sInfoMessage);

private:
    QList<SIGNATURE_RECORD> *g_pSignaturesList;
    XBinary *g_pBinary;
    Binary_Script *g_pBinaryScript;
    XBinary *g_pExtra;
    Binary_Script *g_pExtraScript;
    XBinary::PDSTRUCT *g_pPdStruct;

    QList<RESULT> g_listResult;
#ifndef QT_SCRIPT_LIB
    global_script g_globalScript;
#endif
};

#endif // DIE_SCRIPTENGINE_H
