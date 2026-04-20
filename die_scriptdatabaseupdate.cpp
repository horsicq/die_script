/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
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
#include "die_scriptdatabaseupdate.h"
#include <QDirIterator>

const QString DiE_ScriptDatabaseUpdate::S_GITHUB_USER = "horsicq";
const QString DiE_ScriptDatabaseUpdate::S_GITHUB_REPO = "Detect-It-Easy";
const QString DiE_ScriptDatabaseUpdate::S_RELEASE_TAG = "current-database";
const QString DiE_ScriptDatabaseUpdate::S_DB_ZIP_NAME = "db.zip";
const QString DiE_ScriptDatabaseUpdate::S_DB_EXTRA_ZIP_NAME = "db_extra.zip";

DiE_ScriptDatabaseUpdate::DiE_ScriptDatabaseUpdate(QObject *pParent) : QObject(pParent)
{
}

DiE_ScriptDatabaseUpdate::DATABASE_INFO DiE_ScriptDatabaseUpdate::getRemoteDatabaseInfo()
{
    DATABASE_INFO result = {};

    XGitHub github(S_GITHUB_USER, S_GITHUB_REPO);
    connect(&github, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));

    XGitHub::RELEASE_HEADER releaseHeader = github.getTagRelease(S_RELEASE_TAG);

    if (releaseHeader.bValid) {
        result.bValid = true;
        result.sTag = releaseHeader.sTag;
        result.dtPublished = releaseHeader.dt;

        qint32 nCount = releaseHeader.listRecords.count();

        for (qint32 i = 0; i < nCount; i++) {
            XGitHub::RELEASE_RECORD record = releaseHeader.listRecords.at(i);

            if (record.sName == S_DB_ZIP_NAME) {
                result.sDbUrl = record.sSrc;
                result.nDbSize = record.nSize;
            } else if (record.sName == S_DB_EXTRA_ZIP_NAME) {
                result.sDbExtraUrl = record.sSrc;
                result.nDbExtraSize = record.nSize;
            }
        }
    } else {
        if (releaseHeader.bNetworkError) {
            emit errorMessage(tr("Network error while checking for database updates"));
        }
    }

    return result;
}

DiE_ScriptDatabaseUpdate::UPDATE_RESULT DiE_ScriptDatabaseUpdate::updateDatabaseMain(const QString &sDbPath)
{
    UPDATE_RESULT result = {};

    DATABASE_INFO dbInfo = getRemoteDatabaseInfo();

    if (!dbInfo.bValid) {
        result.sErrorString = tr("Failed to get remote database info");
        return result;
    }

    if (dbInfo.sDbUrl.isEmpty()) {
        result.sErrorString = tr("No %1 found in release").arg(QString("db.zip"));
        return result;
    }

    emit infoMessage(tr("Updating main database") + QString("..."));

    QString sError;

    if (_downloadAndExtract(dbInfo.sDbUrl, sDbPath, &sError)) {
        result.bSuccess = true;
        result.bDbUpdated = true;
        emit infoMessage(tr("Main database updated successfully"));
    } else {
        result.sErrorString = sError;
    }

    return result;
}

DiE_ScriptDatabaseUpdate::UPDATE_RESULT DiE_ScriptDatabaseUpdate::updateDatabaseExtra(const QString &sDbExtraPath)
{
    UPDATE_RESULT result = {};

    DATABASE_INFO dbInfo = getRemoteDatabaseInfo();

    if (!dbInfo.bValid) {
        result.sErrorString = tr("Failed to get remote database info");
        return result;
    }

    if (dbInfo.sDbExtraUrl.isEmpty()) {
        result.sErrorString = tr("No %1 found in release").arg(QString("db_extra.zip"));
        return result;
    }

    emit infoMessage(tr("Updating extra database") + QString("..."));

    QString sError;

    if (_downloadAndExtract(dbInfo.sDbExtraUrl, sDbExtraPath, &sError)) {
        result.bSuccess = true;
        result.bDbExtraUpdated = true;
        emit infoMessage(tr("Extra database updated successfully"));
    } else {
        result.sErrorString = sError;
    }

    return result;
}

DiE_ScriptDatabaseUpdate::UPDATE_RESULT DiE_ScriptDatabaseUpdate::updateDatabases(const QString &sDbPath, const QString &sDbExtraPath)
{
    UPDATE_RESULT result = {};

    DATABASE_INFO dbInfo = getRemoteDatabaseInfo();

    if (!dbInfo.bValid) {
        result.sErrorString = tr("Failed to get remote database info");
        return result;
    }

    result.bSuccess = true;

    if (!dbInfo.sDbUrl.isEmpty()) {
        emit infoMessage(tr("Updating main database") + QString("..."));

        QString sError;

        if (_downloadAndExtract(dbInfo.sDbUrl, sDbPath, &sError)) {
            result.bDbUpdated = true;
            emit infoMessage(tr("Main database updated successfully"));
        } else {
            result.bSuccess = false;
            result.sErrorString = sError;
        }
    }

    if (result.bSuccess && !dbInfo.sDbExtraUrl.isEmpty()) {
        emit infoMessage(tr("Updating extra database") + QString("..."));

        QString sError;

        if (_downloadAndExtract(dbInfo.sDbExtraUrl, sDbExtraPath, &sError)) {
            result.bDbExtraUpdated = true;
            emit infoMessage(tr("Extra database updated successfully"));
        } else {
            result.bSuccess = false;
            result.sErrorString = sError;
        }
    }

    return result;
}

bool DiE_ScriptDatabaseUpdate::_downloadAndExtract(const QString &sUrl, const QString &sTargetPath, QString *psError)
{
    QTemporaryDir tempDir;

    if (!tempDir.isValid()) {
        *psError = tr("Failed to create temporary directory");
        return false;
    }

    QString sTempZipPath = tempDir.path() + QDir::separator() + "download.zip";

    emit infoMessage(QString("Downloading: %1").arg(sUrl));

    if (!XGitHub::downloadFile(sUrl, sTempZipPath)) {
        *psError = tr("Failed to download file") + QString(": %1").arg(sUrl);
        return false;
    }

    QFileInfo fiZip(sTempZipPath);

    if (!fiZip.exists() || fiZip.size() == 0) {
        *psError = tr("Downloaded file is empty");
        return false;
    }

    emit infoMessage(QString("Downloaded %1 bytes").arg(fiZip.size()));

    // Extract to a temp subfolder first
    QString sTempExtractPath = tempDir.path() + QDir::separator() + "extracted";
    QDir().mkpath(sTempExtractPath);

    XBinary::PDSTRUCT pdStruct = XBinary::createPdStruct();

    {
        QFile zipFile(sTempZipPath);

        if (!zipFile.open(QIODevice::ReadOnly)) {
            *psError = tr("Failed to open downloaded zip file");
            return false;
        }

        XZip xzip(&zipFile);

        if (!xzip.isValid(&pdStruct)) {
            zipFile.close();
            *psError = tr("Downloaded file is not a valid ZIP archive");
            return false;
        }

        QMap<XArchive::UNPACK_PROP, QVariant> mapProperties;
        XArchive::UNPACK_STATE state = {};

        if (!xzip.initUnpack(&state, mapProperties, &pdStruct)) {
            zipFile.close();
            *psError = tr("Failed to initialize ZIP unpacking");
            return false;
        }

        while ((state.nCurrentIndex < state.nNumberOfRecords) && XBinary::isPdStructNotCanceled(&pdStruct)) {
            XBinary::ARCHIVERECORD archiveRecord = xzip.infoCurrent(&state, &pdStruct);

            QString sRecordName = archiveRecord.mapProperties.value(XBinary::FPART_PROP_ORIGINALNAME).toString();
            bool bIsFolder = archiveRecord.mapProperties.value(XBinary::FPART_PROP_ISFOLDER).toBool();

            if (!sRecordName.isEmpty() && !bIsFolder) {
                QString sResultFileName = sTempExtractPath + QDir::separator() + sRecordName;
                QDir().mkpath(QFileInfo(sResultFileName).absolutePath());

                QFile outFile(sResultFileName);

                if (outFile.open(QIODevice::WriteOnly)) {
                    xzip.unpackCurrent(&state, &outFile, &pdStruct);
                    outFile.close();
                }
            }

            if (!xzip.moveToNext(&state, &pdStruct)) {
                break;
            }
        }

        xzip.finishUnpack(&state, &pdStruct);
        zipFile.close();
    }

    // Check if any files were extracted
    QDirIterator itCheck(sTempExtractPath, QDir::Files, QDirIterator::Subdirectories);

    if (!itCheck.hasNext()) {
        *psError = tr("Failed to extract archive: no files extracted");
        return false;
    }

    // If the extracted folder contains a single subfolder, use its contents
    // (e.g., db.zip contains "db/" root folder)
    QString sSourceRoot = sTempExtractPath;
    QStringList listTopEntries = QDir(sTempExtractPath).entryList(QDir::NoDotAndDotDot | QDir::AllEntries);

    if (listTopEntries.count() == 1) {
        QString sSingleEntry = sTempExtractPath + QDir::separator() + listTopEntries.at(0);

        if (QFileInfo(sSingleEntry).isDir()) {
            sSourceRoot = sSingleEntry;
        }
    }

    // Remove old database contents
    if (XBinary::isDirectoryExists(sTargetPath)) {
        _removeDirectoryContents(sTargetPath);
    } else {
        QDir().mkpath(sTargetPath);
    }

    // Move extracted files to target
    QDirIterator it(sSourceRoot, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        QString sSourcePath = it.filePath();
        QString sRelativePath = QDir(sSourceRoot).relativeFilePath(sSourcePath);
        QString sDestPath = sTargetPath + QDir::separator() + sRelativePath;

        if (it.fileInfo().isDir()) {
            QDir().mkpath(sDestPath);
        } else {
            QDir().mkpath(QFileInfo(sDestPath).absolutePath());
            QFile::copy(sSourcePath, sDestPath);
        }
    }

    emit infoMessage(QString("Extracted to: %1").arg(sTargetPath));

    return true;
}

bool DiE_ScriptDatabaseUpdate::_removeDirectoryContents(const QString &sDirPath)
{
    bool bResult = true;

    QDir dir(sDirPath);
    QStringList listEntries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    qint32 nCount = listEntries.count();

    for (qint32 i = 0; i < nCount; i++) {
        QString sEntryPath = sDirPath + QDir::separator() + listEntries.at(i);
        QFileInfo fi(sEntryPath);

        if (fi.isDir()) {
            QDir(sEntryPath).removeRecursively();
        } else {
            QFile::remove(sEntryPath);
        }
    }

    return bResult;
}
