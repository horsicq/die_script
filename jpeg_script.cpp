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
#include "jpeg_script.h"

Jpeg_Script::Jpeg_Script(XJpeg *pJpeg, XBinary::FILEPART filePart, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : Image_Script(pJpeg, filePart, pOptions, pPdStruct)
{
    g_pJpeg = pJpeg;

    g_listChunks = g_pJpeg->getChunks(pPdStruct);
    g_osExif = g_pJpeg->getExif(&g_listChunks);
    g_listExifChunks = XTiff::getExifChunks(pJpeg->getDevice(), g_osExif);
    g_sExifCameraName = XTiff::getExifCameraName(pJpeg->getDevice(), g_osExif, &g_listExifChunks);
}

QString Jpeg_Script::getComment()
{
    return g_pJpeg->getComment(&g_listChunks);
}

QString Jpeg_Script::getDqtMD5()
{
    return g_pJpeg->getDqtMD5(&g_listChunks);
}

bool Jpeg_Script::isChunkPresent(qint32 nID)
{
    return g_pJpeg->isChunkPresent(&g_listChunks, (qint8)nID);
}

bool Jpeg_Script::isExifPresent()
{
    return g_pJpeg->isExifPresent(g_osExif);
}

QString Jpeg_Script::getExifCameraName()
{
    return g_sExifCameraName;
}
