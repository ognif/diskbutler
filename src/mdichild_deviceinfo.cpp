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
#include "mdichild_deviceinfo.h"
#include "FoxSDKBurningLib.h"
#include "mainwindow.h"
#include "settingspages.h"

MdiChildDeviceInfo::MdiChildDeviceInfo( QWidget*, const QString &device )
    :strBurnDrive( device ){
    setAttribute( Qt::WA_DeleteOnClose );
    setWindowIcon( QIcon( ":/icons/deviceinfo32.png" ) );
    mProjectType = RuleManager::TYPE_PROJECT_DEVICEINFO;
    bShowToolTips = ConfigurationPage::mSettings.value( "deviceInfoshowToolTips", false ).toBool();
    bHideEmptyFields = ConfigurationPage::mSettings.value( "deviceInfoHideEmptyFields", false ).toBool();
    nRegionChanges = 0;
    nCurrentRegion = 0;

    treeWidget = new QTreeWidget();

    QString wTitle = tr("Device Info");
    wTitle += " (";
    wTitle += strBurnDrive;
    wTitle += ")";

    setWindowTitle( wTitle );

    splitter = new QSplitter;
    splitter->addWidget( treeWidget );
    //setCentralWidget(splitter);
    setWidget( splitter );


    //myRefreshMenus = new QAction(this);
    //connect(myRefreshMenus, SIGNAL(triggered()), parent, SLOT(specialActionDeviceInfo()));

    readDeviceInfoEx();
}

void MdiChildDeviceInfo::updateDeviceInfo()
{

    readDeviceInfoEx();
    myRefreshMenus->trigger();

}

void MdiChildDeviceInfo::closeEvent(QCloseEvent *event)
{
    event->accept();
}


void MdiChildDeviceInfo::fillInfoPairPlain(QTreeWidgetItem* rootItem, TCHAR *itemName, QString itemText)
{
    QString strItemName = "";


    strItemName = convertToQT( itemName );

    QTreeWidgetItem *treeGeneralInfo1 = new QTreeWidgetItem( rootItem );
    treeGeneralInfo1->setText( 0, itemText );
    treeGeneralInfo1->setText( 1, strItemName );
}

void MdiChildDeviceInfo::fillInfoPairPlainInt( QTreeWidgetItem* rootItem, int itemValue, QString itemText )
{
    QString strItemName = QString::number( itemValue );

    QTreeWidgetItem *treeGeneralInfo1 = new QTreeWidgetItem( rootItem );
    treeGeneralInfo1->setText( 0, itemText );
    treeGeneralInfo1->setText( 1, strItemName );
}

void MdiChildDeviceInfo::fillInfoPairPlainCaps( QTreeWidgetItem* rootItem, int32 capsHandle, int64 capsID, QString capsText, QString itemText )
{
    BS_BOOL bRes;
    ::AnalyseDeviceCapability( capsHandle, capsID, &bRes );

    QTreeWidgetItem *treeReadCapaItem = new QTreeWidgetItem( rootItem );
    treeReadCapaItem->setText( 0, capsText );
    treeReadCapaItem->setText( 1, itemText );
    if(bRes==BS_TRUE){
        treeReadCapaItem->setIcon( 0, QIcon( ":/icons/yes16.png" ) );
    }else{
        treeReadCapaItem->setIcon(0, QIcon( ":/icons/no16.png" ) );
    }
}

void MdiChildDeviceInfo::readDeviceInfoEx()
{
    treeWidget->clear();

    QStringList labels;
    labels << tr("Capa") << tr("Description") ;
    treeWidget->setHeaderLabels( labels );
    treeWidget->header()->setStretchLastSection( true );
    treeWidget->header()->setDefaultSectionSize( 300 );

    //Wir bauen einen Treeview und übergeben den dann
    //Root
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText( 0, tr("Device") );
    treeItem->setIcon( 0, QIcon( ":/icons/disk_main16.png" ) );
    treeItem->setExpanded( true );

    QTreeWidgetItem *treeInfoItem = new QTreeWidgetItem();
    treeInfoItem->setText( 0, tr("General") );
    treeItem->addChild( treeInfoItem );

    pDevice stDeviceInfo;
    stDeviceInfo.isDevice = 0;

    TCHAR vendorId_[9];
    TCHAR productId_[17];
    TCHAR productRevision_[5];

    ::SetBurnDevice( strBurnDrive.at(0).toLatin1() );
    ::GetDeviceInformation( BS_CURRENT_DEVICE, vendorId_, productId_, productRevision_ );

    SExtendedDeviceInformation	infoEx;
    if ( ::GetDeviceInformationEx( BS_CURRENT_DEVICE, &infoEx ) == BS_SDK_ERROR_NO )
    {
        fillInfoPairPlain( treeInfoItem, vendorId_, "Vendor name" );
        fillInfoPairPlain( treeInfoItem, productId_, "Model name" );
        fillInfoPairPlain( treeInfoItem, infoEx.lpszName, "Read name" );
        fillInfoPairPlain( treeInfoItem, infoEx.lpszRevision, "Firmware revision" );
        fillInfoPairPlain( treeInfoItem, infoEx.lpszSerialNumber, "Serial number" );

        fillInfoPairPlain( treeInfoItem, infoEx.lpszLoaderType, "Loader type" );
        fillInfoPairPlain( treeInfoItem, infoEx.lpszConnectionInterface, "Connection interface" );
        fillInfoPairPlain( treeInfoItem, infoEx.lpszPhysicalInterface, "Drive interface" );

        fillInfoPairPlainInt( treeInfoItem, infoEx.nBufferSize, "Buffer size" );
        fillInfoPairPlainInt( treeInfoItem, infoEx.nNumberOfVolumeLevels, "Volume levels" );
        fillInfoPairPlainInt( treeInfoItem, infoEx.nReadRetryCount, "Read retry count" );
        fillInfoPairPlainInt( treeInfoItem, infoEx.nRegionCode, "Region code" );
        fillInfoPairPlainInt( treeInfoItem, infoEx.nRegionCodeChangesLeft, "RC user changes left" );
        fillInfoPairPlainInt( treeInfoItem, infoEx.nRegionCodeVendorResetsLeft, "RC vendor resets left" );

    }
    int32 nCapsHandle;
    int32 nErr = ::GetDeviceCapabilitiesHandle( BS_CURRENT_DEVICE, &nCapsHandle );
    if( nErr == BS_SDK_ERROR_NO )
    {
        QTreeWidgetItem *treeReadItem = new QTreeWidgetItem();
        treeReadItem->setText( 0, tr("Read Capabilities") );
        treeItem->addChild( treeReadItem );

        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_CDR, tr("Reads CD-R"), tr("Device is able to read CD-R media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_CDRW, tr("Reads CD-RW"), tr("Device is able to read CD ReWriteable media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_CDRW_CAV, tr("Reads CD-RW CAV"), tr("Device is able to read CD-RW media that is designed for CAV recording.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_DVD, tr("Reads DVD-ROM"), tr("Device is able to read DVD-ROM media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_DVDR, tr("Reads DVD-R"), tr("Device is able to read DVD-R media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_DVDRW, tr("Reads DVD-RW"), tr("Device is able to read DVD-ReWriteable media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_DVDRAM, tr("Reads DVD-RAM"), tr("Device is able to read DVD-RAM media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_DVD_DL, tr("Reads DVD-R Dual Layer"), tr("Device is able to read DVD-R Double Layer media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_DVDR_PLUS, tr("Reads DVD+R"), tr("Device is able to read DVD+R media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_DVDRW_PLUS, tr("Reads DVD+RW"), tr("Device is able to read DVD+ReWriteable media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_DVD_RDL_PLUS, tr("Reads DVD+RW Dual Layer"), tr("Device is able to read DVD+R Double Layer media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_DVD_RWDL_PLUS, tr("Reads DVD+RW DL"), tr("Device is able to read DVD+ReWriteable Double Layer media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_DVD_MRDL, tr("Reads DVD-R DL"), tr("Device is able to read DVD-R Double Layer media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_BLURAY_ROM, tr("Reads Blu-ray ROM (BD-ROM)"), tr("Device is able to read Blu-Ray ROM media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_BLURAY_R, tr("Reads Blu-Ray R"), tr("Device is able to read Blu-Ray R media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_BLURAY_RE, tr("Reads Blu-Ray RE"), tr("Device is able to read Blu-Ray ReWriteable media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_BLURAY_R_XL, tr("Reads Blu-Ray XL R"), tr("Device is able to read Blu-Ray XL 100GB recordable media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_BLURAY_RE_XL, tr("Reads Blu-Ray XL RE"), tr("Device is able to read Blu-Ray XL 100GB ReWriteable media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_HDDVD_ROM, tr("Reads HD-DVD ROM"), tr("Device is able to read HDDVD ROM media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_HDDVD_R, tr("Reads HD-DVD R"), tr("Device is able to read HDDVD R media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_HDDVD_RW, tr("Reads HD-DVD RW"), tr("Device is able to read HDDVD ReWriteable media.") );
        fillInfoPairPlainCaps( treeReadItem, nCapsHandle, BS_READ_MOUNT_RAINER, tr("Read Mount Rainer"), tr("Device is able to read Mount Rainer recorded media.") );

        QTreeWidgetItem *treeWriteItem = new QTreeWidgetItem();
        treeWriteItem->setText( 0, tr("Write Capabilities") );
        treeItem->addChild( treeWriteItem );

        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_CDR, tr("Writes CD-R"), tr("Device is able to write CD-Recordable media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_CDRW, tr("Writes CD-RW"), tr("Device is able to write CD-ReWriteable media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_CDRW_CAV, tr("Writes CD-RW CAV"), tr("Device is able to write CD-RW media that is designed for CAV recording.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_DVDR, tr("Writes DVD-R"), tr("Device is able to write DVD-R media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_DVDRW, tr("Writes DVD-RW"), tr("Device is able to write DVD-ReWriteable media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_DVDRAM, tr("Writes DVD-RAM"), tr("Device is able to write DVD-RAM media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_DVD_DL, tr("Writes DVD-R Dual Layer"), tr("Device is able to write DVD-R Double Layer media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_DVDR_PLUS, tr("Writes DVD+R"), tr("Device is able to write DVD+R media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_DVDRW_PLUS, tr("Writes DVD+RW"), tr("Device is able to write DVD+ReWriteable media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_DVD_RDL_PLUS, tr("Writes DVD+R Dual Layer"), tr("Device is able to write DVD+R Double Layer media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_DVD_RWDL_PLUS, tr("Writes DVD+RW Dual Layer"), tr("Device is able to write DVD+ReWriteable Double Layer media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_DVD_MRDL, tr("Writes DVD-MRDL"), tr("Device is able to write DVD-R Double Layer media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_BLURAY_R, tr("Writes Blu-ray (BD-R)"), tr("Device is able to write Blu-Ray Recordable media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_BLURAY_RE, tr("Writes Blu-ray (BD-RE)"), tr("Device is able to write Blu-Ray ReWriteable media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_BLURAY_R_XL, tr("Writes BD-R XL"), tr("Device is able to write BD-R XL media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_BLURAY_RE_XL, tr("Writes BD-RE XL"), tr("Device is able to write BD-RE XL media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_HDDVD_R, tr("Writes HD-DVD-R"), tr("Device is able to write HDDVD Reordable media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_HDDVD_RW, tr("Writes HD-DVD-RW"), tr("Device is able to write HDDVD ReWriteable media.") );
        fillInfoPairPlainCaps( treeWriteItem, nCapsHandle, BS_WRITE_MOUNT_RAINER, tr("Writes Mount Rainer"), tr("Device is able to write Mount Rainer media.") );

        QTreeWidgetItem *treeMiscItem = new QTreeWidgetItem();
        treeMiscItem->setText( 0, tr("Misc Capabilities") );
        treeItem->addChild( treeMiscItem );

        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_WRITE_TEST, tr("Simulate write"), tr("Device is able to simulate burning."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_MULTISESSION, tr("Multisession"), tr("Device is able to perform multi-session disks."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_SMART, tr("S.M.A.R.T."), tr("Device is able to perform Self-Monitoring Analysis and Reporting Technology."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_DEFECTMANAGEMENT, tr("Defect Management"), tr("Device has defect management feature. Available to provide defect-free address space. For example BD recorders provide this feature."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_CPRMAUTH, tr("CPRM Auth"), tr("Device is able to CPRM authentification and key management."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_STREAMING, tr("Streaming"), tr("Device is able to perform reading and writing within specified performance ranges."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_UNDERRUN_PROTECTION, tr("Buffer Underrun Protection"), tr("Device is able to perform buffer protection functions like Burn-Proof."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_LAYER_JUMP_RECORDING, tr("Layer Jump Recording"), tr("Device is able to perform Layer Jump Recording."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_ANALOG_AUDIO_PLAYBACK, tr("Analog audio playback"), tr("Device supports playback analog audio (Audio CD)"));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_COMPOSITE_AUDIO_AND_VIDEO, tr("Composite audio/video"), tr("Device is able to deliver a composite Audio and Video data stream from an independent digital port."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_DIGITAL_PORT_1, tr("Digital port 1"), tr("Device supports digital output (IEC958) on port 1."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_DIGITAL_PORT_2, tr("Digital port 2"), tr("Device supports digital output (IEC958) on port 2."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_MODE2_FORM1_READ, tr("Mode 2 Form 1 Read"), tr("Device is able to read disks that are recorded with Mode2 Form1 format."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_MODE2_FORM2_READ, tr("Mode 2 Form 2 Read"), tr("Device is able to read disks that are recorded with Mode2 Form2 format."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_CDDA_COMMANDS, tr("CDDA Commands"), tr("Device is able to work with CDDA command set."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_CDDA_STREAM_IS_ACCURATE, tr("CDDA Accurate Stream"), tr("Device supports an audio location without losing place to continue the READ CD command."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_R_W_SUBCHANNELS_READ, tr("R-W SC read"), tr("Device supports reading R-W sub-channel via the READ CD command."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_R_W_SUBCHANNELS_DEINT_AND_CORR, tr("R-W SC deint."), tr("Device supports reading R-W sub-channel via the READ CD command with the returned data de-interleaved and error corrected."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_C2_POINTERS, tr("C2 Pointers"), tr("Device supports reading C2 error pointers and C2 block error flags."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_ISRC_READ, tr("ISRC Read"), tr("Device is able to read the ISRC (International Standard Recording Code) info of a disk."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_UPC_READ, tr("UPC Read"), tr("Device is able to read the UPC info of a disk."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_BARCODE_READ, tr("Barcode Read"), tr("Device is able to read the disk barcode information."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_LOCK_MEDIA, tr("Locking media"), tr("Device is able to lock the media."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_LOCK_STATE, tr("Lock state"), tr("Device is able return the lock state."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_PREVENT_JUMPER, tr("Prevent lock jumper"), tr("Device has a physical jumper named the Prevent/Allow Jumper and the jumper is present."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_EJECT_INDIVIDUAL_OR_MAGAZINE, tr("Eject support"), tr("Device supports media eject via the START STOP UNIT command with the LoEj bit set."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_SEPARATE_VOLUME_LEVELS, tr("Sep. volume levels"), tr("Supports separately controllable audio levels for each supported channel via the CD Audio Control Page."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_SEPARATE_CHANNEL_MUTE, tr("Sep. channel mute"), tr("Independently muting each audio channel via the CD Audio Control Page."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_CHANGER_SUPPORTS_DISC_PRESENT, tr("Disc present"), tr("Contains an embedded changer, and after a reset condition or a magazine change, the Logical Unit is capable of reporting the exact contents of the slots."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_CHANGER_SOFTWARE_SLOT_SELECTION, tr(""), tr("Supports the LOAD/UNLOAD MEDIUM command when trying to load a Slot with no Disc present."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_CHANGER_SIDE_CHANGE_CAPABLE, tr("Side change"), tr(""));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_R_W_SUBCHANNELS_IN_LEAD_IN_READ, tr("R-W SC in LI Read"), tr("Device is capable of reading the raw R-W Sub-channel information from the Lead-in."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_METHOD_2_ADDRESSING_FIXED_PACKETS, tr("Method 2 FP Read"), tr(""));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_CD_TEXT_READ, tr("Reads CD Text"), tr("Device is able to read the cd-text infromation of a disk (subchannel)."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_CD_TEXT_WRITE, tr("Writes CD Text"), tr("Device is able to write the cd-text infromation of a disk (subchannel)."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_DAO_RAW, tr("DAO Raw"), tr("Device can write DAO raw write method."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_DAO_16, tr("DAO 16"), tr("Device can write DAO 16 method."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_DAO_96_PACK, tr("DAO 96 Pack"), tr("Device can write DAO 96 packet write method."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_DAO_96_RAW, tr("DAO 96 Raw"), tr("Device can write DAO raw write method."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_PACKET_WRITE, tr("Packet Write"), tr("The drive can perform packet writing."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_LABELFLASH, tr("Labelflash"), tr("Device is able to write Labelflash discs."));
        fillInfoPairPlainCaps(treeMiscItem, nCapsHandle, BS_LIGHTSCRIBE, tr("LightScribe"), tr("Device is able to write Lightscribe discs."));

        ::ReleaseDeviceCapabilities(nCapsHandle);

    }





    treeWidget->addTopLevelItem(treeItem);
    treeWidget ->expandAll();
}

void MdiChildDeviceInfo::setUIControls(Ribbon *, QWidget* parent)
{
    MainWindow *ribbonOwner = qobject_cast<MainWindow *>(parent);

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

    ribbonOwner->imageMediaButton->setEnabled(false);
    ribbonOwner->grabAudioMediaButton->setEnabled(false);


}

//SetBurnDevice List ist in den pDevice Projekten dann das ReadDevice.
void MdiChildDeviceInfo::setBurnDeviceList(QWidget* parent)
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
void MdiChildDeviceInfo::setRibbonTabs(Ribbon *baseRibbon, QWidget* parent)
{

    //Für morgen
    //IM Child wird das letzte aktive Tab gespeichert.
    //Bei DiskInfo wird gespeichert ob 2Image aktiv wwar.
    baseRibbon->blockSignals(true);

    baseRibbon->hideAll();

    if(parent!=nullptr){
        setUIControls(baseRibbon, parent);
        setBurnDeviceList(parent);
    }

    baseRibbon->blockSignals(false);



}
