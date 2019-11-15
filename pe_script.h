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
    qint32 getNumberOfImports();
    QString getImportLibraryName(quint32 nNumber);
    bool isLibraryPresent(QString sLibraryName);
    qint32 getImportSection();
    qint32 getResourceSection();
    qint32 getEntryPointSection();
    quint8 getMajorLinkerVersion();
    quint8 getMinorLinkerVersion();
    QString getManifest();
    QString getVersionStringInfo(QString sKey);
    qint32 getNumberOfImportThunks(quint32 nNumber);
    qint32 getNumberOfRichIDs();
    qint64 getResourceNameOffset(QString sName);
    bool isResourceNamePresent(QString sName);
    qint64 getDosStubOffset();
    qint64 getDosStubSize();

private:
    XPE *pPE;
};

#endif // PE_SCRIPT_H
