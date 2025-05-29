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
#include "dex_script.h"

DEX_Script::DEX_Script(XDEX *pDex, XBinary::FILEPART filePart, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : Binary_Script(pDex, filePart, pOptions, pPdStruct)
{
    this->g_pDex = pDex;

    g_listItems = pDex->getMapItems(pPdStruct);
    g_bIsStringPoolSorted = pDex->isStringPoolSorted(&g_listItems, pPdStruct);
    g_bIsOverlayPresent = pDex->isOverlayPresent(getMemoryMap(), pPdStruct);
    g_listStrings = pDex->getStrings(&g_listItems, pPdStruct);
    g_listItemStrings = pDex->getTypeItemStrings(&g_listItems, &g_listStrings, pPdStruct);
}

DEX_Script::~DEX_Script()
{
}

bool DEX_Script::isStringPoolSorted()
{
    return g_bIsStringPoolSorted;
}

bool DEX_Script::isOverlayPresent()
{
    return g_bIsOverlayPresent;
}

bool DEX_Script::isDexStringPresent(const QString &sString)
{
    return g_pDex->isStringInListPresent(&g_listStrings, sString, getPdStruct());
}

bool DEX_Script::isDexItemStringPresent(const QString &sItemString)
{
    return g_pDex->isStringInListPresent(&g_listItemStrings, sItemString, getPdStruct());
}
