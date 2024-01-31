/* Copyright (c) 2019-2023 hors<horsicq@gmail.com>
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
#include "die_scriptengine.h"

DiE_ScriptEngine::DiE_ScriptEngine(QList<DiE_ScriptEngine::SIGNATURE_RECORD> *pSignaturesList, QList<SCAN_STRUCT> *pListScanStructs, QIODevice *pDevice,
                                   XBinary::FT fileType, Binary_Script::OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct)
    : XScriptEngine()
{
    g_pSignaturesList = pSignaturesList;
    g_pListScanStructs = pListScanStructs;
    g_pPdStruct = pPdStruct;

#ifdef QT_SCRIPT_LIB
    _addFunction(includeScript, "includeScript");
    _addFunction(_log, "_log");
    _addFunction(_setResult, "_setResult");
    _addFunction(_isResultPresent, "_isResultPresent");
    _addFunction(_getNumberOfResults, "_getNumberOfResults");
    _addFunction(_removeResult, "_removeResult");
    _addFunction(_isStop, "_isStop");
    _addFunction(_encodingList, "_encodingList");
#else
    connect(&g_globalScript, SIGNAL(includeScriptSignal(QString)), this, SLOT(includeScriptSlot(QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_logSignal(QString)), this, SLOT(_logSlot(QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_setResultSignal(QString, QString, QString, QString)), this, SLOT(_setResultSlot(QString, QString, QString, QString)),
            Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_isResultPresentSignal(bool *, QString, QString)), this, SLOT(_isResultPresentSlot(bool *, QString, QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_getNumberOfResultsSignal(qint32 *, QString)), this, SLOT(_getNumberOfResultsSlot(qint32 *, QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_removeResultSignal(QString)), this, SLOT(_removeResultSlot(QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_isStopSignal(bool *)), this, SLOT(_isStopSlot(bool *)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_encodingListSignal()), this, SLOT(_encodingListSlot()), Qt::DirectConnection);

    QJSValue valueGlobalScript = newQObject(&g_globalScript);
    globalObject().setProperty("includeScript", valueGlobalScript.property("includeScript"));
    globalObject().setProperty("_log", valueGlobalScript.property("_log"));
    globalObject().setProperty("_setResult", valueGlobalScript.property("_setResult"));
    globalObject().setProperty("_isResultPresent", valueGlobalScript.property("_isResultPresent"));
    globalObject().setProperty("_getNumberOfResults", valueGlobalScript.property("_getNumberOfResults"));
    globalObject().setProperty("_removeResult", valueGlobalScript.property("_removeResult"));
    globalObject().setProperty("_isStop", valueGlobalScript.property("_isStop"));
    globalObject().setProperty("_encodingList", valueGlobalScript.property("_encodingList"));
#endif

    g_pBinary = 0;
    g_pBinaryScript = 0;

    g_pExtra = 0;
    g_pExtraScript = 0;

    QSet<XBinary::FT> fileTypes = XBinary::getFileTypes(pDevice, true);

    if (fileTypes.contains(XBinary::FT_JPEG)) {
        g_pBinary = new XJpeg(pDevice);
    } else {
        g_pBinary = new XBinary(pDevice);
    }

    g_pBinaryScript = new Binary_Script(g_pBinary, pOptions, pPdStruct);
    _addClass(g_pBinaryScript, "Binary");

    if (XBinary::checkFileType(XBinary::FT_COM, fileType)) {
        XCOM *pCOM = new XCOM(pDevice);
        g_pExtraScript = new COM_Script(pCOM, pOptions, pPdStruct);
        _addClass(g_pExtraScript, "COM");
        g_pExtra = pCOM;
    } else if (XBinary::checkFileType(XBinary::FT_PE, fileType)) {
        XPE *pPE = new XPE(pDevice);
        g_pExtraScript = new PE_Script(pPE, pOptions, pPdStruct);
        _addClass(g_pExtraScript, "PE");
        g_pExtra = pPE;
    } else if (XBinary::checkFileType(XBinary::FT_ELF, fileType)) {
        XELF *pELF = new XELF(pDevice);
        g_pExtraScript = new ELF_Script(pELF, pOptions, pPdStruct);
        _addClass(g_pExtraScript, "ELF");
        g_pExtra = pELF;
    } else if (XBinary::checkFileType(XBinary::FT_MACHO, fileType)) {
        XMACH *pMACH = new XMACH(pDevice);
        g_pExtraScript = new MACH_Script(pMACH, pOptions, pPdStruct);
        _addClass(g_pExtraScript, "MACH");
        g_pExtra = pMACH;
    } else if (XBinary::checkFileType(XBinary::FT_NE, fileType)) {
        XNE *pNE = new XNE(pDevice);
        g_pExtraScript = new NE_Script(pNE, pOptions, pPdStruct);
        _addClass(g_pExtraScript, "NE");
        g_pExtra = pNE;
    } else if (XBinary::checkFileType(XBinary::FT_LE, fileType)) {
        XLE *pLE = new XLE(pDevice);
        g_pExtraScript = new LE_Script(pLE, pOptions, pPdStruct);
        _addClass(g_pExtraScript, "LE");
        g_pExtra = pLE;
    } else if (XBinary::checkFileType(XBinary::FT_LX, fileType)) {
        XLE *pLE = new XLE(pDevice);
        g_pExtraScript = new LX_Script(pLE, pOptions, pPdStruct);
        _addClass(g_pExtraScript, "LX");
        g_pExtra = pLE;
    } else if (XBinary::checkFileType(XBinary::FT_MSDOS, fileType)) {
        XMSDOS *pXMSDOS = new XMSDOS(pDevice);
        g_pExtraScript = new MSDOS_Script(pXMSDOS, pOptions, pPdStruct);
        _addClass(g_pExtraScript, "MSDOS");
        g_pExtra = pXMSDOS;
    }
    //    else if(XBinary::checkFileType(XBinary::FT_JAR,fileType))
    //    {
    //        XZip *pZIP=new XZip(pDevice);
    //        g_pBinaryScript=new JAR_Script(pZIP);
    //        _addClass(g_pBinaryScript,"JAR");
    //        g_pBinary=pZIP;
    //    }
    //    else if(XBinary::checkFileType(XBinary::FT_APK,fileType))
    //    {
    //        XZip *pZIP=new XZip(pDevice);
    //        g_pBinaryScript=new APK_Script(pZIP);
    //        _addClass(g_pBinaryScript,"APK");
    //        g_pBinary=pZIP;
    //    }
    //    else if(XBinary::checkFileType(XBinary::FT_IPA,fileType))
    //    {
    //        XZip *pZIP=new XZip(pDevice);
    //        g_pBinaryScript=new IPA_Script(pZIP);
    //        _addClass(g_pBinaryScript,"IPA");
    //        g_pBinary=pZIP;
    //    }
    //    if(XBinary::checkFileType(XBinary::FT_DEX,fileType))
    //    {
    //        XDEX *pDEX=new XDEX(pDevice);
    //        g_pBinaryScript=new DEX_Script(pDEX);
    //        _addClass(g_pBinaryScript,"DEX");
    //        g_pBinary=pDEX;
    //    }

    if (g_pBinaryScript) {
        connect(g_pBinaryScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
    }

    if (g_pExtraScript) {
        connect(g_pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
    }
}

DiE_ScriptEngine::~DiE_ScriptEngine()
{
    if (g_pBinary) delete g_pBinary;
    if (g_pExtra) delete g_pExtra;
    if (g_pBinaryScript) delete g_pBinaryScript;
    if (g_pExtraScript) delete g_pExtraScript;
}

bool DiE_ScriptEngine::handleError(XSCRIPTVALUE value, QString *psErrorString)
{
    bool bResult = true;

    if (value.isError()) {
        // TODO Check more information
        *psErrorString = QString("%1: %2").arg(value.property("lineNumber").toString(), value.toString());

        bResult = false;
    }

    return bResult;
}

QList<DiE_ScriptEngine::RESULT> DiE_ScriptEngine::getListLocalResult()
{
    return g_listResult;
}

void DiE_ScriptEngine::clearListLocalResult()
{
    g_listResult.clear();
}

#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::includeScript(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        pEngine->currentContext()->setActivationObject(pEngine->currentContext()->parentContext()->activationObject());

        QString sScript = pContext->argument(0).toString();

        pScriptEngine->includeScriptSlot(sScript);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_log(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sText = pContext->argument(0).toString();

        pScriptEngine->_logSlot(sText);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_setResult(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sType = pContext->argument(0).toString();
        QString sName = pContext->argument(1).toString();
        QString sVersion = pContext->argument(2).toString();
        QString sOptions = pContext->argument(3).toString();

        pScriptEngine->_setResultSlot(sType, sName, sVersion, sOptions);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_isResultPresent(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sType = pContext->argument(0).toString();
        QString sName = pContext->argument(1).toString();
        bool bResult = false;

        pScriptEngine->_isResultPresentSlot(&bResult, sType, sName);

        result = bResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_getNumberOfResults(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sType = pContext->argument(0).toString();
        qint32 nResult = 0;

        pScriptEngine->_getNumberOfResultsSlot(&nResult, sType);

        result = nResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_removeResult(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sType = pContext->argument(0).toString();
        QString sName = pContext->argument(1).toString();

        pScriptEngine->_removeResultSlot(sType, sName);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_isStop(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        bool bResult = false;

        pScriptEngine->_isStopSlot(&bResult);

        result = bResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_encodingList(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        bool bResult = false;

        pScriptEngine->_encodingListSlot();

        result = bResult;
    }

    return result;
}
#endif

void DiE_ScriptEngine::includeScriptSlot(const QString &sScript)
{
    // TODO error, cannot find signature
    qint32 nNumberOfSignatures = g_pSignaturesList->count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        if (g_pSignaturesList->at(i).fileType == XBinary::FT_UNKNOWN) {
            if (g_pSignaturesList->at(i).sName == sScript) {
                // TODO error
                evaluate(g_pSignaturesList->at(i).sText, sScript);

                break;
            }
        }
    }
}

void DiE_ScriptEngine::_logSlot(const QString &sText)
{
    emit infoMessage(sText);
}

void DiE_ScriptEngine::_setResultSlot(const QString &sType, const QString &sName, const QString &sVersion, const QString &sOptions)
{
    RESULT record = {};
    record.sType = sType;
    record.sName = sName;
    record.sVersion = sVersion;
    record.sOptions = sOptions;

    g_listResult.append(record);
}

void DiE_ScriptEngine::_isResultPresentSlot(bool *pbResult, const QString &sType, const QString &sName)
{
    *pbResult = false;

    qint32 nNumberOfResults = g_pListScanStructs->count();

    for (qint32 i = 0; i < nNumberOfResults; i++) {
        if ((g_pListScanStructs->at(i).sType.toUpper() == sType.toUpper()) && ((g_pListScanStructs->at(i).sName.toUpper() == sName.toUpper()) || (sName == ""))) {
            *pbResult = true;
            break;
        }
    }
}

void DiE_ScriptEngine::_getNumberOfResultsSlot(qint32 *pnResult, const QString &sType)
{
    *pnResult = 0;

    qint32 nNumberOfResults = g_pListScanStructs->count();

    for (qint32 i = 0; i < nNumberOfResults; i++) {
        if ((g_pListScanStructs->at(i).sType.toUpper() == sType.toUpper()) || (sType == "")) {
            (*pnResult)++;
        }
    }
}

void DiE_ScriptEngine::_removeResultSlot(const QString &sType, const QString &sName)
{
    qint32 nNumberOfResults = g_pListScanStructs->count();

    for (qint32 i = 0; i < nNumberOfResults; i++) {
        if ((g_pListScanStructs->at(i).sType.toUpper() == sType.toUpper()) && (g_pListScanStructs->at(i).sName.toUpper() == sName.toUpper())) {
            g_pListScanStructs->removeAt(i);
            break;
        }
    }
}

void DiE_ScriptEngine::_isStopSlot(bool *pResult)
{
    *pResult = (g_pPdStruct->bIsStop);
}

void DiE_ScriptEngine::_encodingListSlot()
{
    QList<QString> listCodePages = XOptions::getCodePages(false);

    qint32 nNumberOfCodePages = listCodePages.count();

    for (qint32 i = 0; i < nNumberOfCodePages; i++) {
        emit infoMessage(listCodePages.at(i));
    }
}

DiE_ScriptEngine::RESULT DiE_ScriptEngine::stringToResult(const QString &sString, bool bShowType, bool bShowVersion, bool bShowOptions)
{
    QString sStringTmp = sString;
    RESULT result = {};

    if (bShowType) {
        result.sType = sStringTmp.section(": ", 0, 0);
        sStringTmp = sStringTmp.section(": ", 1, -1);
    }

    QString _sString = sStringTmp;

    if (bShowOptions) {
        if (_sString.count("[") == 1) {
            result.sName = _sString.section("[", 0, 0);
            result.sOptions = _sString.section("[", 1, -1).section("]", 0, 0);
            _sString = _sString.section("[", 0, 0);
        }
    }

    if (bShowVersion) {
        if (_sString.count("(") == 1) {
            result.sVersion = _sString.section("(", 1, -1).section(")", 0, 0);
            result.sName = _sString.section("(", 0, 0);
        }
    }

    return result;
}
