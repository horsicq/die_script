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
#include "mach_script.h"

MACH_Script::MACH_Script(XMACH *pMACH) : Binary_Script(pMACH)
{
    this->pMACH=pMACH;

    listLibraryRecords=pMACH->getLibraryRecords();
    listSectionRecords=pMACH->getSectionRecords();
    listCommandRecords=pMACH->getCommandRecords();

    nNumberOfSection=listSectionRecords.count();
    nNumberOfSegments=listSegmentRecords.count();

    bIs64=pMACH->is64();

    sGeneralOptions=QString("%1%2").arg(XMACH::getHeaderFileTypesS().value(pMACH->getHeader_filetype())).arg(bIs64?("64"):("32"));
}

MACH_Script::~MACH_Script()
{

}

bool MACH_Script::isLibraryPresent(QString sLibraryName)
{
    return pMACH->isLibraryRecordNamePresent(sLibraryName,&listLibraryRecords);
}

quint32 MACH_Script::getNumberOfSections()
{
    return nNumberOfSection;
}

quint32 MACH_Script::getNumberOfSegments()
{
    return nNumberOfSegments;
}

qint32 MACH_Script::getSectionNumber(QString sSectionName)
{
    return pMACH->getSectionNumber(sSectionName,&listSectionRecords);
}

QString MACH_Script::getGeneralOptions()
{
    return sGeneralOptions;
}

quint32 MACH_Script::getLibraryCurrentVersion(QString sLibraryName)
{
    return pMACH->getLibraryCurrentVersion(sLibraryName,&listLibraryRecords);
}
