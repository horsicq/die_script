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
#include "mach_script.h"

MACH_Script::MACH_Script(XMACH *pMACH,XBinary::PDSTRUCT *pPdStruct) : Binary_Script(pMACH,pPdStruct)
{
    this->g_pMACH=pMACH;

    g_listLibraryRecords=pMACH->getLibraryRecords();
    listSectionRecords=pMACH->getSectionRecords();
    listCommandRecords=pMACH->getCommandRecords();

    nNumberOfSection=listSectionRecords.count();
    nNumberOfSegments=listSegmentRecords.count();

    bool bIs64=pMACH->is64(&g_memoryMap);

    sGeneralOptions=QString("%1%2").arg(XMACH::getHeaderFileTypesS().value(pMACH->getHeader_filetype())).arg(bIs64?("64"):("32"));
}

MACH_Script::~MACH_Script()
{

}

bool MACH_Script::isLibraryPresent(QString sLibraryName)
{
    return g_pMACH->isLibraryRecordNamePresent(sLibraryName,&g_listLibraryRecords);
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
    return g_pMACH->getSectionNumber(sSectionName,&listSectionRecords);
}

QString MACH_Script::getGeneralOptions()
{
    return sGeneralOptions;
}

quint32 MACH_Script::getLibraryCurrentVersion(QString sLibraryName)
{
    return g_pMACH->getLibraryCurrentVersion(sLibraryName,&g_listLibraryRecords);
}

quint64 MACH_Script::getSectionFileOffset(quint32 nNumber)
{
    return g_pMACH->getSectionFileOffset(nNumber,&listSectionRecords);
}

quint64 MACH_Script::getSectionFileSize(quint32 nNumber)
{
    return g_pMACH->getSectionFileSize(nNumber,&listSectionRecords);
}
