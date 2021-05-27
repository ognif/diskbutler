/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 2021 Ingo Foerster (pixbytesl@gmail.com).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License 3 as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License 3 for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <QtWidgets>
#include "mdichild_scan.h"
#include "FoxSDKBurningLib.h"
#include "mainwindow.h"
#include "settingspages.h"
#include "messanger.h"

void ThreadAnalyze::run()
{
    myChild->analyzeDisc();
}

void readDiskSectors::run()
{

    const int32 buff_length = bufferSize;
    uint8 buff[2352];
    threadStop = false;

    if(readSectors==0) readSectors = maxSectors;

    for(int nCurLba = startSectors; nCurLba<readSectors; nCurLba++){

        if(threadStop){
            break;
        }

        int32 nRes = 0;

        if(bufferSize==2048){
            nRes = ::ReadSectors(BS_CURRENT_DEVICE, nCurLba, 1, BS_IMG_ISO, buff, buff_length);
        }else{
            nRes = ::ReadSectors(BS_CURRENT_DEVICE, nCurLba, 1, BS_IMG_BIN, buff, buff_length);
        }

        bool isBad = false;
        if (nRes != BS_SDK_ERROR_NO){
            isBad = true;

        }
        emit tCurrentSector(nCurLba, isBad);

    }

    emit tStopScan();


}

MdiChildScan::MdiChildScan(const QString &device, QWidget*)
    : scrollArea(new QScrollArea)
    ,strBurnDrive(device){
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(":/icons/diskscan32.png"));
    mProjectType = RuleManager::TYPE_PROJECT_SCAN;
    mreadDiskSectors = nullptr;
    manalyzeDisk = nullptr;

    QString wTitle = tr("Scan ");
    wTitle += " (";
    wTitle += strBurnDrive;
    wTitle += ")";

    setWindowTitle(wTitle);



    scanBoard = new QScanBoard(100, 50);
    setWidget(scanBoard);

    //Default values
    setScanState(false);
    setSectorOffset(0);
    setReadOffset(0);
    setDataState(0);
    setBufferSize(2352);
    setMaxSector(0);


    //try to enumerate the disc if possible.
    //startUpdateInfo();
    QTimer::singleShot( 500, this, SLOT(startUpdateInfo()));
}

void MdiChildScan::qDebugAusgabeSDK(int32 errCode, const QString &customMessage)
{
    TCHAR chError[2048];
    int32 nLength = 2048;

    emit stopSpinner();

    ::GetText(errCode,chError, &nLength);
    QString errDesc;
    errDesc= convertToQT(chError);

    qDebug("SDKError %s: %s", customMessage.toLatin1().constData(), errDesc.toLatin1().constData());

}



void MdiChildScan::startScanThread(int offsetSectors, int readSectors)
{

    qDebug("Offset: %d",offsetSectors);
    qDebug("Read: %d",readSectors);
    qDebug("max: %d",getMaxSector());
    qDebug("Buffer: %d",getBufferSize());
    qDebug("Drive: %s",getBurnDrive().toLatin1().constData());


    scanBoard->startScan(offsetSectors);
    scanBoard->isActive=true;

    int32 ret = ::SetBurnDevice(getBurnDrive().at(0).toLatin1());
    if (ret != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(ret, "SetBurnDevice");
        return;
    }

    if(!mreadDiskSectors){
        mreadDiskSectors  = new readDiskSectors();
        connect(mreadDiskSectors, SIGNAL(tCurrentSector(int,bool)), this, SLOT(onCurrentSector(int,bool)));
        connect(mreadDiskSectors, SIGNAL(tStopScan()), this, SLOT(onStopScan()));
    }

    mreadDiskSectors->maxSectors = getMaxSector();
    mreadDiskSectors->bufferSize = getBufferSize();
    if(readSectors==0){
        mreadDiskSectors->readSectors = getMaxSector();
    }else{
        mreadDiskSectors->readSectors = offsetSectors + readSectors;
    }

    setScanState(true);

    mreadDiskSectors->start();

    qDebug("Start Thread");
}

//SLOTS for interaction

//Wie starten wir einen Thread in QT?
void MdiChildScan::onCurrentSector(int newSector, bool isBad)
{
    if(isBad==true){
        scanBoard->setBadSector(newSector);
    }
    scanBoard->setCurrentSector(newSector);

}

void MdiChildScan::onStop()
{
    mreadDiskSectors->threadStop = true;
}

void MdiChildScan::onStopScan()
{
    qDebug("Stop can");
    scanBoard->isActive=false;
    setScanState(false);
    mreadDiskSectors = nullptr;
    emit enableControls();

}

void MdiChildScan::onStartScan(int offsetSectors, int readSectors)
{
    startScanThread(offsetSectors, readSectors);
}

void MdiChildScan::onWriteReport()
{
    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Repor"),
                QDir::currentPath(),
                tr("Text File (*.txt)") );

    if (fileName.isEmpty()){
        QMessageBox::information(this, tr("Information"),
                                 tr("No file selected"));
        return;
    }

    scanBoard->saveTofile(fileName,strBurnDrive);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void MdiChildScan::analyzeDisc()
{

    emit startSpinner();

    int bufferSize = 2352;
    int dataState = 0;
    int mMaxSize = 0;

    int32 ret = ::SetBurnDevice(getBurnDrive().at(0).toLatin1());
    if (ret == BS_SDK_ERROR_NO){
        BS_BOOL	bReady = BS_FALSE;
        ret = ::IsDeviceReady(BS_CURRENT_DEVICE, &bReady);
        if (ret == BS_SDK_ERROR_NO){
            bool bIsEmptyDisk = false;
            SMediumInfo mi;
            ret = ::GetMediumInformation(BS_CURRENT_DEVICE, &mi);
            if (ret == BS_SDK_ERROR_NO){
                if(mi.nMediumStatus==BS_MS_EMPTY_DISK){
                    bIsEmptyDisk = true;
                }
                if(bIsEmptyDisk==false){
                    bufferSize = 2048;
                    if(mi.nExtendedMediumType==BS_EMT_CD_AUDIO){
                        bufferSize = 2352;
                    }

                    //Disable all controls. Default text possible for control?
                    mMaxSize = static_cast<int>(mi.dMediumUsedSize / static_cast<double>(bufferSize));
                    dataState = 1;
                }
            }
        }
    }

    emit thread_finished(mMaxSize, bufferSize, dataState);

}

void MdiChildScan::onUpdateSectors()
{

    QTimer::singleShot( 500, this, SLOT(startUpdateInfo()));

}

void MdiChildScan::startUpdateInfo()
{

    if(mreadDiskSectors){
        mreadDiskSectors->threadStop = true;
        mreadDiskSectors = nullptr;
    }



    scanBoard->setMaxSide(0);
    scanBoard->setCurrentSector(0);

    if(!manalyzeDisk){
        manalyzeDisk = new ThreadAnalyze(this);
        connect(this, SIGNAL(thread_finished(int,int,int)), this, SLOT(on_thread_completed(int,int,int)));
    }

    manalyzeDisk->start();

}

void MdiChildScan::on_thread_completed(int mMaxSize, int bufferSize, int dataState)
{

    setDataState(dataState);
    setBufferSize(bufferSize);
    setMaxSector(mMaxSize);
    scanBoard->setMaxSide(mMaxSize);
    emit stopSpinner();
    emit datatrack_changed2();

}

void MdiChildScan::setUIControls(Ribbon *, QWidget* parent)
{
    MainWindow *ribbonOwner = qobject_cast<MainWindow *>(parent);

    ribbonOwner->delEditButton->setEnabled(false);
    ribbonOwner->renameEditButton->setEnabled(false);
    ribbonOwner->viewBrowserButton->setEnabled(false);
    ribbonOwner->viewPropertyGridButton->setEnabled(false);
    ribbonOwner->delAllEditButton->setEnabled(false);
    ribbonOwner->updtEditButton->setEnabled(false);
    ribbonOwner->inverseSelectEditButton->setEnabled(false);
    ribbonOwner->allSelectEditButton->setEnabled(false);
    ribbonOwner->addFolderEditButton->setEnabled(false);
    ribbonOwner->addFileEditButton->setEnabled(false);
    ribbonOwner->dataTrackEditButton->setEnabled(false);
    ribbonOwner->audioTrackEditButton->setEnabled(false);
    ribbonOwner->appSaveButton->setEnabled(false);
    ribbonOwner->appSaveAsButton->setEnabled(false);
    ribbonOwner->createFolderEditButton->setEnabled(false);

    ribbonOwner->grabAudioMediaButton->setEnabled(false);
    ribbonOwner->imageMediaButton->setEnabled(false);

    ribbonOwner->scanReadOffset->setRange( 0, getMaxSector() );
    ribbonOwner->scanStartOffset->setRange( 0, getMaxSector() );
    ribbonOwner->scanReadOffset->setValue( getReadOffset() );
    ribbonOwner->scanStartOffset->setValue( getSectorOffset() );

    //Scan ist das Aktive Fenster
    ribbonOwner->updateScanEditor->setEnabled( true );

    if( getDataState() == 1 ){
        qDebug() << "DataState=1";
        if( getScanState() == false ){
            qDebug() << "ScanState = flase";
            ribbonOwner->startScanEditor->setEnabled( true );
            ribbonOwner->stopScanEditor->setEnabled( false );
            ribbonOwner->saveScanEditor->setEnabled( true );
            ribbonOwner->scanStartOffset->setEnabled( true );
            ribbonOwner->scanReadOffset->setEnabled( true );
            ribbonOwner->updateScanEditor->setEnabled( true );
        }else{
            qDebug() << "ScanState = true";
            ribbonOwner->updateScanEditor->setEnabled( false );
            ribbonOwner->startScanEditor->setEnabled( false );
            ribbonOwner->stopScanEditor->setEnabled( true );
            ribbonOwner->saveScanEditor->setEnabled( false );
            ribbonOwner->scanStartOffset->setEnabled( false );
            ribbonOwner->scanReadOffset->setEnabled( false );
        }
    }else{
        qDebug() << "DataState!=1";
        ribbonOwner->startScanEditor->setEnabled( false );
        ribbonOwner->stopScanEditor->setEnabled( false );
        ribbonOwner->saveScanEditor->setEnabled( false );
        ribbonOwner->scanStartOffset->setEnabled( false );
        ribbonOwner->scanReadOffset->setEnabled( false );
    }
}

//SetBurnDevice List ist in den pDevice Projekten dann das ReadDevice.
void MdiChildScan::setBurnDeviceList(QWidget* parent)
{
    MainWindow *ribbonOwner = qobject_cast<MainWindow *>(parent);

    for(int i = 0; i < ribbonOwner->listReadDevicesWidget->count(); ++i)
    {
        if(ribbonOwner->listReadDevicesWidget->itemText(i) == getBurnDrive()){
            ribbonOwner->listReadDevicesWidget->setCurrentIndex(i);
        }
    }
}

//emit subwindowchanged(GetProjectType());
void MdiChildScan::setRibbonTabs(Ribbon *baseRibbon, QWidget* parent)
{

    //Für morgen
    //IM Child wird das letzte aktive Tab gespeichert.
    //Bei DiskInfo wird gespeichert ob 2Image aktiv wwar.
    baseRibbon->blockSignals(true);
    QString lastTab = getlastSelectedTab();
    qDebug() << "Scan";
    baseRibbon->hideAll();
    baseRibbon->showTab(":/icons/diskscan32.png",tr("Scan Editor"));
    if(baseRibbon->isTabVisible(lastTab)){
        baseRibbon->currentTab(lastTab);
    }else{
        baseRibbon->currentTab(tr("Scan Editor"));
    }

    if(parent!=nullptr){
        setUIControls(baseRibbon, parent);
        setBurnDeviceList(parent);
    }

    baseRibbon->blockSignals(false);

}
