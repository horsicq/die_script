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
#include "die_script.h"

bool sort_signature_prio(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2)
{
    if (sr1.fileType != sr2.fileType) {
        return (sr1.fileType < sr2.fileType);
    }

    // if ((sr1.sName == "_init") && (sr2.sName == "_init")) {
    //     return false;
    // }

    // if (sr1.sName == "_init") {
    //     return true;
    // } else if (sr2.sName == "_init") {
    //     return false;
    // }

    QString sPrio1 = "9";
    QString sPrio2 = "9";

    qint32 nPos1 = sr1.sName.count(".");
    qint32 nPos2 = sr2.sName.count(".");

    if ((nPos1 > 1) && (nPos2 > 1)) {
        sPrio1 = sr1.sName.section(".", nPos1 - 1, nPos1 - 1);
        sPrio2 = sr2.sName.section(".", nPos2 - 1, nPos2 - 1);
    }

    if ((sPrio1 != "") && (sPrio2 != "")) {
        if (sPrio1 > sPrio2) {
            return false;
        } else if (sPrio1 < sPrio2) {
            return true;
        }
    }

    return (sr1.sName < sr2.sName);
}

bool sort_signature_name(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2)
{
    if ((sr1.sName == "_init") && (sr2.sName == "_init")) {
        return false;
    }

    if (sr1.sName == "_init") {
        return true;
    } else if (sr2.sName == "_init") {
        return false;
    }

    return (sr1.sName < sr2.sName);
}

DiE_Script::DiE_Script(QObject *pParent) : XScanEngine(pParent)
{
#ifdef QT_SCRIPTTOOLS_LIB
    m_pDebugger = nullptr;
#endif
}

DiE_Script::DiE_Script(const DiE_Script &other) : XScanEngine(other)
{
    m_listSignatures = other.m_listSignatures;
#ifdef QT_SCRIPTTOOLS_LIB
    m_pDebugger = other.m_pDebugger;
#endif
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> DiE_Script::_loadDatabaseFromPath(const QString &sDatabasePath, DiE_ScriptEngine::DT databaseType, XBinary::FT fileType,
                                                                            XBinary::PDSTRUCT *pPdStruct)
{
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> listResult;

    QDir dir(sDatabasePath);

    QFileInfoList eil = dir.entryInfoList();

    qint32 nNumberOfFiles = eil.count();

    for (qint32 i = 0; (i < nNumberOfFiles) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        if (eil.at(i).isFile()) {
            QString sSuffix = eil.at(i).suffix().toLower();

            if ((sSuffix == "sg") || (sSuffix == "")) {
                DiE_ScriptEngine::SIGNATURE_RECORD record = {};

                record.fileType = fileType;
                record.sName = eil.at(i).fileName();
                record.sText = XBinary::readFile(eil.at(i).absoluteFilePath());
                record.sFilePath = eil.at(i).absoluteFilePath();
                record.databaseType = databaseType;
                listResult.append(record);
            }
        }
    }

    return listResult;
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> DiE_Script::_loadDatabaseFromArchive(XArchive *pArchive, QList<XArchive::RECORD> *pListRecords,
                                                                               DiE_ScriptEngine::DT databaseType, const QString &sPrefix, XBinary::FT fileType)
{
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> listResult;

    qint32 nNumberOfRecords = pListRecords->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        XArchive::RECORD _record = pListRecords->at(i);

        if (((sPrefix == "") && (!_record.spInfo.sRecordName.contains("/"))) ||
            ((_record.spInfo.sRecordName.contains("/")) && (_record.spInfo.sRecordName.section("/", 0, 0) == sPrefix) &&
             (_record.spInfo.sRecordName.section("/", 1, 1) != ""))) {
            QFileInfo fi(_record.spInfo.sRecordName);

            DiE_ScriptEngine::SIGNATURE_RECORD record = {};

            record.fileType = fileType;
            record.sName = fi.fileName();
            record.sText = pArchive->decompress(&_record, nullptr);
            record.sFilePath = _record.spInfo.sRecordName;
            record.databaseType = databaseType;
            record.bReadOnly = true;

            listResult.append(record);
        }
    }

    return listResult;
}

void DiE_Script::processDetect(SCANID *pScanID, XScanEngine::SCAN_RESULT *pScanResult, QIODevice *pDevice, const SCANID &parentId, XBinary::FT fileType,
                               XScanEngine::SCAN_OPTIONS *pScanOptions, const QString &sSignatureFilePath, bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct)
{
    QString sDetectFunction = "detect";

    if (pScanOptions->sDetectFunction != "") {
        sDetectFunction = pScanOptions->sDetectFunction;
    }

    QList<DiE_ScriptEngine::SCAN_STRUCT> listRecords;
    XScanEngine::SCANID resultId = {};

    resultId.fileType = fileType;
    resultId.sUuid = XBinary::generateUUID();
    resultId.nOffset = XIODevice::getInitLocation(pDevice);
    resultId.nSize = pDevice->size();
    resultId.filePart = XBinary::FILEPART_HEADER;

    qint32 nNumberOfSignatures = m_listSignatures.count();

    DiE_ScriptEngine::SIGNATURE_RECORD srGlobalInit = {};
    DiE_ScriptEngine::SIGNATURE_RECORD srInit = {};

    bool bGlobalInit = false;
    bool bInit = false;

    for (qint32 i = 0; (i < nNumberOfSignatures) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        if (m_listSignatures.at(i).sName == "_init") {
            if (m_listSignatures.at(i).fileType == XBinary::FT_UNKNOWN) {
                srGlobalInit = m_listSignatures.at(i);
                bGlobalInit = true;
            }

            if (XBinary::checkFileType(m_listSignatures.at(i).fileType, fileType)) {
                srInit = m_listSignatures.at(i);
                bInit = true;
            }
        }

        if (bGlobalInit && bInit) {
            break;
        }
    }

    Binary_Script::OPTIONS _options = {};
    _options.bIsDeepScan = pScanOptions->bIsDeepScan;
    _options.bIsHeuristicScan = pScanOptions->bIsHeuristicScan;
    _options.bIsAggressiveScan = pScanOptions->bIsAggressiveScan;
    _options.bIsRecursiveScan = pScanOptions->bIsRecursiveScan;
    _options.bIsOverlayScan = pScanOptions->bIsOverlayScan;
    _options.bIsVerbose = pScanOptions->bIsVerbose;
    _options.bIsProfiling = pScanOptions->bLogProfiling;

    DiE_ScriptEngine scriptEngine(&m_listSignatures, &listRecords, pDevice, fileType, parentId.filePart, &_options, pPdStruct);

    connect(&scriptEngine, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
    connect(&scriptEngine, SIGNAL(warningMessage(QString)), this, SIGNAL(warningMessage(QString)));
    connect(&scriptEngine, SIGNAL(infoMessage(QString)), this, SIGNAL(infoMessage(QString)));

#ifdef QT_SCRIPTTOOLS_LIB
    if (m_pDebugger) {
        m_pDebugger->attachTo(&scriptEngine);
    }
#endif

    if (nNumberOfSignatures) {
        if (bGlobalInit) {
            _handleError(&scriptEngine, scriptEngine.evaluate(srGlobalInit.sText, srGlobalInit.sFilePath), &srGlobalInit, pScanResult);
        }

        if (bInit) {
            _handleError(&scriptEngine, scriptEngine.evaluate(srInit.sText, srInit.sFilePath), &srInit, pScanResult);
        }
    }

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nNumberOfSignatures);

    for (qint32 i = 0; (i < nNumberOfSignatures) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        DiE_ScriptEngine::SIGNATURE_RECORD signatureRecord = m_listSignatures.at(i);

        XBinary::setPdStructStatus(pPdStruct, _nFreeIndex, signatureRecord.sName);

        bool bExec = false;

        if (XBinary::checkFileType(signatureRecord.fileType, fileType)) {
            bExec = true;
        }

        if (bExec) {
            if (pScanOptions->sSignatureName != "") {
                if (pScanOptions->sSignatureName != signatureRecord.sName) {
                    bExec = false;
                }
            }
        }

        if (bExec) {
            if (!pScanOptions->bIsDeepScan) {
                QString sPrefix = signatureRecord.sName.section(".", 0, 0).toUpper();

                if ((sPrefix == "DS") || (sPrefix == "EP")) {
                    bExec = false;
                }
            }
        }

        if (bExec) {
            if (!pScanOptions->bIsHeuristicScan) {
                QString sPrefix = signatureRecord.sName.section(".", 0, 0).toUpper();

                if (sPrefix == "HEUR") {
                    bExec = false;
                }
            }
        }

        if (bExec) {
            if (sSignatureFilePath != "")  // TODO Check!
            {
                if (sSignatureFilePath != signatureRecord.sFilePath) {
                    bExec = false;
                }
            } else {
                if (signatureRecord.sName == "_init") {
                    bExec = false;
                }
            }
        }

        if (bExec) {
            if (signatureRecord.databaseType != DiE_ScriptEngine::DT_MAIN) {
                bool _bExec = false;
                if (pScanOptions->bUseCustomDatabase && (signatureRecord.databaseType == DiE_ScriptEngine::DT_CUSTOM)) {
                    _bExec = true;
                } else if (pScanOptions->bUseExtraDatabase && (signatureRecord.databaseType == DiE_ScriptEngine::DT_EXTRA)) {
                    _bExec = true;
                }

                bExec = _bExec;
            }
        }

        if (bExec) {
            if (pScanOptions->scanEngineCallback) {
                if (!pScanOptions->scanEngineCallback(signatureRecord.sName, nNumberOfSignatures, i, pScanOptions->pUserData)) {
                    XBinary::setPdStructStopped(pPdStruct);
                }
            }
        }

        if (bExec) {
            // scriptEngine.clearListLocalResult();
            if (pScanOptions->bLogProfiling) {
                emit warningMessage(QString("%1").arg(signatureRecord.sName));
            }

            QElapsedTimer *pElapsedTimer = nullptr;

            if ((pScanOptions->bShowScanTime) || (pScanOptions->bLogProfiling)) {
                pElapsedTimer = new QElapsedTimer;
                pElapsedTimer->start();
            }

            XSCRIPTVALUE script = scriptEngine.evaluateEx(parentId, resultId, signatureRecord.sText, signatureRecord.sName, signatureRecord.sFilePath);

            if (_handleError(&scriptEngine, script, &signatureRecord, pScanResult)) {
#ifdef QT_SCRIPTTOOLS_LIB
                if (m_pDebugger) {
                    m_pDebugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
                }
#endif
                XSCRIPTVALUE _scriptValue = scriptEngine.globalObject().property(sDetectFunction);

                if (_handleError(&scriptEngine, _scriptValue, &signatureRecord, pScanResult)) {
                    XSCRIPTVALUELIST valuelist;

                    if (sDetectFunction == "detect") {
                        valuelist << pScanOptions->bShowType << pScanOptions->bShowVersion << pScanOptions->bShowInfo;
                    }

#ifdef QT_SCRIPT_LIB
                    QScriptValue result = _scriptValue.call(script, valuelist);
#else
                    QJSValue result = _scriptValue.callWithInstance(script, valuelist);
#endif

                    if (_handleError(&scriptEngine, result, &signatureRecord, pScanResult)) {
                        // // TODO getResult
                        // QString sResult = result.toString();

                        // QList<DiE_ScriptEngine::RESULT> listLocalResult = scriptEngine.getListLocalResult();
                        // qint32 nNumberOfDetects = listLocalResult.count();

                        // if ((nNumberOfDetects == 0) && (sResult != "")) {
                        //     listLocalResult.append(DiE_ScriptEngine::stringToResult(sResult, pOptions->bShowType, pOptions->bShowVersion, pOptions->bShowOptions));
                        // }

                        // for (qint32 j = 0; j < nNumberOfDetects; j++) {
                        //     DiE_ScriptEngine::SCAN_STRUCT ssRecord = {};

                        //     // TODO IDs
                        //     ssRecord.id = resultId;
                        //     ssRecord.parentId = parentId;

                        //     ssRecord.sSignature = signatureRecord.sName;
                        //     ssRecord.sSignatureFileName = signatureRecord.sFilePath;
                        //     ssRecord.sType = listLocalResult.at(j).sType;
                        //     ssRecord.sName = listLocalResult.at(j).sName;
                        //     ssRecord.sVersion = listLocalResult.at(j).sVersion;
                        //     ssRecord.sOptions = listLocalResult.at(j).sOptions;
                        //     ssRecord.sFullString = QString("%1: %2(%3)[%4]").arg(ssRecord.sType, ssRecord.sName, ssRecord.sVersion, ssRecord.sOptions);
                        //     ssRecord.sResult = QString("%1(%2)[%3]").arg(ssRecord.sName, ssRecord.sVersion, ssRecord.sOptions);

                        //     listRecords.append(ssRecord);
                        // }
                    }
                }
            }

            if (pElapsedTimer) {
                qint64 nElapsedTime = pElapsedTimer->elapsed();
                delete pElapsedTimer;

                if (pScanOptions->bShowScanTime) {
                    XScanEngine::DEBUG_RECORD debugRecord = {};
                    debugRecord.sScript = signatureRecord.sName;
                    debugRecord.nElapsedTime = nElapsedTime;

                    // #ifdef QT_DEBUG
                    //                     qDebug("%s: %lld msec", debugRecord.sScript.toLatin1().data(), debugRecord.nElapsedTime);
                    // #endif
                    pScanResult->listDebugRecords.append(debugRecord);
                }

                if (pScanOptions->bLogProfiling) {
                    emit warningMessage(QString("%1: [%2 ms]").arg(signatureRecord.sName, QString::number(nElapsedTime)));
                }
            }
        }

        XBinary::setPdStructCurrentIncrement(pPdStruct, _nFreeIndex);
    }

    if (bAddUnknown) {
        if (listRecords.count() == 0) {
            DiE_ScriptEngine::SCAN_STRUCT ssRecord = {};

            ssRecord.id = resultId;
            ssRecord.parentId = parentId;
            ssRecord.sType = tr("Unknown");
            ssRecord.sName = tr("Unknown");
            ssRecord.bIsUnknown = true;

            listRecords.append(ssRecord);
        }
    }

    QList<XScanEngine::SCANSTRUCT> listScanStruct = convert(&listRecords);

    if (pScanOptions->bIsSort) {
        sortRecords(&listScanStruct);
    }

    pScanResult->listRecords.append(listScanStruct);

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    if (pScanID) {
        *pScanID = resultId;
    }
}

bool DiE_Script::_handleError(DiE_ScriptEngine *pScriptEngine, XSCRIPTVALUE scriptValue, DiE_ScriptEngine::SIGNATURE_RECORD *pSignatureRecord,
                              XScanEngine::SCAN_RESULT *pScanResult)
{
    bool bResult = false;

    QString sErrorString;
    QString sPrefix = QString("%1/%2").arg(XBinary::fileTypeIdToString(pSignatureRecord->fileType), pSignatureRecord->sName);
    if (pScriptEngine->handleError(sPrefix, scriptValue, &sErrorString)) {
        bResult = true;
    } else {
        XScanEngine::ERROR_RECORD errorRecord = {};
        errorRecord.sScript = pSignatureRecord->sName;
        errorRecord.sErrorString = sErrorString;

        pScanResult->listErrors.append(errorRecord);
    }

    return bResult;
}

void DiE_Script::_processDetect(SCANID *pScanID, SCAN_RESULT *pScanResult, QIODevice *pDevice, const SCANID &parentId, XBinary::FT fileType, SCAN_OPTIONS *pOptions,
                                bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct)
{
    processDetect(pScanID, pScanResult, pDevice, parentId, fileType, pOptions, "", bAddUnknown, pPdStruct);
}

void DiE_Script::initDatabase()
{
    m_listSignatures.clear();
}

bool DiE_Script::loadDatabase(const QString &sDatabasePath, DiE_ScriptEngine::DT databaseType, XBinary::PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    QString _sDatabasePath = sDatabasePath;

    if (_sDatabasePath == "") {
        if (databaseType == DiE_ScriptEngine::DT_MAIN) {
            _sDatabasePath = "$app/db";
        } else if (databaseType == DiE_ScriptEngine::DT_EXTRA) {
            _sDatabasePath = "$app/db_extra";
        } else if (databaseType == DiE_ScriptEngine::DT_CUSTOM) {
            _sDatabasePath = "$app/db_custom";
        }
    }

    if (_sDatabasePath != "") {
#ifdef QT_DEBUG
        QElapsedTimer *pElapsedTimer = new QElapsedTimer;
        pElapsedTimer->start();
#endif
        _sDatabasePath = XBinary::convertPathName(_sDatabasePath);

        if (XBinary::isFileExists(_sDatabasePath)) {
            // Load from zip
            // TODO load from tar(cache)
            QFile file;
            file.setFileName(_sDatabasePath);

            if (file.open(QIODevice::ReadOnly)) {
                XZip zip(&file);

                if (zip.isValid(pPdStruct)) {
                    QList<XArchive::RECORD> listRecords = zip.getRecords(-1, pPdStruct);  // TODO Check

                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "", XBinary::FT_UNKNOWN));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "Binary", XBinary::FT_BINARY));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "COM", XBinary::FT_COM));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "Archive", XBinary::FT_ARCHIVE));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "ZIP", XBinary::FT_ZIP));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "JAR", XBinary::FT_JAR));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "APK", XBinary::FT_APK));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "IPA", XBinary::FT_IPA));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "NPM", XBinary::FT_NPM));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "MACHOFAT", XBinary::FT_MACHOFAT));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "DEB", XBinary::FT_DEB));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "DEX", XBinary::FT_DEX));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "MSDOS", XBinary::FT_MSDOS));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "LE", XBinary::FT_LE));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "LX", XBinary::FT_LX));  // TODO Check
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "NE", XBinary::FT_NE));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "PE", XBinary::FT_PE));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "ELF", XBinary::FT_ELF));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "MACH", XBinary::FT_MACHO));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "DOS16M", XBinary::FT_DOS16M));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "DOS4G", XBinary::FT_DOS4G));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "Amiga", XBinary::FT_AMIGAHUNK));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "AtariST", XBinary::FT_ATARIST));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "JavaClass", XBinary::FT_JAVACLASS));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "PYC", XBinary::FT_PYC));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "PDF", XBinary::FT_PDF));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "CFBF", XBinary::FT_CFBF));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "Image", XBinary::FT_IMAGE));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "JPEG", XBinary::FT_JPEG));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "PNG", XBinary::FT_PNG));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "RAR", XBinary::FT_RAR));
                    m_listSignatures.append(_loadDatabaseFromArchive(&zip, &listRecords, databaseType, "ISO9660", XBinary::FT_ISO9660));

                    bResult = true;
                }

                file.close();
            }
        } else if (XBinary::isDirectoryExists(_sDatabasePath)) {
            // Load from directory
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath, databaseType, XBinary::FT_UNKNOWN, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "Binary", databaseType, XBinary::FT_BINARY, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "COM", databaseType, XBinary::FT_COM, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "Archive", databaseType, XBinary::FT_ARCHIVE, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "ZIP", databaseType, XBinary::FT_ZIP, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "JAR", databaseType, XBinary::FT_JAR, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "APK", databaseType, XBinary::FT_APK, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "IPA", databaseType, XBinary::FT_IPA, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "NPM", databaseType, XBinary::FT_NPM, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "MACHOFAT", databaseType, XBinary::FT_MACHOFAT, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "DEB", databaseType, XBinary::FT_DEB, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "DEX", databaseType, XBinary::FT_DEX, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "MSDOS", databaseType, XBinary::FT_MSDOS, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "LE", databaseType, XBinary::FT_LE, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "LX", databaseType, XBinary::FT_LX, pPdStruct));  // TODO Check
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "NE", databaseType, XBinary::FT_NE, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "PE", databaseType, XBinary::FT_PE, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "ELF", databaseType, XBinary::FT_ELF, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "MACH", databaseType, XBinary::FT_MACHO, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "DOS16M", databaseType, XBinary::FT_DOS16M, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "DOS4G", databaseType, XBinary::FT_DOS4G, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "Amiga", databaseType, XBinary::FT_AMIGAHUNK, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "AtariST", databaseType, XBinary::FT_ATARIST, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "JavaClass", databaseType, XBinary::FT_JAVACLASS, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "PYC", databaseType, XBinary::FT_PYC, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "PDF", databaseType, XBinary::FT_PDF, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "CFBF", databaseType, XBinary::FT_CFBF, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "Image", databaseType, XBinary::FT_IMAGE, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "JPEG", databaseType, XBinary::FT_JPEG, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "PNG", databaseType, XBinary::FT_PNG, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "RAR", databaseType, XBinary::FT_RAR, pPdStruct));
            m_listSignatures.append(_loadDatabaseFromPath(_sDatabasePath + QDir::separator() + "ISO9660", databaseType, XBinary::FT_ISO9660, pPdStruct));

            bResult = true;
        } else {
            if (databaseType == DiE_ScriptEngine::DT_MAIN) {
                QString sErrorString = QString("%1: %2").arg(tr("Cannot load database"), _sDatabasePath);

                emit errorMessage(sErrorString);
            }
        }

        std::sort(m_listSignatures.begin(), m_listSignatures.end(), sort_signature_prio);

#ifdef QT_DEBUG
        qDebug("DiE_Script::loadDatabase: %lld ms", pElapsedTimer->elapsed());
#endif
    }

    return bResult;
}

QList<DiE_Script::SIGNATURE_STATE> DiE_Script::getSignatureStates()
{
    QList<SIGNATURE_STATE> listResult;

    QList<XBinary::FT> listFT;

    listFT.append(XBinary::FT_BINARY);
    listFT.append(XBinary::FT_COM);
    listFT.append(XBinary::FT_MSDOS);
    listFT.append(XBinary::FT_NE);
    listFT.append(XBinary::FT_LE);
    listFT.append(XBinary::FT_LX);
    listFT.append(XBinary::FT_PE);
    listFT.append(XBinary::FT_ELF);
    listFT.append(XBinary::FT_MACHO);
    listFT.append(XBinary::FT_PDF);
    listFT.append(XBinary::FT_CFBF);
    listFT.append(XBinary::FT_IMAGE);
    listFT.append(XBinary::FT_JPEG);
    listFT.append(XBinary::FT_PNG);
    listFT.append(XBinary::FT_RAR);
    listFT.append(XBinary::FT_ISO9660);
    listFT.append(XBinary::FT_ARCHIVE);
    listFT.append(XBinary::FT_ZIP);
    listFT.append(XBinary::FT_JAR);
    listFT.append(XBinary::FT_APK);
    listFT.append(XBinary::FT_IPA);
    listFT.append(XBinary::FT_DEB);
    listFT.append(XBinary::FT_DEX);
    listFT.append(XBinary::FT_NPM);
    listFT.append(XBinary::FT_MACHOFAT);
    listFT.append(XBinary::FT_AMIGAHUNK);
    listFT.append(XBinary::FT_ATARIST);
    listFT.append(XBinary::FT_DOS16M);
    listFT.append(XBinary::FT_DOS4G);

    qint32 nNumberOfFileTypes = listFT.count();

    for (qint32 i = 0; i < nNumberOfFileTypes; i++) {
        SIGNATURE_STATE state = {};
        state.fileType = listFT.at(i);
        state.nNumberOfSignatures = getNumberOfSignatures(state.fileType);

        listResult.append(state);
    }

    return listResult;
}

qint32 DiE_Script::getNumberOfSignatures(XBinary::FT fileType)
{
    qint32 nResult = 0;

    qint32 nNumberOfSignatures = m_listSignatures.count();

    for (qint32 i = 0; (i < nNumberOfSignatures); i++) {
        if ((m_listSignatures.at(i).sName != "_init") && (XBinary::checkFileType(m_listSignatures.at(i).fileType, fileType))) {
            nResult++;
        }
    }

    return nResult;
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> *DiE_Script::getSignatures()
{
    return &m_listSignatures;
}

DiE_ScriptEngine::SIGNATURE_RECORD DiE_Script::getSignatureByFilePath(const QString &sSignatureFilePath)
{
    DiE_ScriptEngine::SIGNATURE_RECORD result = {};

    qint32 nNumberOfSignatures = m_listSignatures.count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        if (m_listSignatures.at(i).sFilePath == sSignatureFilePath) {
            result = m_listSignatures.at(i);

            break;
        }
    }

    return result;
}

bool DiE_Script::updateSignature(const QString &sSignatureFilePath, const QString &sText)
{
    bool bResult = false;

    qint32 nNumberOfSignatures = m_listSignatures.count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        if (m_listSignatures.at(i).sFilePath == sSignatureFilePath) {
            if (XBinary::writeToFile(sSignatureFilePath, QByteArray().append(sText.toUtf8()))) {
                m_listSignatures[i].sText = sText;
                bResult = true;
            }

            break;
        }
    }

    return bResult;
}

DiE_Script::STATS DiE_Script::getStats()
{
    STATS result = {};

    qint32 nNumberOfSignatures = m_listSignatures.count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        QString sText = m_listSignatures.at(i).sText;

        QString sType = XBinary::regExp("init\\(\"(.*?)\",", sText, 1);

        if (sType != "") {
            result.mapTypes.insert(sType, result.mapTypes.value(sType, 0) + 1);
        }
    }

    return result;
}

bool DiE_Script::isSignaturesPresent(XBinary::FT fileType)
{
    bool bResult = false;

    qint32 nNumberOfSignatures = m_listSignatures.count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        if (m_listSignatures.at(i).fileType == fileType) {
            bResult = true;

            break;
        }
    }

    return bResult;
}

QList<XScanEngine::SCANSTRUCT> DiE_Script::convert(QList<DiE_ScriptEngine::SCAN_STRUCT> *pListScanStructs)
{
    QList<XScanEngine::SCANSTRUCT> listResult;

    qint32 nNumberOfRecords = pListScanStructs->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        XScanEngine::SCANSTRUCT record = {};

        record.bIsHeuristic = isHeurType(pListScanStructs->at(i).sType);
        record.bIsAHeuristic = isAHeurType(pListScanStructs->at(i).sType);
        record.bIsUnknown = pListScanStructs->at(i).bIsUnknown;
        record.id = pListScanStructs->at(i).id;
        record.parentId = pListScanStructs->at(i).parentId;
        record.sType = pListScanStructs->at(i).sType;
        record.sName = pListScanStructs->at(i).sName;
        record.type = recordTypeStringToId(pListScanStructs->at(i).sType);
        record.name = recordNameStringToId(pListScanStructs->at(i).sName);
        record.sVersion = pListScanStructs->at(i).sVersion;
        record.sInfo = pListScanStructs->at(i).sOptions;
        record.varInfo = pListScanStructs->at(i).sSignature;
        record.varInfo2 = pListScanStructs->at(i).sSignatureFileName;
        // record.sResult = pListScanStructs->at(i).sResult;

        record.globalColorRecord = typeToGlobalColorRecord(record.sType);
        record.nPrio = typeToPrio(record.sType);
        record.bIsProtection = isProtection(record.sType);
        record.sType = translateType(record.sType);

        listResult.append(record);
    }

    // sortRecords(&listResult);

    return listResult;
}

bool DiE_Script::loadDatabaseFromGlobalOptions(XOptions *pXOptions)
{
    bool bResult = false;

    initDatabase();
    bResult = loadDatabase(pXOptions->getValue(XOptions::ID_SCAN_DATABASE_MAIN_PATH).toString(), DiE_ScriptEngine::DT_MAIN);
    loadDatabase(pXOptions->getValue(XOptions::ID_SCAN_DATABASE_EXTRA_PATH).toString(), DiE_ScriptEngine::DT_EXTRA);
    loadDatabase(pXOptions->getValue(XOptions::ID_SCAN_DATABASE_CUSTOM_PATH).toString(), DiE_ScriptEngine::DT_CUSTOM);

    return bResult;
}

#ifdef QT_SCRIPTTOOLS_LIB
void DiE_Script::setDebugger(QScriptEngineDebugger *pDebugger)
{
    this->m_pDebugger = pDebugger;
}
#endif

#ifdef QT_SCRIPTTOOLS_LIB
void DiE_Script::removeDebugger()
{
    this->m_pDebugger = nullptr;
}
#endif
