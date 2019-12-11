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

    resVersion=pPE->getResourceVersion(&listResources);

    nNumberOfResources=listResources.count();

    listImports=pPE->getImports(&listMM);

    nNumberOfImports=listImports.count();

    listRich=pPE->getRichSignatureRecords();

    nNumberOfRich=listRich.count();

    bIsNETPresent=pPE->isNETPresent();
    bIs64=pPE->is64();
    bIsDll=pPE->isDll();
    bIsConsole=pPE->isConsole();
    bIsSignPresent=pPE->isSignPresent();
    bIisRichSignaturePresent=pPE->isRichSignaturePresent();

    nDosStubOffset=pPE->getDosStubOffset();
    nDosStubSize=pPE->getDosStubSize();
    bIsDosStubPresent=pPE->isDosStubPresent();

    nImportSection=pPE->getImportSection(&listMM);
    nResourcesSection=pPE->getResourcesSection(&listMM);
    nEntryPointSection=pPE->getEntryPointSection(&listMM);
    nRelocsSection=pPE->getRelocsSection(&listMM);

    nMajorLinkerVersion=pPE->getOptionalHeader_MajorLinkerVersion();
    nMinorLinkerVersion=pPE->getOptionalHeader_MinorLinkerVersion();
    nSizeOfCode=pPE->getOptionalHeader_SizeOfCode();
    nSizeOfUninitializedData=pPE->getOptionalHeader_SizeOfUninitializedData();

    sCompilerVersion=QString("%1.%2").arg(nMajorLinkerVersion).arg(nMinorLinkerVersion);

    sGeneralOptions=QString("%1%2").arg(XPE::getTypesS().value(pPE->getType())).arg(bIs64?("64"):("32"));
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
    return pPE->isSectionNamePresent(sSectionName,&listSH); // TODO !!! listNames!
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
    return sGeneralOptions;
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
    return pPE->getImportLibraryName(nNumber,&listImports);
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
    return nImportSection;
}

qint32 PE_Script::getResourceSection()
{
    return nResourcesSection;
}

qint32 PE_Script::getEntryPointSection()
{
    return nEntryPointSection;
}

qint32 PE_Script::getRelocsSection()
{
    return nRelocsSection;
}

quint8 PE_Script::getMajorLinkerVersion()
{
    return nMajorLinkerVersion;
}

quint8 PE_Script::getMinorLinkerVersion()
{
    return nMinorLinkerVersion;
}

QString PE_Script::getManifest()
{
    return pPE->getResourceManifest(&listResources);
}

QString PE_Script::getVersionStringInfo(QString sKey)
{
    return pPE->getResourceVersionValue(sKey,&resVersion);
}

qint32 PE_Script::getNumberOfImportThunks(quint32 nNumber)
{
    return pPE->getNumberOfImportThunks(nNumber,&listImports);
}

qint32 PE_Script::getNumberOfRichIDs()
{
    return nNumberOfRich;
}

bool PE_Script::isRichVersionPresent(quint32 nVersion)
{
    return pPE->isRichVersionPresent(nVersion,&listRich);
}

qint64 PE_Script::getResourceNameOffset(QString sName)
{
    return pPE->getResourceNameOffset(sName,&listResources);
}

bool PE_Script::isResourceNamePresent(QString sName)
{
    return pPE->isResourceNamePresent(sName,&listResources);
}

qint64 PE_Script::getDosStubOffset()
{
    return nDosStubOffset;
}

qint64 PE_Script::getDosStubSize()
{
    return nDosStubSize;
}

bool PE_Script::isDosStubPresent()
{
    return bIsDosStubPresent;
}

QString PE_Script::getCompilerVersion()
{
    return sCompilerVersion;
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

QString PE_Script::getSectionNameCollision(QString sString1, QString sString2)
{
    return pPE->getStringCollision(&listSN,sString1,sString2);
}

qint32 PE_Script::getSectionNumber(QString sSectionName)
{
    return pPE->getSectionNumber(sSectionName,&listSH); // TODO listNames !!!
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
    return nSizeOfCode;
}

quint32 PE_Script::getSizeOfUninitializedData()
{
    return nSizeOfUninitializedData; // TODO
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

QString PE_Script::getFileVersion()
{
    return pPE->getFileVersion(&resVersion);
}
