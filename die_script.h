/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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
#ifndef DIE_SCRIPT_H
#define DIE_SCRIPT_H

#include "die_scriptengine.h"
#include "xscanengine.h"
#ifdef QT_SCRIPTTOOLS_LIB
#include <QAction>
#include <QScriptEngineDebugger>
#endif

bool sort_signature_prio(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2);
bool sort_signature_name(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2);

class DiE_Script : public XScanEngine {
    Q_OBJECT

public:
    struct STATS {
        QMap<QString, qint32> mapTypes;
    };

    struct SIGNATURE_STATE {
        XBinary::FT fileType;
        qint32 nNumberOfSignatures;
    };

    explicit DiE_Script(QObject *pParent = nullptr);
    void initDatabase();
    bool loadDatabase(XScanEngine::SCAN_OPTIONS *pScanOptions, const QString &sDatabasePath, QString sDatabaseName, XBinary::PDSTRUCT *pPdStruct = nullptr);

    QList<SIGNATURE_STATE> getSignatureStates();
    qint32 getNumberOfSignatures(XBinary::FT fileType);
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> *getSignatures();

    DiE_ScriptEngine::SIGNATURE_RECORD getSignatureByFilePath(const QString &sSignatureFilePath);
    bool updateSignature(const QString &sSignatureFilePath, const QString &sText);
    STATS getStats();
    bool isSignaturesPresent(XBinary::FT fileType);

    static QString getErrorsString(XScanEngine::SCAN_RESULT *pScanResult);                        // TODO move to XBinary
    static QList<QString> getErrorsAndWarningsStringList(XScanEngine::SCAN_RESULT *pScanResult);  // TODO move to XBinary
#ifdef QT_SCRIPTTOOLS_LIB
    void setDebugger(QScriptEngineDebugger *pDebugger);
    void removeDebugger();
#endif
    static QList<XScanEngine::SCANSTRUCT> convert(QList<DiE_ScriptEngine::SCAN_STRUCT> *pListScanStructs);

private:
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> _loadDatabasePath(const QString &sDatabasePath, const QString &sDatabaseName, XBinary::FT fileType, XBinary::PDSTRUCT *pPdStruct);
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> _loadDatabaseFromZip(XZip *pZip, QList<XArchive::RECORD> *pListRecords, const QString &sDatabaseName, const QString &sPrefix, XBinary::FT fileType);
    void processDetect(XScanEngine::SCANID *pScanID, XScanEngine::SCAN_RESULT *pScanResult, QIODevice *pDevice, const XScanEngine::SCANID &parentId, XBinary::FT fileType,
                       XScanEngine::SCAN_OPTIONS *pOptions, const QString &sSignatureFilePath, bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct);
    bool _handleError(DiE_ScriptEngine *pScriptEngine, XSCRIPTVALUE scriptValue, DiE_ScriptEngine::SIGNATURE_RECORD *pSignatureRecord,
                      XScanEngine::SCAN_RESULT *pScanResult);

protected:
    virtual void _processDetect(XScanEngine::SCANID *pScanID, SCAN_RESULT *pScanResult, QIODevice *pDevice, const SCANID &parentId, XBinary::FT fileType,
                                SCAN_OPTIONS *pOptions, bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct);

private:
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> g_listSignatures;
#ifdef QT_SCRIPTTOOLS_LIB
    QScriptEngineDebugger *g_pDebugger;
#endif
    //    QMutex g_mutex;
    //    QSemaphore g_semaphore;
};

#endif  // DIE_SCRIPT_H
