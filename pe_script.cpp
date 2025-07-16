/* Copyright (c) 2019-2025 hors<horsicq@gmail.com>
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

PE_Script::PE_Script(XPE *pPE, XBinary::FILEPART filePart, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : MSDOS_Script(pPE, filePart, pOptions, pPdStruct)
{
    g_pPE = pPE;

    g_nNumberOfSections = g_pPE->getFileHeader_NumberOfSections();
    g_listSectionHeaders = g_pPE->getSectionHeaders(getPdStruct());
    g_listSectionRecords = g_pPE->getSectionRecords(&g_listSectionHeaders, getPdStruct());
    g_listSectionNameStrings = g_pPE->getSectionNames(&g_listSectionRecords, getPdStruct());
    g_cliInfo = g_pPE->getCliInfo(true, getMemoryMap(), getPdStruct());
    g_bNetGlobalCctorPresent = g_pPE->isNetGlobalCctorPresent(&g_cliInfo, getPdStruct());

    if (g_cliInfo.bValid) {
        g_listNetAnsiStrings = g_pPE->getAnsiStrings(&g_cliInfo, getPdStruct());
        g_listNetUnicodeStrings = g_pPE->getUnicodeStrings(&g_cliInfo, getPdStruct());
        g_sNetModuleName = g_pPE->getMetadataModuleName(&g_cliInfo, 0);
        g_sNetAssemblyName = g_pPE->getMetadataAssemblyName(&g_cliInfo, 0);
    }

    g_listResourceRecords = g_pPE->getResources(getMemoryMap(), 10000, getPdStruct());
    g_resourcesVersion = g_pPE->getResourcesVersion(&g_listResourceRecords, getPdStruct());
    g_nNumberOfResources = g_listResourceRecords.count();
    g_listImportHeaders = g_pPE->getImports(getMemoryMap(), getPdStruct());
    g_listImportRecords = g_pPE->getImportRecords(getMemoryMap(), getPdStruct());

    g_nNumberOfImports = g_listImportHeaders.count();

    g_bIsNETPresent = (g_pPE->isNETPresent()) && (g_cliInfo.bValid);
    g_bIs64 = g_pPE->is64(getMemoryMap());
    g_bIsDll = g_pPE->isDll();
    g_bIsDriver = g_pPE->isDriver();
    g_bIsConsole = g_pPE->isConsole();
    g_bIsSignPresent = g_pPE->isSignPresent();
    g_bIsExportPresent = g_pPE->isExportPresent();
    g_bIsTLSPresent = g_pPE->isTLSPresent();
    g_bIsImportPresent = g_pPE->isImportPresent();
    g_bIsResourcesPresent = g_pPE->isResourcesPresent();

    g_nImportSection = g_pPE->getImageDirectoryEntrySection(getMemoryMap(), XPE_DEF::S_IMAGE_DIRECTORY_ENTRY_IMPORT);
    g_nExportSection = g_pPE->getImageDirectoryEntrySection(getMemoryMap(), XPE_DEF::S_IMAGE_DIRECTORY_ENTRY_EXPORT);
    g_nResourcesSection = g_pPE->getImageDirectoryEntrySection(getMemoryMap(), XPE_DEF::S_IMAGE_DIRECTORY_ENTRY_RESOURCE);
    g_nEntryPointSection = g_pPE->getEntryPointSection(getMemoryMap());
    g_nRelocsSection = g_pPE->getImageDirectoryEntrySection(getMemoryMap(), XPE_DEF::S_IMAGE_DIRECTORY_ENTRY_BASERELOC);
    g_nTLSSection = g_pPE->getImageDirectoryEntrySection(getMemoryMap(), XPE_DEF::S_IMAGE_DIRECTORY_ENTRY_TLS);

    g_nMajorLinkerVersion = g_pPE->getOptionalHeader_MajorLinkerVersion();
    g_nMinorLinkerVersion = g_pPE->getOptionalHeader_MinorLinkerVersion();
    g_nSizeOfCode = g_pPE->getOptionalHeader_SizeOfCode();
    g_nSizeOfUninitializedData = g_pPE->getOptionalHeader_SizeOfUninitializedData();

    g_sCompilerVersion = QString("%1.%2").arg(QString::number(g_nMajorLinkerVersion), QString::number(g_nMinorLinkerVersion));
    g_sGeneralOptions = QString("%1%2").arg(g_pPE->getTypeAsString(), g_bIs64 ? ("64") : ("32"));

    g_sFileVersion = g_pPE->getFileVersion(&g_resourcesVersion);
    g_sFileVersionMS = g_pPE->getFileVersionMS(&g_resourcesVersion);

    g_nCalculateSizeOfHeaders = g_pPE->calculateHeadersSize();

    g_exportHeader = g_pPE->getExport(false, getPdStruct());
    g_nNumberOfExportFunctions = g_exportHeader.listPositions.count();

    g_listExportFunctionNameStrings = g_pPE->getExportFunctionsList(&g_exportHeader, getPdStruct());

    g_nImportHash64 = g_pPE->getImportHash64(&g_listImportRecords, getPdStruct());
    g_nImportHash32 = g_pPE->getImportHash32(&g_listImportRecords, getPdStruct());
    g_listImportPositionHashes = g_pPE->getImportPositionHashes(&g_listImportHeaders);

    g_imageFileHeader = g_pPE->getFileHeader();
    g_imageOptionalHeader32 = {};
    g_imageOptionalHeader64 = {};

    if (!g_bIs64) {
        g_imageOptionalHeader32 = g_pPE->getOptionalHeader32();
    } else {
        g_imageOptionalHeader64 = g_pPE->getOptionalHeader64();
    }
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
    return g_pPE->getSection_NameAsString(nNumber, &g_listSectionNameStrings);
}

quint32 PE_Script::getSectionVirtualSize(quint32 nNumber)
{
    return g_pPE->getSection_VirtualSize(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionVirtualAddress(quint32 nNumber)
{
    return g_pPE->getSection_VirtualAddress(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionFileSize(quint32 nNumber)
{
    return g_pPE->getSection_SizeOfRawData(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionFileOffset(quint32 nNumber)
{
    return g_pPE->getSection_PointerToRawData(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionCharacteristics(quint32 nNumber)
{
    return g_pPE->getSection_Characteristics(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getNumberOfResources()
{
    return g_nNumberOfResources;
}

bool PE_Script::isSectionNamePresent(const QString &sSectionName)
{
    return XBinary::isStringInListPresent(&g_listSectionNameStrings, sSectionName, getPdStruct());
}

bool PE_Script::isSectionNamePresentExp(const QString &sSectionName)
{
    return XBinary::isStringInListPresentExp(&g_listSectionNameStrings, sSectionName, getPdStruct());
}

bool PE_Script::isNET()
{
    return g_bIsNETPresent;
}

bool PE_Script::isNet()
{
    return g_bIsNETPresent;
}

bool PE_Script::isPEPlus()
{
    return is64();
}

QString PE_Script::getGeneralOptions()
{
    return g_sGeneralOptions;
}

quint32 PE_Script::getResourceIdByNumber(quint32 nNumber)
{
    return g_pPE->getResourceIdByNumber(nNumber, &g_listResourceRecords);
}

QString PE_Script::getResourceNameByNumber(quint32 nNumber)
{
    return g_pPE->getResourceNameByNumber(nNumber, &g_listResourceRecords);
}

qint64 PE_Script::getResourceOffsetByNumber(quint32 nNumber)
{
    return g_pPE->getResourceOffsetByNumber(nNumber, &g_listResourceRecords);
}

qint64 PE_Script::getResourceSizeByNumber(quint32 nNumber)
{
    return g_pPE->getResourceSizeByNumber(nNumber, &g_listResourceRecords);
}

quint32 PE_Script::getResourceTypeByNumber(quint32 nNumber)
{
    return g_pPE->getResourceTypeByNumber(nNumber, &g_listResourceRecords);
}

bool PE_Script::isNETStringPresent(const QString &sString)
{
    return g_pPE->isStringInListPresent(&g_listNetAnsiStrings, sString, getPdStruct());
}

bool PE_Script::isNetObjectPresent(const QString &sString)
{
    return g_pPE->isStringInListPresent(&g_listNetAnsiStrings, sString, getPdStruct());
}

bool PE_Script::isNETUnicodeStringPresent(const QString &sString)
{
    return g_pPE->isStringInListPresent(&g_listNetUnicodeStrings, sString, getPdStruct());
}

bool PE_Script::isNetUStringPresent(const QString &sString)
{
    return g_pPE->isStringInListPresent(&g_listNetUnicodeStrings, sString, getPdStruct());
}

qint64 PE_Script::findSignatureInBlob_NET(const QString &sSignature)
{
    return g_pPE->findSignatureInBlob_NET(sSignature, getMemoryMap(), getPdStruct());
}

bool PE_Script::isSignatureInBlobPresent_NET(const QString &sSignature)
{
    return g_pPE->isSignatureInBlobPresent_NET(sSignature, getMemoryMap(), getPdStruct());
}

bool PE_Script::isNetGlobalCctorPresent()
{
    return g_bNetGlobalCctorPresent;
}

bool PE_Script::isNetTypePresent(const QString &sTypeNamespace, const QString &sTypeName)
{
    return g_pPE->isNetTypePresent(&g_cliInfo, sTypeNamespace, sTypeName, getPdStruct());
}

bool PE_Script::isNetMethodPresent(const QString &sTypeNamespace, const QString &sTypeName, const QString &sMethodName)
{
    return g_pPE->isNetMethodPresent(&g_cliInfo, sTypeNamespace, sTypeName, sMethodName, getPdStruct());
}

bool PE_Script::isNetFieldPresent(const QString &sTypeNamespace, const QString &sTypeName, const QString &sFieldName)
{
    return g_pPE->isNetFieldPresent(&g_cliInfo, sTypeNamespace, sTypeName, sFieldName, getPdStruct());
}

QString PE_Script::getNetModuleName()
{
    return g_sNetModuleName;
}

QString PE_Script::getNetAssemblyName()
{
    return g_sNetAssemblyName;
}

qint32 PE_Script::getNumberOfImports()
{
    return g_nNumberOfImports;
}

QString PE_Script::getImportLibraryName(quint32 nNumber)
{
    return g_pPE->getImportLibraryName(nNumber, &g_listImportHeaders);
}

bool PE_Script::isLibraryPresent(const QString &sLibraryName, bool bCheckCase)
{
    bool bResult = false;

    if (bCheckCase) {
        bResult = g_pPE->isImportLibraryPresent(sLibraryName, &g_listImportHeaders, getPdStruct());
    } else {
        bResult = g_pPE->isImportLibraryPresentI(sLibraryName, &g_listImportHeaders, getPdStruct());
    }

    return bResult;
}

bool PE_Script::isLibraryFunctionPresent(const QString &sLibraryName, const QString &sFunctionName)
{
    return g_pPE->isImportFunctionPresentI(sLibraryName, sFunctionName, &g_listImportHeaders, getPdStruct());
}

bool PE_Script::isFunctionPresent(const QString &sFunctionName)
{
    return g_pPE->isFunctionPresent(sFunctionName, &g_listImportHeaders, getPdStruct());
}

QString PE_Script::getImportFunctionName(quint32 nImport, quint32 nFunctionNumber)
{
    return g_pPE->getImportFunctionName(nImport, nFunctionNumber, &g_listImportHeaders);
}

qint32 PE_Script::getImportSection()
{
    return g_nImportSection;
}

qint32 PE_Script::getExportSection()
{
    return g_nExportSection;
}

qint32 PE_Script::getResourceSection()
{
    return g_nResourcesSection;
}

qint32 PE_Script::getEntryPointSection()
{
    return g_nEntryPointSection;
}

qint32 PE_Script::getRelocsSection()
{
    return g_nRelocsSection;
}

qint32 PE_Script::getTLSSection()
{
    return g_nTLSSection;
}

quint8 PE_Script::getMajorLinkerVersion()
{
    return g_nMajorLinkerVersion;
}

quint8 PE_Script::getMinorLinkerVersion()
{
    return g_nMinorLinkerVersion;
}

QString PE_Script::getManifest()
{
    return g_pPE->getResourceManifest(&g_listResourceRecords);
}

QString PE_Script::getVersionStringInfo(const QString &sKey)
{
    return g_pPE->getResourcesVersionValue(sKey, &g_resourcesVersion);
}

qint32 PE_Script::getNumberOfImportThunks(quint32 nNumber)
{
    return g_pPE->getNumberOfImportThunks(nNumber, &g_listImportHeaders);
}

qint64 PE_Script::getResourceNameOffset(const QString &sName)
{
    return g_pPE->getResourceNameOffset(sName, &g_listResourceRecords);
}

bool PE_Script::isResourceNamePresent(const QString &sName)
{
    return g_pPE->isResourceNamePresent(sName, &g_listResourceRecords);
}

bool PE_Script::isResourceGroupNamePresent(const QString &sName)
{
    return g_pPE->isResourceGroupNamePresent(sName, &g_listResourceRecords);
}

bool PE_Script::isResourceGroupIdPresent(quint32 nID)
{
    return g_pPE->isResourceGroupIdPresent(nID, &g_listResourceRecords);
}

QString PE_Script::getCompilerVersion()
{
    return g_sCompilerVersion;
}

bool PE_Script::isConsole()
{
    return g_bIsConsole;
}

bool PE_Script::isSignedFile()
{
    return g_bIsSignPresent;
}

QString PE_Script::getSectionNameCollision(const QString &sString1, const QString &sString2)
{
    return g_pPE->getStringCollision(&g_listSectionNameStrings, sString1, sString2);
}

qint32 PE_Script::getSectionNumber(const QString &sSectionName)
{
    return XBinary::getStringNumberFromList(&g_listSectionNameStrings, sSectionName, getPdStruct());
}

qint32 PE_Script::getSectionNumberExp(const QString &sSectionName)
{
    return XBinary::getStringNumberFromListExp(&g_listSectionNameStrings, sSectionName, getPdStruct());
}

bool PE_Script::isDll()
{
    return g_bIsDll;
}

bool PE_Script::isDriver()
{
    return g_bIsDriver;
}

QString PE_Script::getNETVersion()
{
    return g_cliInfo.metaData.header.sVersion;
}

bool PE_Script::compareEP_NET(const QString &sSignature, qint64 nOffset)
{
    return g_pPE->compareSignatureOnAddress(getMemoryMap(), sSignature, getBaseAddress() + g_cliInfo.metaData.nEntryPoint + nOffset);
}

quint32 PE_Script::getSizeOfCode()
{
    return g_nSizeOfCode;
}

quint32 PE_Script::getSizeOfUninitializedData()
{
    return g_nSizeOfUninitializedData;
}

QString PE_Script::getPEFileVersion(const QString &sFileName)
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
    return g_sFileVersion;
}

QString PE_Script::getFileVersionMS()
{
    return g_sFileVersionMS;
}

qint64 PE_Script::calculateSizeOfHeaders()
{
    return g_nCalculateSizeOfHeaders;
}

bool PE_Script::isExportFunctionPresent(const QString &sFunctionName)
{
    return XBinary::isStringInListPresent(&g_listExportFunctionNameStrings, sFunctionName, getPdStruct());
}

bool PE_Script::isExportFunctionPresentExp(const QString &sFunctionName)
{
    return XBinary::isStringInListPresentExp(&g_listExportFunctionNameStrings, sFunctionName, getPdStruct());
}

qint32 PE_Script::getNumberOfExportFunctions()
{
    return g_nNumberOfExportFunctions;
}

qint32 PE_Script::getNumberOfExports()
{
    return getNumberOfExportFunctions();
}

QString PE_Script::getExportFunctionName(quint32 nNumber)
{
    return g_pPE->getStringByIndex(&g_listExportFunctionNameStrings, nNumber, -1);
}

QString PE_Script::getExportNameByNumber(quint32 nNumber)
{
    return getExportFunctionName(nNumber);
}

bool PE_Script::isExportPresent()
{
    return g_bIsExportPresent;
}

bool PE_Script::isTLSPresent()
{
    return g_bIsTLSPresent;
}

bool PE_Script::isImportPresent()
{
    return g_bIsImportPresent;
}

bool PE_Script::isResourcesPresent()
{
    return g_bIsResourcesPresent;
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
    return XPE::isImportPositionHashPresent(&g_listImportPositionHashes, nIndex, nHash, getPdStruct());
}

quint64 PE_Script::getImageFileHeader(const QString &sString)
{
    return g_pPE->getImageFileHeader(&g_imageFileHeader, sString);
}

quint64 PE_Script::getImageOptionalHeader(const QString &sString)
{
    if (!g_bIs64) {
        return g_pPE->getImageOptionalHeader32(&g_imageOptionalHeader32, sString);
    } else {
        return g_pPE->getImageOptionalHeader64(&g_imageOptionalHeader64, sString);
    }
}
