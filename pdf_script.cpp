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
#include "pdf_script.h"

PDF_Script::PDF_Script(XPDF *pPDF, XBinary::FILEPART filePart, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : Binary_Script(pPDF, filePart, pOptions, pPdStruct)
{
    this->g_pPDF = pPDF;
    g_listObjects = pPDF->getParts(20, getPdStruct());
}

PDF_Script::~PDF_Script()
{
}

QList<QVariant> PDF_Script::getValuesByKey(const QString &sKey)
{
    QList<XBinary::XVARIANT> listXVariants = XPDF::getValuesByKey(&g_listObjects, sKey, getPdStruct());

    QList<QVariant> listResult;

    qint32 nNumberOfXVariants = listXVariants.count();

    for (qint32 i = 0; i < nNumberOfXVariants; i++) {
        listResult.append(listXVariants.at(i).var.toString());
    }

    return listResult;
}

QList<QVariant> PDF_Script::getStringValuesByKey(const QString &sKey)
{
    QList<XBinary::XVARIANT> listXVariants = XPDF::getValuesByKey(&g_listObjects, sKey, getPdStruct());

    QList<QVariant> listResult;

    qint32 nNumberOfXVariants = listXVariants.count();

    for (qint32 i = 0; i < nNumberOfXVariants; i++) {
        if (listXVariants.at(i).varType == XBinary::VT_STRING) {
            listResult.append(listXVariants.at(i).var.toString());
        }
    }

    return listResult;
}

bool PDF_Script::isValuesHexByKey(const QString &sKey)
{
    QList<XBinary::XVARIANT> listXVariants = XPDF::getValuesByKey(&g_listObjects, sKey, getPdStruct());

    bool bResult = false;

    qint32 nNumberOfXVariants = listXVariants.count();

    for (qint32 i = 0; i < nNumberOfXVariants; i++) {
        if (listXVariants.at(i).varType == XBinary::VT_HEX) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

QString PDF_Script::getHeaderCommentAsHex()
{
    return g_pPDF->getHeaderCommentAsHex();
}
