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
#include "xdynstructswidget.h"

#include "ui_xdynstructswidget.h"

XDynStructsWidget::XDynStructsWidget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::XDynStructsWidget)
{
    ui->setupUi(this);

    g_nPageIndex = 0;
    g_bAddPageEnable = true;
    g_pStructsEngine = nullptr;

    //    XOptions::setMonoFont(ui->textBrowserStructs); // mb TODO TODO Check
    connect(ui->textBrowserStructs, SIGNAL(anchorClicked(QUrl)), this, SLOT(onAnchorClicked(QUrl)));
}

void XDynStructsWidget::setData(XDynStructsEngine *pStructsEngine, quint64 nAddress)
{
    g_pStructsEngine = pStructsEngine;

    ui->comboBoxStructsCurrent->clear();
    ui->comboBoxStructsCurrent->addItem("", "");
    ui->comboBoxStructsCurrent->addItem(tr("Array"), "array");

    ui->comboBoxStructsType->clear();
    ui->comboBoxStructsType->addItem(tr("Variable"), XDynStructsEngine::STRUCTTYPE_VARIABLE);
    ui->comboBoxStructsType->addItem(tr("Pointer"), XDynStructsEngine::STRUCTTYPE_POINTER);

    ui->spinBoxStructsCount->setValue(1);

    QList<XDynStructsEngine::DYNSTRUCT> *pListStructs = pStructsEngine->getStructs();

    qint32 nNumberOfStructs = pListStructs->count();

    for (qint32 i = 0; i < nNumberOfStructs; i++) {
        ui->comboBoxStructsCurrent->addItem(pListStructs->at(i).sName, pListStructs->at(i).sName);
    }

    ui->lineEditStructsCurrentAddress->setValue32_64(nAddress);

    reload();
}

XDynStructsWidget::~XDynStructsWidget()
{
    delete ui;
}

bool XDynStructsWidget::reload()
{
    bool bResult = true;

    qint64 nAddress = ui->lineEditStructsCurrentAddress->getValue_int64();
    QString sStructName = ui->comboBoxStructsCurrent->currentData().toString();
    XDynStructsEngine::STRUCTTYPE structType = (XDynStructsEngine::STRUCTTYPE)(ui->comboBoxStructsType->currentData().toInt());
    qint32 nCount = ui->spinBoxStructsCount->value();

    XDynStructsEngine::INFO info = g_pStructsEngine->getInfo(nAddress, sStructName, structType, nCount);

    bResult = info.bIsValid;

    if (bResult) {
        XHtml xtml;

        qint32 nNumberOfRecords = info.listRecords.count();

        xtml.addTableBegin();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            QList<XHtml::TABLECELL> listCells;

            XHtml::TABLECELL cellAddress = {};

            if (info.listRecords.at(i).nAddress != (quint64)-1) {
                cellAddress.sText = XBinary::valueToHex(info.listRecords.at(i).nAddress);
            } else {
                cellAddress.sText = "";
            }
            listCells.append(cellAddress);

            XHtml::TABLECELL cellOffset = {};

            if (info.listRecords.at(i).nOffset != (quint64)-1) {
                cellOffset.sText = XBinary::valueToHex(info.listRecords.at(i).nOffset);
            } else {
                cellOffset.sText = "";
            }
            listCells.append(cellOffset);

            XHtml::TABLECELL cellType = {};
            cellType.sText = info.listRecords.at(i).sType;
            listCells.append(cellType);

            XHtml::TABLECELL cellName = {};
            cellName.sText = info.listRecords.at(i).sName;
            listCells.append(cellName);

            XHtml::TABLECELL cellValue = {};

            qint32 nValueSize = info.listRecords.at(i).sValue.size();

            if (info.listRecords.at(i).sValue != "") {
                if ((nValueSize == 4) && (info.listRecords.at(i).sValue != "0x00")) {
                    cellValue.bBold = true;
                } else if ((nValueSize == 6) && (info.listRecords.at(i).sValue != "0x0000")) {
                    cellValue.bBold = true;
                } else if ((nValueSize == 10) && (info.listRecords.at(i).sValue != "0x00000000")) {
                    cellValue.bBold = true;
                } else if ((nValueSize == 18) && (info.listRecords.at(i).sValue != "0x0000000000000000")) {
                    cellValue.bBold = true;
                }
            }

            cellValue.sText = XHtml::makeLink(info.listRecords.at(i).sValue, info.listRecords.at(i).sValueData);
            listCells.append(cellValue);

            XHtml::TABLECELL cellComment = {};
            cellComment.sText = info.listRecords.at(i).sComment;

            listCells.append(cellComment);

            xtml.addTableRow(listCells);
        }

        xtml.addTableEnd();

        QString sHtml = xtml.toString();

        ui->textBrowserStructs->setHtml(sHtml);

        PAGE page = {};

        page.nAddress = nAddress;
        page.sStructName = sStructName;
        page.structType = structType;
        page.nCount = nCount;
        page.sText = sHtml;

        addPage(page);
    }

    adjusStatus();

    return bResult;
}

void XDynStructsWidget::onAnchorClicked(const QUrl &urlLink)
{
    QString sLink = urlLink.toString();

    quint64 nAddress = sLink.section("&", 0, 0).section("0x", 1, 1).toULongLong(0, 16);
    QString sStructName = sLink.section("&", 1, 1);
    XDynStructsEngine::STRUCTTYPE structType = (XDynStructsEngine::STRUCTTYPE)(sLink.section("&", 2, 2).toInt());
    qint32 nCount = sLink.section("&", 3, 3).toInt();

    if (nCount < ui->spinBoxStructsCount->minimum()) {
        nCount = ui->spinBoxStructsCount->minimum();
    }

    if (nCount > ui->spinBoxStructsCount->maximum()) {
        nCount = ui->spinBoxStructsCount->maximum();
    }

    ui->lineEditStructsCurrentAddress->setValue32_64(nAddress);
    ui->spinBoxStructsCount->setValue(nCount);

    if (adjustComboBoxName(sStructName) && adjustComboBoxType(structType)) {
        reload();
    } else {
        // TODO pDevice !!!
        if (g_pStructsEngine->getProcessId()) {
            restorePage(ui->textBrowserStructs->verticalScrollBar()->value());

            showViewer(nAddress, VIEWTYPE_HEX);
        }
    }
}

void XDynStructsWidget::on_pushButtonStructsReload_clicked()
{
    g_bAddPageEnable = false;
    reload();
    g_bAddPageEnable = true;

    qint32 nPageCount = g_listPages.count();

    if (nPageCount && (g_nPageIndex < nPageCount)) {
        g_listPages[g_nPageIndex].nAddress = ui->lineEditStructsCurrentAddress->getValue_int64();
        g_listPages[g_nPageIndex].sStructName = ui->comboBoxStructsCurrent->currentData().toString();
        g_listPages[g_nPageIndex].structType = (XDynStructsEngine::STRUCTTYPE)(ui->comboBoxStructsType->currentData().toInt());
        g_listPages[g_nPageIndex].nCount = ui->spinBoxStructsCount->value();
        g_listPages[g_nPageIndex].sText = ui->textBrowserStructs->toHtml();
    }
}

void XDynStructsWidget::addPage(PAGE page)
{
    if (g_bAddPageEnable) {
        qint32 nNumberOfPages = g_listPages.count();

        for (qint32 i = nNumberOfPages - 1; i > g_nPageIndex; i--) {
            g_listPages.removeAt(i);
        }

        g_listPages.append(page);

        if (g_listPages.count() > 100)  // TODO consts
        {
            g_listPages.removeFirst();
        }

        g_nPageIndex = g_listPages.count() - 1;
    }
}

XDynStructsWidget::PAGE XDynStructsWidget::getCurrentPage()
{
    PAGE result = {};

    qint32 nPageCount = g_listPages.count();

    if (nPageCount && (g_nPageIndex < nPageCount)) {
        result = g_listPages.at(g_nPageIndex);
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
    if (g_nPageIndex > 0) {
        g_nPageIndex--;
        restorePage(0);
    }

    adjusStatus();
}

void XDynStructsWidget::on_pushButtonStructsForward_clicked()
{
    if (g_nPageIndex < (g_listPages.count() - 1)) {
        g_nPageIndex++;
        restorePage(0);
    }

    adjusStatus();
}

void XDynStructsWidget::adjusStatus()
{
    ui->pushButtonStructsBack->setEnabled(g_nPageIndex > 0);
    ui->pushButtonStructsForward->setEnabled(g_nPageIndex < (g_listPages.count() - 1));
}

bool XDynStructsWidget::adjustComboBoxName(QString sName)
{
    bool bResult = false;

    qint32 nNumberOfRecords = ui->comboBoxStructsCurrent->count();

    if (sName != "") {
        bResult = false;

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            if (ui->comboBoxStructsCurrent->itemData(i).toString() == sName) {
                ui->comboBoxStructsCurrent->setCurrentIndex(i);

                bResult = true;

                break;
            }
        }
    } else {
        if (nNumberOfRecords) {
            ui->comboBoxStructsCurrent->setCurrentIndex(0);
        }

        bResult = true;
    }

    return bResult;
}

bool XDynStructsWidget::adjustComboBoxType(XDynStructsEngine::STRUCTTYPE structType)
{
    bool bResult = false;

    qint32 nNumberOfRecords = ui->comboBoxStructsType->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((XDynStructsEngine::STRUCTTYPE)(ui->comboBoxStructsType->itemData(i).toInt()) == structType) {
            ui->comboBoxStructsType->setCurrentIndex(i);

            bResult = true;

            break;
        }
    }

    return bResult;
}

void XDynStructsWidget::restorePage(qint32 nProgressBarValue)
{
    PAGE currentPage = getCurrentPage();

    ui->lineEditStructsCurrentAddress->setValue32_64(currentPage.nAddress);
    adjustComboBoxName(currentPage.sStructName);
    adjustComboBoxType(currentPage.structType);
    ui->spinBoxStructsCount->setValue(currentPage.nCount);
    ui->textBrowserStructs->setHtml(currentPage.sText);
    ui->textBrowserStructs->verticalScrollBar()->setValue(nProgressBarValue);
}

void XDynStructsWidget::showViewer(quint64 nAddress, XDynStructsWidget::VIEWTYPE viewType)
{
    bool bSuccess = false;

    // TODO Device
    XProcess::MEMORY_REGION memoryRegion = {};

    if (g_pStructsEngine->getIOMode() == XDynStructsEngine::IOMODE_PROCESS_USER) {
        memoryRegion = XProcess::getMemoryRegion_Id(g_pStructsEngine->getProcessId(), nAddress);
    }
#ifdef Q_OS_WIN
    else if (g_pStructsEngine->getIOMode() == XDynStructsEngine::IOMODE_PROCESS_KERNEL) {
        // TODO
        memoryRegion.nAddress = S_ALIGN_DOWN(nAddress, 0x1000);
        memoryRegion.nSize = 0x1000;
    }
#endif

    if (memoryRegion.nSize) {
        XIODevice *pIODevice = g_pStructsEngine->createIODevice(memoryRegion.nAddress, memoryRegion.nSize);

        if (pIODevice->open(QIODevice::ReadOnly)) {
            if (viewType == VIEWTYPE_HEX) {
                XHexView::OPTIONS hexOptions = {};
                hexOptions.sTitle = QString("%1: %2").arg(QString("PID"), QString::number(g_pStructsEngine->getProcessId()));
                hexOptions.nStartOffset = memoryRegion.nAddress;
                hexOptions.nStartSelectionOffset = nAddress - memoryRegion.nAddress;

                DialogHexView dialogHexView(this);

                // dialogHexView.setData(pIODevice, hexOptions, nullptr,);  // TODO XInfoDB
                dialogHexView.setGlobal(getShortcuts(), getGlobalOptions());

                dialogHexView.exec();

                bSuccess = true;
            } else if (viewType == VIEWTYPE_DISASM) {
                XMultiDisasmWidget::OPTIONS disasmOptions = {};
                disasmOptions.sTitle = QString("%1: %2").arg(QString("PID"), QString::number(g_pStructsEngine->getProcessId()));
                disasmOptions.nStartAddress = memoryRegion.nAddress;
                disasmOptions.nInitAddress = nAddress;
                disasmOptions.fileType = XBinary::FT_REGION;

                // TODO ARM!!!
#ifdef Q_OS_WIN32
                disasmOptions.sArch = "386";
#endif
#ifdef Q_OS_WIN64
                disasmOptions.sArch = "AMD64";
#endif

                DialogMultiDisasm dialogMultiDisasm(this);

                dialogMultiDisasm.setData(pIODevice, disasmOptions);
                dialogMultiDisasm.setGlobal(getShortcuts(), getGlobalOptions());

                dialogMultiDisasm.exec();

                bSuccess = true;
            }

            pIODevice->close();
        }

        delete pIODevice;
    }

    if (!bSuccess) {
        QMessageBox::critical(XOptions::getMainWidget(this), tr("Error"), QString("%1: %2").arg(tr("Cannot read memory at address"), XBinary::valueToHexOS(nAddress)));
    }
}

void XDynStructsWidget::on_pushButtonStructsHex_clicked()
{
    quint64 nAddress = ui->lineEditStructsCurrentAddress->getValue_uint64();

    showViewer(nAddress, VIEWTYPE_HEX);
}

void XDynStructsWidget::on_pushButtonStructsDisasm_clicked()
{
    quint64 nAddress = ui->lineEditStructsCurrentAddress->getValue_uint64();

    showViewer(nAddress, VIEWTYPE_DISASM);
}

void XDynStructsWidget::on_pushButtonStructsSave_clicked()
{
    QString sFileName = QString("%1.html").arg(tr("Result"));

    sFileName = QFileDialog::getSaveFileName(this, tr("Save"), sFileName, QString("HTML %1 (*.html);;%2 (*)").arg(tr("Files"), tr("All files")));

    if (!sFileName.isEmpty()) {
        XOptions::saveTextBrowserHtml(ui->textBrowserStructs, sFileName);
    }
}

void XDynStructsWidget::on_comboBoxStructsCurrent_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    QString sName = ui->comboBoxStructsCurrent->currentData().toString();

    XDynStructsEngine::DYNSTRUCT dynStruct = g_pStructsEngine->getDynStructByName(sName);

    ui->pushButtonStructsPrototype->setEnabled(dynStruct.sInfoFile != "");
}

void XDynStructsWidget::on_pushButtonStructsPrototype_clicked()
{
    QString sName = ui->comboBoxStructsCurrent->currentData().toString();

    XDynStructsEngine::DYNSTRUCT dynStruct = g_pStructsEngine->getDynStructByName(sName);

    if (XArchives::isArchiveRecordPresent(dynStruct.sInfoFilePrefix + ".zip", dynStruct.sInfoFile)) {
        DialogTextInfo dialogTextInfo(this);

        dialogTextInfo.setArchive(dynStruct.sInfoFilePrefix + ".zip", dynStruct.sInfoFile);
        dialogTextInfo.exec();
    } else if (XBinary::isFileExists(dynStruct.sInfoFilePrefix + QDir::separator() + dynStruct.sInfoFile))  // TODO Archive
    {
        DialogTextInfo dialogTextInfo(this);

        dialogTextInfo.setFileName(dynStruct.sInfoFilePrefix + QDir::separator() + dynStruct.sInfoFile);
        dialogTextInfo.exec();
    }
}
