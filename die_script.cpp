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

    SIGNATURE_RECORD srGlobalInit = {};
    SIGNATURE_RECORD srInit = {};

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
    _options.bIsResourcesScan = pScanOptions->bIsResourcesScan;
    _options.bIsArchivesScan = pScanOptions->bIsArchivesScan;
    _options.bIsOverlayScan = pScanOptions->bIsOverlayScan;
    _options.bIsVerbose = pScanOptions->bIsVerbose;
    _options.bIsProfiling = pScanOptions->bLogProfiling;
    _options.sScanID = pScanOptions->sScanID;

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
        SIGNATURE_RECORD signatureRecord = m_listSignatures.at(i);

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
            if (signatureRecord.databaseType != DT_MAIN) {
                bool _bExec = false;
                if (pScanOptions->bUseCustomDatabase && (signatureRecord.databaseType == DT_CUSTOM)) {
                    _bExec = true;
                } else if (pScanOptions->bUseExtraDatabase && (signatureRecord.databaseType == DT_EXTRA)) {
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

bool DiE_Script::_handleError(DiE_ScriptEngine *pScriptEngine, XSCRIPTVALUE scriptValue, SIGNATURE_RECORD *pSignatureRecord, XScanEngine::SCAN_RESULT *pScanResult)
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

QString DiE_Script::getEngineName()
{
    return QString("die");
}

void DiE_Script::_processDetect(SCANID *pScanID, SCAN_RESULT *pScanResult, QIODevice *pDevice, const SCANID &parentId, XBinary::FT fileType, SCAN_OPTIONS *pOptions,
                                bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct)
{
    processDetect(pScanID, pScanResult, pDevice, parentId, fileType, pOptions, "", bAddUnknown, pPdStruct);
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
    bResult = loadDatabase(pXOptions->getValue(XOptions::ID_SCAN_DATABASE_MAIN_PATH).toString(), DT_MAIN);
    loadDatabase(pXOptions->getValue(XOptions::ID_SCAN_DATABASE_EXTRA_PATH).toString(), DT_EXTRA);
    loadDatabase(pXOptions->getValue(XOptions::ID_SCAN_DATABASE_CUSTOM_PATH).toString(), DT_CUSTOM);

    return bResult;
}

bool DiE_Script::isSignatureValid(const QString &sSignatureFilePath)
{
    bool bResult = false;

    QFileInfo fileInfo(sSignatureFilePath);

    if (fileInfo.isFile()) {
        QString sExt = fileInfo.suffix().toLower();

        if ((sExt == "sg") || (sExt == "")) {
            bResult = true;
        }
    }

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
