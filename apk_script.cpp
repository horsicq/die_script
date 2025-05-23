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
#include "apk_script.h"

APK_Script::APK_Script(XAPK *pAPK, XBinary::FILEPART filePart, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : JAR_Script(pAPK, filePart, pOptions, pPdStruct)
{
    this->g_pAPK = pAPK;

    XArchive::RECORD record = pAPK->getArchiveRecord("AndroidManifest.xml", getArchiveRecords(), pPdStruct);

    QByteArray baAndroidManifest = XArchives::decompress(pAPK->getDevice(), &record, pPdStruct);
    if (baAndroidManifest.size() > 0) {
        g_sAndroidManifest = XAndroidBinary::getDecoded(&baAndroidManifest, pPdStruct);
    }
}

APK_Script::~APK_Script()
{
}

QString APK_Script::getAndroidManifest()
{
    return g_sAndroidManifest;
}

QString APK_Script::getAndroidManifestRecord(const QString &sRecord)
{
    QString sResult;
    QString sRegex = sRecord + "=\"(.*?)\"";

    sResult = XBinary::regExp(sRegex, g_sAndroidManifest, 1);

    return sResult;
}
