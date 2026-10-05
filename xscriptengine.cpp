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
#include "xscriptengine.h"

#include <QMetaMethod>

XScriptEngine::XScriptEngine()
{
}

#ifdef QT_SCRIPT_LIB
void XScriptEngine::_addFunction(QScriptEngine::FunctionSignature function, const QString &sFunctionName)
{
    QScriptValue func = this->newFunction(function);
    this->globalObject().setProperty(sFunctionName, func);
}
#endif

void XScriptEngine::_addClass(QObject *pClass, QString sClassName)
{
#if defined(QT_QML_LIB) && !defined(QT_SCRIPT_LIB)
    // QJSEngine makes every method of a wrapped QObject a read-only property,
    // so "PE.getSize = function () {...}" throws, and the database's _init
    // files replace dozens of methods with caching versions that way. QtScript
    // allowed it. A read-only property also blocks the assignment when it is
    // only inherited, so the script sees a plain object that holds its own
    // writable copy of each method; the copies stay bound to pClass. The
    // wrapper remains the prototype for everything else (toString, signals'
    // connect). As with QtScript, each name gets its own object, so Binary
    // and PE no longer share overrides.
    XSCRIPTVALUE wrapper = this->newQObject(pClass);
    XSCRIPTVALUE objectWnd = this->newObject();

    // The copies go in before the wrapper becomes the prototype: setProperty
    // is an ordinary assignment, and an inherited read-only method would
    // silently block it exactly as it blocks the scripts.
    const QMetaObject *pMetaObject = pClass->metaObject();
    qint32 nNumberOfMethods = pMetaObject->methodCount();

    for (qint32 i = 0; i < nNumberOfMethods; i++) {
        QString sMethodName = QString::fromLatin1(pMetaObject->method(i).name());

        if (!objectWnd.hasOwnProperty(sMethodName)) {
            XSCRIPTVALUE method = wrapper.property(sMethodName);

            if (method.isCallable()) {
                objectWnd.setProperty(sMethodName, method);
            }
        }
    }

    objectWnd.setPrototype(wrapper);
#else
    XSCRIPTVALUE objectWnd = this->newQObject(pClass);
#endif
    this->globalObject().setProperty(sClassName, objectWnd);
}
