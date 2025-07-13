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
#include "binary_script.h"

Binary_Script::Binary_Script(XBinary *pBinary, XBinary::FILEPART filePart, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct)
{
    this->g_pBinary = pBinary;
    this->g_filePart = filePart;
    this->g_pPdStruct = pPdStruct;
    this->g_pOptions = pOptions;

    connect(pBinary, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
    connect(pBinary, SIGNAL(infoMessage(QString)), this, SIGNAL(infoMessage(QString)));

    g_nSize = pBinary->getSize();
    g_memoryMap = pBinary->getMemoryMap(XBinary::MAPMODE_UNKNOWN, pPdStruct);
    g_nBaseAddress = pBinary->getBaseAddress();

    g_nEntryPointOffset = pBinary->getEntryPointOffset(&g_memoryMap);
    g_nEntryPointAddress = pBinary->getEntryPointAddress(&g_memoryMap);
    g_nOverlayOffset = pBinary->getOverlayOffset(&g_memoryMap, pPdStruct);
    g_nOverlaySize = pBinary->getOverlaySize(&g_memoryMap, pPdStruct);
    g_bIsOverlayPresent = pBinary->isOverlayPresent(&g_memoryMap, pPdStruct);
    g_bIsBigEndian = pBinary->isBigEndian();

    g_sHeaderSignature = pBinary->getSignature(0, 256);  // TODO const
    g_nHeaderSignatureSize = g_sHeaderSignature.size() / 2;

    g_sEntryPointSignature = pBinary->getSignature(g_nEntryPointOffset, 256);  // TODO const
    g_nEntryPointSignatureSize = g_sEntryPointSignature.size();

    if (g_nOverlayOffset > 0) {
        g_sOverlaySignature = pBinary->getSignature(g_nOverlayOffset, 256);  // TODO const
        g_nOverlaySignatureSize = g_sOverlaySignature.size();
    }

    g_sFileDirectory = XBinary::getDeviceDirectory(pBinary->getDevice());
    g_sFileBaseName = XBinary::getDeviceFileBaseName(pBinary->getDevice());
    g_sFileCompleteSuffix = XBinary::getDeviceFileCompleteSuffix(pBinary->getDevice());
    g_sFileSuffix = XBinary::getDeviceFileSuffix(pBinary->getDevice());

    g_bIsPlainText = pBinary->isPlainTextType();
    g_bIsUTF8Text = pBinary->isUTF8TextType();
    XBinary::UNICODE_TYPE unicodeType = pBinary->getUnicodeType();

    if (unicodeType != XBinary::UNICODE_TYPE_NONE) {
        g_sHeaderString = pBinary->read_unicodeString(2, qMin(g_nSize, (qint64)0x1000), (unicodeType == XBinary::UNICODE_TYPE_BE));
        g_bIsUnicodeText = true;
    } else if (g_bIsUTF8Text) {
        g_sHeaderString = pBinary->read_utf8String(3, qMin(g_nSize, (qint64)0x1000));
    } else if (g_bIsPlainText) {
        g_sHeaderString = pBinary->read_ansiString(0, qMin(g_nSize, (qint64)0x1000));
    }

    g_bIsSigned = pBinary->isSigned();
    g_fileFormatInfo = pBinary->getFileFormatInfo(pPdStruct);
    g_sFileFormatInfoString = XBinary::getFileFormatInfoString(&g_fileFormatInfo);

    g_bIsFmtChecking = false;
    g_bIsFmtCheckingDeep = false;

    g_disasmOptions = {};
    g_disasmOptions.bIsUppercase = true;
    g_disasmCore.setMode(XBinary::getDisasmMode(&g_memoryMap));
}

Binary_Script::~Binary_Script()
{
}

qint64 Binary_Script::getSize()
{
    return g_nSize;
}

bool Binary_Script::compare(const QString &sSignature, qint64 nOffset)
{
    bool bResult = false;

    QString _sSignature = XBinary::convertSignature(sSignature);

    qint32 nSignatureSize = _sSignature.size();

    if ((nSignatureSize + nOffset < g_nHeaderSignatureSize) && (!_sSignature.contains('$')) && (!_sSignature.contains('#')) && (!_sSignature.contains('+')) &&
        (!_sSignature.contains('%')) && (!_sSignature.contains('*'))) {
        bResult = g_pBinary->compareSignatureStrings(g_sHeaderSignature.mid(nOffset * 2, nSignatureSize * 2), _sSignature);
    } else {
        bResult = g_pBinary->compareSignature(&g_memoryMap, _sSignature, nOffset, g_pPdStruct);
    }

    return bResult;
}

bool Binary_Script::compareEP(const QString &sSignature, qint64 nOffset)
{
    bool bResult = false;

    QString _sSignature = XBinary::convertSignature(sSignature);

    qint32 nSignatureSize = sSignature.size();

    if ((nSignatureSize + nOffset < g_nEntryPointSignatureSize) && (!_sSignature.contains('$')) && (!_sSignature.contains('#')) && (!_sSignature.contains('+')) &&
        (!_sSignature.contains('%')) && (!_sSignature.contains('*'))) {
        bResult = g_pBinary->compareSignatureStrings(g_sEntryPointSignature.mid(nOffset * 2, nSignatureSize * 2), _sSignature);
    } else {
        bResult = g_pBinary->compareEntryPoint(&g_memoryMap, _sSignature, nOffset);  // TODO g_pPdStruct
    }

    return bResult;
}

quint8 Binary_Script::readByte(qint64 nOffset)
{
    return g_pBinary->read_uint8(nOffset);
}

qint16 Binary_Script::readSByte(qint64 nOffset)
{
    return g_pBinary->read_int8(nOffset);
}

quint16 Binary_Script::readWord(qint64 nOffset)
{
    return g_pBinary->read_uint16(nOffset);
}

qint16 Binary_Script::readSWord(qint64 nOffset)
{
    return g_pBinary->read_int16(nOffset);
}

quint32 Binary_Script::readDword(qint64 nOffset)
{
    return g_pBinary->read_uint32(nOffset);
}

qint32 Binary_Script::readSDword(qint64 nOffset)
{
    return g_pBinary->read_int32(nOffset);
}

quint64 Binary_Script::readQword(qint64 nOffset)
{
    return g_pBinary->read_uint64(nOffset);
}

qint64 Binary_Script::readSQword(qint64 nOffset)
{
    return g_pBinary->read_int64(nOffset);
}

QString Binary_Script::getString(qint64 nOffset, qint64 nMaxSize)
{
    return g_pBinary->read_ansiString(nOffset, nMaxSize);
}

qint64 Binary_Script::findSignature(qint64 nOffset, qint64 nSize, const QString &sSignature)
{
    qint64 nResult = -1;

    QElapsedTimer *pTimer = _startProfiling();

    qint64 nResultSize = 0;

    _fixOffsetAndSize(&nOffset, &nSize);

    nResult = g_pBinary->find_signature(&g_memoryMap, nOffset, nSize, sSignature, &nResultSize, g_pPdStruct);

    if (pTimer) {
        _finishProfiling(pTimer, QString("find_signature[%1]: %2 %3").arg(sSignature, XBinary::valueToHexEx(nOffset), XBinary::valueToHexEx(nSize)));
    }

    return nResult;
}

qint64 Binary_Script::findString(qint64 nOffset, qint64 nSize, const QString &sString)
{
    qint64 nResult = -1;

    QElapsedTimer *pTimer = _startProfiling();

    _fixOffsetAndSize(&nOffset, &nSize);

    nResult = g_pBinary->find_ansiString(nOffset, nSize, sString, g_pPdStruct);

    if (pTimer) {
        _finishProfiling(pTimer, QString("findString[%1]: %2 %3").arg(sString, XBinary::valueToHexEx(nOffset), XBinary::valueToHexEx(nSize)));
    }

    return nResult;
}

qint64 Binary_Script::findByte(qint64 nOffset, qint64 nSize, quint8 nValue)
{
    qint64 nResult = -1;

    QElapsedTimer *pTimer = _startProfiling();

    _fixOffsetAndSize(&nOffset, &nSize);

    nResult = g_pBinary->find_uint8(nOffset, nSize, nValue, g_pPdStruct);

    if (pTimer) {
        _finishProfiling(pTimer, QString("findByte[%1]: %2 %3").arg(XBinary::valueToHex(nValue), XBinary::valueToHexEx(nOffset), XBinary::valueToHexEx(nSize)));
    }

    return nResult;
}

qint64 Binary_Script::findWord(qint64 nOffset, qint64 nSize, quint16 nValue)
{
    qint64 nResult = -1;

    QElapsedTimer *pTimer = _startProfiling();

    _fixOffsetAndSize(&nOffset, &nSize);

    nResult = g_pBinary->find_uint16(nOffset, nSize, nValue, g_pPdStruct);

    if (pTimer) {
        _finishProfiling(pTimer, QString("findWord[%1]: %2 %3").arg(XBinary::valueToHex(nValue), XBinary::valueToHexEx(nOffset), XBinary::valueToHexEx(nSize)));
    }

    return nResult;
}

qint64 Binary_Script::findDword(qint64 nOffset, qint64 nSize, quint32 nValue)
{
    qint64 nResult = -1;

    QElapsedTimer *pTimer = _startProfiling();

    _fixOffsetAndSize(&nOffset, &nSize);

    nResult = g_pBinary->find_uint32(nOffset, nSize, nValue, g_pPdStruct);

    if (pTimer) {
        _finishProfiling(pTimer, QString("findDword[%1]: %2 %3").arg(XBinary::valueToHex(nValue), XBinary::valueToHexEx(nOffset), XBinary::valueToHexEx(nSize)));
    }

    return nResult;
}

qint64 Binary_Script::getEntryPointOffset()
{
    return g_nEntryPointOffset;
}

qint64 Binary_Script::getOverlayOffset()
{
    return g_nOverlayOffset;
}

qint64 Binary_Script::getOverlaySize()
{
    return g_nOverlaySize;
}

qint64 Binary_Script::getAddressOfEntryPoint()
{
    return g_nEntryPointAddress;
}

bool Binary_Script::isOverlayPresent()
{
    return g_bIsOverlayPresent;
}

bool Binary_Script::compareOverlay(const QString &sSignature, qint64 nOffset)
{
    bool bResult = false;

    QString _sSignature = XBinary::convertSignature(sSignature);

    qint32 nSignatureSize = sSignature.size();

    if ((nSignatureSize + nOffset < g_nOverlaySignatureSize) && (!_sSignature.contains('$')) && (!_sSignature.contains('#')) && (!_sSignature.contains('+')) &&
        (!_sSignature.contains('%')) && (!_sSignature.contains('*'))) {
        bResult = g_pBinary->compareSignatureStrings(g_sOverlaySignature.mid(nOffset * 2, nSignatureSize * 2), _sSignature);
    } else {
        bResult = g_pBinary->compareOverlay(&g_memoryMap, _sSignature, nOffset, g_pPdStruct);
    }

    return bResult;
}

bool Binary_Script::isSignaturePresent(qint64 nOffset, qint64 nSize, const QString &sSignature)
{
    bool bResult = false;

    QElapsedTimer *pTimer = _startProfiling();

    bResult = g_pBinary->isSignaturePresent(&g_memoryMap, nOffset, nSize, sSignature, g_pPdStruct);

    if (pTimer) {
        _finishProfiling(pTimer, QString("isSignaturePresent[%1]: %2 %3").arg(sSignature, XBinary::valueToHexEx(nOffset), XBinary::valueToHexEx(nSize)));
    }

    return bResult;
}

quint32 Binary_Script::swapBytes(quint32 nValue)
{
    return g_pBinary->swapBytes(nValue);
}

QString Binary_Script::getGeneralOptions()
{
    return "";
}

qint64 Binary_Script::RVAToOffset(qint64 nRVA)
{
    return g_pBinary->addressToOffset(&g_memoryMap, nRVA + g_nBaseAddress);
}

qint64 Binary_Script::VAToOffset(qint64 nVA)
{
    return g_pBinary->addressToOffset(&g_memoryMap, nVA);
}

qint64 Binary_Script::OffsetToVA(qint64 nOffset)
{
    return g_pBinary->offsetToAddress(&g_memoryMap, nOffset);
}

qint64 Binary_Script::OffsetToRVA(qint64 nOffset)
{
    qint64 nResult = g_pBinary->offsetToAddress(&g_memoryMap, nOffset);

    if (nResult != -1) {
        nResult -= g_nBaseAddress;
    }

    return nResult;
}

QString Binary_Script::getFileDirectory()
{
    return g_sFileDirectory;
}

QString Binary_Script::getFileBaseName()
{
    return g_sFileBaseName;
}

QString Binary_Script::getFileCompleteSuffix()
{
    return g_sFileCompleteSuffix;
}

QString Binary_Script::getFileSuffix()
{
    return g_sFileSuffix;
}

QString Binary_Script::getSignature(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->getSignature(nOffset, nSize);
}

double Binary_Script::calculateEntropy(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->getBinaryStatus(XBinary::BSTATUS_ENTROPY, nOffset, nSize, g_pPdStruct);
}

QString Binary_Script::calculateMD5(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->getHash(XBinary::HASH_MD5, nOffset, nSize, g_pPdStruct);
}

quint32 Binary_Script::calculateCRC32(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->_getCRC32(nOffset, nSize, 0, g_pPdStruct);
}

quint16 Binary_Script::crc16(qint64 nOffset, qint64 nSize, quint16 nInit)
{
    return g_pBinary->_getCRC16(nOffset, nSize, nInit, g_pPdStruct);
}

quint32 Binary_Script::crc32(qint64 nOffset, qint64 nSize, quint32 nInit)
{
    return g_pBinary->_getCRC32(nOffset, nSize, nInit, g_pPdStruct);
}

quint32 Binary_Script::adler32(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->getAdler32(nOffset, nSize, g_pPdStruct);
}

bool Binary_Script::isSignatureInSectionPresent(quint32 nNumber, const QString &sSignature)
{
    bool bResult = false;

    QElapsedTimer *pTimer = _startProfiling();

    qint32 _nNumber = nNumber;
    QString sClassName = metaObject()->className();

    if (sClassName == "PE_Script") {
        _nNumber++;
    }

    bResult = g_pBinary->isSignatureInFilePartPresent(&g_memoryMap, _nNumber, sSignature, g_pPdStruct);

    if (pTimer) {
        _finishProfiling(pTimer, QString("isSignatureInSectionPresent[%1]: %2 ").arg(sSignature, QString::number(nNumber)));
    }

    return bResult;
}

qint64 Binary_Script::getImageBase()
{
    return g_memoryMap.nModuleAddress;
}

QString Binary_Script::upperCase(const QString &sString)
{
    return sString.toUpper();
}

QString Binary_Script::lowerCase(const QString &sString)
{
    return sString.toLower();
}

bool Binary_Script::isPlainText()
{
    return g_bIsPlainText;
}

bool Binary_Script::isUTF8Text()
{
    return g_bIsUTF8Text;
}

bool Binary_Script::isUnicodeText()
{
    return g_bIsUnicodeText;
}

bool Binary_Script::isText()
{
    return g_bIsPlainText | g_bIsUTF8Text | g_bIsUnicodeText;
}

QString Binary_Script::getHeaderString()
{
    return g_sHeaderString;
}

qint32 Binary_Script::getDisasmLength(qint64 nAddress)
{
    return g_disasmCore.disAsm(g_pBinary->getDevice(), XBinary::addressToOffset(&g_memoryMap, nAddress), nAddress, g_disasmOptions).nSize;
}

QString Binary_Script::getDisasmString(qint64 nAddress)
{
    qint64 nOffset = XBinary::addressToOffset(&g_memoryMap, nAddress);

    XDisasmAbstract::DISASM_RESULT _disasmResult = g_disasmCore.disAsm(g_pBinary->getDevice(), nOffset, nAddress, g_disasmOptions);

    QString sResult = _disasmResult.sMnemonic;
    if (_disasmResult.sOperands != "") {
        sResult += " " + _disasmResult.sOperands;
    }

    return sResult;
}

qint64 Binary_Script::getDisasmNextAddress(qint64 nAddress)
{
    return g_disasmCore.disAsm(g_pBinary->getDevice(), XBinary::addressToOffset(&g_memoryMap, nAddress), nAddress, g_disasmOptions).nNextAddress;
}

bool Binary_Script::is16()
{
    return XBinary::is16(&g_memoryMap);
}

bool Binary_Script::is32()
{
    return XBinary::is32(&g_memoryMap);
}

bool Binary_Script::is64()
{
    return XBinary::is64(&g_memoryMap);
}

bool Binary_Script::isDeepScan()
{
    return g_pOptions->bIsDeepScan;
}

bool Binary_Script::isHeuristicScan()
{
    return g_pOptions->bIsHeuristicScan;
}

bool Binary_Script::isAggressiveScan()
{
    return g_pOptions->bIsAggressiveScan;
}

bool Binary_Script::isVerbose()
{
    return g_pOptions->bIsVerbose;
}

bool Binary_Script::isProfiling()
{
    return g_pOptions->bIsProfiling;
}

quint8 Binary_Script::read_uint8(qint64 nOffset)
{
    return g_pBinary->read_uint8(nOffset);
}

qint16 Binary_Script::read_int8(qint64 nOffset)
{
    return g_pBinary->read_int8(nOffset);
}

quint16 Binary_Script::read_uint16(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_uint16(nOffset, bIsBigEndian);
}

qint16 Binary_Script::read_int16(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_int16(nOffset, bIsBigEndian);
}

quint32 Binary_Script::read_uint32(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_uint32(nOffset, bIsBigEndian);
}

qint32 Binary_Script::read_int32(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_int32(nOffset, bIsBigEndian);
}

quint64 Binary_Script::read_uint64(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_uint64(nOffset, bIsBigEndian);
}

qint64 Binary_Script::read_int64(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_int64(nOffset, bIsBigEndian);
}

QString Binary_Script::read_ansiString(qint64 nOffset, qint64 nMaxSize)
{
    return g_pBinary->read_ansiString(nOffset, nMaxSize);
}

QString Binary_Script::read_unicodeString(qint64 nOffset, qint64 nMaxSize)
{
    return g_pBinary->read_unicodeString(nOffset, nMaxSize);
}

QString Binary_Script::read_utf8String(qint64 nOffset, qint64 nMaxSize)
{
    return g_pBinary->read_utf8String(nOffset, nMaxSize);
}

QString Binary_Script::read_ucsdString(qint64 nOffset)
{
    return g_pBinary->read_ucsdString(nOffset);
}

QString Binary_Script::read_codePageString(qint64 nOffset, qint64 nMaxByteSize, const QString &sCodePage)
{
    return g_pBinary->read_codePageString(nOffset, nMaxByteSize, sCodePage);
}

QString Binary_Script::bytesCountToString(quint64 nValue)
{
    return g_pBinary->bytesCountToString(nValue);
}

qint64 Binary_Script::find_ansiString(qint64 nOffset, qint64 nSize, const QString &sString)
{
    qint64 nResult = -1;

    nResult = g_pBinary->find_ansiString(nOffset, nSize, sString, g_pPdStruct);

    return nResult;
}

qint64 Binary_Script::find_unicodeString(qint64 nOffset, qint64 nSize, const QString &sString)
{
    qint64 nResult = -1;

    nResult = g_pBinary->find_unicodeString(nOffset, nSize, sString, g_bIsBigEndian, g_pPdStruct);

    return nResult;
}

qint64 Binary_Script::find_utf8String(qint64 nOffset, qint64 nSize, const QString &sString)
{
    qint64 nResult = -1;

    nResult = g_pBinary->find_utf8String(nOffset, nSize, sString, g_pPdStruct);

    return nResult;
}

QString Binary_Script::read_UUID_bytes(qint64 nOffset)
{
    return g_pBinary->read_UUID_bytes(nOffset);
}

QString Binary_Script::read_UUID(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_UUID(nOffset, bIsBigEndian);
}

float Binary_Script::read_float(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_float(nOffset, bIsBigEndian);
}

double Binary_Script::read_double(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_double(nOffset, bIsBigEndian);
}

float Binary_Script::read_float16(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_float16(nOffset, bIsBigEndian);
}

float Binary_Script::read_float32(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_float(nOffset, bIsBigEndian);
}

double Binary_Script::read_float64(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_double(nOffset, bIsBigEndian);
}

quint32 Binary_Script::read_uint24(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_uint24(nOffset, bIsBigEndian);
}

qint32 Binary_Script::read_int24(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_int24(nOffset, bIsBigEndian);
}

quint8 Binary_Script::read_bcd_uint8(qint64 nOffset)
{
    return g_pBinary->read_bcd_uint8(nOffset);
}

quint16 Binary_Script::read_bcd_uint16(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_bcd_uint16(nOffset, bIsBigEndian);
}

quint16 Binary_Script::read_bcd_uint32(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_bcd_uint32(nOffset, bIsBigEndian);
}

quint16 Binary_Script::read_bcd_uint64(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_bcd_uint64(nOffset, bIsBigEndian);
}

QString Binary_Script::getOperationSystemName()
{
    return XBinary::osNameIdToString(g_fileFormatInfo.osName);
}

QString Binary_Script::getOperationSystemVersion()
{
    return g_fileFormatInfo.sOsVersion;
}

QString Binary_Script::getOperationSystemOptions()
{
    QString sResult = QString("%1, %2, %3").arg(g_fileFormatInfo.sArch, XBinary::modeIdToString(g_fileFormatInfo.mode), g_fileFormatInfo.sType);

    if (g_fileFormatInfo.endian == XBinary::ENDIAN_BIG) {
        if (sResult != "") {
            sResult.append(", ");
        }
        sResult.append(XBinary::endianToString(XBinary::ENDIAN_BIG));
    }

    return sResult;
}

QString Binary_Script::getFileFormatName()
{
    // return XBinary::getFileFormatString(&g_fileFormatInfo);
    return XBinary::fileTypeIdToString(g_fileFormatInfo.fileType);
}

QString Binary_Script::getFileFormatVersion()
{
    return g_fileFormatInfo.sVersion;
}

QString Binary_Script::getFileFormatOptions()
{
    return g_sFileFormatInfoString;
}

bool Binary_Script::isSigned()
{
    return g_bIsSigned;
}

QString Binary_Script::cleanString(const QString &sString)
{
    return XBinary::cleanString(sString);
}

quint8 Binary_Script::U8(qint64 nOffset)
{
    return read_uint8(nOffset);
}

qint16 Binary_Script::I8(qint64 nOffset)
{
    return read_int8(nOffset);
}

quint16 Binary_Script::U16(qint64 nOffset, bool bIsBigEndian)
{
    return read_uint16(nOffset, bIsBigEndian);
}

qint16 Binary_Script::I16(qint64 nOffset, bool bIsBigEndian)
{
    return read_int16(nOffset, bIsBigEndian);
}

quint32 Binary_Script::U24(qint64 nOffset, bool bIsBigEndian)
{
    return read_uint24(nOffset, bIsBigEndian);
}

qint32 Binary_Script::I24(qint64 nOffset, bool bIsBigEndian)
{
    return read_int24(nOffset, bIsBigEndian);
}

quint32 Binary_Script::U32(qint64 nOffset, bool bIsBigEndian)
{
    return read_uint32(nOffset, bIsBigEndian);
}

qint32 Binary_Script::I32(qint64 nOffset, bool bIsBigEndian)
{
    return read_int32(nOffset, bIsBigEndian);
}

quint64 Binary_Script::U64(qint64 nOffset, bool bIsBigEndian)
{
    return read_uint64(nOffset, bIsBigEndian);
}

qint64 Binary_Script::I64(qint64 nOffset, bool bIsBigEndian)
{
    return read_int64(nOffset, bIsBigEndian);
}

float Binary_Script::F16(qint64 nOffset, bool bIsBigEndian)
{
    return read_float16(nOffset, bIsBigEndian);
}

float Binary_Script::F32(qint64 nOffset, bool bIsBigEndian)
{
    return read_float32(nOffset, bIsBigEndian);
}

double Binary_Script::F64(qint64 nOffset, bool bIsBigEndian)
{
    return read_float64(nOffset, bIsBigEndian);
}

QString Binary_Script::SA(qint64 nOffset, qint64 nMaxSize)
{
    return read_ansiString(nOffset, nMaxSize);
}

QString Binary_Script::SU16(qint64 nOffset, qint64 nMaxSize)
{
    return read_unicodeString(nOffset, nMaxSize);
}

QString Binary_Script::SU8(qint64 nOffset, qint64 nMaxSize)
{
    return read_utf8String(nOffset, nMaxSize);
}

QString Binary_Script::UCSD(qint64 nOffset)
{
    return read_ucsdString(nOffset);
}

QString Binary_Script::SC(qint64 nOffset, qint64 nMaxByteSize, const QString &sCodePage)
{
    return read_codePageString(nOffset, nMaxByteSize, sCodePage);
}

qint64 Binary_Script::Sz()
{
    return getSize();
}

qint64 Binary_Script::fSig(qint64 nOffset, qint64 nSize, const QString &sSignature)
{
    return findSignature(nOffset, nSize, sSignature);
}

qint64 Binary_Script::fStr(qint64 nOffset, qint64 nSize, const QString &sString)
{
    return findString(nOffset, nSize, sString);
}

bool Binary_Script::c(const QString &sSignature, qint64 nOffset)
{
    return compare(sSignature, nOffset);
}

QList<QVariant> Binary_Script::BA(qint64 nOffset, qint64 nSize, bool bReplaceZeroWithSpace)
{
    return readBytes(nOffset, nSize, bReplaceZeroWithSpace);
}

void Binary_Script::_fixOffsetAndSize(qint64 *pnOffset, qint64 *pnSize)
{
    if ((*pnOffset) < g_nSize) {
        if ((*pnOffset) + (*pnSize) > g_nSize) {
            *pnSize = g_nSize - (*pnOffset);
        }
    }
}

QElapsedTimer *Binary_Script::_startProfiling()
{
    QElapsedTimer *pResult = nullptr;

    if (g_pOptions->bIsProfiling) {
        pResult = new QElapsedTimer;
        pResult->start();
    }

    return pResult;
}

void Binary_Script::_finishProfiling(QElapsedTimer *pElapsedTimer, const QString &sInfo)
{
    if (g_pOptions->bIsProfiling) {
        qint64 nElapsed = pElapsedTimer->elapsed();
        delete pElapsedTimer;

        emit warningMessage(QString("%1 [%2 ms]").arg(sInfo, QString::number(nElapsed)));
    }
}

bool Binary_Script::_loadFmtChecking(bool bDeep, XBinary::PDSTRUCT *pPdStruct)
{
    if ((!g_bIsFmtCheckingDeep) && bDeep) {
        g_bIsFmtCheckingDeep = true;
        g_bIsFmtChecking = true;
        g_listFmtMsg = g_pBinary->checkFileFormat(true, pPdStruct);
        g_listFormatMessages = g_pBinary->getFileFormatMessages(&g_listFmtMsg);
    } else if (!g_bIsFmtChecking) {
        g_bIsFmtChecking = true;
        g_listFmtMsg = g_pBinary->checkFileFormat(false, pPdStruct);
        g_listFormatMessages = g_pBinary->getFileFormatMessages(&g_listFmtMsg);
    }

    return true;
}

qint64 Binary_Script::startTiming()
{
    quint32 nResult = 0;

    QElapsedTimer *pElapsedTimer = _startProfiling();

    nResult = XBinary::random32();

    g_mapProfiling.insert(nResult, pElapsedTimer);

    return nResult;
}

qint64 Binary_Script::endTiming(qint64 nHandle, const QString &sInfo)
{
    qint64 nResult = 0;

    if (g_mapProfiling.contains(nHandle)) {
        QElapsedTimer *pElapsedTimer = g_mapProfiling.value(nHandle);

        _finishProfiling(pElapsedTimer, sInfo);

        g_mapProfiling.remove(nHandle);
    } else {
        emit errorMessage(QString("%1: %2").arg(tr("Invalid handle"), QString::number(nHandle)));
    }

    return nResult;
}

qint64 Binary_Script::detectZLIB(qint64 nOffset, qint64 nSize)
{
    qint64 nResult = XFormats::getFileFormatSize(XBinary::FT_ZLIB, g_pBinary->getDevice(), false, -1, g_pPdStruct, nOffset, nSize);

    if (nResult) {
        return nResult;
    } else {
        return -1;
    }
}

qint64 Binary_Script::detectGZIP(qint64 nOffset, qint64 nSize)
{
    qint64 nResult = XFormats::getFileFormatSize(XBinary::FT_GZIP, g_pBinary->getDevice(), false, -1, g_pPdStruct, nOffset, nSize);

    if (nResult) {
        return nResult;
    } else {
        return -1;
    }
}

qint64 Binary_Script::detectZIP(qint64 nOffset, qint64 nSize)
{
    qint64 nResult = XFormats::getFileFormatSize(XBinary::FT_ZIP, g_pBinary->getDevice(), false, -1, g_pPdStruct, nOffset, nSize);

    if (nResult) {
        return nResult;
    } else {
        return -1;
    }
}

bool Binary_Script::isOverlay()
{
    return (g_filePart == XBinary::FILEPART_OVERLAY);
}

bool Binary_Script::isResource()
{
    return (g_filePart == XBinary::FILEPART_RESOURCE);
}

bool Binary_Script::isDebugData()
{
    return (g_filePart == XBinary::FILEPART_DEBUGDATA);
}

bool Binary_Script::isFilePart()
{
    return (g_filePart != XBinary::FILEPART_HEADER);
}

QList<QVariant> Binary_Script::readBytes(qint64 nOffset, qint64 nSize, bool bReplaceZeroWithSpace)
{
    QList<QVariant> listResult;

    QByteArray baData = g_pBinary->read_array(nOffset, nSize, g_pPdStruct);
    qint32 _nSize = baData.size();
    listResult.reserve(_nSize);

    for (qint32 i = 0; i < _nSize; i++) {
        if (bReplaceZeroWithSpace && baData.at(i) == 0) {
            listResult.append(32);
        } else {
            quint32 nRecord = (quint8)(baData.at(i));
            listResult.append(nRecord);
        }
    }

    return listResult;
}

bool Binary_Script::isReleaseBuild()
{
    return g_pBinary->isReleaseBuild();
}

bool Binary_Script::isDebugBuild()
{
    return g_pBinary->isDebugBuild();
}

QStringList Binary_Script::getFormatMessages()
{
    _loadFmtChecking(true, g_pPdStruct);

    return g_listFormatMessages;
}

bool Binary_Script::isChecksumCorrect()
{
    _loadFmtChecking(true, g_pPdStruct);
    return !(XBinary::isFmtMsgCodePresent(&g_listFmtMsg, XBinary::FMT_MSG_CODE_INVALID_CHECKSUM, XBinary::FMT_MSG_TYPE_ERROR, g_pPdStruct));
}

bool Binary_Script::isEntryPointCorrect()
{
    _loadFmtChecking(false, g_pPdStruct);
    return !(XBinary::isFmtMsgCodePresent(&g_listFmtMsg, XBinary::FMT_MSG_CODE_INVALID_ENTRYPOINT, XBinary::FMT_MSG_TYPE_ERROR, g_pPdStruct));
}

bool Binary_Script::isSectionAlignmentCorrect()
{
    _loadFmtChecking(false, g_pPdStruct);
    return !(XBinary::isFmtMsgCodePresent(&g_listFmtMsg, XBinary::FMT_MSG_CODE_INVALID_SECTIONALIGNMENT, XBinary::FMT_MSG_TYPE_ERROR, g_pPdStruct));
}

bool Binary_Script::isFileAlignmentCorrect()
{
    _loadFmtChecking(false, g_pPdStruct);
    return !(XBinary::isFmtMsgCodePresent(&g_listFmtMsg, XBinary::FMT_MSG_CODE_INVALID_FILEALIGNMENT, XBinary::FMT_MSG_TYPE_ERROR, g_pPdStruct));
}

bool Binary_Script::isHeaderCorrect()
{
    _loadFmtChecking(false, g_pPdStruct);
    return !(XBinary::isFmtMsgCodePresent(&g_listFmtMsg, XBinary::FMT_MSG_CODE_INVALID_HEADER, XBinary::FMT_MSG_TYPE_ERROR, g_pPdStruct));
}

bool Binary_Script::isRelocsTableCorrect()
{
    _loadFmtChecking(false, g_pPdStruct);
    return !(XBinary::isFmtMsgCodePresent(&g_listFmtMsg, XBinary::FMT_MSG_CODE_INVALID_RELOCSTABLE, XBinary::FMT_MSG_TYPE_ERROR, g_pPdStruct));
}

bool Binary_Script::isImportTableCorrect()
{
    _loadFmtChecking(false, g_pPdStruct);
    return !(XBinary::isFmtMsgCodePresent(&g_listFmtMsg, XBinary::FMT_MSG_CODE_INVALID_IMPORTTABLE, XBinary::FMT_MSG_TYPE_ERROR, g_pPdStruct));
}

bool Binary_Script::isExportTableCorrect()
{
    _loadFmtChecking(false, g_pPdStruct);
    return !(XBinary::isFmtMsgCodePresent(&g_listFmtMsg, XBinary::FMT_MSG_CODE_INVALID_EXPORTTABLE, XBinary::FMT_MSG_TYPE_ERROR, g_pPdStruct));
}

bool Binary_Script::isResourcesTableCorrect()
{
    _loadFmtChecking(false, g_pPdStruct);
    return !(XBinary::isFmtMsgCodePresent(&g_listFmtMsg, XBinary::FMT_MSG_CODE_INVALID_RESOURCESTABLE, XBinary::FMT_MSG_TYPE_ERROR, g_pPdStruct));
}

bool Binary_Script::isSectionsTableCorrect()
{
    _loadFmtChecking(false, g_pPdStruct);
    return !(XBinary::isFmtMsgCodePresent(&g_listFmtMsg, XBinary::FMT_MSG_CODE_INVALID_SECTIONSTABLE, XBinary::FMT_MSG_TYPE_ERROR, g_pPdStruct));
}

XBinary::_MEMORY_MAP *Binary_Script::getMemoryMap()
{
    return &g_memoryMap;
}

XADDR Binary_Script::getBaseAddress()
{
    return g_nBaseAddress;
}

XBinary::PDSTRUCT *Binary_Script::getPdStruct()
{
    return g_pPdStruct;
}
