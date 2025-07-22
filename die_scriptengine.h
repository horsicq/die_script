/* Copyright (c) 2019-2025 hors<horsicq@gmail.com>
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
#include "util_script.h"
#include "xscriptengine.h"
#include "xscanengine.h"

class DiE_ScriptEngine : public XScriptEngine {
    Q_OBJECT

public:
    enum DT {
        DT_MAIN = 0,
        DT_EXTRA,
        DT_CUSTOM
    };

    struct SIGNATURE_RECORD {
        XBinary::FT fileType;
        QString sName;
        QString sFilePath;
        DT databaseType;
        QString sText;
        bool bReadOnly;
    };

    //    struct SCAN_HEADER
    //    {
    //        XBinary::FT fileType;
    //        QString sArch;
    //        XBinary::MODE mode;
    //        bool bIsBigEndian;
    //        QString sType;
    //        qint64 nSize;
    //        qint64 nOffset;
    //        XBinary::FILEPART filePart;
    //    };

    struct SCAN_STRUCT {
        bool bIsUnknown;
        XScanEngine::SCANID id;
        XScanEngine::SCANID parentId;
        //        SCAN_HEADER scanHeader;
        //        XBinary::FT fileType;
        // QString sFullString;
        QString sType;
        // QString sResult;
        QString sName;
        QString sVersion;
        QString sOptions;
        QString sSignature;
        QString sSignatureFileName;
    };

    struct BLRECORD {
        QString sType;
        QString sName;
    };

    DiE_ScriptEngine(QList<SIGNATURE_RECORD> *pSignaturesList, QList<SCAN_STRUCT> *pListScanStructs, QIODevice *pDevice, XBinary::FT fileType, XBinary::FILEPART filePart,
                     Binary_Script::OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct);
    ~DiE_ScriptEngine();

    void _adjustScript(XBinary *pBinary, Binary_Script *pScript, const QString &sName);

    bool handleError(QString sPrefix, XSCRIPTVALUE value, QString *psErrorString);
    // QList<RESULT> getListLocalResult();
    // void clearListLocalResult();
    // static RESULT stringToResult(const QString &sString, bool bShowType, bool bShowVersion, bool bShowOptions);
    XSCRIPTVALUE evaluateEx(const XScanEngine::SCANID &parentId, const XScanEngine::SCANID &resultId, const QString &sProgram, const QString &sName,
                            const QString &sFileName);

private:
#ifdef QT_SCRIPT_LIB
    static QScriptValue includeScript(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _log(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _setResult(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _isResultPresent(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _getNumberOfResults(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _removeResult(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _isStop(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _encodingList(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _isConsoleMode(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _isLiteMode(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _isGuiMode(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _isLibraryMode(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _breakScan(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _getEngineVersion(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _getOS(QScriptContext *pContext, QScriptEngine *pEngine);
#endif

private slots:
    void includeScriptSlot(const QString &sScript);
    void _logSlot(const QString &sText);
    void _setResultSlot(const QString &sType, const QString &sName, const QString &sVersion, const QString &sOptions);
    void _isResultPresentSlot(bool *pbResult, const QString &sType, const QString &sName);
    void _getNumberOfResultsSlot(qint32 *pnResult, const QString &sType);
    void _removeResultSlot(const QString &sType, const QString &sName);
    void _isStopSlot(bool *pResult);
    void _encodingListSlot();
    void _isConsoleModeSlot(bool *pResult);
    void _isLiteModeSlot(bool *pResult);
    void _isGuiModeSlot(bool *pResult);
    void _isLibraryModeSlot(bool *pResult);
    void _breakScanSlot();
    void _getEngineVersionSlot(QString *pResult);
    void _getOSSlot(QString *pResult);
    void _getQtVersionSlot(QString *pResult);

private:
    QList<SIGNATURE_RECORD> *g_pSignaturesList;
    QList<SCAN_STRUCT> *g_pListScanStructs;
    QList<XBinary *> g_listBinaries;
    QList<QObject *> g_listScriptClasses;
    XBinary::PDSTRUCT *g_pPdStruct;

    XScanEngine::SCANID g_parentId;
    XScanEngine::SCANID g_resultId;  // TODO rename
    QString g_sName;                 // TODO rename
    QString g_sFileName;             // TODO rename

    QList<BLRECORD> g_listBLRecords;

    // QList<RESULT> g_listResult; // TODO remove
#ifndef QT_SCRIPT_LIB
    global_script g_globalScript;
#endif
};

#endif  // DIE_SCRIPTENGINE_H
