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
#ifndef GLOBAL_SCRIPT_H
#define GLOBAL_SCRIPT_H

// TODO rename to die_global_script
#include <QObject>

class global_script : public QObject {
    Q_OBJECT

public:
    explicit global_script(QObject *pParent = nullptr);

public slots:
    void includeScript(QString sScript);
    void _log(QString sText);
    //    qint64 _min(qint64 nValue1,qint64 nValue2);
    //    qint64 _max(qint64 nValue1,qint64 nValue2);
    void _setResult(const QString &sType, QString sName, QString sVersion, QString sOptions);

signals:
    void includeScriptSignal(QString sScript);
    void _logSignal(QString sText);
    void _setResultSignal(QString sType, QString sName, QString sVersion, QString sOptions);
};

#endif  // GLOBAL_SCRIPT_H
