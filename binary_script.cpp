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
#include "binary_script.h"

Binary_Script::Binary_Script(XBinary *pBinary)
{
    this->pBinary=pBinary;

    nSize=pBinary->getSize();
    memoryMap=pBinary->getMemoryMap();
    nBaseAddress=pBinary->getBaseAddress();

    nEntryPointOffset=pBinary->getEntryPointOffset(&memoryMap);
    nEntryPointAddress=pBinary->getEntryPointAddress(&memoryMap);
    nOverlayOffset=pBinary->getOverlayOffset(&memoryMap);
    nOverlaySize=pBinary->getOverlaySize(&memoryMap);
    bIsOverlayPresent=pBinary->isOverlayPresent(&memoryMap);

    sHeaderSignature=pBinary->getSignature(0,256);
    nHeaderSignatureSize=sHeaderSignature.size();

    if(nEntryPointOffset>0)
    {
        sEntryPointSignature=pBinary->getSignature(nEntryPointOffset,256);
        nEntryPointSignatureSize=sEntryPointSignature.size();
    }

    if(nOverlayOffset>0)
    {
        sOverlaySignature=pBinary->getSignature(nOverlayOffset,256);
        nOverlaySignatureSize=sOverlaySignature.size();
    }

    sFileDirectory=XBinary::getDeviceDirectory(pBinary->getDevice());
    sFileBaseName=XBinary::getDeviceFileBaseName(pBinary->getDevice());
    sFileCompleteSuffix=XBinary::getDeviceFileCompleteSuffix(pBinary->getDevice());
    sFileSuffix=XBinary::getDeviceFileSuffix(pBinary->getDevice());
}

Binary_Script::~Binary_Script()
{

}

qint64 Binary_Script::getSize()
{
    return nSize;
}

bool Binary_Script::compare(QString sSignature, qint64 nOffset)
{
    bool bResult=false;

    int nSignatureSize=sSignature.size();

    if((nSignatureSize+nOffset<nHeaderSignatureSize)&&(!sSignature.contains('$'))&&(!sSignature.contains('#')))
    {
        return pBinary->compareSignatureStrings(sHeaderSignature.mid(nOffset*2,nSignatureSize*2),sSignature);
    }
    else
    {
        return pBinary->compareSignature(&memoryMap,sSignature,nOffset);
    }

    return bResult;
}

bool Binary_Script::compareEP(QString sSignature, qint64 nOffset)
{
    bool bResult=false;

    int nSignatureSize=sSignature.size();

    if((nSignatureSize+nOffset<nEntryPointSignatureSize)&&(!sSignature.contains('$'))&&(!sSignature.contains('#')))
    {
        bResult=pBinary->compareSignatureStrings(sEntryPointSignature.mid(nOffset*2,nSignatureSize*2),sSignature);
    }
    else
    {
        bResult=pBinary->compareEntryPoint(&memoryMap,sSignature,nOffset);
    }

    return bResult;
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
    return pBinary->find_signature(&memoryMap,nOffset,nSize,sSignature);
}

qint64 Binary_Script::findString(qint64 nOffset, qint64 nSize, QString sString)
{
    return pBinary->find_ansiString(nOffset,nSize,sString);
}

qint64 Binary_Script::findByte(qint64 nOffset, qint64 nSize, quint8 value)
{
    return pBinary->find_uint8(nOffset,nSize,value);
}

qint64 Binary_Script::findWord(qint64 nOffset, qint64 nSize, quint16 value)
{
    return pBinary->find_uint16(nOffset,nSize,value);
}

qint64 Binary_Script::findDword(qint64 nOffset, qint64 nSize, quint32 value)
{
    return pBinary->find_uint32(nOffset,nSize,value);
}

qint64 Binary_Script::getEntryPointOffset()
{
    return nEntryPointOffset;
}

qint64 Binary_Script::getOverlayOffset()
{
    return nOverlayOffset;
}

qint64 Binary_Script::getOverlaySize()
{
    return nOverlaySize;
}

qint64 Binary_Script::getAddressOfEntryPoint()
{
    return nEntryPointAddress;
}

bool Binary_Script::isOverlayPresent()
{
    return bIsOverlayPresent;
}

bool Binary_Script::compareOverlay(QString sSignature, qint64 nOffset)
{
    bool bResult=false;

    int nSignatureSize=sSignature.size();

    if((nSignatureSize+nOffset<nOverlaySignatureSize)&&(!sSignature.contains('$'))&&(!sSignature.contains('#')))
    {
        bResult=pBinary->compareSignatureStrings(sOverlaySignature.mid(nOffset*2,nSignatureSize*2),sSignature);
    }
    else
    {
        bResult=pBinary->compareOverlay(&memoryMap,sSignature,nOffset);
    }

    return bResult;
}

bool Binary_Script::isSignaturePresent(qint64 nOffset, qint64 nSize, QString sSignature)
{
    return pBinary->isSignaturePresent(&memoryMap,nOffset,nSize,sSignature);
}

quint32 Binary_Script::swapBytes(quint32 nValue)
{
    return pBinary->swapBytes(nValue);
}

QString Binary_Script::getGeneralOptions()
{
    return "";
}

qint64 Binary_Script::RVAToOffset(qint64 nRVA)
{
    return pBinary->addressToOffset(&memoryMap,nRVA+nBaseAddress);
}

qint64 Binary_Script::VAToOffset(qint64 nVA)
{
    return pBinary->addressToOffset(&memoryMap,nVA);
}

qint64 Binary_Script::OffsetToVA(qint64 nOffset)
{
    return pBinary->offsetToAddress(&memoryMap,nOffset);
}

qint64 Binary_Script::OffsetToRVA(qint64 nOffset)
{
    qint64 nResult=pBinary->offsetToAddress(&memoryMap,nOffset);

    if(nResult!=-1)
    {
        nResult-=nBaseAddress;
    }

    return nResult;
}

QString Binary_Script::getFileDirectory()
{
    return sFileDirectory;
}

QString Binary_Script::getFileBaseName()
{
    return sFileBaseName;
}

QString Binary_Script::getFileCompleteSuffix()
{
    return sFileCompleteSuffix;
}

QString Binary_Script::getFileSuffix()
{
    return sFileSuffix;
}

QString Binary_Script::getSignature(qint64 nOffset, qint64 nSize)
{
    return pBinary->getSignature(nOffset,nSize);
}

double Binary_Script::calculateEntropy(qint64 nOffset, qint64 nSize)
{
    return pBinary->getEntropy(nOffset,nSize);
}

QString Binary_Script::calculateMD5(qint64 nOffset, qint64 nSize)
{
    return pBinary->getHash(XBinary::HASH_MD5,nOffset,nSize);
}

bool Binary_Script::isSignatureInSectionPresent(quint32 nNumber, QString sSignature)
{
    return pBinary->isSignatureInLoadSectionPresent(&memoryMap,nNumber,sSignature);
}

qint64 Binary_Script::getImageBase()
{
    return memoryMap.nBaseAddress;
}

QString Binary_Script::upperCase(QString sString)
{
    return sString.toUpper();
}

QString Binary_Script::lowerCase(QString sString)
{
    return sString.toLower();
}
