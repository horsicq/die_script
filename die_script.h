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
#ifndef DIE_SCRIPT_H
#define DIE_SCRIPT_H

#include <QObject>
#include <QDir>
#include <QXmlStreamWriter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "die_scriptengine.h"
#include "xformats.h"
#include "xzip.h"
#ifdef QT_SCRIPTTOOLS_LIB
#include <QScriptEngineDebugger>
#include <QAction>
#endif

class DiE_Script : public QObject
{
    Q_OBJECT

public:
    struct SCAN_HEADER
    {
        XBinary::FT fileType;
        QString sArch;
    };

    struct SCAN_STRUCT
    {
        SCAN_HEADER scanHeader;
        QString sType;
        QString sString;
        QString sName;
        QString sVersion;
        QString sOptions;
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
        SCAN_HEADER scanHeader; // TODO set
        QList<SCAN_STRUCT> listRecords;
        QList<ERROR_RECORD> listErrors;
        QList<DEBUG_RECORD> listDebugRecords;
        bool bIsValidType;
    };

    struct SCAN_OPTIONS
    {
        bool bDeepScan;
        bool bDebug;
        bool bShowType;
        bool bShowVersion;
        bool bShowOptions;
        XBinary::FT fileType; // Optional
        QString sSignatureName; // Optional
        bool bResultAsXML;
        bool bResultAsJSON;
    };

    struct STATS
    {
        QMap<QString,qint32> mapTypes;
    };

    enum DBT
    {
        DBT_UNKNOWN=0,
        DBT_FOLDER,
        DBT_COMPRESSED
    };

    explicit DiE_Script(QObject *parent=nullptr);
    bool loadDatabase(QString sDatabasePath);
    QString getDatabasePath();
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> *getSignatures();
    SCAN_RESULT scanFile(QString sFileName,SCAN_OPTIONS *pOptions);
    SCAN_RESULT scanDevice(QIODevice *pDevice,SCAN_OPTIONS *pOptions);
    DiE_ScriptEngine::SIGNATURE_RECORD getSignatureByFilePath(QString sSignatureFilePath);
    bool updateSignature(QString sSignatureFilePath,QString sText);
    STATS getStats();
    DBT getDatabaseType();
    bool isSignaturesPresent(XBinary::FT fileType);
    static QString scanResultToPlainString(SCAN_RESULT *pScanResult);
    static QString scanResultToJsonString(SCAN_RESULT *pScanResult);
    static QString scanResultToXmlString(SCAN_RESULT *pScanResult);
    static QString getErrorsString(SCAN_RESULT *pScanResult);

#ifdef QT_SCRIPTTOOLS_LIB
    void setDebugger(QScriptEngineDebugger *pDebugger);
#endif

public slots:
    void stop();

private:
    static QList<DiE_ScriptEngine::SIGNATURE_RECORD> _loadDatabasePath(QString sDatabasePath, XBinary::FT fileType);
    static QList<DiE_ScriptEngine::SIGNATURE_RECORD> _loadDatabaseFromZip(XZip *pZip, QList<XArchive::RECORD> *pListRecords, QString sPrefix, XBinary::FT fileType);
    SCAN_RESULT _scan(QIODevice *pDevice,XBinary::FT fileType,SCAN_OPTIONS *pOptions,QString sSignatureFilePath="");
    bool _handleError(DiE_ScriptEngine *pScriptEngine,QScriptValue scriptValue,DiE_ScriptEngine::SIGNATURE_RECORD *pSignatureRecord,SCAN_RESULT *pScanResult);
    SCAN_STRUCT getScanStructFromString(QIODevice *pDevice, SCAN_HEADER scanHeader, QString sString);

signals:
    void progressMaximumChanged(qint32 nMaximum);
    void progressValueChanged(qint32 nValue);
    void stopEngine();

private:
    QString sDatabasePath;
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> listSignatures;
    bool bIsStop;
    DBT databaseType;
#ifdef QT_SCRIPTTOOLS_LIB
    QScriptEngineDebugger *pDebugger;
#endif
};

#endif // DIE_SCRIPT_H
