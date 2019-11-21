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
#include "elf_script.h"

ELF_Script::ELF_Script(XELF *pELF) : Binary_Script(pELF)
{
    this->pELF=pELF;

    bIs64=pELF->is64();

    elfHeader=pELF->getHdr();

    nStringTableSection=pELF->getSectionStringTable(bIs64);
    baStringTable=pELF->getSection(nStringTableSection);
    listSH=pELF->getElf_ShdrList();
    listPH=pELF->getElf_PhdrList();

    listSR=pELF->getSectionRecords(&listSH,bIs64,&baStringTable);

    QString sType;
    QString sBits;
    unsigned int nType=elfHeader.e_type;

    if(nType==0)
    {
        sType="unknown";
    }
    else if(nType==1)
    {
        sType="relocatable";
    }
    else if(nType==2)
    {
        sType="executable";
    }
    else if(nType==3)
    {
        sType="shared object";
    }
    else if(nType==4)
    {
        sType="core file";
    }

    if(bIs64)
    {
        sBits="64";
    }
    else
    {
        sBits="32";
    }

    sGeneralOptions=QString("%1 %2-%3").arg(sType).arg(XELF::getMachinesS().value(elfHeader.e_machine)).arg(sBits);
}

ELF_Script::~ELF_Script()
{

}

bool ELF_Script::isSectionNamePresent(QString sSectionName)
{
    return pELF->isSectionNamePresent(sSectionName,&listSR);
}

quint32 ELF_Script::getNumberOfSections()
{
    return elfHeader.e_shnum;
}

quint32 ELF_Script::getNumberOfPrograms()
{
    return elfHeader.e_phnum;
}

QString ELF_Script::getGeneralOptions()
{
    return sGeneralOptions;
}

qint32 ELF_Script::getSectionNumber(QString sSectionName)
{
    return pELF->getSectionNumber(sSectionName,&listSR);
}

quint16 ELF_Script::getElfHeader_type()
{
    return elfHeader.e_type;
}

quint16 ELF_Script::getElfHeader_machine()
{
    return elfHeader.e_machine;
}

quint32 ELF_Script::getElfHeader_version()
{
    return elfHeader.e_version;
}

quint64 ELF_Script::getElfHeader_entry()
{
    return elfHeader.e_entry;
}

quint64 ELF_Script::getElfHeader_phoff()
{
    return elfHeader.e_phoff;
}

quint64 ELF_Script::getElfHeader_shoff()
{
    return elfHeader.e_shoff;
}

quint32 ELF_Script::getElfHeader_flags()
{
    return elfHeader.e_flags;
}

quint16 ELF_Script::getElfHeader_ehsize()
{
    return elfHeader.e_ehsize;
}

quint16 ELF_Script::getElfHeader_phentsize()
{
    return elfHeader.e_phentsize;
}

quint16 ELF_Script::getElfHeader_phnum()
{
    return elfHeader.e_phnum;
}

quint16 ELF_Script::getElfHeader_shentsize()
{
    return elfHeader.e_shentsize;
}

quint16 ELF_Script::getElfHeader_shnum()
{
    return elfHeader.e_shnum;
}

quint16 ELF_Script::getElfHeader_shstrndx()
{
    return elfHeader.e_shstrndx;
}

quint64 ELF_Script::getProgramFileSize(quint32 nNumber)
{
    return pELF->getElf_Phdr_filesz(nNumber,&listPH);
}

quint64 ELF_Script::getProgramFileOffset(quint32 nNumber)
{
    return pELF->getElf_Phdr_offset(nNumber,&listPH);
}

quint64 ELF_Script::getSectionFileOffset(quint32 nNumber)
{
    return pELF->getElf_Shdr_offset(nNumber,&listSH);
}

quint64 ELF_Script::getSectionFileSize(quint32 nNumber)
{
    return pELF->getElf_Shdr_size(nNumber,&listSH);
}

bool ELF_Script::isStringInTablePresent(QString sSectionName, QString sString)
{
    bool bResult=false;

    qint32 nSection=pELF->getSectionNumber(sSectionName,&listSR);

    if(nSection!=-1)
    {
        bResult=(pELF->getStringsFromSection(nSection).key(sString,-1)!=-1);
    }

    return bResult;
}
