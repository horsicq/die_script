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
#ifndef DIE_SCRIPTDATABASEUPDATE_H
#define DIE_SCRIPTDATABASEUPDATE_H

#include <QDir>
#include <QTemporaryDir>
// #include "xgithub.h"
#include "xzip.h"

class DiE_ScriptDatabaseUpdate : public QObject {
    Q_OBJECT

public:
    struct DATABASE_INFO {
        bool bValid;
        QString sTag;
        QDateTime dtPublished;
        QString sDbUrl;
        qint64 nDbSize;
        QString sDbExtraUrl;
        qint64 nDbExtraSize;
    };

    struct UPDATE_RESULT {
        bool bSuccess;
        QString sErrorString;
        bool bDbUpdated;
        bool bDbExtraUpdated;
    };

    explicit DiE_ScriptDatabaseUpdate(QObject *pParent = nullptr);

    DATABASE_INFO getRemoteDatabaseInfo();
    UPDATE_RESULT updateDatabaseMain(const QString &sDbPath);
    UPDATE_RESULT updateDatabaseExtra(const QString &sDbExtraPath);
    UPDATE_RESULT updateDatabases(const QString &sDbPath, const QString &sDbExtraPath);

signals:
    void errorMessage(QString sText);
    void infoMessage(QString sText);

private:
    bool _downloadAndExtract(const QString &sUrl, const QString &sTargetPath, QString *psError);
    static bool _removeDirectoryContents(const QString &sDirPath);

    static const QString S_GITHUB_USER;
    static const QString S_GITHUB_REPO;
    static const QString S_RELEASE_TAG;
    static const QString S_DB_ZIP_NAME;
    static const QString S_DB_EXTRA_ZIP_NAME;
};

#endif  // DIE_SCRIPTDATABASEUPDATE_H
