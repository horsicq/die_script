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

    enum DBT {
        DBT_UNKNOWN = 0,
        DBT_FOLDER,
        DBT_COMPRESSED
    };

    explicit DiE_Script(QObject *pParent = nullptr);
    bool loadDatabase(const QString &sDatabasePath, bool bInit, XBinary::PDSTRUCT *pPdStruct = nullptr);

    QList<SIGNATURE_STATE> getSignatureStates();
    qint32 getNumberOfSignatures(XBinary::FT fileType);
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> *getSignatures();

    // TODO move to XScanEngine
    XScanEngine::SCAN_RESULT scanFile(const QString &sFileName, XScanEngine::SCAN_OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct = nullptr);
    XScanEngine::SCAN_RESULT scanDevice(QIODevice *pDevice, XScanEngine::SCAN_OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct = nullptr);
    XScanEngine::SCAN_RESULT processFile(const QString &sFileName, XScanEngine::SCAN_OPTIONS *pOptions, const QString &sFunction, XBinary::PDSTRUCT *pPdStruct = nullptr);
    XScanEngine::SCAN_RESULT processDevice(QIODevice *pDevice, XScanEngine::SCAN_OPTIONS *pOptions, const QString &sFunction, XBinary::PDSTRUCT *pPdStruct = nullptr);

    void process(QIODevice *pDevice, const QString &sFunction, XScanEngine::SCAN_RESULT *pScanResult, qint64 nOffset, qint64 nSize, XScanEngine::SCANID parentId, XScanEngine::SCAN_OPTIONS *pOptions,
                 bool bInit, XBinary::PDSTRUCT *pPdStruct);
    DiE_ScriptEngine::SIGNATURE_RECORD getSignatureByFilePath(const QString &sSignatureFilePath);
    bool updateSignature(const QString &sSignatureFilePath, const QString &sText);
    STATS getStats();
    DBT getDatabaseType();
    bool isSignaturesPresent(XBinary::FT fileType);

    static QString getErrorsString(XScanEngine::SCAN_RESULT *pScanResult); // TODO move to XBinary
    static QList<QString> getErrorsAndWarningsStringList(XScanEngine::SCAN_RESULT *pScanResult); // TODO move to XBinary
#ifdef QT_SCRIPTTOOLS_LIB
    void setDebugger(QScriptEngineDebugger *pDebugger);
    void removeDebugger();
#endif
    void setData(const QString &sDirectory, const XScanEngine::SCAN_OPTIONS &scanOptions, XBinary::PDSTRUCT *pPdStruct);
    void setData(QIODevice *pDevice, const XScanEngine::SCAN_OPTIONS &scanOptions, XBinary::PDSTRUCT *pPdStruct);

    static QList<XScanEngine::SCANSTRUCT> convert(QList<DiE_ScriptEngine::SCAN_STRUCT> *pListScanStructs);
    XScanEngine::SCAN_RESULT getScanResultProcess();

public slots:
    void process();

private:
    DiE_ScriptEngine::SIGNATURE_RECORD _loadSignatureRecord(const QFileInfo &fileInfo, XBinary::FT fileType);
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> _loadDatabasePath(const QString &sDatabasePath, XBinary::FT fileType, XBinary::PDSTRUCT *pPdStruct);
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> _loadDatabaseFromZip(XZip *pZip, QList<XArchive::RECORD> *pListRecords, const QString &sPrefix, XBinary::FT fileType);
    void _processDetect(XScanEngine::SCANID *pScanID, XScanEngine::SCAN_RESULT *pScanResult, QIODevice *pDevice, const QString &sDetectFunction, const XScanEngine::SCANID &parentId, XBinary::FT fileType,
                        XScanEngine::SCAN_OPTIONS *pOptions, const QString &sSignatureFilePath, bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct);
    bool _handleError(DiE_ScriptEngine *pScriptEngine, XSCRIPTVALUE scriptValue, DiE_ScriptEngine::SIGNATURE_RECORD *pSignatureRecord, XScanEngine::SCAN_RESULT *pScanResult);

protected:
    virtual void _processDetect(XScanEngine::SCANID *pScanID, SCAN_RESULT *pScanResult, QIODevice *pDevice, const SCANID &parentId, XBinary::FT fileType,
                        SCAN_OPTIONS *pOptions, bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct);

signals:
    void scanCompleted(qint64 nTime);
    void directoryScanFileStarted(const QString &sFileName);
    void directoryScanResult(const XScanEngine::SCAN_RESULT &scanResult);
    void errorMessage(const QString &sErrorMessage);
    void warningMessage(const QString &sWarningMessage);
    void infoMessage(const QString &sInfoMessage);

private slots:
    void _errorMessage(const QString &sErrorMessage);
    void _warningMessage(const QString &sWarningMessage);
    void _infoMessage(const QString &sInfoMessage);

private:
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> g_listSignatures;
    DBT g_databaseType;
    bool g_bIsErrorLogEnable;
    bool g_bIsWarningLogEnable;
    bool g_bIsInfoLogEnable;
#ifdef QT_SCRIPTTOOLS_LIB
    QScriptEngineDebugger *g_pDebugger;
#endif
    QString g_sDirectoryProcess;
    QIODevice *g_pDeviceProcess;
    XScanEngine::SCAN_OPTIONS g_scanOptionsProcess;
    XScanEngine::SCAN_RESULT g_scanResultProcess;
    XBinary::PDSTRUCT *g_pPdStruct;
    //    QMutex g_mutex;
    //    QSemaphore g_semaphore;
};

#endif  // DIE_SCRIPT_H
