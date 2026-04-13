/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
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
#ifndef DIE_SCRIPT_H
#define DIE_SCRIPT_H

#include "die_scriptengine.h"
#ifdef QT_SCRIPTTOOLS_LIB
#include <QAction>
#include <QScriptEngineDebugger>
#endif

class DiE_Script : public XScanEngine {
    Q_OBJECT

public:
    explicit DiE_Script(QObject *pParent = nullptr);
    DiE_Script(const DiE_Script &other);
#ifdef QT_SCRIPTTOOLS_LIB
    void setDebugger(QScriptEngineDebugger *pDebugger);
    void removeDebugger();
#endif
    static QList<XScanEngine::SCANSTRUCT> convert(const QList<DiE_ScriptEngine::SCAN_STRUCT> *pListScanStructs);

    // bool loadDatabaseFromGlobalOptions(XOptions *pXOptions);
    bool isSignatureFileValid(const QString &sSignatureFilePath) override;

private:
    void processDetect(XScanEngine::SCANID *pScanID, XScanEngine::SCAN_RESULT *pScanResult, QIODevice *pDevice, const XScanEngine::SCANID &parentId, XBinary::FT fileType,
                       XScanEngine::SCAN_OPTIONS *pScanOptions, const QString &sSignatureFilePath, bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct);
    bool _handleError(DiE_ScriptEngine *pScriptEngine, XSCRIPTVALUE scriptValue, const XScanEngine::SIGNATURE_RECORD *pSignatureRecord,
                      XScanEngine::SCAN_RESULT *pScanResult);
    bool _shouldExecuteSignature(const SIGNATURE_RECORD &signatureRecord, XBinary::FT fileType, const SCAN_OPTIONS *pScanOptions, const QString &sSignatureFilePath) const;
    void _executeInitSignature(DiE_ScriptEngine *pScriptEngine, const SIGNATURE_RECORD &signatureRecord, SCAN_RESULT *pScanResult);
    void _executeSignature(DiE_ScriptEngine *pScriptEngine, const QString &sDetectFunction, const SIGNATURE_RECORD &signatureRecord, const SCANID &parentId,
                           const SCANID &resultId, SCAN_RESULT *pScanResult, SCAN_OPTIONS *pScanOptions);
    void _handleElapsedTime(const SIGNATURE_RECORD &signatureRecord, qint64 nElapsedTime, SCAN_RESULT *pScanResult, const SCAN_OPTIONS *pScanOptions);

protected:
    void _processDetect(XScanEngine::SCANID *pScanID, SCAN_RESULT *pScanResult, QIODevice *pDevice, const SCANID &parentId, XBinary::FT fileType,
                        SCAN_OPTIONS *pOptions, bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct) override;
    QString getEngineName() override;
    SCANENGINETYPE getEngineType() override;

#ifdef QT_SCRIPTTOOLS_LIB
    QScriptEngineDebugger *m_pDebugger = nullptr;
#endif
};

#endif  // DIE_SCRIPT_H
