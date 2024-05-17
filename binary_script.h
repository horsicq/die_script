/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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
#ifndef BINARY_SCRIPT_H
#define BINARY_SCRIPT_H

#include "xbinary.h"
#include "xcapstone.h"
#include "xjpeg.h"
#include "xzip.h"

class Binary_Script : public QObject {
    Q_OBJECT

public:
    struct OPTIONS {
        bool bIsDeepScan;
        bool bIsHeuristicScan;
        bool bIsVerbose;
        bool bIsProfiling;
    };

    explicit Binary_Script(XBinary *pBinary, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct);
    ~Binary_Script();

public slots:
    qint64 getSize();
    bool compare(const QString &sSignature, qint64 nOffset = 0);
    bool compareEP(const QString &sSignature, qint64 nOffset = 0);
    quint8 readByte(qint64 nOffset);
    qint16 readSByte(qint64 nOffset);  // qint16 not qint8 js shows as char
    quint16 readWord(qint64 nOffset);
    qint16 readSWord(qint64 nOffset);
    quint32 readDword(qint64 nOffset);
    qint32 readSDword(qint64 nOffset);
    quint64 readQword(qint64 nOffset);
    qint64 readSQword(qint64 nOffset);
    QString getString(qint64 nOffset, qint64 nMaxSize = 50);
    qint64 findSignature(qint64 nOffset, qint64 nSize, const QString &sSignature);
    qint64 findString(qint64 nOffset, qint64 nSize, const QString &sString);
    qint64 findByte(qint64 nOffset, qint64 nSize, quint8 nValue);
    qint64 findWord(qint64 nOffset, qint64 nSize, quint16 nValue);
    qint64 findDword(qint64 nOffset, qint64 nSize, quint32 nValue);
    qint64 getEntryPointOffset();
    qint64 getOverlayOffset();
    qint64 getOverlaySize();
    qint64 getAddressOfEntryPoint();
    bool isOverlayPresent();
    bool compareOverlay(const QString &sSignature, qint64 nOffset = 0);
    bool isSignaturePresent(qint64 nOffset, qint64 nSize, const QString &sSignature);
    quint32 swapBytes(quint32 nValue);
    virtual QString getGeneralOptions();
    qint64 RVAToOffset(qint64 nRVA);
    qint64 VAToOffset(qint64 nVA);
    qint64 OffsetToVA(qint64 nOffset);
    qint64 OffsetToRVA(qint64 nOffset);
    QString getFileDirectory();
    QString getFileBaseName();
    QString getFileCompleteSuffix();
    QString getFileSuffix();
    QString getSignature(qint64 nOffset, qint64 nSize);
    double calculateEntropy(qint64 nOffset, qint64 nSize);
    QString calculateMD5(qint64 nOffset, qint64 nSize);
    quint32 calculateCRC32(qint64 nOffset, qint64 nSize);
    quint16 crc16(qint64 nOffset, qint64 nSize, quint16 nInit = 0);
    quint32 crc32(qint64 nOffset, qint64 nSize, quint32 nInit = 0);
    quint32 adler32(qint64 nOffset, qint64 nSize);

    bool isSignatureInSectionPresent(quint32 nNumber, const QString &sSignature);
    qint64 getImageBase();  // Check mb quint64
    QString upperCase(const QString &sString);
    QString lowerCase(const QString &sString);
    bool isPlainText();
    bool isUTF8Text();
    bool isUnicodeText();
    bool isText();
    QString getHeaderString();
    qint32 getDisasmLength(qint64 nAddress);
    QString getDisasmString(qint64 nAddress);
    qint64 getDisasmNextAddress(qint64 nAddress);
    bool is16();
    bool is32();
    bool is64();

    bool isDeepScan();
    bool isHeuristicScan();
    bool isVerbose();
    bool isProfiling();

    quint8 read_uint8(qint64 nOffset);
    qint16 read_int8(qint64 nOffset);  // qint16 not qint8 / qint8 qjs shows as char
    quint16 read_uint16(qint64 nOffset, bool bIsBigEndian = false);
    qint16 read_int16(qint64 nOffset, bool bIsBigEndian = false);
    quint32 read_uint32(qint64 nOffset, bool bIsBigEndian = false);
    qint32 read_int32(qint64 nOffset, bool bIsBigEndian = false);
    quint64 read_uint64(qint64 nOffset, bool bIsBigEndian = false);
    qint64 read_int64(qint64 nOffset, bool bIsBigEndian = false);
    QString read_ansiString(qint64 nOffset, qint64 nMaxSize = 50);
    QString read_unicodeString(qint64 nOffset, qint64 nMaxSize = 50);
    QString read_utf8String(qint64 nOffset, qint64 nMaxSize = 50);
    QString read_ucsdString(qint64 nOffset);
    QString read_codePageString(qint64 nOffset, qint64 nMaxByteSize = 256, const QString &sCodePage = "System");
    QString bytesCountToString(quint64 nValue);

    qint64 find_ansiString(qint64 nOffset, qint64 nSize, const QString &sString);
    qint64 find_unicodeString(qint64 nOffset, qint64 nSize, const QString &sString);
    qint64 find_utf8String(qint64 nOffset, qint64 nSize, const QString &sString);

    QString read_UUID_bytes(qint64 nOffset);
    QString read_UUID(qint64 nOffset, bool bIsBigEndian = false);

    float read_float(qint64 nOffset, bool bIsBigEndian = false);
    double read_double(qint64 nOffset, bool bIsBigEndian = false);
    float read_float16(qint64 nOffset, bool bIsBigEndian = false);
    float read_float32(qint64 nOffset, bool bIsBigEndian = false);
    double read_float64(qint64 nOffset, bool bIsBigEndian = false);
    quint32 read_uint24(qint64 nOffset, bool bIsBigEndian = false);
    qint32 read_int24(qint64 nOffset, bool bIsBigEndian = false);
    quint8 read_bcd_uint8(qint64 nOffset);
    quint16 read_bcd_uint16(qint64 nOffset, bool bIsBigEndian = false);
    quint16 read_bcd_uint32(qint64 nOffset, bool bIsBigEndian = false);
    quint16 read_bcd_uint64(qint64 nOffset, bool bIsBigEndian = false);

    bool isJpeg();
    QString getJpegComment();
    QString getJpegDqtMD5();
    bool isJpegChunkPresent(qint32 nID);
    bool isJpegExifPresent();
    QString getJpegExifCameraName();

    QString getOperationSystemName();
    QString getOperationSystemVersion();
    QString getOperationSystemOptions();
    bool isSigned();

    // alliases
    quint8 U8(qint64 nOffset);
    qint16 I8(qint64 nOffset);  // qint16 not qint8 / qint8 qjs shows as char
    quint16 U16(qint64 nOffset, bool bIsBigEndian = false);
    qint16 I16(qint64 nOffset, bool bIsBigEndian = false);
    quint32 U24(qint64 nOffset, bool bIsBigEndian = false);
    qint32 I24(qint64 nOffset, bool bIsBigEndian = false);
    quint32 U32(qint64 nOffset, bool bIsBigEndian = false);
    qint32 I32(qint64 nOffset, bool bIsBigEndian = false);
    quint64 U64(qint64 nOffset, bool bIsBigEndian = false);
    qint64 I64(qint64 nOffset, bool bIsBigEndian = false);
    float F16(qint64 nOffset, bool bIsBigEndian = false);
    float F32(qint64 nOffset, bool bIsBigEndian = false);
    double F64(qint64 nOffset, bool bIsBigEndian = false);

    QString SA(qint64 nOffset, qint64 nMaxSize = 50);
    QString SU16(qint64 nOffset, qint64 nMaxSize = 50);
    QString SU8(qint64 nOffset, qint64 nMaxSize = 50);
    QString UCSD(qint64 nOffset);
    QString SC(qint64 nOffset, qint64 nMaxByteSize = 256, const QString &sCodePage = "System");

    qint64 Sz();
    qint64 fSig(qint64 nOffset, qint64 nSize, const QString &sSignature);
    qint64 fStr(qint64 nOffset, qint64 nSize, const QString &sString);
    bool c(const QString &sSignature, qint64 nOffset = 0);

    // function X.U8(a) { return File.read_uint8(a) }
    // function X.I8(a) { return File.read_int8(a) }

    // function X.U16(a,b) { return File.read_uint16(a,b) }
    // function X.I16(a,b) { return File.read_int16(a,b) }
    // function X.F16(a,b) { return File.read_float16(a,b) }

    // function X.U24(a,b) { return File.read_uint24(a,b) }
    // function X.I24(a,b) { return File.read_int24(a,b) }

    // function X.U32(a,b) { return File.read_uint32(a,b) }
    // function X.I32(a,b) { return File.read_int32(a,b) }
    // function X.F32(a,b) { return File.read_float32(a,b) }

    // function X.U64(a,b) { return File.read_uint64(a,b) }
    // function X.I64(a,b) { return File.read_int64(a,b) }
    // function X.F64(a,b) { return File.read_float64(a,b) }

    // function X.SA(a,b) { return File.read_ansiString(a,b) }
    // function X.SC(a,b,c) { return File.read_codePageString(a,b,c) }
    // function X.UСSD(a,b) { return File.read_ucsdString(a,b) }
    // function X.SU8(a,b,c) { return File.read_utf8String(a,b,c) }
    // function X.SU16(a,b,c) { return File.read_unicodeString(a,b,c) }

    // function X.Sz() { return File.getSize() }
    // function X.fSig(a,b,c) { return File.findSignature(a,b,c) }
    // function X.fStr(a,b,c) { return File.findString(a,b,c) }
    // function X.c(a,b) { return File.compare(a,b) }

private:
    void _fixOffsetAndSize(qint64 *pnOffset, qint64 *pnSize);
    QElapsedTimer *startProfiling();
    void finishProfiling(QElapsedTimer *pElapsedTimer, QString sInfo);

protected:
    XBinary::_MEMORY_MAP *getMemoryMap();
    XADDR getBaseAddress();
    XBinary::PDSTRUCT *getPdStruct();
    QList<XArchive::RECORD> *getArchiveRecords();

signals:
    void errorMessage(const QString &sErrorMessage);
    void warningMessage(const QString &sWarningMessage);
    void infoMessage(const QString &sInfoMessage);

private:
    XBinary *g_pBinary;
    OPTIONS *g_pOptions;
    XBinary::PDSTRUCT *g_pPdStruct;
    XBinary::_MEMORY_MAP g_memoryMap;
    XADDR g_nBaseAddress;
    csh g_disasmHandle;
    qint64 g_nSize;
    qint64 g_nEntryPointOffset;
    qint64 g_nEntryPointAddress;
    qint64 g_nOverlayOffset;
    qint64 g_nOverlaySize;
    qint64 g_bIsOverlayPresent;
    QString g_sHeaderSignature;
    qint32 g_nHeaderSignatureSize;
    QString g_sEntryPointSignature;
    qint32 g_nEntryPointSignatureSize;
    QString g_sOverlaySignature;
    qint32 g_nOverlaySignatureSize;
    bool g_bIsPlainText;
    bool g_bIsUTF8Text;
    bool g_bIsUnicodeText;
    QString g_sHeaderString;
    QString g_sFileDirectory;
    QString g_sFileBaseName;
    QString g_sFileCompleteSuffix;
    QString g_sFileSuffix;
    XBinary::OSINFO g_osInfo;
    // JPEG
    bool g_bIsJpeg;
    XJpeg *g_pJpeg;
    QList<XJpeg::CHUNK> g_listJpegChunks;
    XBinary::OFFSETSIZE g_osJpegExif;
    QList<XTiff::CHUNK> g_listJpegExifChunks;
    QString g_sJpegExifCameraName;
    bool g_bIsBigEndian;
    bool g_bIsSigned;
};

#endif  // BINARY_SCRIPT_H
