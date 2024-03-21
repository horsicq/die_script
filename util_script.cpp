/* Copyright (c) 2019-2023 hors<horsicq@gmail.com>
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
#include "util_script.h"

Util_script::Util_script(QObject *pParent)
    : QObject(pParent)
{

}

quint64 Util_script::shlu64(quint64 nValue, quint64 nShift)
{
    quint64 nResult = nValue << nShift;
    return nResult;
}

quint64 Util_script::shru64(quint64 nValue, quint64 nShift)
{
    quint64 nResult = nValue >> nShift;
    return nResult;
}

qint64 Util_script::shl64(qint64 nValue, qint64 nShift)
{
    qint64 nResult = nValue << nShift;
    return nResult;
}

qint64 Util_script::shr64(qint64 nValue, qint64 nShift)
{
    qint64 nResult = nValue >> nShift;
    return nResult;
}
