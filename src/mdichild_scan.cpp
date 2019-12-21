/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 20019 Ingo Foerster (pixbytesl@gmail.com).
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

void readDiskSectors::run()
{

    const int32 buff_length = 2048;
    uint8 buff[buff_length];
    threadStop = false;

    if(readSectors==0) readSectors = maxSectors;

    for(int nCurLba = startSectors; nCurLba<readSectors; nCurLba++){

        if(threadStop){
            break;
        }

        int32 nRes = ::ReadSectors(BS_CURRENT_DEVICE, nCurLba, 1,
                                    BS_IMG_ISO, buff, buff_length);
        bool isBad = false;
        if (nRes != BS_SDK_ERROR_NO){
            isBad = true;
            /*
            TCHAR chError[2048];
            int32 nLength = 2048;

            ::GetText(nRes,chError, &nLength);
            QString errDesc;
            errDesc= QString::fromUtf8(chError);

            qDebug("error: %s",errDesc.toLatin1().constData());
            */
        }
        emit tCurrentSector(nCurLba, isBad);

    }

    emit tStopScan();
    qDebug("Ende vom Thread");


}

MdiChildScan::MdiChildScan(QWidget* parent, const QString &device)
    : scrollArea(new QScrollArea)
     ,strBurnDrive(device){
    setAttribute(Qt::WA_DeleteOnClose);
    mProjectType = RuleManager::TYPE_PROJECT_DEVICEINFO;
    thisSuccessfullCreated = true;
    mreadDiskSectors = nullptr;

    QString wTitle = tr("Scan ");
    wTitle += " (";
    wTitle += strBurnDrive;
    wTitle += ")";

    setWindowTitle(wTitle);

    QVBoxLayout * dlgSplitter = new QVBoxLayout(this);
    QHBoxLayout * topNav = new QHBoxLayout(this);
    QWidget * wdgSplitter = new QWidget(this);

    QFont font;
    font.setFamily("Font Awesome 5 Free");
    font.setPixelSize(16);

    QLabel *labelTopNav = new QLabel(this);
    labelTopNav->setText(tr("Sector Offset: "));

    offsetSectors = new QDoubleSpinBox;
    offsetSectors->setMinimumWidth(100);
    offsetSectors->setMaximumWidth(100);
    offsetSectors->setRange(0, 2048.0);
    offsetSectors->setSingleStep(1.0);
    offsetSectors->setValue(0.0);
    offsetSectors->setDecimals(0); //No need to abstract int64 spinbox

    QSpacerItem *item2 = new QSpacerItem(15,1, QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel *labelReadTopNav = new QLabel(this);
    labelReadTopNav->setText(tr("Read Sectors: "));

    readSectors = new QDoubleSpinBox;
    readSectors->setMinimumWidth(100);
    readSectors->setMaximumWidth(100);
    readSectors->setRange(0, 2048.0);
    readSectors->setSingleStep(1.0);
    readSectors->setValue(0.0);
    readSectors->setDecimals(0); //No need to abstract int64 spinbox

    QSpacerItem *item = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed);

    startButton = new QPushButton();
    startButton->setFont(font);
    startButton->setToolTip(tr("Start disc scan"));
    startButton->setText("\uf51f");
    startButton->setEnabled(false);
    connect(startButton, SIGNAL(clicked()), this, SLOT(onStartScan()));

    stopButton = new QPushButton();
    stopButton->setFont(font);
    stopButton->setToolTip(tr("Stop scanning dsic"));
    stopButton->setText("\uf28d");
    stopButton->setEnabled(false);
    connect(stopButton, SIGNAL(clicked()), this, SLOT(onStop()));

    rescanButton = new QPushButton();
    rescanButton->setFont(font);
    rescanButton->setToolTip(tr("Re-Analyze disc"));
    rescanButton->setText("\uf079");
    connect(rescanButton, SIGNAL(clicked()), this, SLOT(onUpdateSectors()));

    reportButton = new QPushButton();
    reportButton->setFont(font);
    reportButton->setToolTip(tr("Save report to file"));
    reportButton->setText("\uf1c9");
    reportButton->setEnabled(false);
    connect(reportButton, SIGNAL(clicked()), this, SLOT(onWriteReport()));

    //Report
    //f15c

    topNav->addWidget(labelTopNav);
    topNav->addWidget(offsetSectors);
    topNav->addSpacerItem(item2);
    topNav->addWidget(labelReadTopNav);
    topNav->addWidget(readSectors);

    topNav->addSpacerItem(item);
    topNav->addWidget(startButton);
    topNav->addWidget(stopButton);
    topNav->addWidget(rescanButton);
    topNav->addWidget(reportButton);
    topNav->setContentsMargins(1,8,1,0);

    dlgSplitter->setContentsMargins(0,0,0,0);
    dlgSplitter->setSpacing(0);
    dlgSplitter->setMargin(0);

    dlgSplitter->addLayout(topNav);

    scanBoard = new QScanBoard(100, 50);

    dlgSplitter->addWidget(scanBoard);


    wdgSplitter->setLayout(dlgSplitter);
    this->setCentralWidget(wdgSplitter);


    //try to enumerate the disc if possible.
    analyzeDisc();
}

void MdiChildScan::setDisabled()
{
    thisSuccessfullCreated = false;
    startButton->setEnabled(false);
    stopButton->setEnabled(false);
    reportButton->setEnabled(false);
    offsetSectors->setValue(0.0);
    readSectors->setValue(0.0);
}

void MdiChildScan::analyzeDisc()
{

    if(mreadDiskSectors){
        mreadDiskSectors->threadStop = true;
        mreadDiskSectors = nullptr;
    };


    scanBoard->setMaxSide(0);
    scanBoard->setCurrentSector(0);


    int32 ret = ::SetBurnDevice(strBurnDrive.at(0).toLatin1());
    if(showDiskbutlerMessage(ret, this)==false) {
        setDisabled();
        return;
    }

    //We need here threading function with isDeviceReady call.
    BS_BOOL	bReady = BS_FALSE;
    ret = ::IsDeviceReady(BS_CURRENT_DEVICE, &bReady);
    /*
    if(showDiskbutlerMessage(ret, this)==false) {
        thisSuccessfullCreated = false;
        return;
    }*/

    bool bIsEmptyDisk = false;
    SMediumInfo mi;
    ret = ::GetMediumInformation(BS_CURRENT_DEVICE, &mi);
    if(showDiskbutlerMessage(ret, this)==false) {
        setDisabled();
        return;
    }

    if(mi.nMediumStatus==BS_MS_EMPTY_DISK){
        bIsEmptyDisk = true;
        //Paint empty text on board
        setDisabled();
        return;

    }

    if(bIsEmptyDisk==false){
        bufferSize = 2048;
        if(mi.nExtendedMediumType==BS_EMT_CD_AUDIO){
            bufferSize = 2352;
        }
        //Disable all controls. Default text possible for control?
        maxSectors = mi.dMediumUsedSize / bufferSize;

        startButton->setEnabled(true);
        stopButton->setEnabled(false);
        offsetSectors->setValue(0.0);
        readSectors->setValue(0);

        //Set max Sectors. Should paint them.
        scanBoard->setMaxSide(maxSectors);

    }
}

void MdiChildScan::startScanThread()
{
    scanBoard->startScan(offsetSectors->value());
    scanBoard->isActive=true;

    startButton->setEnabled(false);
    stopButton->setEnabled(true);
    rescanButton->setEnabled(false);
    readSectors->setEnabled(false);
    offsetSectors->setEnabled(false);
    reportButton->setEnabled(false);

    mreadDiskSectors  = new readDiskSectors();
    mreadDiskSectors->maxSectors = maxSectors;
    if(readSectors->value()==0){
        mreadDiskSectors->readSectors = maxSectors;
    }else{
        mreadDiskSectors->readSectors = offsetSectors->value() + readSectors->value();
    }


    mreadDiskSectors->startSectors = offsetSectors->value();
    connect(mreadDiskSectors, SIGNAL(tCurrentSector(int,bool)), this, SLOT(onCurrentSector(int,bool)));
    connect(mreadDiskSectors, SIGNAL(tStopScan()), this, SLOT(onStopScan()));

    mreadDiskSectors->start();
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
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    rescanButton->setEnabled(true);
    readSectors->setEnabled(true);
    offsetSectors->setEnabled(true);
    reportButton->setEnabled(true);
    scanBoard->isActive=false;
    mreadDiskSectors = nullptr;

}

void MdiChildScan::onUpdateSectors()
{
    analyzeDisc();
}

void MdiChildScan::onStartScan()
{
    startScanThread();
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

