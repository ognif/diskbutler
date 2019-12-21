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
#include "mdichild_deviceinfo.h"
#include "FoxSDKBurningLib.h"
#include "mainwindow.h"
#include "settingspages.h"

MdiChildDeviceInfo::MdiChildDeviceInfo(QWidget* parent, const QString &device)
    :strBurnDrive(device){
    setAttribute(Qt::WA_DeleteOnClose);
    mProjectType = RuleManager::TYPE_PROJECT_DEVICEINFO;
    bShowToolTips = ConfigurationPage::mSettings.value("deviceInfoshowToolTips", false).toBool();
    bHideEmptyFields = ConfigurationPage::mSettings.value("deviceInfoHideEmptyFields", false).toBool();
    nRegionChanges = 0;
    nCurrentRegion = 0;

    deviceInfoTable = new QTableWidget();
    treeWidget = new QTreeWidget();

    QString wTitle = tr("Device Info");
    wTitle += " (";
    wTitle += strBurnDrive;
    wTitle += ")";

    setWindowTitle(wTitle);

    splitter = new QSplitter;
    splitter->addWidget(deviceInfoTable);
    splitter->addWidget(treeWidget);
    setCentralWidget(splitter);

    //myRefreshMenus = new QAction(this);
    //connect(myRefreshMenus, SIGNAL(triggered()), parent, SLOT(specialActionDeviceInfo()));

    readDeviceInfo();
}

void MdiChildDeviceInfo::updateDeviceInfo()
{

    readDeviceInfo();
    myRefreshMenus->trigger();

}

void MdiChildDeviceInfo::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void MdiChildDeviceInfo::readDeviceInfo()
{
    deviceInfoTable->clear();
    treeWidget->clear();

    deviceInfoTable->setRowCount(50);
    deviceInfoTable->setColumnCount(2);
    deviceInfoTable->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    deviceInfoTable->horizontalHeader()->setStretchLastSection(true);
    deviceInfoTable->horizontalHeader()->hide();
    deviceInfoTable->verticalHeader()->hide();
    //diskInfoTable->setContentsMargins(0, 0, 0, 0);
    deviceInfoTable->verticalHeader()->setDefaultSectionSize(21);

    //Group
    QTableWidgetItem* item1 = new QTableWidgetItem(tr("General Information"));
    item1->setFlags(Qt::NoItemFlags);
    QFont originalFont = (item1)->font();
    originalFont.setBold(true);
    item1->setFont(originalFont);
    QTableWidgetItem* item2 = new QTableWidgetItem("");
    item2->setFlags(Qt::NoItemFlags);
    deviceInfoTable->setItem(0,0,item1);
    deviceInfoTable->setItem(0,1,item2);
    deviceInfoTable->setSpan(0,0,1,2);

    QStringList labels;
    labels << tr("Capa") << tr("Description") ;
    treeWidget->setHeaderLabels(labels);
    treeWidget->header()->setStretchLastSection(true);
    treeWidget->header()->setDefaultSectionSize(300);
    //treeWidget->header() ->close ();

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(treeWidget);
    treeItem->setText(0, tr("Device"));
    treeItem->setIcon(0,QIcon(":/icons/disk_main.png"));
    treeItem->setExpanded(true);

    pDevice stDeviceInfo;
    stDeviceInfo.isDevice = 0;

    TCHAR vendorId_[9];
    TCHAR productId_[17];
    TCHAR productRevision_[5];

    ::SetBurnDevice(strBurnDrive.at(0).toLatin1());

    ::GetDeviceInformation(BS_CURRENT_DEVICE, vendorId_, productId_, productRevision_);

    SExtendedDeviceInformation	infoEx;
    if (::GetDeviceInformationEx(BS_CURRENT_DEVICE, &infoEx) == BS_SDK_ERROR_NO)
    {
        stDeviceInfo.isDevice = 1;
        //QString::fromWCharArray(pISOVolumeInfo.chVolumeLabel);

#if defined (WIN32)
    stDeviceInfo.strSerialNumber = QString::fromUtf16(infoEx.lpszSerialNumber);
    stDeviceInfo.strLoader = QString::fromUtf16(infoEx.lpszLoaderType);
    stDeviceInfo.strConnInterface = QString::fromUtf16(infoEx.lpszConnectionInterface);
    stDeviceInfo.strDriveInterface = QString::fromUtf16(infoEx.lpszPhysicalInterface);
    stDeviceInfo.strVendor = QString::fromUtf16(vendorId_);
    stDeviceInfo.strModel = QString::fromUtf16(productId_);
    stDeviceInfo.strName = QString::fromUtf16(infoEx.lpszName);
    stDeviceInfo.strRevision = QString::fromUtf16(infoEx.lpszRevision);
#else
    stDeviceInfo.strSerialNumber = QString::fromUtf8(infoEx.lpszSerialNumber);
    stDeviceInfo.strLoader = QString::fromUtf8(infoEx.lpszLoaderType);
    stDeviceInfo.strConnInterface = QString::fromUtf8(infoEx.lpszConnectionInterface);
    stDeviceInfo.strDriveInterface = QString::fromUtf8(infoEx.lpszPhysicalInterface);
    stDeviceInfo.strVendor = QString::fromUtf8(vendorId_);
    stDeviceInfo.strModel = QString::fromUtf8(productId_);
    stDeviceInfo.strName = QString::fromUtf8(infoEx.lpszName);
    stDeviceInfo.strRevision = QString::fromUtf8(infoEx.lpszRevision);
#endif

//IF FIXEN
        //stDeviceInfo.strFirmwareDate = QDateTime::fromTime_t( infoEx.FirmwareCreationDate ).toString("yyyy-MM-dd hh:mm:ss");


        stDeviceInfo.strBufferSize = QString::number(infoEx.nBufferSize);
        stDeviceInfo.strVolumeLevels = QString::number(infoEx.nNumberOfVolumeLevels);
        stDeviceInfo.strReadRetry = QString::number(infoEx.nReadRetryCount);
        stDeviceInfo.strRegionCode = QString::number(infoEx.nRegionCode);
        stDeviceInfo.strUserChangesLeft = QString::number(infoEx.nRegionCodeChangesLeft);
        stDeviceInfo.strVendorResetLeft = QString::number(infoEx.nRegionCodeVendorResetsLeft);

        nRegionChanges = infoEx.nRegionCodeChangesLeft;
        nCurrentRegion = infoEx.nRegionCode;

    }

    int nRowCount = 0;

    if(bHideEmptyFields==false || stDeviceInfo.strVendor.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceVendorFix = new QTableWidgetItem(tr("Vendor Name:"));
        itemDeviceVendorFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceVendor = new QTableWidgetItem(stDeviceInfo.strVendor);
        itemDeviceVendor->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceVendorFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceVendor);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strModel.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceModelFix = new QTableWidgetItem(tr("Model Name:"));
        itemDeviceModelFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceModel = new QTableWidgetItem(stDeviceInfo.strModel);
        itemDeviceModel->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceModelFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceModel);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strName.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceNameFix = new QTableWidgetItem(tr("Read Name:"));
        itemDeviceNameFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceName = new QTableWidgetItem(stDeviceInfo.strName);
        itemDeviceName->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceNameFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceName);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strSerialNumber.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceSerialNumberFix = new QTableWidgetItem(tr("Serial Number:"));
        itemDeviceSerialNumberFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceSerialNumber = new QTableWidgetItem(stDeviceInfo.strSerialNumber);
        itemDeviceSerialNumber->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceSerialNumberFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceSerialNumber);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strRevision.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceRevisionFix = new QTableWidgetItem(tr("Firmware Revision:"));
        itemDeviceRevisionFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceRevision = new QTableWidgetItem(stDeviceInfo.strRevision);
        itemDeviceRevision->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceRevisionFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceRevision);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strFirmwareDate.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceFirmwareDateFix = new QTableWidgetItem(tr("Firmware Date:"));
        itemDeviceFirmwareDateFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceFirmwareDate = new QTableWidgetItem(stDeviceInfo.strFirmwareDate);
        itemDeviceFirmwareDate->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceFirmwareDateFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceFirmwareDate);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strLoader.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceLoaderFix = new QTableWidgetItem(tr("Loader Type:"));
        itemDeviceLoaderFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceLoader = new QTableWidgetItem(stDeviceInfo.strLoader);
        itemDeviceLoader->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceLoaderFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceLoader);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strConnInterface.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceConInterfaceFix = new QTableWidgetItem(tr("Connection Interface:"));
        itemDeviceConInterfaceFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceConInterface = new QTableWidgetItem(stDeviceInfo.strConnInterface);
        itemDeviceConInterface->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceConInterfaceFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceConInterface);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strDriveInterface.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceInterfaceFix = new QTableWidgetItem(tr("Drive Interface:"));
        itemDeviceInterfaceFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceInterface = new QTableWidgetItem(stDeviceInfo.strDriveInterface);
        itemDeviceInterface->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceInterfaceFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceInterface);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strBufferSize.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceBufferSizeFix = new QTableWidgetItem(tr("Buffer Size:"));
        itemDeviceBufferSizeFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceBufferSize = new QTableWidgetItem(stDeviceInfo.strBufferSize);
        itemDeviceBufferSize->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceBufferSizeFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceBufferSize);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strVolumeLevels.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceVolumeLevelFix = new QTableWidgetItem(tr("Volume Levels:"));
        itemDeviceVolumeLevelFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceVolumeLevel = new QTableWidgetItem(stDeviceInfo.strVolumeLevels);
        itemDeviceVolumeLevel->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceVolumeLevelFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceVolumeLevel);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strReadRetry.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceReadRetryFix = new QTableWidgetItem(tr("Read Retry Count:"));
        itemDeviceReadRetryFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceReadRetry = new QTableWidgetItem(stDeviceInfo.strReadRetry);
        itemDeviceReadRetry->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceReadRetryFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceReadRetry);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strRegionCode.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceRegionCodeFix = new QTableWidgetItem(tr("Region Code:"));
        itemDeviceRegionCodeFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceRegionCode = new QTableWidgetItem(stDeviceInfo.strRegionCode);
        itemDeviceRegionCode->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceRegionCodeFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceRegionCode);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strUserChangesLeft.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceRegionUserChangesFix = new QTableWidgetItem(tr("User Changes Left:"));
        itemDeviceRegionUserChangesFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceRegionUserChanges = new QTableWidgetItem(stDeviceInfo.strUserChangesLeft);
        itemDeviceRegionUserChanges->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceRegionUserChangesFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceRegionUserChanges);
    }

    if(bHideEmptyFields==false || stDeviceInfo.strVendorResetLeft.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemDeviceRegionVendorResetFix = new QTableWidgetItem(tr("Vendor Resets Left:"));
        itemDeviceRegionVendorResetFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemDeviceRegionVendorReset = new QTableWidgetItem(stDeviceInfo.strVendorResetLeft);
        itemDeviceRegionVendorReset->setFlags(Qt::NoItemFlags);
        deviceInfoTable->setItem(nRowCount,0,itemDeviceRegionVendorResetFix);
        deviceInfoTable->setItem(nRowCount,1,itemDeviceRegionVendorReset);
    }

    deviceInfoTable->setRowCount(nRowCount);
    deviceInfoTable->resizeColumnsToContents();

    QString style(
        "QTableView:disabled {"
            "gridline-color: #32414B;"
                "border: 1px solid #32414B;"
                  "color: #32414B;"
        "}"
    );
    deviceInfoTable->setStyleSheet(style);

    //TreeWidget


    int32 nCapsHandle;
    int32 nErr = ::GetDeviceCapabilitiesHandle(BS_CURRENT_DEVICE, &nCapsHandle);
    if(nErr == BS_SDK_ERROR_NO)
    {
        BS_BOOL bRes;
        QTreeWidgetItem *treechildItem = new QTreeWidgetItem();
        treechildItem->setText(0, tr("Read Capabilities"));
        treeItem->addChild(treechildItem);

        nErr = ::AnalyseDeviceCapability(nCapsHandle, BS_READ_CDR, &bRes);
        QTreeWidgetItem *treeReadCapaItem1 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem1->setText(0, tr("Reads CD-R"));

        if(bShowToolTips==true){
            treeReadCapaItem1->setToolTip(0, tr("Device is able to read CD-R media."));
        }else{
            treeReadCapaItem1->setText(1, tr("Device is able to read CD-R media."));
        }

        if(bRes==BS_TRUE){
            treeReadCapaItem1->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem1->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle, BS_READ_CDRW, &bRes);
        QTreeWidgetItem *treeReadCapaItem2 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem2->setText(0, tr("Reads CD-RW"));

        if(bShowToolTips==true) treeReadCapaItem2->setToolTip(0, tr("Device is able to read CD ReWriteable media."));
        else treeReadCapaItem2->setText(1, tr("Device is able to read CD ReWriteable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem2->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem2->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle, BS_READ_CDRW_CAV, &bRes);
        QTreeWidgetItem *treeReadCapaItem3 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem3->setText(0, tr("Reads CD-RW CAV"));

        if(bShowToolTips==true) treeReadCapaItem3->setToolTip(0, tr("Device is able to read CD-RW media that is designed for CAV recording."));
        else treeReadCapaItem3->setText(1, tr("Device is able to read CD-RW media that is designed for CAV recording."));

        if(bRes==BS_TRUE){
            treeReadCapaItem3->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem3->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle, BS_READ_DVD, &bRes);
        QTreeWidgetItem *treeReadCapaItem4 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem4->setText(0, tr("Reads DVD-ROM"));

        if(bShowToolTips==true) treeReadCapaItem4->setToolTip(0, tr("Device is able to read DVD-ROM media."));
        else treeReadCapaItem4->setText(1, tr("Device is able to read DVD-ROM media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem4->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem4->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle, BS_READ_DVDR, &bRes);
        QTreeWidgetItem *treeReadCapaItem5 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem5->setText(0, tr("Reads DVD-R"));

        if(bShowToolTips==true) treeReadCapaItem5->setToolTip(0, tr("Device is able to read DVD-R media."));
        else treeReadCapaItem5->setText(1, tr("Device is able to read DVD-R media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem5->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem5->setIcon(0,QIcon(":/icons/029.png"));
        }        
        nErr = ::AnalyseDeviceCapability(nCapsHandle, BS_READ_DVDRW, &bRes);
        QTreeWidgetItem *treeReadCapaItem6 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem6->setText(0, tr("Reads DVD-RW"));

        if(bShowToolTips==true) treeReadCapaItem6->setToolTip(0, tr("Device is able to read DVD-ReWriteable media."));
        else treeReadCapaItem6->setText(1, tr("Device is able to read DVD-ReWriteable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem6->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem6->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_DVDRAM, &bRes);
        QTreeWidgetItem *treeReadCapaItem7 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem7->setText(0, tr("Reads DVD-RAM"));

        if(bShowToolTips==true) treeReadCapaItem7->setToolTip(0, tr("Device is able to read DVD-RAM media."));
        else treeReadCapaItem7->setText(1, tr("Device is able to read DVD-RAM media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem7->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem7->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_DVD_DL, &bRes);
        QTreeWidgetItem *treeReadCapaItem8 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem8->setText(0, tr("Reads DVD-R Dual Layer"));

        if(bShowToolTips==true) treeReadCapaItem8->setToolTip(0, tr("Device is able to read DVD-R Double Layer media."));
        else treeReadCapaItem8->setText(1, tr("Device is able to read DVD-R Double Layer media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem8->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem8->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_DVDR_PLUS, &bRes);
        QTreeWidgetItem *treeReadCapaItem9 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem9->setText(0, tr("Reads DVD+R"));

        if(bShowToolTips==true) treeReadCapaItem9->setToolTip(0, tr("Device is able to read DVD+R media."));
        else treeReadCapaItem9->setText(1, tr("Device is able to read DVD+R media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem9->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem9->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_DVDRW_PLUS, &bRes);
        QTreeWidgetItem *treeReadCapaItem10 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem10->setText(0, tr("Reads DVD+RW"));

        if(bShowToolTips==true) treeReadCapaItem10->setToolTip(0, tr("Device is able to read DVD+ReWriteable media."));
        else treeReadCapaItem10->setText(1, tr("Device is able to read DVD+ReWriteable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem10->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem10->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_DVD_RDL_PLUS, &bRes);
        QTreeWidgetItem *treeReadCapaItem11 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem11->setText(0, tr("Reads DVD+RW Dual Layer"));

        if(bShowToolTips==true) treeReadCapaItem11->setToolTip(0, tr("Device is able to read DVD+R Double Layer media."));
        else treeReadCapaItem11->setText(1, tr("Device is able to read DVD+R Double Layer media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem11->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem11->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_DVD_RWDL_PLUS, &bRes);
        QTreeWidgetItem *treeReadCapaItem12 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem12->setText(0, tr("Reads DVD+RW DL"));

        if(bShowToolTips==true) treeReadCapaItem12->setToolTip(0, tr("Device is able to read DVD+ReWriteable Double Layer media."));
        else treeReadCapaItem12->setText(1, tr("Device is able to read DVD+ReWriteable Double Layer media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem12->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem12->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_DVD_MRDL, &bRes);
        QTreeWidgetItem *treeReadCapaItem13 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem13->setText(0, tr("Reads DVD-R DL"));

        if(bShowToolTips==true) treeReadCapaItem13->setToolTip(0, tr("Device is able to read DVD-R Double Layer media."));
        else treeReadCapaItem13->setText(1, tr("Device is able to read DVD-R Double Layer media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem13->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem13->setIcon(0,QIcon(":/icons/029.png"));
        }

        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_BLURAY_ROM, &bRes);
        QTreeWidgetItem *treeReadCapaItem14 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem14->setText(0, tr("Reads Blu-ray ROM (BD-ROM)"));

        if(bShowToolTips==true) treeReadCapaItem14->setToolTip(0, tr("Device is able to read Blu-Ray ROM media."));
        else treeReadCapaItem14->setText(1, tr("Device is able to read Blu-Ray ROM media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem14->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem14->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_BLURAY_R, &bRes);
        QTreeWidgetItem *treeReadCapaItem15 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem15->setText(0, tr("Reads Blu-Ray R"));

        if(bShowToolTips==true) treeReadCapaItem15->setToolTip(0, tr("Device is able to read Blu-Ray R media."));
        else treeReadCapaItem15->setText(1, tr("Device is able to read Blu-Ray R media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem15->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem15->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_BLURAY_RE, &bRes);
        QTreeWidgetItem *treeReadCapaItem16 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem16->setText(0, tr("Reads Blu-Ray RE"));

        if(bShowToolTips==true) treeReadCapaItem16->setToolTip(0, tr("Device is able to read Blu-Ray ReWriteable media."));
        else treeReadCapaItem16->setText(1, tr("Device is able to read Blu-Ray ReWriteable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem16->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem16->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_BLURAY_R_XL, &bRes);
        QTreeWidgetItem *treeReadCapaItem17 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem17->setText(0, tr("Reads Blu-Ray XL R"));

        if(bShowToolTips==true) treeReadCapaItem17->setToolTip(0, tr(""));
        else treeReadCapaItem17->setText(1, tr(""));

        if(bRes==BS_TRUE){
            treeReadCapaItem17->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem17->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle, BS_READ_BLURAY_RE_XL, &bRes);
        QTreeWidgetItem *treeReadCapaItem18 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem18->setText(0, tr("Reads Blu-Ray XL RE"));

        if(bShowToolTips==true) treeReadCapaItem18->setToolTip(0, tr(""));
        else treeReadCapaItem18->setText(1, tr(""));

        if(bRes==BS_TRUE){
            treeReadCapaItem18->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem18->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_HDDVD_ROM, &bRes);
        QTreeWidgetItem *treeReadCapaItem19 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem19->setText(0, tr("Reads HD-DVD ROM"));

        if(bShowToolTips==true) treeReadCapaItem19->setToolTip(0, tr("Device is able to read HDDVD ROM media."));
        else treeReadCapaItem19->setText(1, tr("Device is able to read HDDVD ROM media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem19->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem19->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_HDDVD_R, &bRes);
        QTreeWidgetItem *treeReadCapaItem20 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem20->setText(0, tr("Reads HD-DVD R"));

        if(bShowToolTips==true) treeReadCapaItem20->setToolTip(0, tr("Device is able to read HDDVD R media."));
        else treeReadCapaItem20->setText(1, tr("Device is able to read HDDVD R media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem20->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem20->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_READ_HDDVD_RW, &bRes);
        QTreeWidgetItem *treeReadCapaItem21 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem21->setText(0, tr("Reads HD-DVD RW"));

        if(bShowToolTips==true) treeReadCapaItem21->setToolTip(0, tr("Device is able to read HDDVD ReWriteable media."));
        else treeReadCapaItem21->setText(1, tr("Device is able to read HDDVD ReWriteable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem21->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem21->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle, BS_READ_MOUNT_RAINER, &bRes);
        QTreeWidgetItem *treeReadCapaItem22 = new QTreeWidgetItem(treechildItem);
        treeReadCapaItem22->setText(0, tr("Read Mount Rainer"));

        if(bShowToolTips==true) treeReadCapaItem22->setToolTip(0, tr("Device is able to read Mount Rainer recorded media."));
        else treeReadCapaItem22->setText(1, tr("Device is able to read Mount Rainer recorded media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem22->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem22->setIcon(0,QIcon(":/icons/029.png"));
        }

        treechildItem->setExpanded(true);

        QTreeWidgetItem *treeWriteItems = new QTreeWidgetItem();
        treeWriteItems->setText(0, tr("Write Capabilities"));
        treeItem->addChild(treeWriteItems);

        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_CDR, &bRes);
        QTreeWidgetItem *treeReadCapaItem30 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem30->setText(0, tr("Writes CD-R"));

        if(bShowToolTips==true) treeReadCapaItem30->setToolTip(0, tr("Device is able to write CD-Recordable media."));
        else treeReadCapaItem30->setText(1, tr("Device is able to write CD-Recordable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem30->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem30->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_CDRW, &bRes);
        QTreeWidgetItem *treeReadCapaItem31 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem31->setText(0, tr("Writes CD-RW"));

        if(bShowToolTips==true) treeReadCapaItem31->setToolTip(0, tr("Device is able to write CD-ReWriteable media."));
        else treeReadCapaItem31->setText(1, tr("Device is able to write CD-ReWriteable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem31->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem31->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_CDRW_CAV, &bRes);
        QTreeWidgetItem *treeReadCapaItem32 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem32->setText(0, tr("Writes CD-RW CAV"));

        if(bShowToolTips==true) treeReadCapaItem32->setToolTip(0, tr("Device is able to write CD-RW media that is designed for CAV recording."));
        else treeReadCapaItem32->setText(1, tr("Device is able to write CD-RW media that is designed for CAV recording."));

        if(bRes==BS_TRUE){
            treeReadCapaItem32->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem32->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_DVDR, &bRes);
        QTreeWidgetItem *treeReadCapaItem33 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem33->setText(0, tr("Writes DVD-R"));

        if(bShowToolTips==true) treeReadCapaItem33->setToolTip(0, tr("Device is able to write DVD-R media."));
        else treeReadCapaItem33->setText(1, tr("Device is able to write DVD-R media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem33->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem33->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_DVDRW, &bRes);
        QTreeWidgetItem *treeReadCapaItem34 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem34->setText(0, tr("Writes DVD-RW"));

        if(bShowToolTips==true) treeReadCapaItem34->setToolTip(0, tr("Device is able to write DVD-ReWriteable media."));
        else treeReadCapaItem34->setText(1, tr("Device is able to write DVD-ReWriteable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem34->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem34->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_DVDRAM, &bRes);
        QTreeWidgetItem *treeReadCapaItem35 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem35->setText(0, tr("Writes DVD-RAM"));

        if(bShowToolTips==true) treeReadCapaItem35->setToolTip(0, tr("Device is able to write DVD-RAM media."));
        else treeReadCapaItem35->setText(1, tr("Device is able to write DVD-RAM media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem35->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem35->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_DVD_DL, &bRes);
        QTreeWidgetItem *treeReadCapaItem36 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem36->setText(0, tr("Writes DVD-R Dual Layer"));

        if(bShowToolTips==true) treeReadCapaItem36->setToolTip(0, tr("Device is able to write DVD-R Double Layer media."));
        else treeReadCapaItem36->setText(1, tr("Device is able to write DVD-R Double Layer media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem36->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem36->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_DVDR_PLUS, &bRes);
        QTreeWidgetItem *treeReadCapaItem37 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem37->setText(0, tr("Writes DVD+R"));

        if(bShowToolTips==true) treeReadCapaItem37->setToolTip(0, tr("Device is able to write DVD+R media."));
        else treeReadCapaItem37->setText(1, tr("Device is able to write DVD+R media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem37->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem37->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_DVDRW_PLUS, &bRes);
        QTreeWidgetItem *treeReadCapaItem38 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem38->setText(0, tr("Writes DVD+RW"));

        if(bShowToolTips==true) treeReadCapaItem38->setToolTip(0, tr("Device is able to write DVD+ReWriteable media."));
        else treeReadCapaItem38->setText(1, tr("Device is able to write DVD+ReWriteable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem38->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem38->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_DVD_RDL_PLUS, &bRes);
        QTreeWidgetItem *treeReadCapaItem39 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem39->setText(0, tr("Writes DVD+R Dual Layer"));

        if(bShowToolTips==true) treeReadCapaItem39->setToolTip(0, tr("Device is able to write DVD+R Double Layer media."));
        else treeReadCapaItem39->setText(1, tr("Device is able to write DVD+R Double Layer media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem39->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem39->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_DVD_RWDL_PLUS, &bRes);
        QTreeWidgetItem *treeReadCapaItem40 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem40->setText(0, tr("Writes DVD+RW Dual Layer"));

        if(bShowToolTips==true) treeReadCapaItem40->setToolTip(0, tr("Device is able to write DVD+ReWriteable Double Layer media."));
        else treeReadCapaItem40->setText(1, tr("Device is able to write DVD+ReWriteable Double Layer media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem40->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem40->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_DVD_MRDL, &bRes);
        QTreeWidgetItem *treeReadCapaItem41 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem41->setText(0, tr("Writes DVD-MRDL"));

        if(bShowToolTips==true) treeReadCapaItem41->setToolTip(0, tr("Device is able to write DVD-R Double Layer media."));
        else treeReadCapaItem41->setText(1, tr("Device is able to write DVD-R Double Layer media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem41->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem41->setIcon(0,QIcon(":/icons/029.png"));
        }

        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_BLURAY_R, &bRes);
        QTreeWidgetItem *treeReadCapaItem42 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem42->setText(0, tr("Writes Blu-ray (BD-R)"));

        if(bShowToolTips==true) treeReadCapaItem42->setToolTip(0, tr("Device is able to write Blu-Ray Recordable media."));
        else treeReadCapaItem42->setText(1, tr("Device is able to write Blu-Ray Recordable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem42->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem42->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_BLURAY_RE, &bRes);
        QTreeWidgetItem *treeReadCapaItem43 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem43->setText(0, tr("Writes Blu-ray (BD-RE)"));

        if(bShowToolTips==true) treeReadCapaItem43->setToolTip(0, tr("Device is able to write Blu-Ray ReWriteable media."));
        else treeReadCapaItem43->setText(1, tr("Device is able to write Blu-Ray ReWriteable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem43->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem43->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_BLURAY_R_XL, &bRes);
        QTreeWidgetItem *treeReadCapaItem44 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem44->setText(0, tr("Writes BD-R XL"));

        if(bShowToolTips==true) treeReadCapaItem44->setToolTip(0, tr("Device is able to write BD-R XL media."));
        else treeReadCapaItem44->setText(1, tr("Device is able to write BD-R XL media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem44->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem44->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_BLURAY_RE_XL, &bRes);
        QTreeWidgetItem *treeReadCapaItem45 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem45->setText(0, tr("Writes BD-RE XL"));

        if(bShowToolTips==true) treeReadCapaItem45->setToolTip(0, tr("Device is able to write BD-RE XL media."));
        else treeReadCapaItem45->setText(1, tr("Device is able to write BD-RE XL media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem45->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem45->setIcon(0,QIcon(":/icons/029.png"));
        }

        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_HDDVD_R, &bRes);
        QTreeWidgetItem *treeReadCapaItem46 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem46->setText(0, tr("Writes HD-DVD-R"));

        if(bShowToolTips==true) treeReadCapaItem46->setToolTip(0, tr("Device is able to write HDDVD Reordable media."));
        else treeReadCapaItem46->setText(1, tr("Device is able to write HDDVD Reordable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem46->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem46->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_HDDVD_RW, &bRes);
        QTreeWidgetItem *treeReadCapaItem47 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem47->setText(0, tr("Writes HD-DVD-RW"));

        if(bShowToolTips==true) treeReadCapaItem47->setToolTip(0, tr("Device is able to write HDDVD ReWriteable media."));
        else treeReadCapaItem47->setText(1, tr("Device is able to write HDDVD ReWriteable media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem47->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem47->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_MOUNT_RAINER, &bRes);
        QTreeWidgetItem *treeReadCapaItem48 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem48->setText(0, tr("Writes Mount Rainer"));

        if(bShowToolTips==true) treeReadCapaItem48->setToolTip(0, tr("Device is able to write Mount Rainer media."));
        else treeReadCapaItem48->setText(1, tr("Device is able to write Mount Rainer media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem48->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem48->setIcon(0,QIcon(":/icons/029.png"));
        }


        treeWriteItems->setExpanded(true);

        QTreeWidgetItem *treeMiscItems = new QTreeWidgetItem();
        treeMiscItems->setText(0, tr("Misc Capabilities"));
        treeItem->addChild(treeMiscItems);

        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_WRITE_TEST, &bRes);
        QTreeWidgetItem *treeReadCapaItem50 = new QTreeWidgetItem(treeWriteItems);
        treeReadCapaItem50->setText(0, tr("Simulate write"));

        if(bShowToolTips==true) treeReadCapaItem50->setToolTip(0, tr("Device is able to simulate burning."));
        else treeReadCapaItem50->setText(1, tr("Device is able to simulate burning."));

        if(bRes==BS_TRUE){
            treeReadCapaItem50->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem50->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_MULTISESSION, &bRes);
        QTreeWidgetItem *treeReadCapaItem51 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem51->setText(0, tr("Multisession"));

        if(bShowToolTips==true) treeReadCapaItem51->setToolTip(0, tr("Device is able to perform multi-session disks."));
        else treeReadCapaItem51->setText(1, tr("Device is able to perform multi-session disks."));

        if(bRes==BS_TRUE){
            treeReadCapaItem51->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem51->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_SMART, &bRes);
        QTreeWidgetItem *treeReadCapaItem52 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem52->setText(0, tr("S.M.A.R.T."));

        if(bShowToolTips==true) treeReadCapaItem52->setToolTip(0, tr("Device is able to perform Self-Monitoring Analysis and Reporting Technology."));
        else treeReadCapaItem52->setText(1, tr("Device is able to perform Self-Monitoring Analysis and Reporting Technology."));

        if(bRes==BS_TRUE){
            treeReadCapaItem52->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem52->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle, BS_DEFECTMANAGEMENT, &bRes);
        QTreeWidgetItem *treeReadCapaItem53 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem53->setText(0, tr("Defect Management"));

        if(bShowToolTips==true) treeReadCapaItem53->setToolTip(0, tr("Device has defect management feature. Available to provide defect-free address space. For example BD recorders provide this feature."));
        else treeReadCapaItem53->setText(1, tr("Device has defect management feature. Available to provide defect-free address space. For example BD recorders provide this feature."));

        if(bRes==BS_TRUE){
            treeReadCapaItem53->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem53->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_CPRMAUTH, &bRes);
        QTreeWidgetItem *treeReadCapaItem54 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem54->setText(0, tr("CPRM Auth"));

        if(bShowToolTips==true) treeReadCapaItem54->setToolTip(0, tr("Device is able to CPRM authentification and key management."));
        else treeReadCapaItem54->setText(1, tr("Device is able to CPRM authentification and key management."));

        if(bRes==BS_TRUE){
            treeReadCapaItem54->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem54->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle, BS_STREAMING, &bRes);
        QTreeWidgetItem *treeReadCapaItem55 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem55->setText(0, tr("Streaming"));

        if(bShowToolTips==true) treeReadCapaItem55->setToolTip(0, tr("Device is able to perform reading and writing within specified performance ranges."));
        else treeReadCapaItem55->setText(1, tr("Device is able to perform reading and writing within specified performance ranges."));

        if(bRes==BS_TRUE){
            treeReadCapaItem55->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem55->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_UNDERRUN_PROTECTION, &bRes);
        QTreeWidgetItem *treeReadCapaItem56 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem56->setText(0, tr("Buffer Underrun Protection"));

        if(bShowToolTips==true) treeReadCapaItem56->setToolTip(0, tr("Device is able to perform buffer protection functions like Burn-Proof."));
        else treeReadCapaItem56->setText(1, tr("Device is able to perform buffer protection functions like Burn-Proof."));

        if(bRes==BS_TRUE){
            treeReadCapaItem56->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem56->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_LAYER_JUMP_RECORDING, &bRes);
        QTreeWidgetItem *treeReadCapaItem57 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem57->setText(0, tr("Layer Jump Recording"));

        if(bShowToolTips==true) treeReadCapaItem57->setToolTip(0, tr("Device is able to perform Layer Jump Recording."));
        else treeReadCapaItem57->setText(1, tr("Device is able to perform Layer Jump Recording."));

        if(bRes==BS_TRUE){
            treeReadCapaItem57->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem57->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_ANALOG_AUDIO_PLAYBACK, &bRes);
        QTreeWidgetItem *treeReadCapaItem58 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem58->setText(0, tr("Analog audio playback"));

        if(bShowToolTips==true) treeReadCapaItem58->setToolTip(0, tr("Device supports playback analog audio (Audio CD)"));
        else treeReadCapaItem58->setText(1, tr("Device supports playback analog audio (Audio CD)"));

        if(bRes==BS_TRUE){
            treeReadCapaItem58->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem58->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_COMPOSITE_AUDIO_AND_VIDEO, &bRes);
        QTreeWidgetItem *treeReadCapaItem59 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem59->setText(0, tr("Composite audio/video"));

        if(bShowToolTips==true) treeReadCapaItem59->setToolTip(0, tr("Device is able to deliver a composite Audio and Video data stream from an independent digital port."));
        else treeReadCapaItem59->setText(1, tr("Device is able to deliver a composite Audio and Video data stream from an independent digital port."));

        if(bRes==BS_TRUE){
            treeReadCapaItem59->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem59->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_DIGITAL_PORT_1, &bRes);
        QTreeWidgetItem *treeReadCapaItem60 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem60->setText(0, tr("Digital port 1"));

        if(bShowToolTips==true) treeReadCapaItem60->setToolTip(0, tr("Device supports digital output (IEC958) on port 1."));
        else treeReadCapaItem60->setText(1, tr("Device supports digital output (IEC958) on port 1."));

        if(bRes==BS_TRUE){
            treeReadCapaItem60->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem60->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_DIGITAL_PORT_2, &bRes);
        QTreeWidgetItem *treeReadCapaItem61 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem61->setText(0, tr("Digital port 2"));

        if(bShowToolTips==true) treeReadCapaItem61->setToolTip(0, tr("Device supports digital output (IEC958) on port 2."));
        else treeReadCapaItem61->setText(1, tr("Device supports digital output (IEC958) on port 2."));

        if(bRes==BS_TRUE){
            treeReadCapaItem61->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem61->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_MODE2_FORM1_READ, &bRes);
        QTreeWidgetItem *treeReadCapaItem62 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem62->setText(0, tr("Mode 2 Form 1 Read"));

        if(bShowToolTips==true) treeReadCapaItem62->setToolTip(0, tr("Device is able to read disks that are recorded with Mode2 Form1 format."));
        else treeReadCapaItem62->setText(1, tr("Device is able to read disks that are recorded with Mode2 Form1 format."));

        if(bRes==BS_TRUE){
            treeReadCapaItem62->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem62->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_MODE2_FORM2_READ, &bRes);
        QTreeWidgetItem *treeReadCapaItem63 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem63->setText(0, tr("Mode 2 Form 2 Read"));

        if(bShowToolTips==true) treeReadCapaItem63->setToolTip(0, tr("Device is able to read disks that are recorded with Mode2 Form2 format."));
        else treeReadCapaItem63->setText(1, tr("Device is able to read disks that are recorded with Mode2 Form2 format."));

        if(bRes==BS_TRUE){
            treeReadCapaItem63->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem63->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_CDDA_COMMANDS, &bRes);
        QTreeWidgetItem *treeReadCapaItem64 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem64->setText(0, tr("CDDA Commands"));

        if(bShowToolTips==true) treeReadCapaItem64->setToolTip(0, tr("Device is able to work with CDDA command set."));
        else treeReadCapaItem64->setText(1, tr("Device is able to work with CDDA command set."));

        if(bRes==BS_TRUE){
            treeReadCapaItem64->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem64->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_CDDA_STREAM_IS_ACCURATE, &bRes);
        QTreeWidgetItem *treeReadCapaItem65 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem65->setText(0, tr("CDDA Accurate Stream"));

        if(bShowToolTips==true) treeReadCapaItem65->setToolTip(0, tr("Device supports an audio location without losing place to continue the READ CD command."));
        else treeReadCapaItem65->setText(1, tr("Device supports an audio location without losing place to continue the READ CD command."));

        if(bRes==BS_TRUE){
            treeReadCapaItem65->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem65->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_R_W_SUBCHANNELS_READ, &bRes);
        QTreeWidgetItem *treeReadCapaItem66 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem66->setText(0, tr("R-W SC read"));

        if(bShowToolTips==true) treeReadCapaItem66->setToolTip(0, tr("Device supports reading R-W sub-channel via the READ CD command."));
        else treeReadCapaItem66->setText(1, tr("Device supports reading R-W sub-channel via the READ CD command."));

        if(bRes==BS_TRUE){
            treeReadCapaItem66->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem66->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_R_W_SUBCHANNELS_DEINT_AND_CORR, &bRes);
        QTreeWidgetItem *treeReadCapaItem67 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem67->setText(0, tr("R-W SC deint."));

        if(bShowToolTips==true) treeReadCapaItem67->setToolTip(0, tr("Device supports reading R-W sub-channel via the READ CD command with the returned data de-interleaved and error corrected."));
        else treeReadCapaItem67->setText(1, tr("Device supports reading R-W sub-channel via the READ CD command with the returned data de-interleaved and error corrected."));

        if(bRes==BS_TRUE){
            treeReadCapaItem67->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem67->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_C2_POINTERS, &bRes);
        QTreeWidgetItem *treeReadCapaItem68 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem68->setText(0, tr("C2 Pointers"));

        if(bShowToolTips==true) treeReadCapaItem68->setToolTip(0, tr("Device supports reading C2 error pointers and C2 block error flags."));
        else treeReadCapaItem68->setText(1, tr("Device supports reading C2 error pointers and C2 block error flags."));

        if(bRes==BS_TRUE){
            treeReadCapaItem68->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem68->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_ISRC_READ, &bRes);
        QTreeWidgetItem *treeReadCapaItem70 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem70->setText(0, tr("ISRC Read"));

        if(bShowToolTips==true) treeReadCapaItem70->setToolTip(0, tr("Device is able to read the ISRC (International Standard Recording Code) info of a disk."));
        else treeReadCapaItem70->setText(1, tr("Device is able to read the ISRC (International Standard Recording Code) info of a disk."));

        if(bRes==BS_TRUE){
            treeReadCapaItem70->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem70->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_UPC_READ, &bRes);
        QTreeWidgetItem *treeReadCapaItem71 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem71->setText(0, tr("UPC Read"));

        if(bShowToolTips==true) treeReadCapaItem71->setToolTip(0, tr("Device is able to read the UPC info of a disk."));
        else treeReadCapaItem71->setText(1, tr("Device is able to read the UPC info of a disk."));

        if(bRes==BS_TRUE){
            treeReadCapaItem71->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem71->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_BARCODE_READ, &bRes);
        QTreeWidgetItem *treeReadCapaItem72 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem72->setText(0, tr("Barcode Read"));

        if(bShowToolTips==true) treeReadCapaItem72->setToolTip(0, tr("Device is able to read the disk barcode information."));
        else treeReadCapaItem72->setText(1, tr("Device is able to read the disk barcode information."));

        if(bRes==BS_TRUE){
            treeReadCapaItem72->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem72->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_LOCK_MEDIA, &bRes);
        QTreeWidgetItem *treeReadCapaItem73 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem73->setText(0, tr("Locking media"));

        if(bShowToolTips==true) treeReadCapaItem73->setToolTip(0, tr("Device is able to lock the media."));
        else treeReadCapaItem73->setText(1, tr("Device is able to lock the media."));

        if(bRes==BS_TRUE){
            treeReadCapaItem73->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem73->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_LOCK_STATE, &bRes);
        QTreeWidgetItem *treeReadCapaItem74 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem74->setText(0, tr("Lock state"));

        if(bShowToolTips==true) treeReadCapaItem74->setToolTip(0, tr("Device is able return the lock state."));
        else treeReadCapaItem74->setText(1, tr("Device is able return the lock state."));

        if(bRes==BS_TRUE){
            treeReadCapaItem74->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem74->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_PREVENT_JUMPER, &bRes);
        QTreeWidgetItem *treeReadCapaItem75 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem75->setText(0, tr("Prevent lock jumper"));

        if(bShowToolTips==true) treeReadCapaItem75->setToolTip(0, tr("Device has a physical jumper named the Prevent/Allow Jumper and the jumper is present."));
        else treeReadCapaItem75->setText(1, tr("Device has a physical jumper named the Prevent/Allow Jumper and the jumper is present."));

        if(bRes==BS_TRUE){
            treeReadCapaItem75->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem75->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_EJECT_INDIVIDUAL_OR_MAGAZINE, &bRes);
        QTreeWidgetItem *treeReadCapaItem76 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem76->setText(0, tr("Eject support"));

        if(bShowToolTips==true) treeReadCapaItem76->setToolTip(0, tr("Device supports media eject via the START STOP UNIT command with the LoEj bit set."));
        else treeReadCapaItem76->setText(1, tr("Device supports media eject via the START STOP UNIT command with the LoEj bit set."));

        if(bRes==BS_TRUE){
            treeReadCapaItem76->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem76->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_SEPARATE_VOLUME_LEVELS, &bRes);
        QTreeWidgetItem *treeReadCapaItem77 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem77->setText(0, tr("Sep. volume levels"));

        if(bShowToolTips==true) treeReadCapaItem77->setToolTip(0, tr("Supports separately controllable audio levels for each supported channel via the CD Audio Control Page."));
        else treeReadCapaItem77->setText(1, tr("Supports separately controllable audio levels for each supported channel via the CD Audio Control Page."));

        if(bRes==BS_TRUE){
            treeReadCapaItem77->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem77->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_SEPARATE_CHANNEL_MUTE, &bRes);
        QTreeWidgetItem *treeReadCapaItem78 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem78->setText(0, tr("Sep. channel mute"));

        if(bShowToolTips==true) treeReadCapaItem78->setToolTip(0, tr("Independently muting each audio channel via the CD Audio Control Page."));
        else treeReadCapaItem78->setText(1, tr("Independently muting each audio channel via the CD Audio Control Page."));

        if(bRes==BS_TRUE){
            treeReadCapaItem78->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem78->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_CHANGER_SUPPORTS_DISC_PRESENT, &bRes);
        QTreeWidgetItem *treeReadCapaItem79 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem79->setText(0, tr("Disc present"));

        if(bShowToolTips==true) treeReadCapaItem79->setToolTip(0, tr("Contains an embedded changer, and after a reset condition or a magazine change, the Logical Unit is capable of reporting the exact contents of the slots."));
        else treeReadCapaItem79->setText(1, tr("Contains an embedded changer, and after a reset condition or a magazine change, the Logical Unit is capable of reporting the exact contents of the slots."));

        if(bRes==BS_TRUE){
            treeReadCapaItem79->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem79->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_CHANGER_SOFTWARE_SLOT_SELECTION, &bRes);
        QTreeWidgetItem *treeReadCapaItem80 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem80->setText(0, tr("SW Slot Selection"));

        if(bShowToolTips==true) treeReadCapaItem80->setToolTip(0, tr("Supports the LOAD/UNLOAD MEDIUM command when trying to load a Slot with no Disc present."));
        else treeReadCapaItem80->setText(1, tr("Supports the LOAD/UNLOAD MEDIUM command when trying to load a Slot with no Disc present."));

        if(bRes==BS_TRUE){
            treeReadCapaItem80->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem80->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_CHANGER_SIDE_CHANGE_CAPABLE, &bRes);
        QTreeWidgetItem *treeReadCapaItem81 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem81->setText(0, tr("Side change"));

        if(bShowToolTips==true) treeReadCapaItem81->setToolTip(0, tr(""));
        else treeReadCapaItem81->setText(1, tr(""));

        if(bRes==BS_TRUE){
            treeReadCapaItem81->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem81->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_R_W_SUBCHANNELS_IN_LEAD_IN_READ, &bRes);
        QTreeWidgetItem *treeReadCapaItem82 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem82->setText(0, tr("R-W SC in LI Read"));

        if(bShowToolTips==true) treeReadCapaItem82->setToolTip(0, tr("Device is capable of reading the raw R-W Sub-channel information from the Lead-in."));
        else treeReadCapaItem82->setText(1, tr("Device is capable of reading the raw R-W Sub-channel information from the Lead-in."));

        if(bRes==BS_TRUE){
            treeReadCapaItem82->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem82->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_METHOD_2_ADDRESSING_FIXED_PACKETS, &bRes);
        QTreeWidgetItem *treeReadCapaItem83 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem83->setText(0, tr("Method 2 FP Read"));

        if(bShowToolTips==true) treeReadCapaItem83->setToolTip(0, tr(""));
        else treeReadCapaItem83->setText(1, tr(""));

        if(bRes==BS_TRUE){
            treeReadCapaItem83->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem83->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_CD_TEXT_READ, &bRes);
        QTreeWidgetItem *treeReadCapaItem84 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem84->setText(0, tr("Reads CD Text"));

        if(bShowToolTips==true) treeReadCapaItem84->setToolTip(0, tr("Device is able to read the cd-text infromation of a disk (subchannel)."));
        else treeReadCapaItem84->setText(1, tr("Device is able to read the cd-text infromation of a disk (subchannel)."));

        if(bRes==BS_TRUE){
            treeReadCapaItem84->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem84->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_CD_TEXT_WRITE, &bRes);
        QTreeWidgetItem *treeReadCapaItem85 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem85->setText(0, tr("Writes CD Text"));

        if(bShowToolTips==true) treeReadCapaItem85->setToolTip(0, tr("Device is able to write the cd-text infromation of a disk (subchannel)."));
        else treeReadCapaItem85->setText(1, tr("Device is able to write the cd-text infromation of a disk (subchannel)."));

        if(bRes==BS_TRUE){
            treeReadCapaItem85->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem85->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_DAO_RAW, &bRes);
        QTreeWidgetItem *treeReadCapaItem86 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem86->setText(0, tr("DAO Raw"));

        if(bShowToolTips==true) treeReadCapaItem86->setToolTip(0, tr("Device can write DAO raw write method."));
        else treeReadCapaItem86->setText(1, tr("Device can write DAO raw write method."));

        if(bRes==BS_TRUE){
            treeReadCapaItem86->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem86->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_DAO_16, &bRes);
        QTreeWidgetItem *treeReadCapaItem87 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem87->setText(0, tr("DAO 16"));

        if(bShowToolTips==true) treeReadCapaItem87->setToolTip(0, tr("Device can write DAO 16 method."));
        else treeReadCapaItem87->setText(1, tr("Device can write DAO 16 method."));

        if(bRes==BS_TRUE){
            treeReadCapaItem87->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem87->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_DAO_96_PACK, &bRes);
        QTreeWidgetItem *treeReadCapaItem88 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem88->setText(0, tr("DAO 96 Pack"));

        if(bShowToolTips==true) treeReadCapaItem88->setToolTip(0, tr("Device can write DAO 96 packet write method."));
        else treeReadCapaItem88->setText(1, tr("Device can write DAO 96 packet write method."));

        if(bRes==BS_TRUE){
            treeReadCapaItem88->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem88->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_DAO_96_RAW, &bRes);
        QTreeWidgetItem *treeReadCapaItem89 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem89->setText(0, tr("DAO 96 Raw"));

        if(bShowToolTips==true) treeReadCapaItem89->setToolTip(0, tr("Device can write DAO raw write method."));
        else treeReadCapaItem89->setText(1, tr("Device can write DAO raw write method."));

        if(bRes==BS_TRUE){
            treeReadCapaItem89->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem89->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_PACKET_WRITE, &bRes);
        QTreeWidgetItem *treeReadCapaItem90 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem90->setText(0, tr("Packet Write"));

        if(bShowToolTips==true) treeReadCapaItem90->setToolTip(0, tr("The drive can perform packet writing."));
        else treeReadCapaItem90->setText(1, tr("The drive can perform packet writing."));

        if(bRes==BS_TRUE){
            treeReadCapaItem90->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem90->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_LABELFLASH, &bRes);
        QTreeWidgetItem *treeReadCapaItem91 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem91->setText(0, tr("Labelflash"));

        if(bShowToolTips==true) treeReadCapaItem91->setToolTip(0, tr("Device is able to write Labelflash discs."));
        else treeReadCapaItem91->setText(1, tr("Device is able to write Labelflash discs."));

        if(bRes==BS_TRUE){
            treeReadCapaItem91->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem91->setIcon(0,QIcon(":/icons/029.png"));
        }
        nErr = ::AnalyseDeviceCapability(nCapsHandle,  BS_LIGHTSCRIBE, &bRes);
        QTreeWidgetItem *treeReadCapaItem92 = new QTreeWidgetItem(treeMiscItems);
        treeReadCapaItem92->setText(0, tr("LightScribe"));

        if(bShowToolTips==true) treeReadCapaItem92->setToolTip(0, tr("Device is able to write Lightscribe discs."));
        else treeReadCapaItem92->setText(1, tr("Device is able to write Lightscribe discs."));

        if(bRes==BS_TRUE){
            treeReadCapaItem92->setIcon(0,QIcon(":/icons/030.png"));
        }else{
            treeReadCapaItem92->setIcon(0,QIcon(":/icons/029.png"));
        }


        treeMiscItems->setExpanded(true);

        ::ReleaseDeviceCapabilities(nCapsHandle);
    }


    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);


}

