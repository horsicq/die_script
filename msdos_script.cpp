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
#include "msdos_script.h"

MSDOS_Script::MSDOS_Script(XMSDOS *pMSDOS, XBinary::FILEPART filePart, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct)
    : Binary_Script(pMSDOS, filePart, pOptions, pPdStruct)
{
    this->g_pMSDOS = pMSDOS;

    g_bIsLE = pMSDOS->isLE();
    g_bIsLX = pMSDOS->isLX();
    g_bIsNE = pMSDOS->isNE();
    g_bIsPE = pMSDOS->isPE();

    g_nNumberOfRichIDs = 0;
    g_bIisRichSignaturePresent = false;

    if (g_bIsLE || g_bIsPE) {
        g_bIisRichSignaturePresent = pMSDOS->isRichSignaturePresent();

        if (g_bIisRichSignaturePresent) {
            g_listRich = pMSDOS->getRichSignatureRecords();

            g_nNumberOfRichIDs = g_listRich.count();
        }
    }

    g_nDosStubOffset = 0;
    g_nDosStubSize = 0;
    g_bIsDosStubPresent = false;

    if (g_bIsLE || g_bIsLX || g_bIsNE || g_bIsPE) {
        g_bIsDosStubPresent = pMSDOS->isDosStubPresent();

        if (g_bIsDosStubPresent) {
            g_nDosStubOffset = pMSDOS->getDosStubOffset();
            g_nDosStubSize = pMSDOS->getDosStubSize();
        }
    }
}

MSDOS_Script::~MSDOS_Script()
{
}

bool MSDOS_Script::isLE()
{
    return g_bIsLE;
}

bool MSDOS_Script::isLX()
{
    return g_bIsLX;
}

bool MSDOS_Script::isNE()
{
    return g_bIsNE;
}

bool MSDOS_Script::isPE()
{
    return g_bIsPE;
}

qint64 MSDOS_Script::getDosStubOffset()
{
    return g_nDosStubOffset;
}

qint64 MSDOS_Script::getDosStubSize()
{
    return g_nDosStubSize;
}

bool MSDOS_Script::isDosStubPresent()
{
    return g_bIsDosStubPresent;
}

qint32 MSDOS_Script::getNumberOfRichIDs()
{
    return g_nNumberOfRichIDs;
}

bool MSDOS_Script::isRichVersionPresent(quint32 nVersion)
{
    return g_pMSDOS->isRichVersionPresent(nVersion, &g_listRich);
}

quint32 MSDOS_Script::getRichVersion(qint32 nPosition)
{
    return g_pMSDOS->getRichVersion(&g_listRich, nPosition);
}

quint32 MSDOS_Script::getRichID(qint32 nPosition)
{
    return g_pMSDOS->getRichID(&g_listRich, nPosition);
}

quint32 MSDOS_Script::getRichCount(qint32 nPosition)
{
    return g_pMSDOS->getRichCount(&g_listRich, nPosition);
}

bool MSDOS_Script::isRichSignaturePresent()
{
    return g_bIisRichSignaturePresent;
}
