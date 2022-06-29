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
#ifndef DIE_SCRIPT_H
#define DIE_SCRIPT_H

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSemaphore>
#include <QTimer>
#include <QXmlStreamWriter>
#include "die_scriptengine.h"
#include "xformats.h"
#include "xzip.h"
#include "scanitemmodel.h"
#ifdef QT_SCRIPTTOOLS_LIB
#include <QScriptEngineDebugger>
#include <QAction>
#endif

bool sort_signature_prio(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1,const DiE_ScriptEngine::SIGNATURE_RECORD &sr2);
bool sort_signature_name(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1,const DiE_ScriptEngine::SIGNATURE_RECORD &sr2);

// TODO signatures stats function
class DiE_Script : public QObject
{
    Q_OBJECT

public:
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

    struct SCAN_STRUCT
    {
        bool bIsHeuristic;
        XBinary::SCANID id;
        XBinary::SCANID parentId;
//        SCAN_HEADER scanHeader;
//        XBinary::FT fileType;
        QString sFullString;
        QString sType;
        QString sResult;
        QString sName;
        QString sVersion;
        QString sOptions;
        QString sSignature;
    };

    struct ERROR_RECORD
    {
        QString sScript;
        QString sErrorString;
    };

    struct DEBUG_RECORD
    {
        QString sScript;
        qint64 nElapsedTime;
    };

    struct SCAN_RESULT
    {
        qint64 nScanTime;
        QString sFileName;
        qint64 nSize;
//        XBinary::SCANID id;
//        SCAN_HEADER scanHeader; // TODO set
        QList<SCAN_STRUCT> listRecords;
        QList<ERROR_RECORD> listErrors;
        QList<DEBUG_RECORD> listDebugRecords;
//        bool bIsValidType;
    };

    struct SCAN_OPTIONS
    {
        bool bIsDeepScan;
        bool bIsHeuristicScan;
        bool bAllTypesScan;
        bool bRecursiveScan;
        bool bDebug;
        bool bShowType;
        bool bShowVersion;
        bool bShowOptions;
        bool bShowEntropy;
        bool bShowExtraInfo;
        QString sSpecial; // Special info
        XBinary::FT fileType; // Optional
        QString sSignatureName; // Optional
        bool bResultAsXML;
        bool bResultAsJSON;
        bool bResultAsCSV;
        bool bResultAsTSV;
        bool bSubdirectories; // For directory scan
    };

    struct STATS
    {
        QMap<QString,qint32> mapTypes;
    };

    struct SIGNATURE_STATE
    {
        XBinary::FT fileType;
        qint32 nNumberOfSignatures;
    };

    enum DBT
    {
        DBT_UNKNOWN=0,
        DBT_FOLDER,
        DBT_COMPRESSED
    };

    explicit DiE_Script(QObject *pParent=nullptr);
    bool loadDatabase(QString sDatabasePath);
    QString getDatabasePath();

    QList<SIGNATURE_STATE> getSignatureStates();
    qint32 getNumberOfSignatures(XBinary::FT fileType);

    QList<DiE_ScriptEngine::SIGNATURE_RECORD> *getSignatures();
    SCAN_RESULT scanFile(QString sFileName,SCAN_OPTIONS *pOptions,XBinary::PDSTRUCT *pPdStruct=nullptr);
    SCAN_RESULT scanDevice(QIODevice *pDevice,SCAN_OPTIONS *pOptions,XBinary::PDSTRUCT *pPdStruct=nullptr);
    void scan(QIODevice *pDevice,SCAN_RESULT *pScanResult,qint64 nOffset,qint64 nSize,XBinary::SCANID parentId,SCAN_OPTIONS *pOptions,bool bInit,XBinary::PDSTRUCT *pPdStruct);
    DiE_ScriptEngine::SIGNATURE_RECORD getSignatureByFilePath(QString sSignatureFilePath);
    bool updateSignature(QString sSignatureFilePath,QString sText);
    STATS getStats();
    DBT getDatabaseType();
    bool isSignaturesPresent(XBinary::FT fileType);

    static QString getErrorsString(SCAN_RESULT *pScanResult);
#ifdef QT_SCRIPTTOOLS_LIB
    void setDebugger(QScriptEngineDebugger *pDebugger);
    void removeDebugger();
#endif
    void setProcessDirectory(QString sDirectory, SCAN_OPTIONS scanOptions, XBinary::PDSTRUCT *pPdStruct);

    static QList<XBinary::SCANSTRUCT> convert(QList<SCAN_STRUCT> *pListScanStructs);

public slots:
    void processDirectory();

private:
    static QList<DiE_ScriptEngine::SIGNATURE_RECORD> _loadDatabasePath(QString sDatabasePath,XBinary::FT fileType);
    static QList<DiE_ScriptEngine::SIGNATURE_RECORD> _loadDatabaseFromZip(XZip *pZip,QList<XArchive::RECORD> *pListRecords,QString sPrefix,XBinary::FT fileType);
    XBinary::SCANID _scan(SCAN_RESULT *pScanResult,QIODevice *pDevice,XBinary::SCANID parentId,XBinary::FT fileType,SCAN_OPTIONS *pOptions,QString sSignatureFilePath,qint64 nOffset,bool bAddUnknown,XBinary::PDSTRUCT *pPdStruct);
    bool _handleError(DiE_ScriptEngine *pScriptEngine,XSCRIPTVALUE scriptValue,DiE_ScriptEngine::SIGNATURE_RECORD *pSignatureRecord,SCAN_RESULT *pScanResult);

signals:
    void progressMaximumChanged(qint32 nMaximum);
    void progressValueChanged(qint32 nValue);
    void directoryScanCompleted(qint64 nTime);
    void directoryScanFileStarted(QString sFileName);
    void directoryScanResult(DiE_Script::SCAN_RESULT scanResult);
    void errorMessage(QString sErrorMessage);
    void infoMessage(QString sInfoMessage);

private:
    QString g_sDatabasePath;
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> g_listSignatures;
    DBT g_databaseType;
#ifdef QT_SCRIPTTOOLS_LIB
    QScriptEngineDebugger *pDebugger;
#endif
    QString g_sDirectoryProcess;
    SCAN_OPTIONS g_scanOptionsProcess;
    QElapsedTimer *g_pDirectoryElapsedTimer;
    XBinary::PDSTRUCT *g_pPdStruct;
//    QMutex g_mutex;
//    QSemaphore g_semaphore;
};

#endif // DIE_SCRIPT_H
