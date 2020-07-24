// copyright (c) 2019-2020 hors<horsicq@gmail.com>
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

#include <QObject>
#include <QIODevice>
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
    quint16 readWord(qint64 nOffset);
    quint32 readDword(qint64 nOffset);
    quint64 readQword(qint64 nOffset);
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
    bool compareOverlay(QString sSignature, qint64 nOffset=0);
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
    XBinary::_MEMORY_MAP memoryMap;
    qint64 nBaseAddress;
    QString sFileDirectory;
    QString sFileBaseName;
    QString sFileCompleteSuffix;
    QString sFileSuffix;

private:
    XBinary *pBinary;
    qint64 nSize;
    qint64 nEntryPointOffset;
    qint64 nEntryPointAddress;
    qint64 nOverlayOffset;
    qint64 nOverlaySize;
    qint64 bIsOverlayPresent;
    QString sHeaderSignature;
    qint32 nHeaderSignatureSize;
    QString sEntryPointSignature;
    qint32 nEntryPointSignatureSize;
    QString sOverlaySignature;
    qint32 nOverlaySignatureSize;
    bool bIsPlainText;
};

#endif // BINARY_SCRIPT_H
