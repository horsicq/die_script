/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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
#ifndef UTIL_SCRIPT_H
#define UTIL_SCRIPT_H

#include <QObject>

class Util_script : public QObject {
    Q_OBJECT
public:
    explicit Util_script(QObject *pParent = nullptr);

public slots:
    quint64 shlu64(quint64 nValue, quint64 nShift);
    quint64 shru64(quint64 nValue, quint64 nShift);
    qint64 shl64(qint64 nValue, qint64 nShift);
    qint64 shr64(qint64 nValue, qint64 nShift);
    quint64 divu64(quint64 nDividend, quint64 nDivisor);
    qint64 div64(qint64 nDividend, qint64 nDivisor);
};

#endif  // UTIL_SCRIPT_H
