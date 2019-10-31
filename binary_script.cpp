// copyright (c) 2019 hors<horsicq@gmail.com>
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
#include "binary_script.h"

Binary_Script::Binary_Script(XBinary *pBinary)
{
    this->pBinary=pBinary;
}

Binary_Script::~Binary_Script()
{

}

qint64 Binary_Script::getSize()
{
    return pBinary->getSize();
}

bool Binary_Script::compare(QString sSignature, qint64 nOffset)
{
    return pBinary->compareSignature(sSignature,nOffset);
}

bool Binary_Script::compareEP(QString sSignature, qint64 nOffset)
{
    return pBinary->compareEntryPoint(sSignature,nOffset);
}

quint8 Binary_Script::readByte(qint64 nOffset)
{
    return pBinary->read_uint8(nOffset);
}

quint16 Binary_Script::readWord(qint64 nOffset)
{
    return pBinary->read_uint16(nOffset);
}

quint32 Binary_Script::readDword(qint64 nOffset)
{
    return pBinary->read_uint32(nOffset);
}

quint64 Binary_Script::readQword(qint64 nOffset)
{
    return pBinary->read_uint64(nOffset);
}

QString Binary_Script::getString(qint64 nOffset, qint64 nMaxSize)
{
    return pBinary->read_ansiString(nOffset,nMaxSize);
}

qint64 Binary_Script::findSignature(qint64 nOffset, qint64 nSize, QString sSignature)
{
    return pBinary->find_signature(nOffset,nSize,sSignature);
}

qint64 Binary_Script::findString(qint64 nOffset, qint64 nSize, QString sString)
{
    return pBinary->find_ansiString(nOffset,nSize,sString);
}

qint64 Binary_Script::getEntryPointOffset()
{
    return pBinary->getEntryPointOffset();
}

qint64 Binary_Script::getOverlayOffset()
{
    return pBinary->getOverlayOffset();
}

qint64 Binary_Script::getOverlaySize()
{
    return pBinary->getOverlaySize();
}

bool Binary_Script::isOverlayPresent()
{
    return pBinary->isOverlayPresent();
}

bool Binary_Script::compareOverlay(QString sSignature, qint64 nOffset)
{
    return pBinary->compareOverlay(sSignature,nOffset);
}

bool Binary_Script::isSignaturePresent(qint64 nOffset, qint64 nSize, QString sSignature)
{
    return pBinary->isSignaturePresent(nOffset,nSize,sSignature);
}

quint32 Binary_Script::swapBytes(quint32 nValue)
{
    return pBinary->swapBytes(nValue);
}

QString Binary_Script::getGeneralOptions()
{
    return "";
}
