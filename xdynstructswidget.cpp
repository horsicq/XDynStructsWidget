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
    XShortcutsWidget(pParent),
    ui(new Ui::XDynStructsWidget)
{
    ui->setupUi(this);

    g_type=TYPE_UNKNOWN;
    g_nOffset=0;
    g_nAddress=0;
    g_nProcessId=0;
    g_pDevice=0;
    g_nPageIndex=0;
    g_bAddPageEnable=true;

    connect(ui->textBrowserStructs,SIGNAL(anchorClicked(const QUrl &)),this,SLOT(onAnchorClicked(const QUrl &)));
}

void XDynStructsWidget::setData(QIODevice *pDevice, qint64 nOffset)
{
    g_pDevice=pDevice;
    g_nOffset=nOffset;
    g_type=TYPE_OFFSET;

    ui->lineEditAddress->setValueOS(nOffset);
    reload("");
}

void XDynStructsWidget::setData(qint64 nProcessId, qint64 nAddress)
{
    g_nProcessId=nProcessId;
    g_nAddress=nAddress;
    g_type=TYPE_PROCESS;

    ui->lineEditAddress->setValueOS(nAddress);
    reload("");
}

XDynStructsWidget::~XDynStructsWidget()
{
    delete ui;
}

bool XDynStructsWidget::reload(QString sStruct)
{
    bool bResult=true;

    qint64 nAddress=ui->lineEditAddress->getValue();

    XDynStructsEngine structEngine;
    XDynStructsEngine::INFO info={};

    // TODO get struct name from combobox
    if(sStruct!="")
    {
        bResult=false;

        int nNumberOfRecords=ui->comboBoxStruct->count();

        for(int i=0;i<nNumberOfRecords;i++)
        {
            if(ui->comboBoxStruct->itemData(i).toString()==sStruct)
            {
                bResult=true;

                break;
            }
        }
    }

    if(bResult)
    {
        if(g_type==TYPE_PROCESS)
        {
            info=structEngine.getInfo(g_nProcessId,nAddress,sStruct);
        }
        else if(g_type==TYPE_OFFSET)
        {
            info=structEngine.getInfo(g_pDevice,g_nOffset,sStruct);
        }

        bResult=info.listRecords.count();
    }

    if(bResult)
    {
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

        PAGE page={};

        page.nAddress=nAddress;
        page.sStruct=sStruct;
        page.sText=sHtml;

        addPage(page);
    }
    else
    {
        PAGE currentPage=getCurrentPage();

        ui->lineEditAddress->setValueOS(currentPage.nAddress);
        // TODO setComboBox
        ui->textBrowserStructs->setHtml(currentPage.sText);

        bool bSuccess=false;

        void *pProcess=XProcess::openProcess(g_nProcessId);

        if(pProcess)
        {
            XProcess::MEMORY_REGION memoryRegion=XProcess::getMemoryRegion(pProcess,nAddress);

            if(memoryRegion.nSize)
            {
                XProcessDevice processDevice;

                if(processDevice.openHandle(pProcess,memoryRegion.nAddress,memoryRegion.nSize,QIODevice::ReadOnly))
                {
                    DialogHexView dialogHexView(this);

                    XHexView::OPTIONS options={};
                    options.sTitle=QString("%1: %2").arg(QString("PID"),QString::number(g_nProcessId));
                    options.nStartAddress=memoryRegion.nAddress;

                    dialogHexView.setData(&processDevice,options);

                    dialogHexView.exec();

                    bSuccess=true;

                    processDevice.close();
                }
            }

            XProcess::closeProcess(pProcess);
        }

        if(!bSuccess)
        {
            QMessageBox::critical(this,tr("Error"),QString("%1: %2").arg(tr("Cannot read memory at address"),XBinary::valueToHexOS(nAddress)));
        }
    }

    return bResult;
}

void XDynStructsWidget::onAnchorClicked(const QUrl &sLink)
{
    qint64 nAddress=sLink.toString().section("&",0,0).toLongLong(0,16);
    QString sStruct=sLink.toString().section("&",1,1);

    ui->lineEditAddress->setValueOS(nAddress);

    reload(sStruct);
}

void XDynStructsWidget::on_pushButtonReload_clicked()
{
    g_bAddPageEnable=false;
    reload(ui->comboBoxStruct->currentData().toString());
    g_bAddPageEnable=true;

    int nPageCount=g_listPages.count();

    if(nPageCount&&(g_nPageIndex<nPageCount))
    {
        g_listPages[g_nPageIndex].nAddress=ui->lineEditAddress->getValue();
        g_listPages[g_nPageIndex].sStruct=ui->comboBoxStruct->currentData().toString();
        g_listPages[g_nPageIndex].sText=ui->textBrowserStructs->toHtml();
    }
}

void XDynStructsWidget::addPage(PAGE page)
{
    if(g_bAddPageEnable)
    {
        qint32 nNumberOfPages=g_listPages.count();

        for(int i=nNumberOfPages-1;i>g_nPageIndex;i--)
        {
            g_listPages.removeAt(i);
        }

        g_listPages.append(page);

        if(g_listPages.count()>100) // TODO consts
        {
            g_listPages.removeFirst();
        }

        g_nPageIndex=g_listPages.count()-1;
    }
}

XDynStructsWidget::PAGE XDynStructsWidget::getCurrentPage()
{
    PAGE result={};

    int nPageCount=g_listPages.count();

    if(nPageCount&&(g_nPageIndex<nPageCount))
    {
        result=g_listPages.at(g_nPageIndex);
    }

    return result;
}

void XDynStructsWidget::registerShortcuts(bool bState)
{
    // TODO
}
