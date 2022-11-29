/* Copyright (c) 2019-2022 hors<horsicq@gmail.com>
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
#include "pe_script.h"

PE_Script::PE_Script(XPE *pPE, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : MSDOS_Script(pPE, pOptions, pPdStruct)
{
    this->pPE = pPE;

    g_nNumberOfSections = pPE->getFileHeader_NumberOfSections();

    g_listSectionHeaders = pPE->getSectionHeaders();
    g_listSectionRecords = pPE->getSectionRecords(&g_listSectionHeaders, pPE->isImage());
    listSectionNameStrings = pPE->getSectionNames(&g_listSectionRecords);

    g_cliInfo = pPE->getCliInfo(true, &g_memoryMap);
    g_listResourceRecords = pPE->getResources(&g_memoryMap);

    resourcesVersion = pPE->getResourcesVersion(&g_listResourceRecords);

    g_nNumberOfResources = g_listResourceRecords.count();

    listImportHeaders = pPE->getImports(&g_memoryMap);
    listImportRecords = pPE->getImportRecords(&g_memoryMap);

    nNumberOfImports = listImportHeaders.count();

    bIsNETPresent = (pPE->isNETPresent()) && (g_cliInfo.bValid);
    bool bIs64 = pPE->is64(&g_memoryMap);
    bIsDll = pPE->isDll();
    bIsDriver = pPE->isDriver();
    bIsConsole = pPE->isConsole();
    bIsSignPresent = pPE->isSignPresent();
    bIsExportPresent = pPE->isExportPresent();
    bIsTLSPresent = pPE->isTLSPresent();
    bIsImportPresent = pPE->isImportPresent();
    bIsResourcesPresent = pPE->isResourcesPresent();

    nImportSection = pPE->getImportSection(&g_memoryMap);
    nExportSection = pPE->getExportSection(&g_memoryMap);
    nResourcesSection = pPE->getResourcesSection(&g_memoryMap);
    nEntryPointSection = pPE->getEntryPointSection(&g_memoryMap);
    nRelocsSection = pPE->getRelocsSection(&g_memoryMap);
    nTLSSection = pPE->getTLSSection(&g_memoryMap);

    nMajorLinkerVersion = pPE->getOptionalHeader_MajorLinkerVersion();
    nMinorLinkerVersion = pPE->getOptionalHeader_MinorLinkerVersion();
    nSizeOfCode = pPE->getOptionalHeader_SizeOfCode();
    nSizeOfUninitializedData = pPE->getOptionalHeader_SizeOfUninitializedData();

    sCompilerVersion = QString("%1.%2").arg(nMajorLinkerVersion).arg(nMinorLinkerVersion);
    sGeneralOptions = QString("%1%2").arg(pPE->getTypeAsString()).arg(bIs64 ? ("64") : ("32"));

    sFileVersion = pPE->getFileVersion(&resourcesVersion);
    sFileVersionMS = pPE->getFileVersionMS(&resourcesVersion);

    nCalculateSizeOfHeaders = pPE->calculateHeadersSize();

    exportHeader = pPE->getExport();

    listExportFunctionNameStrings = pPE->getExportFunctionsList(&exportHeader);

    g_nImportHash64 = pPE->getImportHash64(&listImportRecords);
    g_nImportHash32 = pPE->getImportHash32(&listImportRecords);
    g_listImportPositionHashes = pPE->getImportPositionHashes(&listImportHeaders);
}

PE_Script::~PE_Script()
{
}

quint16 PE_Script::getNumberOfSections()
{
    return g_nNumberOfSections;
}

QString PE_Script::getSectionName(quint32 nNumber)
{
    return pPE->getSection_NameAsString(nNumber, &listSectionNameStrings);
}

quint32 PE_Script::getSectionVirtualSize(quint32 nNumber)
{
    return pPE->getSection_VirtualSize(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionVirtualAddress(quint32 nNumber)
{
    return pPE->getSection_VirtualAddress(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionFileSize(quint32 nNumber)
{
    return pPE->getSection_SizeOfRawData(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionFileOffset(quint32 nNumber)
{
    return pPE->getSection_PointerToRawData(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionCharacteristics(quint32 nNumber)
{
    return pPE->getSection_Characteristics(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getNumberOfResources()
{
    return g_nNumberOfResources;
}

bool PE_Script::isSectionNamePresent(QString sSectionName)
{
    return XBinary::isStringInListPresent(&listSectionNameStrings, sSectionName);
}

bool PE_Script::isSectionNamePresentExp(QString sSectionName)
{
    return XBinary::isStringInListPresentExp(&listSectionNameStrings, sSectionName);
}

bool PE_Script::isNET()
{
    return bIsNETPresent;
}

bool PE_Script::isPEPlus()
{
    return is64();
}

QString PE_Script::getGeneralOptions()
{
    return sGeneralOptions;
}

quint32 PE_Script::getResourceIdByNumber(quint32 nNumber)
{
    return pPE->getResourceIdByNumber(nNumber, &g_listResourceRecords);
}

QString PE_Script::getResourceNameByNumber(quint32 nNumber)
{
    return pPE->getResourceNameByNumber(nNumber, &g_listResourceRecords);
}

qint64 PE_Script::getResourceOffsetByNumber(quint32 nNumber)
{
    return pPE->getResourceOffsetByNumber(nNumber, &g_listResourceRecords);
}

qint64 PE_Script::getResourceSizeByNumber(quint32 nNumber)
{
    return pPE->getResourceSizeByNumber(nNumber, &g_listResourceRecords);
}

quint32 PE_Script::getResourceTypeByNumber(quint32 nNumber)
{
    return pPE->getResourceTypeByNumber(nNumber, &g_listResourceRecords);
}

bool PE_Script::isNETStringPresent(QString sString)
{
    return pPE->isNETAnsiStringPresent(sString, &g_cliInfo);
}

bool PE_Script::isNETUnicodeStringPresent(QString sString)
{
    return pPE->isNETUnicodeStringPresent(sString, &g_cliInfo);
}

qint32 PE_Script::getNumberOfImports()
{
    return nNumberOfImports;
}

QString PE_Script::getImportLibraryName(quint32 nNumber)
{
    return pPE->getImportLibraryName(nNumber, &listImportHeaders);
}

bool PE_Script::isLibraryPresent(QString sLibraryName)
{
    return pPE->isImportLibraryPresentI(sLibraryName, &listImportHeaders);
}

bool PE_Script::isLibraryFunctionPresent(QString sLibraryName, QString sFunctionName)
{
    return pPE->isImportFunctionPresentI(sLibraryName, sFunctionName, &listImportHeaders);
}

QString PE_Script::getImportFunctionName(quint32 nImport, quint32 nFunctionNumber)
{
    return pPE->getImportFunctionName(nImport, nFunctionNumber, &listImportHeaders);
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

qint32 PE_Script::getTLSSection()
{
    return nTLSSection;
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
    return pPE->getResourceManifest(&g_listResourceRecords);
}

QString PE_Script::getVersionStringInfo(QString sKey)
{
    return pPE->getResourcesVersionValue(sKey, &resourcesVersion);
}

qint32 PE_Script::getNumberOfImportThunks(quint32 nNumber)
{
    return pPE->getNumberOfImportThunks(nNumber, &listImportHeaders);
}

qint64 PE_Script::getResourceNameOffset(QString sName)
{
    return pPE->getResourceNameOffset(sName, &g_listResourceRecords);
}

bool PE_Script::isResourceNamePresent(QString sName)
{
    return pPE->isResourceNamePresent(sName, &g_listResourceRecords);
}

bool PE_Script::isResourceGroupNamePresent(QString sName)
{
    return pPE->isResourceGroupNamePresent(sName, &g_listResourceRecords);
}

bool PE_Script::isResourceGroupIdPresent(quint32 nID)
{
    return pPE->isResourceGroupIdPresent(nID, &g_listResourceRecords);
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
    return pPE->getStringCollision(&listSectionNameStrings, sString1, sString2);
}

qint32 PE_Script::getSectionNumber(QString sSectionName)
{
    return XBinary::getStringNumberFromList(&listSectionNameStrings, sSectionName);
}

qint32 PE_Script::getSectionNumberExp(QString sSectionName)
{
    return XBinary::getStringNumberFromListExp(&listSectionNameStrings, sSectionName);
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
    return g_cliInfo.metaData.header.sVersion;
}

bool PE_Script::compareEP_NET(QString sSignature, qint64 nOffset)
{
    return pPE->compareSignatureOnAddress(&g_memoryMap, sSignature, g_nBaseAddress + g_cliInfo.metaData.nEntryPoint + nOffset);
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

    if (file.open(QIODevice::ReadOnly)) {
        XPE pe(&file);

        sResult = pe.getFileVersion();

        file.close();
    }

    return sResult;
}

QString PE_Script::getFileVersion()
{
    return sFileVersion;
}

QString PE_Script::getFileVersionMS()
{
    return sFileVersionMS;
}

qint64 PE_Script::calculateSizeOfHeaders()
{
    return nCalculateSizeOfHeaders;
}

bool PE_Script::isExportFunctionPresent(QString sFunctionName)
{
    return XBinary::isStringInListPresent(&listExportFunctionNameStrings, sFunctionName);
}

bool PE_Script::isExportFunctionPresentExp(QString sFunctionName)
{
    return XBinary::isStringInListPresentExp(&listExportFunctionNameStrings, sFunctionName);
}

bool PE_Script::isExportPresent()
{
    return bIsExportPresent;
}

bool PE_Script::isTLSPresent()
{
    return bIsTLSPresent;
}

bool PE_Script::isImportPresent()
{
    return bIsImportPresent;
}

bool PE_Script::isResourcesPresent()
{
    return bIsResourcesPresent;
}

quint32 PE_Script::getImportHash32()
{
    return g_nImportHash32;
}

quint64 PE_Script::getImportHash64()
{
    return g_nImportHash64;
}

bool PE_Script::isImportPositionHashPresent(qint32 nIndex, quint32 nHash)
{
    return XPE::isImportPositionHashPresent(&g_listImportPositionHashes, nIndex, nHash);
}
