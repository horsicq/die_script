/* Copyright (c) 2019-2021 hors<horsicq@gmail.com>
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

Binary_Script::Binary_Script(XBinary *pBinary)
{
    this->g_pBinary=pBinary;

    g_nSize=pBinary->getSize();
    g_memoryMap=pBinary->getMemoryMap();
    g_nBaseAddress=pBinary->getBaseAddress();

    g_nEntryPointOffset=pBinary->getEntryPointOffset(&g_memoryMap);
    g_nEntryPointAddress=pBinary->getEntryPointAddress(&g_memoryMap);
    g_nOverlayOffset=pBinary->getOverlayOffset(&g_memoryMap);
    g_nOverlaySize=pBinary->getOverlaySize(&g_memoryMap);
    g_bIsOverlayPresent=pBinary->isOverlayPresent(&g_memoryMap);

    g_sHeaderSignature=pBinary->getSignature(0,256); // TODO const
    g_nHeaderSignatureSize=g_sHeaderSignature.size();

    if(g_nEntryPointOffset>0)
    {
        g_sEntryPointSignature=pBinary->getSignature(g_nEntryPointOffset,256); // TODO const
        g_nEntryPointSignatureSize=g_sEntryPointSignature.size();
    }

    if(g_nOverlayOffset>0)
    {
        g_sOverlaySignature=pBinary->getSignature(g_nOverlayOffset,256); // TODO const
        g_nOverlaySignatureSize=g_sOverlaySignature.size();
    }

    g_sFileDirectory=XBinary::getDeviceDirectory(pBinary->getDevice());
    g_sFileBaseName=XBinary::getDeviceFileBaseName(pBinary->getDevice());
    g_sFileCompleteSuffix=XBinary::getDeviceFileCompleteSuffix(pBinary->getDevice());
    g_sFileSuffix=XBinary::getDeviceFileSuffix(pBinary->getDevice());

    g_bIsPlainText=pBinary->isPlainTextType();
}

Binary_Script::~Binary_Script()
{

}

qint64 Binary_Script::getSize()
{
    return g_nSize;
}

bool Binary_Script::compare(QString sSignature, qint64 nOffset)
{
    bool bResult=false;

    qint32 nSignatureSize=sSignature.size();

    if((nSignatureSize+nOffset<g_nHeaderSignatureSize)&&(!sSignature.contains('$'))&&(!sSignature.contains('#')))
    {
        bResult=g_pBinary->compareSignatureStrings(g_sHeaderSignature.mid(nOffset*2,nSignatureSize*2),sSignature);
    }
    else
    {
        bResult=g_pBinary->compareSignature(&g_memoryMap,sSignature,nOffset);
    }

    return bResult;
}

bool Binary_Script::compareEP(QString sSignature, qint64 nOffset)
{
    bool bResult=false;

    qint32 nSignatureSize=sSignature.size();

    if((nSignatureSize+nOffset<g_nEntryPointSignatureSize)&&(!sSignature.contains('$'))&&(!sSignature.contains('#')))
    {
        bResult=g_pBinary->compareSignatureStrings(g_sEntryPointSignature.mid(nOffset*2,nSignatureSize*2),sSignature);
    }
    else
    {
        bResult=g_pBinary->compareEntryPoint(&g_memoryMap,sSignature,nOffset);
    }

    return bResult;
}

quint8 Binary_Script::readByte(qint64 nOffset)
{
    return g_pBinary->read_uint8(nOffset);
}

quint16 Binary_Script::readWord(qint64 nOffset)
{
    return g_pBinary->read_uint16(nOffset);
}

quint32 Binary_Script::readDword(qint64 nOffset)
{
    return g_pBinary->read_uint32(nOffset);
}

quint64 Binary_Script::readQword(qint64 nOffset)
{
    return g_pBinary->read_uint64(nOffset);
}

QString Binary_Script::getString(qint64 nOffset, qint64 nMaxSize)
{
    return g_pBinary->read_ansiString(nOffset,nMaxSize);
}

qint64 Binary_Script::findSignature(qint64 nOffset, qint64 nSize, QString sSignature)
{
    return g_pBinary->find_signature(&g_memoryMap,nOffset,nSize,sSignature);
}

qint64 Binary_Script::findString(qint64 nOffset, qint64 nSize, QString sString)
{
    return g_pBinary->find_ansiString(nOffset,nSize,sString);
}

qint64 Binary_Script::findByte(qint64 nOffset, qint64 nSize, quint8 nValue)
{
    return g_pBinary->find_uint8(nOffset,nSize,nValue);
}

qint64 Binary_Script::findWord(qint64 nOffset, qint64 nSize, quint16 nValue)
{
    return g_pBinary->find_uint16(nOffset,nSize,nValue);
}

qint64 Binary_Script::findDword(qint64 nOffset, qint64 nSize, quint32 nValue)
{
    return g_pBinary->find_uint32(nOffset,nSize,nValue);
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

bool Binary_Script::compareOverlay(QString sSignature, qint64 nOffset)
{
    bool bResult=false;

    qint32 nSignatureSize=sSignature.size();

    if((nSignatureSize+nOffset<g_nOverlaySignatureSize)&&(!sSignature.contains('$'))&&(!sSignature.contains('#')))
    {
        bResult=g_pBinary->compareSignatureStrings(g_sOverlaySignature.mid(nOffset*2,nSignatureSize*2),sSignature);
    }
    else
    {
        bResult=g_pBinary->compareOverlay(&g_memoryMap,sSignature,nOffset);
    }

    return bResult;
}

bool Binary_Script::isSignaturePresent(qint64 nOffset, qint64 nSize, QString sSignature)
{
    return g_pBinary->isSignaturePresent(&g_memoryMap,nOffset,nSize,sSignature);
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
    return g_pBinary->addressToOffset(&g_memoryMap,nRVA+g_nBaseAddress);
}

qint64 Binary_Script::VAToOffset(qint64 nVA)
{
    return g_pBinary->addressToOffset(&g_memoryMap,nVA);
}

qint64 Binary_Script::OffsetToVA(qint64 nOffset)
{
    return g_pBinary->offsetToAddress(&g_memoryMap,nOffset);
}

qint64 Binary_Script::OffsetToRVA(qint64 nOffset)
{
    qint64 nResult=g_pBinary->offsetToAddress(&g_memoryMap,nOffset);

    if(nResult!=-1)
    {
        nResult-=g_nBaseAddress;
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
    return g_pBinary->getSignature(nOffset,nSize);
}

double Binary_Script::calculateEntropy(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->getEntropy(nOffset,nSize);
}

QString Binary_Script::calculateMD5(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->getHash(XBinary::HASH_MD5,nOffset,nSize);
}

bool Binary_Script::isSignatureInSectionPresent(quint32 nNumber, QString sSignature)
{
    return g_pBinary->isSignatureInLoadSegmentPresent(&g_memoryMap,nNumber,sSignature);
}

qint64 Binary_Script::getImageBase()
{
    return g_memoryMap.nModuleAddress;
}

QString Binary_Script::upperCase(QString sString)
{
    return sString.toUpper();
}

QString Binary_Script::lowerCase(QString sString)
{
    return sString.toLower();
}

bool Binary_Script::isPlainText()
{
    return g_bIsPlainText;
}
