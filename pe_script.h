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
#ifndef PE_SCRIPT_H
#define PE_SCRIPT_H

#include "msdos_script.h"
#include "xpe.h"

class PE_Script : public MSDOS_Script
{
    Q_OBJECT

public:
    explicit PE_Script(XPE *pPE);
    ~PE_Script();

public slots:
    quint16 getNumberOfSections();
    QString getSectionName(quint32 nNumber);
    quint32 getSectionVirtualSize(quint32 nNumber);
    quint32 getSectionVirtualAddress(quint32 nNumber);
    quint32 getSectionFileSize(quint32 nNumber);
    quint32 getSectionFileOffset(quint32 nNumber);
    quint32 getSectionCharacteristics(quint32 nNumber);
    quint32 getNumberOfResources();
    bool isSectionNamePresent(QString sSectionName);
    bool isNET();
    bool isPEPlus();
    virtual QString getGeneralOptions();
    quint32 getResourceIdByNumber(quint32 nNumber);
    QString getResourceNameByNumber(quint32 nNumber);
    qint64 getResourceOffsetByNumber(quint32 nNumber);
    qint64 getResourceSizeByNumber(quint32 nNumber);
    quint32 getResourceTypeByNumber(quint32 nNumber);
    bool isNETStringPresent(QString sString);
    bool isNETUnicodeStringPresent(QString sString);
    qint32 getNumberOfImports();
    QString getImportLibraryName(quint32 nNumber);
    bool isLibraryPresent(QString sLibraryName);
    bool isLibraryFunctionPresent(QString sLibraryName, QString sFunctionName);
    QString getImportFunctionName(quint32 nImport, quint32 nFunctionNumber);
    qint32 getImportSection();
    qint32 getExportSection();
    qint32 getResourceSection();
    qint32 getEntryPointSection();
    qint32 getRelocsSection();
    quint8 getMajorLinkerVersion();
    quint8 getMinorLinkerVersion();
    QString getManifest();
    QString getVersionStringInfo(QString sKey);
    qint32 getNumberOfImportThunks(quint32 nNumber);
    qint64 getResourceNameOffset(QString sName);
    bool isResourceNamePresent(QString sName);
    QString getCompilerVersion();
    bool isConsole();
    bool isSignedFile();
    QString getSectionNameCollision(QString sString1, QString sString2);
    qint32 getSectionNumber(QString sSectionName);
    bool isDll();
    QString getNETVersion();
    bool compareEP_NET(QString sSignature,qint64 nOffset=0);
    quint32 getSizeOfCode();
    quint32 getSizeOfUninitializedData();
    QString getPEFileVersion(QString sFileName);
    QString getFileVersion();
    qint64 calculateSizeOfHeaders();

private:
    XPE *pPE;

    qint64 nBaseAddress;
    qint32 nNumberOfSections;
    XPE::CLI_INFO cliInfo;

    QList<XPE::RESOURCE_RECORD> listResources;

    qint32 nNumberOfResources;

    QList<XPE_DEF::IMAGE_SECTION_HEADER> listSH;
    QList<XPE::SECTION_RECORD> listSR;
    QList<QString> listSN;

    QList<XPE::IMPORT_HEADER> listImports;
    qint32 nNumberOfImports;

    XPE::RESOURCE_VERSION resVersion;

    bool bIsNETPresent;
    bool bIs64;
    bool bIsDll;
    bool bIsConsole;
    bool bIsSignPresent;

    QString sGeneralOptions;

    qint32 nImportSection;
    qint32 nExportSection;
    qint32 nResourcesSection;
    qint32 nEntryPointSection;
    qint32 nRelocsSection;

    quint8 nMajorLinkerVersion;
    quint8 nMinorLinkerVersion;
    quint32 nSizeOfCode;
    quint32 nSizeOfUninitializedData;

    QString sCompilerVersion;
    QString sFileVersion;

    qint32 nCalculateSizeOfHeaders;
};

#endif // PE_SCRIPT_H
