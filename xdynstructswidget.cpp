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
#include "xdynstructswidget.h"
#include "ui_xdynstructswidget.h"

XDynStructsWidget::XDynStructsWidget(QWidget *pParent) :
    QWidget(pParent),
    ui(new Ui::XDynStructsWidget)
{
    ui->setupUi(this);

    g_type=TYPE_UNKNOWN;
    g_nOffset=0;
    g_nProcessId=0;
    g_pDevice=0;

    connect(ui->textBrowserStructs,SIGNAL(anchorClicked(const QUrl &)),this,SLOT(onAnchorClicked(const QUrl &)));
}

void XDynStructsWidget::setData(QIODevice *pDevice, qint64 nOffset)
{
    g_pDevice=pDevice;
    g_nOffset=nOffset;
    g_type=TYPE_OFFSET;

    reload();
}

void XDynStructsWidget::setData(qint64 nProcessId)
{
    g_nProcessId=nProcessId;
    g_type=TYPE_PROCESS;

    reload();
}

XDynStructsWidget::~XDynStructsWidget()
{
    delete ui;
}

void XDynStructsWidget::reload()
{
    XDynStructsEngine structEngine;
    XDynStructsEngine::INFO info={};

    if(g_type==TYPE_PROCESS)
    {
        info=structEngine.getInfo(g_nProcessId);
    }
    else if(g_type==TYPE_OFFSET)
    {
        info=structEngine.getInfo(g_pDevice,g_nOffset);
    }

    XHtml xtml;

    int nNumberOfRecords=info.listRecords.count();

    xtml.addTableBegin();

    for(int i=0;i<nNumberOfRecords;i++)
    {
        QList<XHtml::TABLECELL> listCells;
        XHtml::TABLECELL cell;

        if(info.listRecords.at(i).nAddress!=-1)
        {
            cell.sText=XBinary::valueToHex(info.listRecords.at(i).nAddress);
        }
        else
        {
            cell.sText="";
        }
        listCells.append(cell);

        if(info.listRecords.at(i).nOffset!=-1)
        {
            cell.sText=XBinary::valueToHex(info.listRecords.at(i).nOffset);
        }
        else
        {
            cell.sText="";
        }
        listCells.append(cell);

        cell.sText=info.listRecords.at(i).sType;
        listCells.append(cell);

        cell.sText=info.listRecords.at(i).sName;
        listCells.append(cell);

        cell.sText=XHtml::makeLink(info.listRecords.at(i).sValue,info.listRecords.at(i).sValueData);
        listCells.append(cell);

        cell.sText=info.listRecords.at(i).sComment;
        listCells.append(cell);

        xtml.addTableRow(listCells);
    }

    xtml.addTableEnd();

    QString sHtml=xtml.toString();

    ui->textBrowserStructs->setHtml(sHtml);

    // TODO
}

void XDynStructsWidget::onAnchorClicked(const QUrl &sLink)
{
    ui->textBrowserStructs->setHtml(sLink.toString());

    // TODO
}
