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

    g_nPageIndex=0;
    g_bAddPageEnable=true;

//    XOptions::setMonoFont(ui->textBrowserStructs);

    connect(ui->textBrowserStructs,SIGNAL(anchorClicked(QUrl)),this,SLOT(onAnchorClicked(QUrl)));
}

void XDynStructsWidget::setData(XDynStructsEngine *pStructsEngine, qint64 nAddress)
{
    g_pStructsEngine=pStructsEngine;

    ui->comboBoxStructsCurrent->addItem("","");
    ui->comboBoxStructsCurrent->addItem(tr("Array"),"array");

    QList<XDynStructsEngine::DYNSTRUCT> *pListStructs=pStructsEngine->getStructs();

    int nNumberOfStructs=pListStructs->count();

    for(int i=0;i<nNumberOfStructs;i++)
    {
        ui->comboBoxStructsCurrent->addItem(pListStructs->at(i).sName,pListStructs->at(i).sName);
    }

    ui->lineEditStructsCurrentAddress->setValueOS(nAddress);

    reload("");
}

XDynStructsWidget::~XDynStructsWidget()
{
    delete ui;
}

bool XDynStructsWidget::reload(QString sStructName)
{
    bool bResult=true;

    qint64 nAddress=ui->lineEditStructsCurrentAddress->getValue();

    bResult=adjustComboBox(sStructName);

    XDynStructsEngine::INFO info={};

    if(bResult)
    {
        info=g_pStructsEngine->getInfo(nAddress,sStructName);
        bResult=info.bIsValid;
    }

    if(bResult)
    {
        XHtml xtml;

        int nNumberOfRecords=info.listRecords.count();

        xtml.addTableBegin();

        for(int i=0;i<nNumberOfRecords;i++)
        {
            QList<XHtml::TABLECELL> listCells;

            XHtml::TABLECELL cellAddress={};

            if(info.listRecords.at(i).nAddress!=-1)
            {
                cellAddress.sText=XBinary::valueToHex(info.listRecords.at(i).nAddress);
            }
            else
            {
                cellAddress.sText="";
            }
            listCells.append(cellAddress);

            XHtml::TABLECELL cellOffset={};

            if(info.listRecords.at(i).nOffset!=-1)
            {
                cellOffset.sText=XBinary::valueToHex(info.listRecords.at(i).nOffset);
            }
            else
            {
                cellOffset.sText="";
            }
            listCells.append(cellOffset);

            XHtml::TABLECELL cellType={};
            cellType.sText=info.listRecords.at(i).sType;
            listCells.append(cellType);

            XHtml::TABLECELL cellName={};
            cellName.sText=info.listRecords.at(i).sName;
            listCells.append(cellName);

            XHtml::TABLECELL cellValue={};

            qint32 nValueSize=info.listRecords.at(i).sValue.size();

            if((nValueSize==4)&&(info.listRecords.at(i).sValue!="0x00"))
            {
                cellValue.bBold=true;
            }
            else if((nValueSize==6)&&(info.listRecords.at(i).sValue!="0x0000"))
            {
                cellValue.bBold=true;
            }
            else if((nValueSize==10)&&(info.listRecords.at(i).sValue!="0x00000000"))
            {
                cellValue.bBold=true;
            }
            else if((nValueSize==18)&&(info.listRecords.at(i).sValue!="0x0000000000000000"))
            {
                cellValue.bBold=true;
            }

            cellValue.sText=XHtml::makeLink(info.listRecords.at(i).sValue,info.listRecords.at(i).sValueData);
            listCells.append(cellValue);

            XHtml::TABLECELL cellComment={};
            cellComment.sText=info.listRecords.at(i).sComment;
            listCells.append(cellComment);

            xtml.addTableRow(listCells);
        }

        xtml.addTableEnd();

        QString sHtml=xtml.toString();

        ui->textBrowserStructs->setHtml(sHtml);

        PAGE page={};

        page.nAddress=nAddress;
        page.sStructName=sStructName;
        page.sText=sHtml;

        addPage(page);
    }
    else
    {
        // TODO pDevice
        if(g_pStructsEngine->getProcessId())
        {
            restorePage(ui->textBrowserStructs->verticalScrollBar()->value());

            showViewer(nAddress,VIEWTYPE_HEX);
        }
    }

    adjusStatus();

    return bResult;
}

void XDynStructsWidget::onAnchorClicked(const QUrl &sLink)
{
    qint64 nAddress=sLink.toString().section("&",0,0).section("0x",1,1).toLongLong(0,16);
    QString sStruct=sLink.toString().section("&",1,1);

    ui->lineEditStructsCurrentAddress->setValueOS(nAddress);

    reload(sStruct);
}

void XDynStructsWidget::on_pushButtonStructsReload_clicked()
{
    g_bAddPageEnable=false;
    reload(ui->comboBoxStructsCurrent->currentData().toString());
    g_bAddPageEnable=true;

    int nPageCount=g_listPages.count();

    if(nPageCount&&(g_nPageIndex<nPageCount))
    {
        g_listPages[g_nPageIndex].nAddress=ui->lineEditStructsCurrentAddress->getValue();
        g_listPages[g_nPageIndex].sStructName=ui->comboBoxStructsCurrent->currentData().toString();
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
    Q_UNUSED(bState)
    // TODO
}

void XDynStructsWidget::on_pushButtonStructsBack_clicked()
{
    if(g_nPageIndex>0)
    {
        g_nPageIndex--;
        restorePage(0);
    }

    adjusStatus();
}

void XDynStructsWidget::on_pushButtonStructsForward_clicked()
{
    if(g_nPageIndex<(g_listPages.count()-1))
    {
        g_nPageIndex++;
        restorePage(0);
    }

    adjusStatus();
}

void XDynStructsWidget::adjusStatus()
{
    ui->pushButtonStructsBack->setEnabled(g_nPageIndex>0);
    ui->pushButtonStructsForward->setEnabled(g_nPageIndex<(g_listPages.count()-1));
}

bool XDynStructsWidget::adjustComboBox(QString sStructName)
{
    bool bResult=false;

    int nNumberOfRecords=ui->comboBoxStructsCurrent->count();

    if(sStructName!="")
    {
        bResult=false;

        for(int i=0;i<nNumberOfRecords;i++)
        {
            if(ui->comboBoxStructsCurrent->itemData(i).toString()==sStructName)
            {
                ui->comboBoxStructsCurrent->setCurrentIndex(i);

                bResult=true;

                break;
            }
        }
    }
    else
    {
        if(nNumberOfRecords)
        {
            ui->comboBoxStructsCurrent->setCurrentIndex(0);
        }

        bResult=true;
    }

    return bResult;
}

void XDynStructsWidget::restorePage(qint32 nProgressBarValue)
{
    PAGE currentPage=getCurrentPage();

    ui->lineEditStructsCurrentAddress->setValueOS(currentPage.nAddress);
    adjustComboBox(currentPage.sStructName);
    ui->textBrowserStructs->setHtml(currentPage.sText);
    ui->textBrowserStructs->verticalScrollBar()->setValue(nProgressBarValue);
}

void XDynStructsWidget::showViewer(qint64 nAddress, XDynStructsWidget::VIEWTYPE viewType)
{
    bool bSuccess=false;

    XProcess::MEMORY_REGION memoryRegion=XProcess::getMemoryRegion(g_pStructsEngine->getProcessId(),nAddress);

    if(memoryRegion.nSize)
    {
        XProcessDevice processDevice;

        if(processDevice.openPID(g_pStructsEngine->getProcessId(),memoryRegion.nAddress,memoryRegion.nSize,QIODevice::ReadOnly))
        {
            if(viewType==VIEWTYPE_HEX)
            {
                XHexView::OPTIONS hexOptions={};
                hexOptions.sTitle=QString("%1: %2").arg(QString("PID"),QString::number(g_pStructsEngine->getProcessId()));
                hexOptions.nStartAddress=memoryRegion.nAddress;
                hexOptions.nStartSelectionOffset=nAddress-memoryRegion.nAddress;

                DialogHexView dialogHexView(this);

                dialogHexView.setData(&processDevice,hexOptions);
                dialogHexView.setShortcuts(getShortcuts());

                dialogHexView.exec();

                bSuccess=true;
            }
            else if(viewType==VIEWTYPE_DISASM)
            {
                XMultiDisasmWidget::OPTIONS disasmOptions={};
                disasmOptions.sTitle=QString("%1: %2").arg(QString("PID"),QString::number(g_pStructsEngine->getProcessId()));
                disasmOptions.nStartAddress=memoryRegion.nAddress;
                disasmOptions.nInitAddress=nAddress;
                disasmOptions.fileType=XBinary::FT_REGION;

            #ifdef Q_OS_WIN32
                disasmOptions.sArch="386";
            #endif
            #ifdef Q_OS_WIN64
                disasmOptions.sArch="AMD64";
            #endif

                DialogMultiDisasm dialogMultiDisasm(this);

                dialogMultiDisasm.setData(&processDevice,disasmOptions);
                dialogMultiDisasm.setShortcuts(getShortcuts());

                dialogMultiDisasm.exec();

                bSuccess=true;
            }

            processDevice.close();
        }
    }

    if(!bSuccess)
    {
        QMessageBox::critical(XOptions::getMainWidget(this),tr("Error"),QString("%1: %2").arg(tr("Cannot read memory at address"),XBinary::valueToHexOS(nAddress)));
    }
}

void XDynStructsWidget::on_pushButtonStructsHex_clicked()
{
    qint64 nAddress=ui->lineEditStructsCurrentAddress->getValue();

    showViewer(nAddress,VIEWTYPE_HEX);
}

void XDynStructsWidget::on_pushButtonStructsDisasm_clicked()
{
    qint64 nAddress=ui->lineEditStructsCurrentAddress->getValue();

    showViewer(nAddress,VIEWTYPE_DISASM);
}

void XDynStructsWidget::on_pushButtonStructsSave_clicked()
{
    QString sFileName=QString("%1.html").arg(tr("Result"));

    sFileName=QFileDialog::getSaveFileName(this,tr("Save"),sFileName,QString("HTML %1 (*.html);;%2 (*)").arg(tr("Files"),tr("All files")));

    if(!sFileName.isEmpty())
    {
        XOptions::saveTextBrowserHtml(ui->textBrowserStructs,sFileName);
    }
}

void XDynStructsWidget::on_comboBoxStructsCurrent_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    QString sName=ui->comboBoxStructsCurrent->currentData().toString();

    XDynStructsEngine::DYNSTRUCT dynStruct=g_pStructsEngine->getDynStructByName(sName);

    ui->pushButtonStructsPrototype->setEnabled(dynStruct.sInfoFile!="");
}

void XDynStructsWidget::on_pushButtonStructsPrototype_clicked()
{
    QString sName=ui->comboBoxStructsCurrent->currentData().toString();

    XDynStructsEngine::DYNSTRUCT dynStruct=g_pStructsEngine->getDynStructByName(sName);

    if(XBinary::isFileExists(dynStruct.sInfoFile))
    {
        DialogTextInfo dialogTextInfo(this);

        dialogTextInfo.setFile(dynStruct.sInfoFile);
        dialogTextInfo.exec();
    }
}
