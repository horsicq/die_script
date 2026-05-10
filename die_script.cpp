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

#include <QElapsedTimer>

namespace {
const char *const g_sDefaultDetectFunction = "detect";
const char *const g_sInitSignatureName = "_init";

struct INIT_SIGNATURES {
    bool bHasGlobalInit = false;
    XScanEngine::SIGNATURE_RECORD srGlobalInit = {};
    bool bHasTypeInit = false;
    XScanEngine::SIGNATURE_RECORD srTypeInit = {};
};

QString getDetectFunctionName(const XScanEngine::SCAN_OPTIONS *pScanOptions)
{
    if ((pScanOptions == nullptr) || pScanOptions->sDetectFunction.isEmpty()) {
        return QString::fromLatin1(g_sDefaultDetectFunction);
    }

    return pScanOptions->sDetectFunction;
}

QString getSignaturePrefix(const QString &sSignatureName)
{
    return sSignatureName.section(".", 0, 0).toUpper();
}

bool isDeepScanSignature(const XScanEngine::SIGNATURE_RECORD &signatureRecord)
{
    const QString sPrefix = getSignaturePrefix(signatureRecord.sName);

    return (sPrefix == QLatin1String("DS")) || (sPrefix == QLatin1String("EP"));
}

bool isHeuristicSignature(const XScanEngine::SIGNATURE_RECORD &signatureRecord)
{
    return (getSignaturePrefix(signatureRecord.sName) == QLatin1String("HEUR"));
}

bool shouldMeasureElapsedTime(const XScanEngine::SCAN_OPTIONS *pScanOptions)
{
    return (pScanOptions != nullptr) && (pScanOptions->bShowScanTime || pScanOptions->bLogProfiling);
}

XScanEngine::SCANID createResultId(QIODevice *pDevice, const XScanEngine::SCANID &parentId, XBinary::FT fileType)
{
    XScanEngine::SCANID resultId = {};

    resultId.fileType = fileType;
    resultId.sUuid = XBinary::generateUUID();
    resultId.nOffset = XIODevice::getInitLocation(pDevice);
    resultId.nSize = pDevice->size();
    resultId.filePart = parentId.filePart;

    return resultId;
}

INIT_SIGNATURES findInitSignatures(const QList<XScanEngine::SIGNATURE_RECORD> &listSignatures, XBinary::FT fileType, XBinary::PDSTRUCT *pPdStruct)
{
    INIT_SIGNATURES result = {};
    const qint32 nNumberOfSignatures = listSignatures.count();

    for (qint32 i = 0; (i < nNumberOfSignatures) && ((pPdStruct == nullptr) || XBinary::isPdStructNotCanceled(pPdStruct)); i++) {
        const XScanEngine::SIGNATURE_RECORD &signatureRecord = listSignatures.at(i);

        if (signatureRecord.sName != QLatin1String(g_sInitSignatureName)) {
            continue;
        }

        if (signatureRecord.fileType == XBinary::FT_UNKNOWN) {
            result.srGlobalInit = signatureRecord;
            result.bHasGlobalInit = true;
        }

        if (XBinary::checkFileType(signatureRecord.fileType, fileType)) {
            result.srTypeInit = signatureRecord;
            result.bHasTypeInit = true;
        }

        if (result.bHasGlobalInit && result.bHasTypeInit) {
            break;
        }
    }

    return result;
}
}  // namespace

DiE_Script::DiE_Script(QObject *pParent) : XScanEngine(pParent)
{
}

void DiE_Script::copySignaturesFrom(const DiE_Script &other)
{
    m_listSignatures = other.m_listSignatures;
}

void DiE_Script::processDetect(SCANID *pScanID, XScanEngine::SCAN_RESULT *pScanResult, QIODevice *pDevice, const SCANID &parentId, XBinary::FT fileType,
                               XScanEngine::SCAN_OPTIONS *pScanOptions, const QString &sSignatureFilePath, bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct)
{
    const QString sDetectFunction = getDetectFunctionName(pScanOptions);
    QList<SCANSTRUCT> listRecords;
    const XScanEngine::SCANID resultId = createResultId(pDevice, parentId, fileType);
    const qint32 nNumberOfSignatures = m_listSignatures.count();
    const INIT_SIGNATURES initSignatures = findInitSignatures(m_listSignatures, fileType, pPdStruct);

    DiE_ScriptEngine scriptEngine(&m_listSignatures, &listRecords, pDevice, fileType, parentId.filePart, pScanOptions, pPdStruct);

    connect(&scriptEngine, &DiE_ScriptEngine::errorMessage, this, &DiE_Script::errorMessage);
    connect(&scriptEngine, &DiE_ScriptEngine::warningMessage, this, &DiE_Script::warningMessage);
    connect(&scriptEngine, &DiE_ScriptEngine::infoMessage, this, &DiE_Script::infoMessage);

#ifdef QT_SCRIPTTOOLS_LIB
    if (m_pDebugger) {
        m_pDebugger->attachTo(&scriptEngine);
    }
#endif

    if (nNumberOfSignatures) {
        if (initSignatures.bHasGlobalInit) {
            _executeInitSignature(&scriptEngine, initSignatures.srGlobalInit, pScanResult);
        }

        if (initSignatures.bHasTypeInit) {
            _executeInitSignature(&scriptEngine, initSignatures.srTypeInit, pScanResult);
        }
    }

    const qint32 nProgressIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, nProgressIndex, nNumberOfSignatures);

    for (qint32 i = 0; (i < nNumberOfSignatures) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        const SIGNATURE_RECORD &signatureRecord = m_listSignatures.at(i);

        XBinary::setPdStructStatus(pPdStruct, nProgressIndex, signatureRecord.sName);

        const bool bExec = _shouldExecuteSignature(signatureRecord, fileType, pScanOptions, sSignatureFilePath);

        if (bExec && pScanOptions->scanEngineCallback) {
            if (!pScanOptions->scanEngineCallback(signatureRecord.sName, nNumberOfSignatures, i, pScanOptions->pUserData)) {
                XBinary::setPdStructStopped(pPdStruct);
            }
        }

        if (bExec) {
            _executeSignature(&scriptEngine, sDetectFunction, signatureRecord, parentId, resultId, pScanResult, pScanOptions);
        }

        if (scriptEngine.isStopped()) {
            break;
        }

        XBinary::setPdStructCurrentIncrement(pPdStruct, nProgressIndex);
    }

    if (bAddUnknown && listRecords.isEmpty()) {
        XScanEngine::SCANSTRUCT scanStruct = {};

        scanStruct.id = resultId;
        scanStruct.parentId = parentId;
        scanStruct.sType = tr("Unknown");
        scanStruct.sName = tr("Unknown");
        scanStruct.bIsUnknown = true;

        listRecords.append(scanStruct);
    }

    // QList<XScanEngine::SCANSTRUCT> listScanStruct = convert(&listRecords);

    if (pScanOptions->bIsSort) {
        sortRecords(&listRecords);
    }

    pScanResult->listRecords.append(listRecords);

    XBinary::setPdStructFinished(pPdStruct, nProgressIndex);

    if (pScanID) {
        *pScanID = resultId;
    }
}

bool DiE_Script::_handleError(DiE_ScriptEngine *pScriptEngine, XSCRIPTVALUE scriptValue, const SIGNATURE_RECORD *pSignatureRecord, XScanEngine::SCAN_RESULT *pScanResult)
{
    bool bResult = false;

    QString sErrorString;
    const QString sPrefix = QString("%1/%2").arg(XBinary::fileTypeIdToString(pSignatureRecord->fileType)).arg(pSignatureRecord->sName);
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

bool DiE_Script::_shouldExecuteSignature(const SIGNATURE_RECORD &signatureRecord, XBinary::FT fileType, const SCAN_OPTIONS *pScanOptions,
                                         const QString &sSignatureFilePath) const
{
    if (pScanOptions == nullptr) {
        return false;
    }

    if (!XBinary::checkFileType(signatureRecord.fileType, fileType)) {
        return false;
    }

    if (!pScanOptions->sSignatureName.isEmpty() && (pScanOptions->sSignatureName != signatureRecord.sName)) {
        return false;
    }

    if (!pScanOptions->bIsDeepScan && isDeepScanSignature(signatureRecord)) {
        return false;
    }

    if (!pScanOptions->bIsHeuristicScan && isHeuristicSignature(signatureRecord)) {
        return false;
    }

    if (!sSignatureFilePath.isEmpty()) {
        if (sSignatureFilePath != signatureRecord.sFilePath) {
            return false;
        }
    } else if (signatureRecord.sName == QLatin1String(g_sInitSignatureName)) {
        return false;
    }

    if (signatureRecord.databaseType == DT_MAIN) {
        return true;
    }

    if (pScanOptions->bUseCustomDatabase && (signatureRecord.databaseType == DT_CUSTOM)) {
        return true;
    }

    if (pScanOptions->bUseExtraDatabase && (signatureRecord.databaseType == DT_EXTRA)) {
        return true;
    }

    return false;
}

void DiE_Script::_executeInitSignature(DiE_ScriptEngine *pScriptEngine, const SIGNATURE_RECORD &signatureRecord, SCAN_RESULT *pScanResult)
{
    _handleError(pScriptEngine, pScriptEngine->evaluate(signatureRecord.sText, signatureRecord.sFilePath), &signatureRecord, pScanResult);
}

void DiE_Script::_executeSignature(DiE_ScriptEngine *pScriptEngine, const QString &sDetectFunction, const SIGNATURE_RECORD &signatureRecord, const SCANID &parentId,
                                   const SCANID &resultId, SCAN_RESULT *pScanResult, SCAN_OPTIONS *pScanOptions)
{
    if ((pScriptEngine == nullptr) || (pScanResult == nullptr) || (pScanOptions == nullptr)) {
        return;
    }

    if (pScanOptions->bLogProfiling) {
        emit warningMessage(signatureRecord.sName);
    }

    QElapsedTimer elapsedTimer;
    const bool bMeasureTime = shouldMeasureElapsedTime(pScanOptions);

    if (bMeasureTime) {
        elapsedTimer.start();
    }

    XSCRIPTVALUE scriptObject = pScriptEngine->evaluateEx(parentId, resultId, signatureRecord.sText, signatureRecord.sName, signatureRecord.sFilePath);

    if (_handleError(pScriptEngine, scriptObject, &signatureRecord, pScanResult)) {
#ifdef QT_SCRIPTTOOLS_LIB
        if (m_pDebugger) {
            m_pDebugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
        }
#endif
        XSCRIPTVALUE scriptFunction = pScriptEngine->globalObject().property(sDetectFunction);

        if (_handleError(pScriptEngine, scriptFunction, &signatureRecord, pScanResult)) {
            XSCRIPTVALUELIST valueList;

            if (sDetectFunction == QLatin1String(g_sDefaultDetectFunction)) {
                valueList << pScanOptions->bShowType << pScanOptions->bShowVersion << pScanOptions->bShowInfo;
            }

#ifdef QT_SCRIPT_LIB
            QScriptValue result = scriptFunction.call(scriptObject, valueList);
#else
            QJSValue result = scriptFunction.callWithInstance(scriptObject, valueList);
#endif

            _handleError(pScriptEngine, result, &signatureRecord, pScanResult);
        }
    }

    if (bMeasureTime) {
        _handleElapsedTime(signatureRecord, elapsedTimer.elapsed(), pScanResult, pScanOptions);
    }
}

void DiE_Script::_handleElapsedTime(const SIGNATURE_RECORD &signatureRecord, qint64 nElapsedTime, SCAN_RESULT *pScanResult, const SCAN_OPTIONS *pScanOptions)
{
    if ((pScanResult == nullptr) || (pScanOptions == nullptr)) {
        return;
    }

    if (pScanOptions->bShowScanTime) {
        XScanEngine::DEBUG_RECORD debugRecord = {};
        debugRecord.sScript = signatureRecord.sName;
        debugRecord.nElapsedTime = nElapsedTime;

        pScanResult->listDebugRecords.append(debugRecord);
    }

    if (pScanOptions->bLogProfiling) {
        emit warningMessage(QString("%1: [%2 ms]").arg(signatureRecord.sName).arg(QString::number(nElapsedTime)));
    }
}

QString DiE_Script::getEngineName()
{
    return QStringLiteral("die");
}

XScanEngine::SCANENGINETYPE DiE_Script::getEngineType()
{
    return SCANENGINETYPE_DIE;
}

void DiE_Script::_processDetect(SCANID *pScanID, SCAN_RESULT *pScanResult, QIODevice *pDevice, const SCANID &parentId, XBinary::FT fileType, SCAN_OPTIONS *pOptions,
                                bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct)
{
    processDetect(pScanID, pScanResult, pDevice, parentId, fileType, pOptions, "", bAddUnknown, pPdStruct);
}

// bool DiE_Script::loadDatabaseFromGlobalOptions(XOptions *pXOptions)
// {
//     bool bResult = false;

//     initDatabase();
//     bResult = loadDatabase(pXOptions->getValue(XOptions::ID_SCAN_DATABASE_MAIN_PATH).toString(), DT_MAIN);
//     loadDatabase(pXOptions->getValue(XOptions::ID_SCAN_DATABASE_EXTRA_PATH).toString(), DT_EXTRA);
//     loadDatabase(pXOptions->getValue(XOptions::ID_SCAN_DATABASE_CUSTOM_PATH).toString(), DT_CUSTOM);

//     return bResult;
// }

bool DiE_Script::isDatabaseUsing()
{
    return true;
}

bool DiE_Script::isSignatureFileValid(const QString &sSignatureFilePath)
{
    const QFileInfo fileInfo(sSignatureFilePath);

    if (!fileInfo.isFile()) {
        return false;
    }

    const QString sExt = fileInfo.suffix().toLower();

    return (sExt == QLatin1String("sg")) || sExt.isEmpty();
}

#ifdef QT_SCRIPTTOOLS_LIB
void DiE_Script::setDebugger(QScriptEngineDebugger *pDebugger)
{
    m_pDebugger = pDebugger;
}
#endif

#ifdef QT_SCRIPTTOOLS_LIB
void DiE_Script::removeDebugger()
{
    m_pDebugger = nullptr;
}
#endif
