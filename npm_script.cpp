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
#include "npm_script.h"

NPM_Script::NPM_Script(XNPM *pNpm, XBinary::FILEPART filePart, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : Archive_Script(pNpm, filePart, pOptions, pPdStruct)
{
    this->g_pNpm = pNpm;

    g_sPackageJson = pNpm->decompress(getArchiveRecords(), "package/package.json", pPdStruct);
}

NPM_Script::~NPM_Script()
{
}

QString NPM_Script::getPackageJson()
{
    return g_sPackageJson;
}

QString NPM_Script::getPackageJsonRecord(const QString &sRecord)
{
    QString sResult;

    QJsonDocument jsDoc = QJsonDocument::fromJson(g_sPackageJson.toUtf8());

    if (jsDoc.isObject()) {
        sResult = jsDoc.object().value(sRecord).toString();
    }

    return sResult;
}
