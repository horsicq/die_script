/* Copyright (c) 2019-2025 hors<horsicq@gmail.com>
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
                                   XBinary::FT fileType, XBinary::FILEPART filePart, Binary_Script::OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct)
    : XScriptEngine()
{
    g_parentId = {};
    g_resultId = {};

    g_pSignaturesList = pSignaturesList;
    g_pListScanStructs = pListScanStructs;
    g_pPdStruct = pPdStruct;

    // qRegisterMetaType<QList<quint32>>("QList<quint32>");

#ifdef QT_SCRIPT_LIB
    _addFunction(includeScript, "includeScript");
    _addFunction(_log, "_log");
    _addFunction(_setResult, "_setResult");
    _addFunction(_isResultPresent, "_isResultPresent");
    _addFunction(_getNumberOfResults, "_getNumberOfResults");
    _addFunction(_removeResult, "_removeResult");
    _addFunction(_isStop, "_isStop");
    _addFunction(_encodingList, "_encodingList");
    _addFunction(_isConsoleMode, "_isConsoleMode");
    _addFunction(_isLiteMode, "_isLiteMode");
    _addFunction(_isGuiMode, "_isGuiMode");
    _addFunction(_isLibraryMode, "_isLibraryMode");
    _addFunction(_breakScan, "_breakScan");
    _addFunction(_getEngineVersion, "_getEngineVersion");
    _addFunction(_getOS, "_getOS");
#else
    connect(&g_globalScript, SIGNAL(includeScriptSignal(QString)), this, SLOT(includeScriptSlot(QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_logSignal(QString)), this, SLOT(_logSlot(QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_setResultSignal(QString, QString, QString, QString)), this, SLOT(_setResultSlot(QString, QString, QString, QString)),
            Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_isResultPresentSignal(bool *, QString, QString)), this, SLOT(_isResultPresentSlot(bool *, QString, QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_getNumberOfResultsSignal(qint32 *, QString)), this, SLOT(_getNumberOfResultsSlot(qint32 *, QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_removeResultSignal(QString, QString)), this, SLOT(_removeResultSlot(QString, QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_isStopSignal(bool *)), this, SLOT(_isStopSlot(bool *)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_encodingListSignal()), this, SLOT(_encodingListSlot()), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_isConsoleModeSignal(bool *)), this, SLOT(_isConsoleModeSlot(bool *)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_isLiteModeSignal(bool *)), this, SLOT(_isLiteModeSlot(bool *)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_isGuiModeSignal(bool *)), this, SLOT(_isGuiModeSlot(bool *)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_isLibraryModeSignal(bool *)), this, SLOT(_isLibraryModeSlot(bool *)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_breakScanSignal()), this, SLOT(_breakScanSlot()), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_getEngineVersionSignal(QString *)), this, SLOT(_getEngineVersionSlot(QString *)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_getOSSignal(QString *)), this, SLOT(_getOSSlot(QString *)), Qt::DirectConnection);

    QJSValue valueGlobalScript = newQObject(&g_globalScript);
    globalObject().setProperty("includeScript", valueGlobalScript.property("includeScript"));
    globalObject().setProperty("_log", valueGlobalScript.property("_log"));
    globalObject().setProperty("_setResult", valueGlobalScript.property("_setResult"));
    globalObject().setProperty("_isResultPresent", valueGlobalScript.property("_isResultPresent"));
    globalObject().setProperty("_getNumberOfResults", valueGlobalScript.property("_getNumberOfResults"));
    globalObject().setProperty("_removeResult", valueGlobalScript.property("_removeResult"));
    globalObject().setProperty("_isStop", valueGlobalScript.property("_isStop"));
    globalObject().setProperty("_encodingList", valueGlobalScript.property("_encodingList"));
    globalObject().setProperty("_isConsoleMode", valueGlobalScript.property("_isConsoleMode"));
    globalObject().setProperty("_isLiteMode", valueGlobalScript.property("_isLiteMode"));
    globalObject().setProperty("_isGuiMode", valueGlobalScript.property("_isGuiMode"));
    globalObject().setProperty("_isLibraryMode", valueGlobalScript.property("_isLibraryMode"));
    globalObject().setProperty("_breakScan", valueGlobalScript.property("_breakScan"));
    globalObject().setProperty("_getEngineVersion", valueGlobalScript.property("_getEngineVersion"));
    globalObject().setProperty("_getOS", valueGlobalScript.property("_getOS"));
#endif

    Util_script *pUtilScript = new Util_script;
    _addClass(pUtilScript, "Util");
    g_listScriptClasses.append(pUtilScript);

    if (XBinary::checkFileType(XBinary::FT_BINARY, fileType)) {
        XBinary *pBinary = new XBinary(pDevice);
        Binary_Script *pExtraScript = new Binary_Script(pBinary, filePart, pOptions, pPdStruct);
        _adjustScript(pBinary, pExtraScript, "Binary");
    } else if (XBinary::checkFileType(XBinary::FT_COM, fileType)) {
        XCOM *pCOM = new XCOM(pDevice);
        COM_Script *pExtraScript = new COM_Script(pCOM, filePart, pOptions, pPdStruct);
        _adjustScript(pCOM, pExtraScript, "COM");
    } else if (XBinary::checkFileType(XBinary::FT_PE, fileType)) {
        XPE *pPE = new XPE(pDevice);
        PE_Script *pExtraScript = new PE_Script(pPE, filePart, pOptions, pPdStruct);
        _adjustScript(pPE, pExtraScript, "PE");
    } else if (XBinary::checkFileType(XBinary::FT_ELF, fileType)) {
        XELF *pELF = new XELF(pDevice);
        ELF_Script *pExtraScript = new ELF_Script(pELF, filePart, pOptions, pPdStruct);
        _adjustScript(pELF, pExtraScript, "ELF");
    } else if (XBinary::checkFileType(XBinary::FT_MACHO, fileType)) {
        XMACH *pMACH = new XMACH(pDevice);
        MACH_Script *pExtraScript = new MACH_Script(pMACH, filePart, pOptions, pPdStruct);
        _adjustScript(pMACH, pExtraScript, "MACH");
    } else if (XBinary::checkFileType(XBinary::FT_NE, fileType)) {
        XNE *pNE = new XNE(pDevice);
        NE_Script *pExtraScript = new NE_Script(pNE, filePart, pOptions, pPdStruct);
        _adjustScript(pNE, pExtraScript, "NE");
    } else if (XBinary::checkFileType(XBinary::FT_LE, fileType)) {
        XLE *pLE = new XLE(pDevice);
        LE_Script *pExtraScript = new LE_Script(pLE, filePart, pOptions, pPdStruct);
        _adjustScript(pLE, pExtraScript, "LE");
    } else if (XBinary::checkFileType(XBinary::FT_LX, fileType)) {
        XLE *pLE = new XLE(pDevice);
        LX_Script *pExtraScript = new LX_Script(pLE, filePart, pOptions, pPdStruct);
        _adjustScript(pLE, pExtraScript, "LX");
    } else if (XBinary::checkFileType(XBinary::FT_MSDOS, fileType)) {
        XMSDOS *pXMSDOS = new XMSDOS(pDevice);
        MSDOS_Script *pExtraScript = new MSDOS_Script(pXMSDOS, filePart, pOptions, pPdStruct);
        _adjustScript(pXMSDOS, pExtraScript, "MSDOS");
    } else if (XBinary::checkFileType(XBinary::FT_ARCHIVE, fileType)) {
        Archive_Script *pExtraScript = nullptr;

        QSet<XBinary::FT> fileTypes = XBinary::getFileTypes(pDevice, true);
        XBinary::FT _fileType = XBinary::_getPrefFileType(&fileTypes);

        XArchive *_pArchive = XArchives::getClass(_fileType, pDevice);

        if (_pArchive) {
            pExtraScript = new Archive_Script(_pArchive, filePart, pOptions, pPdStruct);
            _adjustScript(_pArchive, pExtraScript, "Archive");
        }
    } else if (XBinary::checkFileType(XBinary::FT_IMAGE, fileType)) {
        Image_Script *pExtraScript = nullptr;

        QSet<XBinary::FT> fileTypes = XBinary::getFileTypes(pDevice, true);

        XBinary *_pImage = nullptr;

        if (fileTypes.contains(XBinary::FT_JPEG)) {
            _pImage = new XJpeg(pDevice);
            pExtraScript = new Jpeg_Script((XJpeg *)_pImage, filePart, pOptions, pPdStruct);
        }
        // TODO more
        _adjustScript(_pImage, pExtraScript, "Image");
    } else if (XBinary::checkFileType(XBinary::FT_ZIP, fileType)) {
        XZip *pZIP = new XZip(pDevice);
        ZIP_Script *pExtraScript = new ZIP_Script(pZIP, filePart, pOptions, pPdStruct);
        _adjustScript(pZIP, pExtraScript, "ZIP");
    } else if (XBinary::checkFileType(XBinary::FT_JAR, fileType)) {
        XJAR *pJAR = new XJAR(pDevice);
        JAR_Script *pExtraScript = new JAR_Script(pJAR, filePart, pOptions, pPdStruct);
        _adjustScript(pJAR, pExtraScript, "JAR");
    } else if (XBinary::checkFileType(XBinary::FT_APK, fileType)) {
        XAPK *pAPK = new XAPK(pDevice);
        APK_Script *pExtraScript = new APK_Script(pAPK, filePart, pOptions, pPdStruct);
        _adjustScript(pAPK, pExtraScript, "APK");
    } else if (XBinary::checkFileType(XBinary::FT_IPA, fileType)) {
        XIPA *pIPA = new XIPA(pDevice);
        IPA_Script *pExtraScript = new IPA_Script(pIPA, filePart, pOptions, pPdStruct);
        _adjustScript(pIPA, pExtraScript, "IPA");
    } else if (XBinary::checkFileType(XBinary::FT_NPM, fileType)) {
        XNPM *pNPNM = new XNPM(pDevice);
        NPM_Script *pExtraScript = new NPM_Script(pNPNM, filePart, pOptions, pPdStruct);
        _adjustScript(pNPNM, pExtraScript, "NPM");
    } else if (XBinary::checkFileType(XBinary::FT_MACHOFAT, fileType)) {
        XMACHOFat *pMachofat = new XMACHOFat(pDevice);
        MACHOFAT_Script *pExtraScript = new MACHOFAT_Script(pMachofat, filePart, pOptions, pPdStruct);
        _adjustScript(pMachofat, pExtraScript, "MACHOFAT");
    } else if (XBinary::checkFileType(XBinary::FT_DOS16M, fileType)) {
        XDOS16 *pDOS16 = new XDOS16(pDevice);
        DOS16M_Script *pExtraScript = new DOS16M_Script(pDOS16, filePart, pOptions, pPdStruct);
        _adjustScript(pDOS16, pExtraScript, "DOS16M");
    } else if (XBinary::checkFileType(XBinary::FT_DOS4G, fileType)) {
        XDOS16 *pDOS16 = new XDOS16(pDevice);
        DOS4G_Script *pExtraScript = new DOS4G_Script(pDOS16, filePart, pOptions, pPdStruct);
        _adjustScript(pDOS16, pExtraScript, "DOS4G");
    } else if (XBinary::checkFileType(XBinary::FT_DEX, fileType)) {
        XDEX *pDEX = new XDEX(pDevice);
        DEX_Script *pExtraScript = new DEX_Script(pDEX, filePart, pOptions, pPdStruct);
        _adjustScript(pDEX, pExtraScript, "DEX");
    } else if (XBinary::checkFileType(XBinary::FT_AMIGAHUNK, fileType)) {
        XAmigaHunk *pAmiga = new XAmigaHunk(pDevice);
        Amiga_Script *pExtraScript = new Amiga_Script(pAmiga, filePart, pOptions, pPdStruct);
        _adjustScript(pAmiga, pExtraScript, "Amiga");
    } else if (XBinary::checkFileType(XBinary::FT_JAVACLASS, fileType)) {
        XJavaClass *pAmiga = new XJavaClass(pDevice);
        JavaClass_Script *pExtraScript = new JavaClass_Script(pAmiga, filePart, pOptions, pPdStruct);
        _adjustScript(pAmiga, pExtraScript, "JavaClass");
    } else if (XBinary::checkFileType(XBinary::FT_PDF, fileType)) {
        XPDF *pPDF = new XPDF(pDevice);
        PDF_Script *pExtraScript = new PDF_Script(pPDF, filePart, pOptions, pPdStruct);
        _adjustScript(pPDF, pExtraScript, "PDF");
    } else if (XBinary::checkFileType(XBinary::FT_CFBF, fileType)) {
        XCFBF *pCFBF = new XCFBF(pDevice);
        CFBF_Script *pExtraScript = new CFBF_Script(pCFBF, filePart, pOptions, pPdStruct);
        _adjustScript(pCFBF, pExtraScript, "CFBF");
    } else if (XBinary::checkFileType(XBinary::FT_JPEG, fileType)) {
        XJpeg *pJpeg = new XJpeg(pDevice);
        Jpeg_Script *pExtraScript = new Jpeg_Script(pJpeg, filePart, pOptions, pPdStruct);
        _adjustScript(pJpeg, pExtraScript, "Jpeg");
    }

    // TODO APKS
}

DiE_ScriptEngine::~DiE_ScriptEngine()
{
    {
        qint32 nNumberOfRecords = g_listBinaries.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            delete g_listBinaries.at(i);
        }
    }

    {
        qint32 nNumberOfRecords = g_listScriptClasses.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            delete g_listScriptClasses.at(i);
        }
    }
}

void DiE_ScriptEngine::_adjustScript(XBinary *pBinary, Binary_Script *pScript, const QString &sName)
{
    if (pScript) {
        connect(pScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        connect(pScript, SIGNAL(warningMessage(QString)), this, SIGNAL(warningMessage(QString)));
        connect(pScript, SIGNAL(infoMessage(QString)), this, SIGNAL(infoMessage(QString)));
    }

    _addClass(pScript, "Binary");

    if (sName != "Binary") {
        _addClass(pScript, sName);
    }

    g_listBinaries.append(pBinary);
    g_listScriptClasses.append(pScript);
}

bool DiE_ScriptEngine::handleError(QString sPrefix, XSCRIPTVALUE value, QString *psErrorString)
{
    bool bResult = true;

    if (value.isError()) {
        // TODO Check more information
        *psErrorString = QString("%1: %2: %4").arg(sPrefix, value.property("lineNumber").toString(), value.toString());

#ifdef QT_DEBUG
        qDebug("%s", (*psErrorString).toUtf8().data());
#endif

        bResult = false;
    }

    return bResult;
}

XSCRIPTVALUE DiE_ScriptEngine::evaluateEx(const XScanEngine::SCANID &parentId, const XScanEngine::SCANID &resultId, const QString &sProgram, const QString &sName,
                                          const QString &sFileName)
{
    g_parentId = parentId;
    g_resultId = resultId;
    g_sName = sName;
    g_sFileName = sFileName;

    return evaluate(sProgram, sFileName);
}

// QList<DiE_ScriptEngine::RESULT> DiE_ScriptEngine::getListLocalResult()
// {
//     return g_listResult;
// }

// void DiE_ScriptEngine::clearListLocalResult()
// {
//     g_listResult.clear();
// }

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
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_isConsoleMode(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        bool bResult = false;

        pScriptEngine->_isConsoleModeSlot(&bResult);

        result = bResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_isGuiMode(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        bool bResult = false;

        pScriptEngine->_isGuiModeSlot(&bResult);

        result = bResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_isLiteMode(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        bool bResult = false;

        pScriptEngine->_isLiteModeSlot(&bResult);

        result = bResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_isLibraryMode(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        bool bResult = false;

        pScriptEngine->_isLibraryModeSlot(&bResult);

        result = bResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_breakScan(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        pScriptEngine->_breakScanSlot();
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_getEngineVersion(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sResult;

        pScriptEngine->_getEngineVersionSlot(&sResult);

        result = sResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_getOS(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sResult;

        pScriptEngine->_getOSSlot(&sResult);

        result = sResult;
    }

    return result;
}
#endif

void DiE_ScriptEngine::includeScriptSlot(const QString &sScript)
{
    bool bSuccess = false;

    qint32 nNumberOfSignatures = g_pSignaturesList->count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        if (g_pSignaturesList->at(i).fileType == XBinary::FT_UNKNOWN) {
            if (g_pSignaturesList->at(i).sName.toUpper() == sScript.toUpper()) {
                XSCRIPTVALUE value = evaluate(g_pSignaturesList->at(i).sText, sScript);

                if (value.isError()) {
                    emit errorMessage(QString("includeScript %1: %2: %3").arg(sScript, value.property("lineNumber").toString(), value.toString()));
                }

                bSuccess = true;

                break;
            }
        }
    }

    if (!bSuccess) {
        emit errorMessage(QString("%1: %2").arg(tr("Cannot find"), sScript));
    }
}

void DiE_ScriptEngine::_logSlot(const QString &sText)
{
#ifdef QT_DEBUG
    qDebug("LOG: %s", sText.toUtf8().data());
#endif

    emit infoMessage(sText);
}

void DiE_ScriptEngine::_setResultSlot(const QString &sType, const QString &sName, const QString &sVersion, const QString &sOptions)
{
    bool bAdd = true;

    qint32 nNumberOfResults = g_listBLRecords.count();

    for (qint32 i = 0; i < nNumberOfResults; i++) {
        if ((g_listBLRecords.at(i).sType.toUpper() == sType.toUpper()) &&
            ((g_listBLRecords.at(i).sName.toUpper() == sName.toUpper()) || (g_listBLRecords.at(i).sName == ""))) {
            bAdd = false;
            break;
        }
    }

    if (bAdd) {
        DiE_ScriptEngine::SCAN_STRUCT ssRecord = {};

        // TODO IDs
        ssRecord.id = g_resultId;
        ssRecord.parentId = g_parentId;

        ssRecord.sSignature = g_sName;
        ssRecord.sSignatureFileName = g_sFileName;

        ssRecord.sType = sType;
        ssRecord.sName = sName;
        ssRecord.sVersion = sVersion;
        ssRecord.sOptions = sOptions;
        // ssRecord.sFullString = QString("%1: %2(%3)[%4]").arg(ssRecord.sType, ssRecord.sName, ssRecord.sVersion, ssRecord.sOptions);
        // ssRecord.sResult = QString("%1(%2)[%3]").arg(ssRecord.sName, ssRecord.sVersion, ssRecord.sOptions);

        g_pListScanStructs->append(ssRecord);
    }
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
    BLRECORD blRecord;
    blRecord.sType = sType;
    blRecord.sName = sName;

    g_listBLRecords.append(blRecord);

    qint32 nNumberOfResults = g_pListScanStructs->count();

    for (qint32 i = 0; i < nNumberOfResults; i++) {
        if ((g_pListScanStructs->at(i).sType.toUpper() == sType.toUpper()) &&
            ((g_pListScanStructs->at(i).sName.toUpper() == sName.toUpper()) || (g_pListScanStructs->at(i).sName == ""))) {
            g_pListScanStructs->removeAt(i);
            break;
        }
    }
}

void DiE_ScriptEngine::_isStopSlot(bool *pResult)
{
    *pResult = XBinary::isPdStructStopped(g_pPdStruct);
}

void DiE_ScriptEngine::_encodingListSlot()
{
#if (QT_VERSION_MAJOR < 6) || defined(QT_CORE5COMPAT_LIB)
    QList<QString> listCodePages = XOptions::getCodePages(false);

    qint32 nNumberOfCodePages = listCodePages.count();

    for (qint32 i = 0; i < nNumberOfCodePages; i++) {
        emit infoMessage(listCodePages.at(i));
    }
#endif
}

void DiE_ScriptEngine::_isConsoleModeSlot(bool *pResult)
{
    bool bConsole = false;
#ifndef QT_GUI_LIB
    bConsole = true;
#endif
    *pResult = bConsole && (qApp->applicationName() == "die");
}

void DiE_ScriptEngine::_isLiteModeSlot(bool *pResult)
{
    *pResult = (qApp->applicationName() == "diel");
}

void DiE_ScriptEngine::_isGuiModeSlot(bool *pResult)
{
    bool bGui = false;
#ifdef QT_GUI_LIB
    bGui = true;
#endif
    *pResult = bGui && (qApp->applicationName() == "die");
}

void DiE_ScriptEngine::_isLibraryModeSlot(bool *pResult)
{
    *pResult = (qApp->applicationName() == "");
}

void DiE_ScriptEngine::_breakScanSlot()
{
    XBinary::setPdStructStopped(g_pPdStruct);
}

void DiE_ScriptEngine::_getEngineVersionSlot(QString *pResult)
{
    QDate qDate = QDate::fromString(__DATE__, "MMM dd yyyy");

    *pResult = QString("%1.%2").arg(qApp->applicationVersion(), qDate.toString("yyyy.MM.dd"));
}

void DiE_ScriptEngine::_getOSSlot(QString *pResult)
{
    *pResult = XOptions::getBundleIdToString(XOptions::getBundle());
}

// DiE_ScriptEngine::RESULT DiE_ScriptEngine::stringToResult(const QString &sString, bool bShowType, bool bShowVersion, bool bShowOptions)
// {
//     QString sStringTmp = sString;
//     RESULT result = {};

//     if (bShowType) {
//         result.sType = sStringTmp.section(": ", 0, 0);
//         sStringTmp = sStringTmp.section(": ", 1, -1);
//     }

//     QString _sString = sStringTmp;

//     if (bShowOptions) {
//         if (_sString.count("[") == 1) {
//             result.sName = _sString.section("[", 0, 0);
//             result.sOptions = _sString.section("[", 1, -1).section("]", 0, 0);
//             _sString = _sString.section("[", 0, 0);
//         }
//     }

//     if (bShowVersion) {
//         if (_sString.count("(") == 1) {
//             result.sVersion = _sString.section("(", 1, -1).section(")", 0, 0);
//             result.sName = _sString.section("(", 0, 0);
//         }
//     }

//     return result;
// }
