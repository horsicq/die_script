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
}

PE_Script::~PE_Script()
{

}

quint16 PE_Script::getNumberOfSections()
{
    return pPE->getFileHeader_NumberOfSections();
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
    return pPE->getResources().count();
}

bool PE_Script::isSectionNamePresent(QString sSectionName)
{
    return pPE->isSectionNamePresent(sSectionName);
}
