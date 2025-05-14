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
#ifndef JPEG_SCRIPT_H
#define JPEG_SCRIPT_H

#include "image_script.h"

class Jpeg_Script : public Image_Script {
    Q_OBJECT
public:
    explicit Jpeg_Script(XJpeg *pJpeg, XBinary::FILEPART filePart, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct);

public slots:
    QString getComment();
    QString getDqtMD5();
    bool isChunkPresent(qint32 nID);
    bool isExifPresent();
    QString getExifCameraName();

private:
    XJpeg *g_pJpeg;
    QList<XJpeg::CHUNK> g_listChunks;
    XBinary::OFFSETSIZE g_osExif;
    QList<XTiff::CHUNK> g_listExifChunks;
    QString g_sExifCameraName;
};

#endif  // JPEG_SCRIPT_H
