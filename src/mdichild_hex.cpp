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
#include "mdichild_hex.h"
#include "FoxSDKBurningLib.h"
#include "mainwindow.h"
#include "settingspages.h"
#include "messanger.h"

void ThreadHex::run()
{

    myChild->updateHex();

}

MdiChildHex::MdiChildHex(QWidget* parent, const QString &device)
    :strBurnDrive(device){
    setAttribute(Qt::WA_DeleteOnClose);
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
    errDesc= QString::fromUtf8(chError);
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
    char *buf = new char[2352];

    int32 ret = ::SetBurnDevice(getBurnDrive().at(0).toLatin1());
    if (ret != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(ret, "SetBurnDevice");
        return;
    }

    BS_BOOL	bReady = BS_FALSE;
    ret = ::IsDeviceReady(BS_CURRENT_DEVICE, &bReady);
    if (ret != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(ret, "IsDeviceReady");
        return;
    }

    bool bIsEmptyDisk = false;
    SMediumInfo mi;
    ret = ::GetMediumInformation(BS_CURRENT_DEVICE, &mi);
    if (ret != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(ret, "GetMediumInfo");
        return;
    }



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

