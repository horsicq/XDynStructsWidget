// copyright (c) 2021 hors<horsicq@gmail.com>
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
#ifndef XDYNSTRUCTSWIDGET_H
#define XDYNSTRUCTSWIDGET_H

#include <QWidget>
#include "xdynstructsengine.h"
#include "xhtml.h"

namespace Ui {
class XDynStructsWidget;
}

class XDynStructsWidget : public QWidget
{
    Q_OBJECT

    enum TYPE
    {
        TYPE_UNKNOWN=0,
        TYPE_OFFSET,
        TYPE_PROCESS
    };

public:
    explicit XDynStructsWidget(QWidget *pParent=nullptr);
    void setData(QIODevice *pDevice,qint64 nOffset);
    void setData(qint64 nProcessId);
    ~XDynStructsWidget();

private slots:
    void reload();

private:
    Ui::XDynStructsWidget *ui;
    TYPE g_type;
    QIODevice *g_pDevice;
    qint64 g_nOffset;
    qint64 g_nProcessId;
};

#endif // XDYNSTRUCTSWIDGET_H
