// Copyright (c) 2019-2021 hors<horsicq@gmail.com>
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
#ifndef BINARY_SCRIPT_H
#define BINARY_SCRIPT_H

#include "xbinary.h"

class Binary_Script : public QObject
{
    Q_OBJECT

public:
    explicit Binary_Script(XBinary *pBinary);
    ~Binary_Script();

public slots:
    qint64 getSize();
    bool compare(QString sSignature,qint64 nOffset=0);
    bool compareEP(QString sSignature,qint64 nOffset=0);
    quint8 readByte(qint64 nOffset);
    qint8 readSByte(qint64 nOffset);
    quint16 readWord(qint64 nOffset);
    qint16 readSWord(qint64 nOffset);
    quint32 readDword(qint64 nOffset);
    qint32 readSDword(qint64 nOffset);
    quint64 readQword(qint64 nOffset);
    qint64 readSQword(qint64 nOffset);
    QString getString(qint64 nOffset,qint64 nMaxSize=50);
    qint64 findSignature(qint64 nOffset,qint64 nSize,QString sSignature);
    qint64 findString(qint64 nOffset,qint64 nSize,QString sString);
    qint64 findByte(qint64 nOffset,qint64 nSize,quint8 nValue);
    qint64 findWord(qint64 nOffset,qint64 nSize,quint16 nValue);
    qint64 findDword(qint64 nOffset,qint64 nSize,quint32 nValue);
    qint64 getEntryPointOffset();
    qint64 getOverlayOffset();
    qint64 getOverlaySize();
    qint64 getAddressOfEntryPoint();
    bool isOverlayPresent();
    bool compareOverlay(QString sSignature,qint64 nOffset=0);
    bool isSignaturePresent(qint64 nOffset,qint64 nSize,QString sSignature);
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
    QString getSignature(qint64 nOffset,qint64 nSize);
    double calculateEntropy(qint64 nOffset,qint64 nSize);
    QString calculateMD5(qint64 nOffset,qint64 nSize);
    bool isSignatureInSectionPresent(quint32 nNumber,QString sSignature);
    qint64 getImageBase();
    QString upperCase(QString sString);
    QString lowerCase(QString sString);
    bool isPlainText();

protected:
    XBinary::_MEMORY_MAP g_memoryMap;
    qint64 g_nBaseAddress;

private:
    XBinary *g_pBinary;
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
    QString g_sFileDirectory;
    QString g_sFileBaseName;
    QString g_sFileCompleteSuffix;
    QString g_sFileSuffix;
};

#endif // BINARY_SCRIPT_H
