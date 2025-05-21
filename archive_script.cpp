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
#include "archive_script.h"

Archive_Script::Archive_Script(XArchive *pArchive, XBinary::FILEPART filePart, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct)
    : Binary_Script(pArchive, filePart, pOptions, pPdStruct)
{
    g_pArchive = pArchive;

    bool bIsArchive = false;

    if (!bIsArchive) {
        XZip *_pArchive = dynamic_cast<XZip *>(g_pArchive);
        if (_pArchive) g_listArchiveRecords = _pArchive->getRecords(20000, pPdStruct), bIsArchive = true;
    }

    if (!bIsArchive) {
        XTGZ *_pArchive = dynamic_cast<XTGZ *>(g_pArchive);
        if (_pArchive) g_listArchiveRecords = _pArchive->getRecords(20000, pPdStruct), bIsArchive = true;
    }

    if (!bIsArchive) {
        XTAR *_pArchive = dynamic_cast<XTAR *>(g_pArchive);
        if (_pArchive) g_listArchiveRecords = _pArchive->getRecords(20000, pPdStruct), bIsArchive = true;
    }

    if (!bIsArchive) {
        XDOS16 *_pArchive = dynamic_cast<XDOS16 *>(g_pArchive);
        if (_pArchive) g_listArchiveRecords = _pArchive->getRecords(20000, pPdStruct), bIsArchive = true;
    }

    if (!bIsArchive) {
        XMACHOFat *_pArchive = dynamic_cast<XMACHOFat *>(g_pArchive);
        if (_pArchive) g_listArchiveRecords = _pArchive->getRecords(20000, pPdStruct), bIsArchive = true;
    }

    if (!bIsArchive) {
        XRar *_pArchive = dynamic_cast<XRar *>(g_pArchive);
        if (_pArchive) g_listArchiveRecords = _pArchive->getRecords(20000, pPdStruct), bIsArchive = true;
    }
}

bool Archive_Script::isArchiveRecordPresent(const QString &sArchiveRecord)
{
    return XArchive::isArchiveRecordPresent(sArchiveRecord, &g_listArchiveRecords, getPdStruct());
}

bool Archive_Script::isArchiveRecordPresentExp(const QString &sArchiveRecord)
{
    return XArchive::isArchiveRecordPresentExp(sArchiveRecord, &g_listArchiveRecords, getPdStruct());
}

QList<XArchive::RECORD> *Archive_Script::getArchiveRecords()
{
    return &g_listArchiveRecords;
}
