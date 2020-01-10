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

    cliInfo=pPE->getCliInfo(true,&memoryMap);
    listResources=pPE->getResources(&memoryMap);

    resVersion=pPE->getResourceVersion(&listResources);

    nNumberOfResources=listResources.count();

    listImports=pPE->getImports(&memoryMap);

    nNumberOfImports=listImports.count();

    bIsNETPresent=pPE->isNETPresent();
    bIs64=pPE->is64();
    bIsDll=pPE->isDll();
    bIsDriver=pPE->isDriver();
    bIsConsole=pPE->isConsole();
    bIsSignPresent=pPE->isSignPresent();

    nImportSection=pPE->getImportSection(&memoryMap);
    nExportSection=pPE->getExportSection(&memoryMap);
    nResourcesSection=pPE->getResourcesSection(&memoryMap);
    nEntryPointSection=pPE->getEntryPointSection(&memoryMap);
    nRelocsSection=pPE->getRelocsSection(&memoryMap);

    nMajorLinkerVersion=pPE->getOptionalHeader_MajorLinkerVersion();
    nMinorLinkerVersion=pPE->getOptionalHeader_MinorLinkerVersion();
    nSizeOfCode=pPE->getOptionalHeader_SizeOfCode();
    nSizeOfUninitializedData=pPE->getOptionalHeader_SizeOfUninitializedData();

    sCompilerVersion=QString("%1.%2").arg(nMajorLinkerVersion).arg(nMinorLinkerVersion);

    sGeneralOptions=QString("%1%2").arg(XPE::getTypesS().value(pPE->getType())).arg(bIs64?("64"):("32"));

    sFileVersion=pPE->getFileVersion(&resVersion);

    nCalculateSizeOfHeaders=pPE->calculateHeadersSize();

    exportHeader=pPE->getExport();

    listExportFunctionNames=pPE->getExportFunctionsList(&exportHeader);
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
    return pPE->getSection_NameAsString(nNumber,&listSN);
}

quint32 PE_Script::getSectionVirtualSize(quint32 nNumber)
{
    return pPE->getSection_VirtualSize(nNumber,&listSH);
}

quint32 PE_Script::getSectionVirtualAddress(quint32 nNumber)
{
    return pPE->getSection_VirtualAddress(nNumber,&listSH);
}

quint32 PE_Script::getSectionFileSize(quint32 nNumber)
{
    return pPE->getSection_SizeOfRawData(nNumber,&listSH);
}

quint32 PE_Script::getSectionFileOffset(quint32 nNumber)
{
    return pPE->getSection_PointerToRawData(nNumber,&listSH);
}

quint32 PE_Script::getSectionCharacteristics(quint32 nNumber)
{
    return pPE->getSection_Characteristics(nNumber,&listSH);
}

quint32 PE_Script::getNumberOfResources()
{
    return nNumberOfResources;
}

bool PE_Script::isSectionNamePresent(QString sSectionName)
{
    return XBinary::isStringInListPresent(&listSN,sSectionName);
}

bool PE_Script::isSectionNamePresentExp(QString sSectionName)
{
    return XBinary::isStringInListPresentExp(&listSN,sSectionName);
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

qint32 PE_Script::getExportSection()
{
    return nExportSection;
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

qint64 PE_Script::getResourceNameOffset(QString sName)
{
    return pPE->getResourceNameOffset(sName,&listResources);
}

bool PE_Script::isResourceNamePresent(QString sName)
{
    return pPE->isResourceNamePresent(sName,&listResources);
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

QString PE_Script::getSectionNameCollision(QString sString1, QString sString2)
{
    return pPE->getStringCollision(&listSN,sString1,sString2);
}

qint32 PE_Script::getSectionNumber(QString sSectionName)
{  
    return XBinary::getStringNumberFromList(&listSN,sSectionName);
}

qint32 PE_Script::getSectionNumberExp(QString sSectionName)
{
    return XBinary::getStringNumberFromListExp(&listSN,sSectionName);
}

bool PE_Script::isDll()
{
    return bIsDll;
}

bool PE_Script::isDriver()
{
    return bIsDriver;
}

QString PE_Script::getNETVersion()
{
    return cliInfo.sCLI_MetaData_Version;
}

bool PE_Script::compareEP_NET(QString sSignature, qint64 nOffset)
{
    return pPE->compareSignatureOnAddress(&memoryMap,sSignature,nBaseAddress+cliInfo.nEntryPoint+nOffset);
}

quint32 PE_Script::getSizeOfCode()
{
    return nSizeOfCode;
}

quint32 PE_Script::getSizeOfUninitializedData()
{
    return nSizeOfUninitializedData;
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
    return sFileVersion;
}

qint64 PE_Script::calculateSizeOfHeaders()
{
    return nCalculateSizeOfHeaders;
}

bool PE_Script::isExportFunctionPresent(QString sFunctionName)
{
    return XBinary::isStringInListPresent(&listExportFunctionNames,sFunctionName);
}

bool PE_Script::isExportFunctionPresentExp(QString sFunctionName)
{
return XBinary::isStringInListPresentExp(&listExportFunctionNames,sFunctionName);
}
