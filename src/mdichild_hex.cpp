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
#include "mdichild_hex.h"
#include "FoxSDKBurningLib.h"
#include "mainwindow.h"
#include "settingspages.h"
#include "messanger.h"

void ThreadHex::run()
{

    myChild->updateHex();

}

MdiChildHex::MdiChildHex(QWidget*, const QString &device)
    :strBurnDrive(device){
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(":/icons/hexedit32.png"));
    mProjectType = RuleManager::TYPE_PROJECT_HEX;
    mWorkThread = nullptr;

    hexView = new QHexView();


    QString wTitle = tr("Hex ");
    wTitle += " (";
    wTitle += strBurnDrive;
    wTitle += ")";

    setWindowTitle(wTitle);

    setWidget(hexView);

    setDataState(0);
    setBufferSize(2352);
    setMaxSector(0) ;
    setSector(0);


    QTimer::singleShot( 500, this, SLOT(startUpdate()));
}



int32 MdiChildHex::readSector(int sectorIndex, int buffersize)
{

    char *buf = new char[2352];
    int32 ret = 0;

    if(buffersize==2048){
        ret = ::ReadSectors(BS_CURRENT_DEVICE, sectorIndex, 1, BS_IMG_ISO, buf, buffersize);
    }else{
        ret = ::ReadSectors(BS_CURRENT_DEVICE, sectorIndex, 1, BS_IMG_BIN, buf, buffersize);
    }

    if (ret != BS_SDK_ERROR_NO){
        //Error weill be handeled outside this function
        //hexView->setData(nullptr);
    }else{
        hexView->clear();
        QByteArray byteArray = QByteArray::fromRawData(buf, buffersize);
        hexView->setData(new QHexView::DataStorageArray(byteArray));
    }

    return ret;
}

void MdiChildHex::qDebugAusgabeSDK(int32 errCode, const QString &customMessage)
{

    TCHAR chError[2048];
    int32 nLength = 2048;

    emit stopSpinner();

    ::GetText(errCode,chError, &nLength);
    QString errDesc;

    errDesc= convertToQT(chError);

    qDebug("SDKError %s: %s", customMessage.toLatin1().constData(), errDesc.toLatin1().constData());


}

void MdiChildHex::saveBlock()
{

    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save Sector "),
                QDir::currentPath(),
                tr("Text File (*.txt)") );

    if (!fileName.isEmpty()){
        hexView->saveTofile(fileName);
        return;
    }

}

void MdiChildHex::startHexThread()
{

    QTimer::singleShot( 500, this, SLOT(startUpdate()));
}

void MdiChildHex::startUpdate()
{
    emit startSpinner();
    setSector(0);
    setMaxSector(0) ;

    hexView->setData(nullptr);

    if(!mWorkThread){
        mWorkThread = new ThreadHex(this);
        connect(this, SIGNAL(updatedHexFinished(int,int)), this, SLOT(on_hex_completed(int,int)));
    }

    mWorkThread->start();

}

void MdiChildHex::updateHex()
{

    int mMaxSize = 0;
    int bufferSize = 2352;
    //char *buf = new char[2352];

    int32 ret = ::SetBurnDevice(getBurnDrive().at(0).toLatin1());
    if (ret == BS_SDK_ERROR_NO){

        BS_BOOL	bReady = BS_FALSE;
        ret = ::IsDeviceReady(BS_CURRENT_DEVICE, &bReady);
        if (ret == BS_SDK_ERROR_NO){

            bool bIsEmptyDisk = false;
            SMediumInfo mi;
            ret = ::GetMediumInformation(BS_CURRENT_DEVICE, &mi);
            if (ret == BS_SDK_ERROR_NO){

                if(mi.nMediumStatus==BS_MS_EMPTY_DISK || ret != BS_SDK_ERROR_NO){
                    bIsEmptyDisk = true;

                }

                if(bIsEmptyDisk==false){
                    bufferSize = 2048;
                    if(mi.nExtendedMediumType==BS_EMT_CD_AUDIO){
                        bufferSize = 2352;
                    }

                    mMaxSize = static_cast<int>(mi.dMediumUsedSize / static_cast<double>(bufferSize));


                    ret = readSector(getSector(),bufferSize);
                    if(ret != BS_SDK_ERROR_NO){
                        qDebugAusgabeSDK(ret, "ReadSector");
                    }
                }

            }else{
                qDebugAusgabeSDK(ret, "GetMediumInfo");
            }

        }

    }

    emit updatedHexFinished(mMaxSize, bufferSize);
}

void MdiChildHex::on_hex_completed(int mMaxSize, int bufferSize)
{

    setDataState(0);

    hexView->clear();
    if(mMaxSize==0){
        hexView->setData(nullptr);
    }else{
        //
        setDataState(1);
    }

    setBufferSize(bufferSize);
    setMaxSector(mMaxSize) ;

    emit stopSpinner();
    emit datatrack_changed();
}

void MdiChildHex::setUIControls( Ribbon *, QWidget* parent )
{
    MainWindow *ribbonOwner = qobject_cast<MainWindow *>(parent);

    //Wir haben
    //View
    //Edit
    ribbonOwner->delEditButton->setEnabled(false);
    ribbonOwner->renameEditButton->setEnabled(false);
    ribbonOwner->viewBrowserButton->setEnabled(false);
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

    //HEx ist das Aktive Fentser.
    //Emulate the burndrive.
    TCHAR chBurnDevice[50];
    int32 nLength = sizeof(chBurnDevice)/sizeof(chBurnDevice[0]);
    ::GetBurnDevice( chBurnDevice, &nLength );

    QString test = convertToQT(chBurnDevice);

    if( test.at(0) != getBurnDrive().at(0) ){
        //activeMdiHexChild()->updateHex(false);
    }

    ribbonOwner->hexSector->setRange( 0, getMaxSector() );
    ribbonOwner->hexSector->setValue( getSector() );
    ribbonOwner->updateHexEditor->setEnabled( true );

    if( getDataState() == 1 ){

        //Is eneabled
        ribbonOwner->saveHexEditor->setEnabled( true );
        if( getSector() == 0 ){
            ribbonOwner->navFirstHexEditor->setEnabled( false );
            ribbonOwner->navStepBackHexEditor->setEnabled( false );
        }else{
            ribbonOwner->navFirstHexEditor->setEnabled( true );
            ribbonOwner->navStepBackHexEditor->setEnabled( true );
        }
        if( getSector() == getMaxSector() ){
            ribbonOwner->navLastHexEditor->setEnabled( false );
            ribbonOwner->navStepForwardHexEditor->setEnabled( false );
        }else{
            ribbonOwner->navLastHexEditor->setEnabled( true );
            ribbonOwner->navStepForwardHexEditor->setEnabled( true );
        }
        ribbonOwner->hexSector->setEnabled( true );
    }else{//Is disabled
        ribbonOwner->hexSector->setValue( getSector() );
        ribbonOwner->saveHexEditor->setEnabled( false );
        ribbonOwner->navFirstHexEditor->setEnabled( false );
        ribbonOwner->navLastHexEditor->setEnabled( false );
        ribbonOwner->navStepBackHexEditor->setEnabled( false );
        ribbonOwner->navStepForwardHexEditor->setEnabled( false );
        ribbonOwner->hexSector->setEnabled( false );
    }
}

//SetBurnDevice List ist in den pDevice Projekten dann das ReadDevice.
void MdiChildHex::setBurnDeviceList(QWidget* parent)
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
void MdiChildHex::setRibbonTabs(Ribbon *baseRibbon, QWidget* parent)
{

    //Für morgen
    //IM Child wird das letzte aktive Tab gespeichert.
    //Bei DiskInfo wird gespeichert ob 2Image aktiv wwar.
    baseRibbon->blockSignals(true);
    QString lastTab = getlastSelectedTab();

    baseRibbon->hideAll();
    baseRibbon->showTab(":/icons/hexedit32.png",tr("Hex Editor"));
    if(baseRibbon->isTabVisible(lastTab)){
        baseRibbon->currentTab(lastTab);
    }else{
        baseRibbon->currentTab(tr("Hex Editor"));
    }

    if(parent!=nullptr){
        setUIControls(baseRibbon, parent);
        setBurnDeviceList(parent);
    }

    baseRibbon->blockSignals(false);

}
