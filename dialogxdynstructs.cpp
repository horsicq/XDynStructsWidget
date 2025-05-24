/* Copyright (c) 2021-2025 hors<horsicq@gmail.com>
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
#include "dialogxdynstructs.h"

#include "ui_dialogxdynstructs.h"

DialogXDynStructs::DialogXDynStructs(QWidget *pParent) : XShortcutsDialog(pParent), ui(new Ui::DialogXDynStructs)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);
}

void DialogXDynStructs::setData(XDynStructsEngine *pStructEngine, quint64 nAddress)
{
    ui->widgetStructs->setData(pStructEngine, nAddress);

    QString sTitle;

    if (pStructEngine->getDevice()) {
        sTitle = XBinary::getDeviceFileName(pStructEngine->getDevice());
    } else if (pStructEngine->getProcessId()) {
        sTitle = QString("%1: %2").arg(QString("PID"), QString::number(pStructEngine->getProcessId()));
    }

    setWindowTitle(sTitle);
}

void DialogXDynStructs::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    ui->widgetStructs->setGlobal(pShortcuts, pXOptions);
}

DialogXDynStructs::~DialogXDynStructs()
{
    delete ui;
}

void DialogXDynStructs::on_pushButtonClose_clicked()
{
    this->close();
}
