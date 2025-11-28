/* Copyright (c) 2017-2025 hors<horsicq@gmail.com>
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
#ifndef XDYNSTRUCTSOPTIONSWIDGET_H
#define XDYNSTRUCTSOPTIONSWIDGET_H

#include <QWidget>

#include "xbinary.h"
#include "xoptions.h"

namespace Ui {
class XDynStructsOptionsWidget;
}

class XDynStructsOptionsWidget : public QWidget {
    Q_OBJECT

public:
    explicit XDynStructsOptionsWidget(QWidget *pParent = nullptr);
    ~XDynStructsOptionsWidget();

    void setOptions(XOptions *pOptions);
    void save();

    static void setDefaultValues(XOptions *pOptions);

private slots:
    void on_toolButtonDynStructsPath_clicked();

private:
    Ui::XDynStructsOptionsWidget *ui;
    XOptions *m_pOptions;
};

#endif  // XDYNSTRUCTSOPTIONSWIDGET_H
