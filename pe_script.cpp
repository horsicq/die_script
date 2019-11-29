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
#include "pe_script.h"

PE_Script::PE_Script(XPE *pPE) : MSDOS_Script(pPE)
{
    this->pPE=pPE;

    nNumberOfSections=pPE->getFileHeader_NumberOfSections();

    listSH=pPE->getSectionHeaders();
    listSR=pPE->getSectionRecords(&listSH,pPE->isImage());
    listSN=pPE->getSectionNames(&listSR);

    cliInfo=pPE->getCliInfo(true,&listMM);
    listResources=pPE->getResources(&listMM);

    nNumberOfResources=listResources.count();

    QList<XPE::IMPORT_HEADER> listImports=pPE->getImports(&listMM);

    nNumberOfImports=listImports.count();

    bIsNETPresent=pPE->isNETPresent();
    bIs64=pPE->is64();
    bIsDll=pPE->isDll();
    bIsConsole=pPE->isConsole();
    bIsSignPresent=pPE->isSignPresent();
    bIisRichSignaturePresent=pPE->isRichSignaturePresent();
}

PE_Script::~PE_Script()
{

}

quint16 PE_Script::getNumberOfSections()
{
    return nNumberOfSections;
}

QString PE_Script::getSectionName(quint32 nNumber)
{
    return pPE->getSection_NameAsString(nNumber);
}

quint32 PE_Script::getSectionVirtualSize(quint32 nNumber)
{
    return pPE->getSection_VirtualSize(nNumber);
}

quint32 PE_Script::getSectionVirtualAddress(quint32 nNumber)
{
    return pPE->getSection_VirtualAddress(nNumber);
}

quint32 PE_Script::getSectionFileSize(quint32 nNumber)
{
    return pPE->getSection_SizeOfRawData(nNumber);
}

quint32 PE_Script::getSectionFileOffset(quint32 nNumber)
{
    return pPE->getSection_PointerToRawData(nNumber);
}

quint32 PE_Script::getSectionCharacteristics(quint32 nNumber)
{
    return pPE->getSection_Characteristics(nNumber);
}

quint32 PE_Script::getNumberOfResources()
{
    return nNumberOfResources;
}

bool PE_Script::isSectionNamePresent(QString sSectionName)
{
    return pPE->isSectionNamePresent(sSectionName);
}

bool PE_Script::isNET()
{
    return bIsNETPresent;
}

bool PE_Script::isPEPlus()
{
    return bIs64;
}

QString PE_Script::getGeneralOptions()
{
    QString sType;
    QString sBits;

    if(pPE->is64())
    {
        sBits="64";
    }
    else
    {
        sBits="32";
    }

    if(pPE->isDll())
    {
        sType="DLL";
    }
    else if(pPE->isDriver())
    {
        sType="Driver";
    }
    else
    {
        sType="EXE";
    }

    return QString("%1%2").arg(sType).arg(sBits);
}

quint32 PE_Script::getResourceIdByNumber(quint32 nNumber)
{
    return pPE->getResourceIdByNumber(nNumber,&listResources);
}

QString PE_Script::getResourceNameByNumber(quint32 nNumber)
{
    return pPE->getResourceNameByNumber(nNumber,&listResources);
}

qint64 PE_Script::getResourceOffsetByNumber(quint32 nNumber)
{
    return pPE->getResourceOffsetByNumber(nNumber,&listResources);
}

qint64 PE_Script::getResourceSizeByNumber(quint32 nNumber)
{
    return pPE->getResourceSizeByNumber(nNumber,&listResources);
}

quint32 PE_Script::getResourceTypeByNumber(quint32 nNumber)
{
    return pPE->getResourceTypeByNumber(nNumber,&listResources);
}

bool PE_Script::isNETStringPresent(QString sString)
{
    return pPE->isNETAnsiStringPresent(sString,&cliInfo);
}

bool PE_Script::isNETUnicodeStringPresent(QString sString)
{
    return pPE->isNETUnicodeStringPresent(sString,&cliInfo);
}

qint32 PE_Script::getNumberOfImports()
{
    return nNumberOfImports;
}

QString PE_Script::getImportLibraryName(quint32 nNumber)
{
    return pPE->getImportLibraryName(nNumber); // TODO
}

bool PE_Script::isLibraryPresent(QString sLibraryName)
{
    return pPE->isImportLibraryPresentI(sLibraryName,&listImports);
}

bool PE_Script::isLibraryFunctionPresent(QString sLibraryName, QString sFunctionName)
{
    return pPE->isImportFunctionPresentI(sLibraryName,sFunctionName,&listImports);
}

QString PE_Script::getImportFunctionName(quint32 nImport, quint32 nFunctionNumber)
{
    return pPE->getImportFunctionName(nImport,nFunctionNumber,&listImports);
}

qint32 PE_Script::getImportSection()
{
    return pPE->getImportSection();
}

qint32 PE_Script::getResourceSection()
{
    return pPE->getResourcesSection();
}

qint32 PE_Script::getEntryPointSection()
{
    return pPE->getEntryPointSection();
}

qint32 PE_Script::getRelocsSection()
{
    return pPE->getRelocsSection();
}

quint8 PE_Script::getMajorLinkerVersion()
{
    return pPE->getOptionalHeader_MajorLinkerVersion();
}

quint8 PE_Script::getMinorLinkerVersion()
{
    return pPE->getOptionalHeader_MinorLinkerVersion();
}

QString PE_Script::getManifest()
{
    return pPE->getResourceManifest(&listResources);
}

QString PE_Script::getVersionStringInfo(QString sKey)
{
    return pPE->getResourceVersionValue(sKey);
}

qint32 PE_Script::getNumberOfImportThunks(quint32 nNumber)
{
    return pPE->getNumberOfImportThunks(nNumber);
}

qint32 PE_Script::getNumberOfRichIDs()
{
    return pPE->getNumberOfRichIDs();
}

bool PE_Script::isRichVersionPresent(quint32 nVersion)
{
    return pPE->isRichVersionPresent(nVersion);
}

qint64 PE_Script::getResourceNameOffset(QString sName)
{
    return pPE->getResourceNameOffset(sName);
}

bool PE_Script::isResourceNamePresent(QString sName)
{
    return pPE->isResourceNamePresent(sName);
}

qint64 PE_Script::getDosStubOffset()
{
    return pPE->getDosStubOffset();
}

qint64 PE_Script::getDosStubSize()
{
    return pPE->getDosStubSize();
}

bool PE_Script::isDosStubPresent()
{
    return pPE->isDosStubPresent();
}

QString PE_Script::getCompilerVersion()
{
    return QString("%1.%2")
            .arg(pPE->getOptionalHeader_MajorLinkerVersion())
            .arg(pPE->getOptionalHeader_MinorLinkerVersion());
}

bool PE_Script::isConsole()
{
    return bIsConsole;
}

bool PE_Script::isSignedFile()
{
    return bIsSignPresent;
}

bool PE_Script::isRichSignaturePresent()
{
    return bIisRichSignaturePresent;
}

bool PE_Script::isSignatureInSectionPresent(quint32 nNumber, QString sSignature)
{
    return pPE->isSignatureInLoadSectionPresent(nNumber,sSignature);
}

QString PE_Script::getSectionNameCollision(QString sString1, QString sString2)
{
    return pPE->getStringCollision(&listSN,sString1,sString2);
}

qint32 PE_Script::getSectionNumber(QString sSectionName)
{
    return pPE->getSectionNumber(sSectionName,&listSH);
}

bool PE_Script::isDll()
{
    return bIsDll;
}

QString PE_Script::getNETVersion()
{
    return cliInfo.sCLI_MetaData_Version;
}

bool PE_Script::compareEP_NET(QString sSignature, qint64 nOffset)
{
    return pPE->compareSignatureOnAddress(sSignature,nBaseAddress+cliInfo.nEntryPoint+nOffset);
}

quint32 PE_Script::getSizeOfCode()
{
    return pPE->getOptionalHeader_SizeOfCode();
}

quint32 PE_Script::getSizeOfUninitializedData()
{
    return pPE->getOptionalHeader_SizeOfUninitializedData();
}

QString PE_Script::getPEFileVersion(QString sFileName)
{
    QString sResult;

    QFile file;

    file.setFileName(sFileName);

    if(file.open(QIODevice::ReadOnly))
    {
        XPE pe(&file);

        sResult=pe.getFileVersion();

        file.close();
    }

    return sResult;
}
