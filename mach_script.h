// copyright (c) 2019-2021 hors<horsicq@gmail.com>
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
#ifndef MACH_SCRIPT_H
#define MACH_SCRIPT_H

#include "binary_script.h"
#include "xmach.h"

class MACH_Script : public Binary_Script
{
    Q_OBJECT

public:
    MACH_Script(XMACH *pMACH);
    ~MACH_Script();

public slots:
    bool isLibraryPresent(QString sLibraryName);
    quint32 getNumberOfSections();
    quint32 getNumberOfSegments();
    qint32 getSectionNumber(QString sSectionName);
    virtual QString getGeneralOptions();
    quint32 getLibraryCurrentVersion(QString sLibraryName);

private:
    XMACH *pMACH;
    QString sGeneralOptions;
    QList<XMACH::LIBRARY_RECORD> listLibraryRecords;
    QList<XMACH::SECTION_RECORD> listSectionRecords;
    QList<XMACH::COMMAND_RECORD> listCommandRecords;
    QList<XMACH::SEGMENT_RECORD> listSegmentRecords;
    qint32 nNumberOfSection;
    qint32 nNumberOfSegments;
    bool bIs64;
};

#endif // MACH_SCRIPT_H
