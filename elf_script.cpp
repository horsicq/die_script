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
}

ELF_Script::~ELF_Script()
{

}

bool ELF_Script::isSectionNamePresent(QString sSectionName)
{
    return pELF->isSectionNamePresent(sSectionName);
}

quint32 ELF_Script::getNumberOfSections()
{
    return pELF->getHdr32_shnum();
}

quint32 ELF_Script::getNumberOfPrograms()
{
    return pELF->getHdr32_phnum();
}

QString ELF_Script::getGeneralOptions()
{
    qDebug("QString ELF_Script::getGeneralOptions() TODO");

    return "";
}

qint32 ELF_Script::getSectionNumber(QString sSectionName)
{
    return pELF->getSectionNumber(sSectionName);
}
