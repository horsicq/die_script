// copyright (c) 2019 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef DIE_SCRIPT_H
#define DIE_SCRIPT_H

#include <QObject>
#include <QDir>
#include "die_scriptengine.h"
#include "xbinary.h"

class DiE_Script : public QObject
{
    Q_OBJECT

    void _loadDatabase(QString sDatabasePath,DiE_ScriptEngine::STYPE stype);
    void _scan(QIODevice *pDevice,DiE_ScriptEngine::STYPE stype);

public:
    explicit DiE_Script(QObject *parent=nullptr);
    bool loadDatabase(QString sDatabasePath);
    bool scanFile(QString sFileName); // TODO Options

private:
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> listSignatures;
};

#endif // DIE_SCRIPT_H
