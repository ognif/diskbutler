
#ifndef RIBBONBUILDER_H
#define RIBBONBUILDER_H

#include "ribbon.h"
#include "mainwindow.h"


extern void ribbonBuilderAll(Ribbon *baseRibbon, MainWindow *baseWindow){
    // Add tabs to ribbon
    baseRibbon->addTab(QIcon(":/icons/blueprint32.png"),  QObject::tr("Project"));
    baseRibbon->addTab(QIcon(":/icons/general32.png"),  QObject::tr("Edit"));
    baseRibbon->addTab(QIcon(":/icons/device32.png"),  QObject::tr("Device"));
    baseRibbon->addTab(QIcon(":/icons/create_image32.png"),  QObject::tr("Create image"));
    baseRibbon->addTab(QIcon(":/icons/hexedit32.png"),  QObject::tr("Hex Editor"));
    baseRibbon->addTab(QIcon(":/icons/diskscan32.png"),  QObject::tr("Scan Editor"));
    baseRibbon->addTab(QIcon(":/icons/burnaction32.png"),  QObject::tr("General"));
    baseRibbon->addTab(QIcon(":/icons/filesystem32.png"),  QObject::tr("File System"));
    baseRibbon->addTab(QIcon(":/icons/extiso32.png"),  QObject::tr("ISO Extended"));
    baseRibbon->addTab(QIcon(":/icons/boot32.png"),  QObject::tr("Boot Disc"));
    baseRibbon->addTab(QIcon(":/icons/view32.png"),  QObject::tr("View"));

    baseWindow->dataIsoProjectButton = new QToolButton;
    baseWindow->dataIsoProjectButton->setText( QObject::tr("ISO"));
    baseWindow->dataIsoProjectButton->setToolTip(QObject::tr("Create ISO Project"));
    baseWindow->dataIsoProjectButton->setIcon(QIcon(":/icons/datacd32.png"));
    baseWindow->dataIsoProjectButton->setEnabled(true);
    baseRibbon->addButton( QObject::tr("Project"), QObject::tr("Data"), baseWindow->dataIsoProjectButton,nullptr);

    baseWindow->dataUdfProjectButton = new QToolButton;
    baseWindow->dataUdfProjectButton->setText(QObject::tr("UDF"));
    baseWindow->dataUdfProjectButton->setToolTip(QObject::tr("Create UDF Project"));
    baseWindow->dataUdfProjectButton->setIcon(QIcon(":/icons/dataudf32.png"));
    baseWindow->dataUdfProjectButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Data"), baseWindow->dataUdfProjectButton,nullptr);

    baseWindow->dataIsoUdfProjectButton = new QToolButton;
    baseWindow->dataIsoUdfProjectButton->setText(QObject::tr("ISO/UDF"));
    baseWindow->dataIsoUdfProjectButton->setToolTip(QObject::tr("Create ISO/UDF Bridge Project"));
    baseWindow->dataIsoUdfProjectButton->setIcon(QIcon(":/icons/dataudfiso32.png"));
    baseWindow->dataIsoUdfProjectButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Data"), baseWindow->dataIsoUdfProjectButton,nullptr);

    baseWindow->mmAudioProjectButton = new QToolButton;
    baseWindow->mmAudioProjectButton->setText(QObject::tr("Audio"));
    baseWindow->mmAudioProjectButton->setToolTip(QObject::tr("Create AudioCD Project"));
    baseWindow->mmAudioProjectButton->setIcon(QIcon(":/icons/audiocd32.png"));
    baseWindow->mmAudioProjectButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Multimedia"), baseWindow->mmAudioProjectButton,nullptr);

    baseWindow->mmEnhancedProjectButton = new QToolButton;
    baseWindow->mmEnhancedProjectButton->setText(QObject::tr("MixedMode"));
    baseWindow->mmEnhancedProjectButton->setToolTip(QObject::tr("Create MixedModeCD Project"));
    baseWindow->mmEnhancedProjectButton->setIcon(QIcon(":/icons/mixedmode32.png"));
    baseWindow->mmEnhancedProjectButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Multimedia"), baseWindow->mmEnhancedProjectButton,nullptr);

    baseWindow->vidSVCDProjectButton = new QToolButton;
    baseWindow->vidSVCDProjectButton->setText(QObject::tr("SVCD"));
    baseWindow->vidSVCDProjectButton->setToolTip(QObject::tr("Create SVCD Video Project"));
    baseWindow->vidSVCDProjectButton->setIcon(QIcon(":/icons/svideocd32.png"));
    baseWindow->vidSVCDProjectButton->setEnabled(false);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Video"), baseWindow->vidSVCDProjectButton,nullptr);

    baseWindow->vidDVDProjectButton = new QToolButton;
    baseWindow->vidDVDProjectButton->setText(QObject::tr("Video DVD"));
    baseWindow->vidDVDProjectButton->setToolTip(QObject::tr("Create VideoDVD Project"));
    baseWindow->vidDVDProjectButton->setIcon(QIcon(":/icons/videodvd32.png"));
    baseWindow->vidDVDProjectButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Video"), baseWindow->vidDVDProjectButton,nullptr);

    baseWindow->vidBlueProjectButton = new QToolButton;
    baseWindow->vidBlueProjectButton->setText(QObject::tr("Blu-ray"));
    baseWindow->vidBlueProjectButton->setToolTip(QObject::tr("Create Blu-ray Video Project"));
    baseWindow->vidBlueProjectButton->setIcon(QIcon(":/icons/bluray32.png"));
    baseWindow->vidBlueProjectButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Video"), baseWindow->vidBlueProjectButton,nullptr);

    baseWindow->appOpenButton = new QToolButton;
    baseWindow->appOpenButton->setText(QObject::tr("Open"));
    baseWindow->appOpenButton->setToolTip(QObject::tr("Open DiskButler project"));
    baseWindow->appOpenButton->setIcon(QIcon(":/icons/open32.png"));
    baseWindow->appOpenButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Open & Save"), baseWindow->appOpenButton,nullptr);

    baseWindow->appSaveButton = new QToolButton;
    baseWindow->appSaveButton->setText(QObject::tr("Save"));
    baseWindow->appSaveButton->setToolTip(QObject::tr("Save disk to DiskButler project"));
    baseWindow->appSaveButton->setIcon(QIcon(":/icons/save32.png"));
    baseWindow->appSaveButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Open & Save"), baseWindow->appSaveButton,nullptr);

    baseWindow->appSaveAsButton = new QToolButton;
    baseWindow->appSaveAsButton->setText(QObject::tr("Save As"));
    baseWindow->appSaveAsButton->setToolTip(QObject::tr("Save disk to DiskButler project"));
    baseWindow->appSaveAsButton->setIcon(QIcon(":/icons/save32.png"));
    baseWindow->appSaveAsButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Open & Save"), baseWindow->appSaveAsButton,nullptr);

    baseWindow->appSettings = new QToolButton;
    baseWindow->appSettings->setText(QObject::tr("Settings"));
    baseWindow->appSettings->setToolTip(QObject::tr("Edit basic software configuration"));
    baseWindow->appSettings->setIcon(QIcon(":/icons/projectset32.png"));
    baseWindow->appSettings->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Project"), QObject::tr("Software"), baseWindow->appSettings,nullptr);
}

extern void ribbonBuilderProject(Ribbon *baseRibbon, MainWindow *baseWindow){

    baseWindow->addFileEditButton = new QToolButton;
    baseWindow->addFileEditButton->setText(QObject::tr("Add File"));
    baseWindow->addFileEditButton->setToolTip(QObject::tr("Add File to the project"));
    baseWindow->addFileEditButton->setIcon(QIcon(":/icons/file_add32.png"));
    baseWindow->addFileEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Files & Folders"), baseWindow->addFileEditButton,nullptr);

    baseWindow->addFolderEditButton = new QToolButton;
    baseWindow->addFolderEditButton->setText(QObject::tr("Add Folder"));
    baseWindow->addFolderEditButton->setToolTip(QObject::tr("Add a folder and its content"));
    baseWindow->addFolderEditButton->setIcon(QIcon(":/icons/folder_add32.png"));
    baseWindow->addFolderEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Files & Folders"), baseWindow->addFolderEditButton,nullptr);

    baseWindow->createFolderEditButton = new QToolButton;
    baseWindow->createFolderEditButton->setText(QObject::tr("Create Folder"));
    baseWindow->createFolderEditButton->setToolTip(QObject::tr("Create a new empty folder"));
    baseWindow->createFolderEditButton->setIcon(QIcon(":/icons/folder_create32.png"));
    baseWindow->createFolderEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Files & Folders"), baseWindow->createFolderEditButton,nullptr);

    baseWindow->delEditButton = new QToolButton;
    baseWindow->delEditButton->setText(QObject::tr("Delete Item"));
    baseWindow->delEditButton->setToolTip(QObject::tr("Delete selected item or folder"));
    baseWindow->delEditButton->setIcon(QIcon(":/icons/delete32.png"));
    baseWindow->delEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Files & Folders"), baseWindow->delEditButton, nullptr);

    QVBoxLayout *myVerticalLayout = new QVBoxLayout();
    baseRibbon->addVerticalLayout(QObject::tr("Edit"), QObject::tr("Files & Folders"), myVerticalLayout);

    baseWindow->renameEditButton = new QToolButton;
    baseWindow->renameEditButton->setText(QObject::tr("Rename"));
    baseWindow->renameEditButton->setToolTip(QObject::tr("Rename selected item"));
    baseWindow->renameEditButton->setIcon(QIcon(":/icons/rename32.png"));
    baseWindow->renameEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Files & Folders"), baseWindow->renameEditButton,myVerticalLayout);

    baseWindow->delAllEditButton = new QToolButton;
    baseWindow->delAllEditButton->setText(QObject::tr("Delete all"));
    baseWindow->delAllEditButton->setToolTip(QObject::tr("Delete all content"));
    baseWindow->delAllEditButton->setIcon(QIcon(":/icons/delete_all32.png"));
    baseWindow->delAllEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Files & Folders"), baseWindow->delAllEditButton, myVerticalLayout);

    baseWindow->updtEditButton = new QToolButton;
    baseWindow->updtEditButton->setText(QObject::tr("Update project"));
    baseWindow->updtEditButton->setToolTip(QObject::tr("Update all file & folder info"));
    baseWindow->updtEditButton->setIcon(QIcon(":/icons/refresh32.png"));
    baseWindow->updtEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Files & Folders"), baseWindow->updtEditButton, myVerticalLayout);

    baseWindow->dataTrackEditButton = new QToolButton;
    baseWindow->dataTrackEditButton->setText(QObject::tr("Data Track"));
    baseWindow->dataTrackEditButton->setToolTip(QObject::tr("Add a data track to the disc"));
    baseWindow->dataTrackEditButton->setIcon(QIcon(":/icons/datatrack32.png"));
    baseWindow->dataTrackEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Tracks"), baseWindow->dataTrackEditButton, nullptr);

    baseWindow->audioTrackEditButton = new QToolButton;
    baseWindow->audioTrackEditButton->setText(QObject::tr("Audio Track"));
    baseWindow->audioTrackEditButton->setToolTip(QObject::tr("Add a audio track to the disc"));
    baseWindow->audioTrackEditButton->setIcon(QIcon(":/icons/audiotrack32.png"));
    baseWindow->audioTrackEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Tracks"), baseWindow->audioTrackEditButton, nullptr);

    baseWindow->allSelectEditButton = new QToolButton;
    baseWindow->allSelectEditButton->setText(QObject::tr("Select All"));
    baseWindow->allSelectEditButton->setToolTip(QObject::tr("Select All files and folders"));
    baseWindow->allSelectEditButton->setIcon(QIcon(":/icons/sel_all32.png"));
    baseWindow->allSelectEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Selection"), baseWindow->allSelectEditButton,nullptr);

    baseWindow->inverseSelectEditButton = new QToolButton;
    baseWindow->inverseSelectEditButton->setText(QObject::tr("Inverse Selection"));
    baseWindow->inverseSelectEditButton->setToolTip(QObject::tr("Selected all expect the selected"));
    baseWindow->inverseSelectEditButton->setIcon(QIcon(":/icons/inverse_sel32.png"));
    baseWindow->inverseSelectEditButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Edit"), QObject::tr("Selection"), baseWindow->inverseSelectEditButton,nullptr);
}

extern void ribbonBuilderDevice(Ribbon *baseRibbon, MainWindow *baseWindow){

    //GridLayout
    QHBoxLayout *test = new QHBoxLayout();



    QGridLayout *devicesGroup = new QGridLayout();

    QLabel *wdReadDevice = new QLabel();
    wdReadDevice->setText(QObject::tr("Read:"));
    devicesGroup->addWidget(wdReadDevice,1,0);
    QLabel *wdBurnDevice = new QLabel();
    wdBurnDevice->setText(QObject::tr("Burn:"));
    devicesGroup->addWidget(wdBurnDevice,0,0);

    baseWindow->listReadDevicesWidget = new QComboBox();
    baseWindow->listReadDevicesWidget->setMinimumWidth(250);
    baseWindow->listReadDevicesWidget->setMaximumWidth(250);
    baseWindow->listReadDevicesWidget->setMaxVisibleItems(4);
    //readdevices->addWidget(baseWindow->listReadDevicesWidget, 0, Qt::AlignTop);
    devicesGroup->addWidget(baseWindow->listReadDevicesWidget,1,1);

    baseWindow->listBurnDevicesWidget = new QComboBox();
    baseWindow->listBurnDevicesWidget->setMinimumWidth(250);
    baseWindow->listBurnDevicesWidget->setMaximumWidth(250);
    baseWindow->listBurnDevicesWidget->setMaxVisibleItems(4);
    //burndevices->addWidget(baseWindow->listBurnDevicesWidget, 0, Qt::AlignTop);
    devicesGroup->addWidget(baseWindow->listBurnDevicesWidget,0,1);

    QWidget * wdgReadDiskWidget = new QWidget();

    test->setContentsMargins(3,0,0,0);
    wdgReadDiskWidget->setLayout(devicesGroup);
    wdgReadDiskWidget->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("Device"), QObject::tr("Read Device"), wdgReadDiskWidget);

    baseWindow->readDeviceUpdateButton = new QToolButton;
    baseWindow->readDeviceUpdateButton->setText(QObject::tr("Refresh"));
    baseWindow->readDeviceUpdateButton->setToolTip(QObject::tr("Refresh device list"));
    baseWindow->readDeviceUpdateButton->setIcon(QIcon(":/icons/refresh32.png"));
    baseWindow->readDeviceUpdateButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Device"), QObject::tr("Read Device"), baseWindow->readDeviceUpdateButton,nullptr);

    baseWindow->m_EjectTrayMenu = new QMenu();
    baseWindow->openDeviceButton = new QToolButton;
    baseWindow->openDeviceButton->setText(QObject::tr("Open tray"));
    baseWindow->openDeviceButton->setToolTip(QObject::tr("This will eject the disc from the drive"));
    baseWindow->openDeviceButton->setIcon(QIcon(":/icons/eject32.png"));
    baseWindow->openDeviceButton->setEnabled(true);
    baseWindow->openDeviceButton->setMenu(baseWindow->m_EjectTrayMenu);
    baseRibbon->addButton(QObject::tr("Device"), QObject::tr("Device"), baseWindow->openDeviceButton,nullptr);

    baseWindow->m_CloseTrayMenu = new QMenu();
    baseWindow->closeDeviceButton = new QToolButton;
    baseWindow->closeDeviceButton->setText(QObject::tr("Close tray"));
    baseWindow->closeDeviceButton->setToolTip(QObject::tr("This will close the device tray"));
    baseWindow->closeDeviceButton->setIcon(QIcon(":/icons/load32.png"));
    baseWindow->closeDeviceButton->setEnabled(true);
    baseWindow->closeDeviceButton->setMenu(baseWindow->m_CloseTrayMenu);
    baseRibbon->addButton(QObject::tr("Device"), QObject::tr("Device"), baseWindow->closeDeviceButton,nullptr);

    baseWindow->unLockDevice = new QToolButton;
    baseWindow->unLockDevice->setText(QObject::tr("Unlock"));
    baseWindow->unLockDevice->setToolTip(QObject::tr("This will try to unlock an locked burning device"));
    baseWindow->unLockDevice->setIcon(QIcon(":/icons/unlock32.png"));
    baseWindow->unLockDevice->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Device"), QObject::tr("Device"), baseWindow->unLockDevice,nullptr);

    baseWindow->infoDeviceButton = new QToolButton;
    baseWindow->infoDeviceButton->setText(QObject::tr("Device Info"));
    baseWindow->infoDeviceButton->setToolTip(QObject::tr("This will open the device information dialog."));
    baseWindow->infoDeviceButton->setIcon(QIcon(":/icons/deviceinfo32.png"));
    baseWindow->infoDeviceButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Device"), QObject::tr("Device"), baseWindow->infoDeviceButton,nullptr);

    baseWindow->m_diskInfoMenu = new QMenu();
    baseWindow->infoMediaButton = new QToolButton;
    baseWindow->infoMediaButton->setText(QObject::tr("Media Info"));
    baseWindow->infoMediaButton->setToolTip(QObject::tr("Get the information about the current disk."));
    baseWindow->infoMediaButton->setIcon(QIcon(":/icons/discinfo32.png"));
    baseWindow->infoMediaButton->setEnabled(true);
    baseWindow->infoMediaButton->setMenu(baseWindow->m_diskInfoMenu);
    baseRibbon->addButton(QObject::tr("Device"), QObject::tr("Media"), baseWindow->infoMediaButton, nullptr);

    baseWindow->imageMediaButton = new QToolButton;
    baseWindow->imageMediaButton->setText(QObject::tr("2Image"));
    baseWindow->imageMediaButton->setToolTip(QObject::tr("Save the current disk to disk image"));
    baseWindow->imageMediaButton->setIcon(QIcon(":/icons/create_image32.png"));
    baseWindow->imageMediaButton->setEnabled(false);
    baseRibbon->addButton(QObject::tr("Device"), QObject::tr("Media"), baseWindow->imageMediaButton, nullptr);


    baseWindow->grabAudioMediaButton = new QToolButton;
    baseWindow->grabAudioMediaButton->setText(QObject::tr("Grab"));
    baseWindow->grabAudioMediaButton->setToolTip(QObject::tr("Grab audio tracks to audio files"));
    baseWindow->grabAudioMediaButton->setIcon(QIcon(":/icons/grabaudio32.png"));
    baseWindow->grabAudioMediaButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Device"), QObject::tr("Media"), baseWindow->grabAudioMediaButton,nullptr);

    baseWindow->scanMediaButton = new QToolButton;
    baseWindow->scanMediaButton->setText(QObject::tr("Scan"));
    baseWindow->scanMediaButton->setToolTip(QObject::tr("Scan disc about read errors"));
    baseWindow->scanMediaButton->setIcon(QIcon(":/icons/diskscan32.png"));
    baseWindow->scanMediaButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Device"), QObject::tr("Media"), baseWindow->scanMediaButton,nullptr);

    baseWindow->hexMediaButton = new QToolButton;
    baseWindow->hexMediaButton->setText(QObject::tr("Sector Viewer"));
    baseWindow->hexMediaButton->setToolTip(QObject::tr("Show sectors in hexdecimal view"));
    baseWindow->hexMediaButton->setIcon(QIcon(":/icons/hexedit32.png"));
    baseWindow->hexMediaButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Device"), QObject::tr("Media"), baseWindow->hexMediaButton,nullptr);
}

extern void ribbonBuilderGeneral(Ribbon *baseRibbon, MainWindow *baseWindow){


    QGridLayout *gridLayoutGeneral = new QGridLayout();
    QWidget * wdgGeneralFeatues = new QWidget();

    baseWindow->simulateBurnGeneralCheck = new QCheckBox(QObject::tr("Simulate Burning"));
    baseWindow->simulateBurnGeneralCheck->setToolTip(QObject::tr("Simulate the burn processs by hardware"));
    baseWindow->simulateBurnGeneralCheck->setCheckState(Qt::Unchecked);
    gridLayoutGeneral->addWidget(baseWindow->simulateBurnGeneralCheck,0,0);

    baseWindow->ejectAfterGeneralCheck = new QCheckBox(QObject::tr("Eject after burn"));
    baseWindow->ejectAfterGeneralCheck->setToolTip(QObject::tr("Eject the disk or tray after burn process is finsihed or aborted."));
    baseWindow->ejectAfterGeneralCheck->setCheckState(Qt::Checked);
    gridLayoutGeneral->addWidget(baseWindow->ejectAfterGeneralCheck,0,1);

    baseWindow->autoEraseGeneralCheck = new QCheckBox(QObject::tr("Auto erase"));
    baseWindow->autoEraseGeneralCheck->setToolTip(QObject::tr("Enable the automatically erase of a disk."));
    baseWindow->autoEraseGeneralCheck->setCheckState(Qt::Unchecked);
    gridLayoutGeneral->addWidget(baseWindow->autoEraseGeneralCheck,0,2);

    baseWindow->burnProofGeneralCheck = new QCheckBox(QObject::tr("BURN-Proof / SafeBurn"));
    baseWindow->burnProofGeneralCheck->setToolTip(QObject::tr("Use buffer protection system"));
    baseWindow->burnProofGeneralCheck->setCheckState(Qt::Checked);
    gridLayoutGeneral->addWidget(baseWindow->burnProofGeneralCheck,1,0);

    baseWindow->verifyAfterGeneralCheck = new QCheckBox(QObject::tr("Verify disc"));
    baseWindow->verifyAfterGeneralCheck->setToolTip(QObject::tr("Verify disk content and source content about write errors."));
    baseWindow->verifyAfterGeneralCheck->setCheckState(Qt::Unchecked);
    gridLayoutGeneral->addWidget(baseWindow->verifyAfterGeneralCheck,1,1);

    baseWindow->padDataGeneralCheck = new QCheckBox(QObject::tr("Pad Data Track"));
    baseWindow->padDataGeneralCheck->setToolTip(QObject::tr("Will add padding packets to data tracks."));
    baseWindow->padDataGeneralCheck->setCheckState(Qt::Unchecked);
    gridLayoutGeneral->addWidget(baseWindow->padDataGeneralCheck,1,2);

    baseWindow->activeOPCGeneralCheck = new QCheckBox(QObject::tr("Active OPC"));
    baseWindow->activeOPCGeneralCheck ->setToolTip(QObject::tr("Enable Optimum Power Calibration before writing."));
    baseWindow->activeOPCGeneralCheck ->setCheckState(Qt::Unchecked);
    gridLayoutGeneral->addWidget(baseWindow->activeOPCGeneralCheck,2,0);

    baseWindow->finishDiscGeneralCheck = new QCheckBox(QObject::tr("Finish Medium"));
    baseWindow->finishDiscGeneralCheck->setToolTip(QObject::tr("Will close the disk to avoid further burning to the disk."));
    baseWindow->finishDiscGeneralCheck->setCheckState(Qt::Unchecked);
    gridLayoutGeneral->addWidget(baseWindow->finishDiscGeneralCheck,2,1);

    baseWindow->avchdGeneralCheck = new QCheckBox(QObject::tr("Allow AVCHD"));
    baseWindow->avchdGeneralCheck->setToolTip(QObject::tr("Allow to burn easy Blu-ray structure to DVD"));
    baseWindow->avchdGeneralCheck->setCheckState(Qt::Unchecked);
    gridLayoutGeneral->addWidget(baseWindow->avchdGeneralCheck,2,2);

    gridLayoutGeneral->setContentsMargins(3,0,0,0);
    wdgGeneralFeatues->setLayout(gridLayoutGeneral);
    wdgGeneralFeatues->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("General"), QObject::tr("Burn features"), wdgGeneralFeatues);

    QGridLayout *gridLayoutErase = new QGridLayout();
    QWidget * wdgEraseFeatues = new QWidget();

    baseWindow->eraseFast = new QCheckBox(QObject::tr("Fast erase"));
    baseWindow->eraseFast->setToolTip(QObject::tr("Activate this to just remove the TOC"));
    baseWindow->eraseFast->setCheckState(Qt::Checked);
    gridLayoutErase->addWidget(baseWindow->eraseFast,0,0);

    baseWindow->ejectAfterErase = new QCheckBox(QObject::tr("Eject after erase"));
    baseWindow->ejectAfterErase->setToolTip(QObject::tr("Eject the disk or tray after erase process is finsihed or aborted."));
    baseWindow->ejectAfterErase->setCheckState(Qt::Checked);
    gridLayoutErase->addWidget(baseWindow->ejectAfterErase,1,0);

    //Why size height + 7

    gridLayoutErase->setContentsMargins(3,0,0,0);
    wdgEraseFeatues->setLayout(gridLayoutErase);
    wdgEraseFeatues->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("General"), QObject::tr("Erase features"), wdgEraseFeatues);

    //Spacer is Checkbox + spacing of gridlayout
    QSpacerItem *spacerItem = new QSpacerItem(10,baseWindow->avchdGeneralCheck->sizeHint().height()+gridLayoutErase->layout()->spacing(), QSizePolicy::Fixed, QSizePolicy::Fixed);
    gridLayoutErase->addItem(spacerItem,2,0);

    QGridLayout *gridLayoutImage = new QGridLayout();
    QWidget * wdgImageFeatues = new QWidget();

    baseWindow->burnISOAutoErase = new QCheckBox(QObject::tr("Auto erase"));
    baseWindow->burnISOAutoErase->setToolTip(QObject::tr("Activate this to just remove the TOC"));
    baseWindow->burnISOAutoErase->setCheckState(Qt::Checked);
    gridLayoutImage->addWidget(baseWindow->burnISOAutoErase,0,0);
    baseWindow->burnISOEjectAfterBurn = new QCheckBox(QObject::tr("Eject after burn"));
    baseWindow->burnISOEjectAfterBurn->setToolTip(QObject::tr("Activate this to just remove the TOC"));
    baseWindow->burnISOEjectAfterBurn->setCheckState(Qt::Checked);
    gridLayoutImage->addWidget(baseWindow->burnISOEjectAfterBurn,1,0);
    baseWindow->burnISOUnderrunProtection = new QCheckBox(QObject::tr("Underrun Protection"));
    baseWindow->burnISOUnderrunProtection->setToolTip(QObject::tr("Activate this to just remove the TOC"));
    baseWindow->burnISOUnderrunProtection->setCheckState(Qt::Checked);
    gridLayoutImage->addWidget(baseWindow->burnISOUnderrunProtection,2,0);

    gridLayoutImage->setContentsMargins(3,0,0,0);
    wdgImageFeatues->setLayout(gridLayoutImage);
    wdgImageFeatues->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("General"), QObject::tr("Burn image features"), wdgImageFeatues);

    baseWindow->burnGeneralButton = new QToolButton;
    baseWindow->burnGeneralButton->setText(QObject::tr("Burn"));
    baseWindow->burnGeneralButton->setToolTip(QObject::tr("Burn selected Project"));
    baseWindow->burnGeneralButton->setIcon(QIcon(":/icons/burn32.png"));
    baseWindow->burnGeneralButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("General"), QObject::tr("Action"), baseWindow->burnGeneralButton,nullptr);

    baseWindow->eraseGeneralButton = new QToolButton;
    baseWindow->eraseGeneralButton->setText(QObject::tr("Erase"));
    baseWindow->eraseGeneralButton->setToolTip(QObject::tr("Erase RW disk"));
    baseWindow->eraseGeneralButton->setIcon(QIcon(":/icons/erase32.png"));
    baseWindow->eraseGeneralButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("General"), QObject::tr("Action"), baseWindow->eraseGeneralButton,nullptr);

    baseWindow->burnDiskImage = new QToolButton;
    baseWindow->burnDiskImage->setText(QObject::tr("Burn image"));
    baseWindow->burnDiskImage->setToolTip(QObject::tr("Burn disc image to disk"));
    baseWindow->burnDiskImage->setIcon(QIcon(":/icons/burnimage32.png"));
    baseWindow->burnDiskImage->setEnabled(true);
    baseRibbon->addButton(QObject::tr("General"), QObject::tr("Action"), baseWindow->burnDiskImage,nullptr);

}

extern void ribbonBuilderView(Ribbon *baseRibbon, MainWindow *baseWindow){

    baseWindow->viewBrowserButton = new QToolButton;
    baseWindow->viewBrowserButton->setText(QObject::tr("File Explorer"));
    baseWindow->viewBrowserButton->setToolTip(QObject::tr("Toggle disc explorer in project view"));
    baseWindow->viewBrowserButton->setIcon(QIcon(":/icons/filebrowser32.png"));
    baseWindow->viewBrowserButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("View"), QObject::tr("Explorer"), baseWindow->viewBrowserButton,nullptr);

    baseWindow->viewTileButton = new QToolButton;
    baseWindow->viewTileButton->setText(QObject::tr("Tile"));
    baseWindow->viewTileButton->setToolTip(QObject::tr("Tile all windows in project view"));
    baseWindow->viewTileButton->setIcon(QIcon(":/icons/tile_window32.png"));
    baseWindow->viewTileButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("View"), QObject::tr("Windows"), baseWindow->viewTileButton,nullptr);

    baseWindow->viewCascadeButton = new QToolButton;
    baseWindow->viewCascadeButton->setText(QObject::tr("Cascade"));
    baseWindow->viewCascadeButton->setToolTip(QObject::tr("Cascade all windows in project view"));
    baseWindow->viewCascadeButton->setIcon(QIcon(":/icons/cascade32.png"));
    baseWindow->viewCascadeButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("View"), QObject::tr("Windows"), baseWindow->viewCascadeButton,nullptr);

    baseWindow->viewCloseAllbutton = new QToolButton;
    baseWindow->viewCloseAllbutton->setText(QObject::tr("Close all"));
    baseWindow->viewCloseAllbutton->setToolTip(QObject::tr("Close all open sub windows"));
    baseWindow->viewCloseAllbutton->setIcon(QIcon(":/icons/close_all32.png"));
    baseWindow->viewCloseAllbutton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("View"), QObject::tr("Windows"), baseWindow->viewCloseAllbutton,nullptr);

    baseWindow->m_windowMenu = new QMenu();
    baseWindow->viewSwitchButton = new QToolButton;
    baseWindow->viewSwitchButton->setText(QObject::tr("Switch"));
    baseWindow->viewSwitchButton->setToolTip(QObject::tr("Switch to selected project in project view"));
    baseWindow->viewSwitchButton->setIcon(QIcon(":/icons/windowswitch32.png"));
    baseWindow->viewSwitchButton->setEnabled(true);
    baseWindow->viewSwitchButton->setMenu(baseWindow->m_windowMenu);
    baseRibbon->addButton(QObject::tr("View"), QObject::tr("Windows"), baseWindow->viewSwitchButton,nullptr);

}

extern void ribbonBuilder2Image(Ribbon *baseRibbon, MainWindow *baseWindow){

    QVBoxLayout *imageFormatLyout = new QVBoxLayout();
    QWidget * wdgImageFeatues = new QWidget();
    QSpacerItem *spaceItem = new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding);

    baseWindow->imageFormatCombo = new QComboBox();
    baseWindow->imageFormatCombo->setToolTip(QObject::tr("Output Format for Image"));
    baseWindow->imageFormatCombo->setMinimumWidth(120);
    baseWindow->imageFormatCombo->setMaximumWidth(120);

    imageFormatLyout->addWidget(baseWindow->imageFormatCombo);
    imageFormatLyout->addSpacerItem(spaceItem);
    imageFormatLyout->setSpacing(0);
    imageFormatLyout->setContentsMargins(0,0,0,0);
    wdgImageFeatues->setLayout(imageFormatLyout);
    wdgImageFeatues->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("Create image"), QObject::tr("Format"), wdgImageFeatues);

    QGridLayout *imageReadErrorLayout = new QGridLayout();
    QWidget * wdgImageReadError = new QWidget();
    baseWindow->imageReadErrorSwitch = new QCheckBox(QObject::tr("Enable Correction"));
    baseWindow->imageReadErrorSwitch->setToolTip(QObject::tr("Enable / Disable read erorr correction"));
    baseWindow->imageReadErrorSwitch->setTristate(false); //Stupid Tristate will always throw 2 instead of 1
    baseWindow->imageReadErrorSwitch->setCheckState(Qt::Unchecked);

    imageReadErrorLayout->addWidget(baseWindow->imageReadErrorSwitch,0,0,1,2);
    QLabel *wdgReadErrorHard = new QLabel();
    wdgReadErrorHard->setText(QObject::tr("Hardware Retry:"));
    imageReadErrorLayout->addWidget(wdgReadErrorHard,1,0);
    baseWindow->imageReadHardRetry = new QDoubleSpinBox;
    baseWindow->imageReadHardRetry->setMinimumWidth(60);
    baseWindow->imageReadHardRetry->setMaximumWidth(60);
    baseWindow->imageReadHardRetry->setRange(0, 100.0);
    baseWindow->imageReadHardRetry->setSingleStep(1.0);
    baseWindow->imageReadHardRetry->setValue(0.0);
    baseWindow->imageReadHardRetry->setDecimals(0); //No need to abstract int64 spinbox
    imageReadErrorLayout->addWidget(baseWindow->imageReadHardRetry,1,1);
    QLabel *wdgReadErrorSoft = new QLabel();
    wdgReadErrorSoft->setText(QObject::tr("Software Retry:"));
    imageReadErrorLayout->addWidget(wdgReadErrorSoft,2,0);
    baseWindow->imageReadSoftRetry = new QDoubleSpinBox;
    baseWindow->imageReadSoftRetry->setMinimumWidth(60);
    baseWindow->imageReadSoftRetry->setMaximumWidth(60);
    baseWindow->imageReadSoftRetry->setRange(0, 32.0);
    baseWindow->imageReadSoftRetry->setSingleStep(1.0);
    baseWindow->imageReadSoftRetry->setValue(0.0);
    baseWindow->imageReadSoftRetry->setDecimals(0); //No need to abstract int64 spinbox
    imageReadErrorLayout->addWidget(baseWindow->imageReadSoftRetry,2,1);
    imageReadErrorLayout->setSpacing(0);
    imageReadErrorLayout->setContentsMargins(0,0,0,0);
    wdgImageReadError->setLayout(imageReadErrorLayout);
    wdgImageReadError->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("Create image"), QObject::tr("Read Error"), wdgImageReadError);

    QGridLayout *imageJobLayout = new QGridLayout();
    QWidget * wdgImageJob = new QWidget();
    QSpacerItem *spaceItem1 = new QSpacerItem(1,25, QSizePolicy::Fixed, QSizePolicy::Fixed); //Expanding

    baseWindow->imageReadJobCreate = new QCheckBox(QObject::tr("Create"));
    baseWindow->imageReadJobCreate->setToolTip(QObject::tr("Enable / Disable read erorr correction"));
    baseWindow->imageReadJobCreate->setTristate(false);
    baseWindow->imageReadJobCreate->setCheckState(Qt::Unchecked);

    baseWindow->imageReadJobVerify = new QCheckBox(QObject::tr("Verify"));
    baseWindow->imageReadJobVerify->setToolTip(QObject::tr("Enable / Disable read erorr correction"));
    baseWindow->imageReadJobVerify->setTristate(false);
    baseWindow->imageReadJobVerify->setCheckState(Qt::Unchecked);

    imageJobLayout->addWidget(baseWindow->imageReadJobCreate,0,0);
    imageJobLayout->addWidget(baseWindow->imageReadJobVerify,1,0);
    imageJobLayout->addItem(spaceItem1, 2, 0, -1, -1);

    imageJobLayout->setSpacing(0);
    imageJobLayout->setContentsMargins(0,0,0,0);
    wdgImageJob->setLayout(imageJobLayout);
    wdgImageJob->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("Create image"), QObject::tr("Job"), wdgImageJob);

    baseWindow->startImageCreateJob = new QToolButton;
    baseWindow->startImageCreateJob->setText(QObject::tr("Start"));
    baseWindow->startImageCreateJob->setToolTip(QObject::tr("Start create disk image"));
    baseWindow->startImageCreateJob->setIcon(QIcon(":/icons/create_image_start32.png"));
    baseWindow->startImageCreateJob->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Create image"), QObject::tr("Job"), baseWindow->startImageCreateJob,nullptr);
}

extern void ribbonBuilderHex(Ribbon *baseRibbon, MainWindow *baseWindow){

    QVBoxLayout *hexSectorLayout = new QVBoxLayout();
    QWidget * wdgHexSector= new QWidget();
    QSpacerItem *spaceHexItem = new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding);

    baseWindow->hexSector = new QDoubleSpinBox;
    baseWindow->hexSector->setMinimumWidth(100);
    baseWindow->hexSector->setMaximumWidth(100);
    baseWindow->hexSector->setRange(0, 10.0);
    baseWindow->hexSector->setSingleStep(1.0);
    baseWindow->hexSector->setValue(0.0);
    baseWindow->hexSector->setDecimals(0); //No need to abstract int64 spinbox
    hexSectorLayout->addWidget(baseWindow->hexSector);
    hexSectorLayout->addSpacerItem(spaceHexItem);
    hexSectorLayout->setSpacing(0);
    hexSectorLayout->setContentsMargins(0,0,0,0);
    wdgHexSector->setLayout(hexSectorLayout);
    wdgHexSector->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("Hex Editor"), QObject::tr("Sector"), wdgHexSector);

    baseWindow->updateHexEditor = new QToolButton;
    baseWindow->updateHexEditor->setText(QObject::tr("Update"));
    baseWindow->updateHexEditor->setToolTip(QObject::tr("Re-read disk content"));
    baseWindow->updateHexEditor->setIcon(QIcon(":/icons/refresh32.png"));
    baseWindow->updateHexEditor->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Hex Editor"), QObject::tr("Actions"), baseWindow->updateHexEditor,nullptr);

    baseWindow->saveHexEditor = new QToolButton;
    baseWindow->saveHexEditor->setText(QObject::tr("Save"));
    baseWindow->saveHexEditor->setToolTip(QObject::tr("Save sector content to text file"));
    baseWindow->saveHexEditor->setIcon(QIcon(":/icons/saveas32.png"));
    baseWindow->saveHexEditor->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Hex Editor"), QObject::tr("Actions"), baseWindow->saveHexEditor,nullptr);

    baseWindow->navFirstHexEditor = new QToolButton;
    baseWindow->navFirstHexEditor->setText(QObject::tr("First"));
    baseWindow->navFirstHexEditor->setToolTip(QObject::tr("Jump to the first sector"));
    baseWindow->navFirstHexEditor->setIcon(QIcon(":/icons/backward32.png"));
    baseWindow->navFirstHexEditor->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Hex Editor"), QObject::tr("Navigate"), baseWindow->navFirstHexEditor,nullptr);

    baseWindow->navStepBackHexEditor = new QToolButton;
    baseWindow->navStepBackHexEditor->setText(QObject::tr("Back"));
    baseWindow->navStepBackHexEditor->setToolTip(QObject::tr("One sector back"));
    baseWindow->navStepBackHexEditor->setIcon(QIcon(":/icons/arrow_left32.png"));
    baseWindow->navStepBackHexEditor->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Hex Editor"), QObject::tr("Navigate"), baseWindow->navStepBackHexEditor,nullptr);

    baseWindow->navStepForwardHexEditor = new QToolButton;
    baseWindow->navStepForwardHexEditor->setText(QObject::tr("Forward"));
    baseWindow->navStepForwardHexEditor->setToolTip(QObject::tr("One sector forward"));
    baseWindow->navStepForwardHexEditor->setIcon(QIcon(":/icons/arrow_right32.png"));
    baseWindow->navStepForwardHexEditor->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Hex Editor"), QObject::tr("Navigate"), baseWindow->navStepForwardHexEditor,nullptr);

    baseWindow->navLastHexEditor = new QToolButton;
    baseWindow->navLastHexEditor->setText(QObject::tr("Last"));
    baseWindow->navLastHexEditor->setToolTip(QObject::tr("Jump to the last sector"));
    baseWindow->navLastHexEditor->setIcon(QIcon(":/icons/forward32.png"));
    baseWindow->navLastHexEditor->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Hex Editor"), QObject::tr("Navigate"), baseWindow->navLastHexEditor,nullptr);
}

extern void ribbonBuilderScan(Ribbon *baseRibbon, MainWindow *baseWindow){

    QGridLayout *scanSectorLayout = new QGridLayout();
    QWidget * wdgScanSector= new QWidget();
    QSpacerItem *spaceScanItem = new QSpacerItem(1,25, QSizePolicy::Fixed, QSizePolicy::Fixed); //Expanding

    QLabel *wdgStartOffset = new QLabel();
    wdgStartOffset->setText(QObject::tr("Start:"));
    scanSectorLayout->addWidget(wdgStartOffset,0,0);

    baseWindow->scanStartOffset = new QDoubleSpinBox;
    baseWindow->scanStartOffset->setMinimumWidth(100);
    baseWindow->scanStartOffset->setMaximumWidth(100);
    baseWindow->scanStartOffset->setRange(0, 10.0);
    baseWindow->scanStartOffset->setSingleStep(1.0);
    baseWindow->scanStartOffset->setValue(0.0);
    baseWindow->scanStartOffset->setDecimals(0); //No need to abstract int64 spinbox
    scanSectorLayout->addWidget(baseWindow->scanStartOffset,0,1);

    QLabel *wdgReadOffset = new QLabel();
    wdgReadOffset->setText(QObject::tr("Read:"));
    scanSectorLayout->addWidget(wdgReadOffset,1,0);

    baseWindow->scanReadOffset = new QDoubleSpinBox;
    baseWindow->scanReadOffset->setMinimumWidth(100);
    baseWindow->scanReadOffset->setMaximumWidth(100);
    baseWindow->scanReadOffset->setRange(0, 10.0);
    baseWindow->scanReadOffset->setSingleStep(1.0);
    baseWindow->scanReadOffset->setValue(0.0);
    baseWindow->scanReadOffset->setDecimals(0); //No need to abstract int64 spinbox
    scanSectorLayout->addWidget(baseWindow->scanReadOffset,1,1);

    scanSectorLayout->addItem(spaceScanItem, 2, 0, -1, -1);

    scanSectorLayout->setSpacing(0);
    scanSectorLayout->setContentsMargins(0,0,0,0);
    wdgScanSector->setLayout(scanSectorLayout);
    wdgScanSector->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("Scan Editor"), QObject::tr("Offset"), wdgScanSector);

    baseWindow->updateScanEditor = new QToolButton;
    baseWindow->updateScanEditor->setText(QObject::tr("Update"));
    baseWindow->updateScanEditor->setToolTip(QObject::tr("Update reading device"));
    baseWindow->updateScanEditor->setIcon(QIcon(":/icons/refresh32.png"));
    baseWindow->updateScanEditor->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Scan Editor"), QObject::tr("Actions"), baseWindow->updateScanEditor,nullptr);

    baseWindow->startScanEditor = new QToolButton;
    baseWindow->startScanEditor->setText(QObject::tr("Start"));
    baseWindow->startScanEditor->setToolTip(QObject::tr("Start scanning the disc"));
    baseWindow->startScanEditor->setIcon(QIcon(":/icons/diskscan_start32.png"));
    baseWindow->startScanEditor->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Scan Editor"), QObject::tr("Actions"), baseWindow->startScanEditor,nullptr);

    baseWindow->stopScanEditor = new QToolButton;
    baseWindow->stopScanEditor->setText(QObject::tr("Stop"));
    baseWindow->stopScanEditor->setToolTip(QObject::tr("Stop scanning the disc"));
    baseWindow->stopScanEditor->setIcon(QIcon(":/icons/diskscan_stop32.png"));
    baseWindow->stopScanEditor->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Scan Editor"), QObject::tr("Actions"), baseWindow->stopScanEditor,nullptr);

    baseWindow->saveScanEditor = new QToolButton;
    baseWindow->saveScanEditor->setText(QObject::tr("Save"));
    baseWindow->saveScanEditor->setToolTip(QObject::tr("Save current scan log to file"));
    baseWindow->saveScanEditor->setIcon(QIcon(":/icons/saveas32.png"));
    baseWindow->saveScanEditor->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Scan Editor"), QObject::tr("Actions"), baseWindow->saveScanEditor,nullptr);
}

extern void ribbonBuilderFileSystem(Ribbon *baseRibbon, MainWindow *baseWindow){

    QGridLayout *fsMainGridLayout = new QGridLayout();
    QWidget * wdgFSFeatues = new QWidget();

    QHBoxLayout *fsMainFillLayout = new QHBoxLayout();

    QLabel *wdgISOLevelText = new QLabel();
    wdgISOLevelText->setText(QObject::tr("ISO Level:"));
    fsMainFillLayout->addWidget(wdgISOLevelText);

    baseWindow->isoLevelFSCombo = new QComboBox();
    baseWindow->isoLevelFSCombo->setToolTip(QObject::tr("ISO Level to use."));
    baseWindow->isoLevelFSCombo->setMinimumWidth(140);
    baseWindow->isoLevelFSCombo->setMaximumWidth(140);
    fsMainFillLayout->addWidget(baseWindow->isoLevelFSCombo);

    fsMainGridLayout->setContentsMargins(3,0,0,0);
    fsMainGridLayout->addLayout(fsMainFillLayout,0,0);

    baseWindow->jolietLongNamesFSCheck = new QCheckBox(QObject::tr("Allow long Joliet names"));
    baseWindow->jolietLongNamesFSCheck->setToolTip(QObject::tr("Allow to use long Joliet file names."));
    baseWindow->jolietLongNamesFSCheck->setCheckState(Qt::Unchecked);
    fsMainGridLayout->addWidget(baseWindow->jolietLongNamesFSCheck,0,1);

    baseWindow->useJolietFSCheck = new QCheckBox(QObject::tr("Joliet Extension"));
    baseWindow->useJolietFSCheck->setToolTip(QObject::tr("Use the Joliet ISO9660 extension"));
    baseWindow->useJolietFSCheck->setCheckState(Qt::Unchecked);
    fsMainGridLayout->addWidget(baseWindow->useJolietFSCheck,1,0);

    baseWindow->isoManyDirectoriesFSCheck = new QCheckBox(QObject::tr("Allow many directories"));
    baseWindow->isoManyDirectoriesFSCheck->setToolTip(QObject::tr("Allow to more directories than ISO9660 allow."));
    baseWindow->isoManyDirectoriesFSCheck->setCheckState(Qt::Unchecked);
    fsMainGridLayout->addWidget(baseWindow->isoManyDirectoriesFSCheck,1,1);

    baseWindow->isoExtent1FSCheck = new QCheckBox(QObject::tr("Not write ISO ';1' Extension"));
    baseWindow->isoExtent1FSCheck ->setToolTip(QObject::tr("The application will not write the ISO ';1' Extension to files."));
    baseWindow->isoExtent1FSCheck ->setCheckState(Qt::Unchecked);
    fsMainGridLayout->addWidget(baseWindow->isoExtent1FSCheck,1,2);

    baseWindow->isoLongPathFSCheck = new QCheckBox(QObject::tr("Allow path longer than 255"));
    baseWindow->isoLongPathFSCheck ->setToolTip(QObject::tr("Set this option the application will allow very long file names"));
    baseWindow->isoLongPathFSCheck ->setCheckState(Qt::Unchecked);
    fsMainGridLayout->addWidget(baseWindow->isoLongPathFSCheck,0,2);

    baseWindow->useRockRidgeFSCheck = new QCheckBox(QObject::tr("Rock Ridge Extension"));
    baseWindow->useRockRidgeFSCheck->setToolTip(QObject::tr("Use the Rock Ridge ISO9660 extension"));
    baseWindow->useRockRidgeFSCheck->setCheckState(Qt::Unchecked);
    fsMainGridLayout->addWidget(baseWindow->useRockRidgeFSCheck,2,0);

    baseWindow->isoLongFileNamesFSCheck = new QCheckBox(QObject::tr("Allow long ISO9660 names"));
    baseWindow->isoLongFileNamesFSCheck->setToolTip(QObject::tr("Allow to use long file names."));
    baseWindow->isoLongFileNamesFSCheck->setCheckState(Qt::Unchecked);
    fsMainGridLayout->addWidget(baseWindow->isoLongFileNamesFSCheck,2,1);

    fsMainGridLayout->setRowMinimumHeight(0,25);
    fsMainGridLayout->setRowMinimumHeight(1,25);
    fsMainGridLayout->setRowMinimumHeight(2,25);
    fsMainGridLayout->setContentsMargins(3,0,0,0);
    wdgFSFeatues->setLayout(fsMainGridLayout);
    wdgFSFeatues->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("File System"), QObject::tr("ISO9660"), wdgFSFeatues);

    QGridLayout *fsUDFLayout = new QGridLayout();
    QWidget * wdgFSUDFFeatues = new QWidget();
    QHBoxLayout *fsUDFFillLayout = new QHBoxLayout();

    QLabel *wdgUDFVersonText = new QLabel();
    wdgUDFVersonText->setText(QObject::tr("Version:"));
    fsUDFFillLayout->addWidget(wdgUDFVersonText);
    baseWindow->udfVersionFSCombo = new QComboBox();
    baseWindow->udfVersionFSCombo->setToolTip(QObject::tr("UDF VErsion to use on disc."));
    baseWindow->udfVersionFSCombo->setMinimumWidth(140);
    baseWindow->udfVersionFSCombo->setMaximumWidth(140);

    fsUDFFillLayout->addWidget(baseWindow->udfVersionFSCombo);
    fsUDFLayout->addLayout(fsUDFFillLayout,0,0);

    QHBoxLayout *udfFSPartitionFiller = new QHBoxLayout();
    QLabel *wdgUDFPartitionText = new QLabel();

    wdgUDFPartitionText->setText(QObject::tr("Partition:"));
    udfFSPartitionFiller->addWidget(wdgUDFPartitionText);
    baseWindow->udfPartitionFSCombo = new QComboBox();
    baseWindow->udfPartitionFSCombo->setToolTip(QObject::tr("UDF Parition type to use on disc."));
    baseWindow->udfPartitionFSCombo->setMinimumWidth(140);
    baseWindow->udfPartitionFSCombo->setMaximumWidth(140);

    udfFSPartitionFiller->addWidget(baseWindow->udfPartitionFSCombo);
    fsUDFLayout->addLayout(udfFSPartitionFiller,1,0);

    baseWindow->udfFileStreamFSCheck = new QCheckBox(QObject::tr("Write File Stream"));
    baseWindow->udfFileStreamFSCheck->setToolTip(QObject::tr("Write File Stream"));
    baseWindow->udfFileStreamFSCheck->setCheckState(Qt::Unchecked);

    fsUDFLayout->addWidget(baseWindow->udfFileStreamFSCheck,2,0);
    fsUDFLayout->setContentsMargins(3,0,0,0);
    wdgFSUDFFeatues->setLayout(fsUDFLayout);
    wdgFSUDFFeatues->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("File System"), QObject::tr("UDF"), wdgFSUDFFeatues);
}

extern void ribbonBuilderISOExtended(Ribbon *baseRibbon, MainWindow *baseWindow){

    QGridLayout *isoExGridLayout = new QGridLayout();
    QWidget * wdgSOExFeatues = new QWidget();

    QLabel *wdgIsoExSystemIdText = new QLabel();
    wdgIsoExSystemIdText->setText(QObject::tr("System ID:"));
    isoExGridLayout->addWidget(wdgIsoExSystemIdText,0,0);
    baseWindow->edIsoExSystemIdValue = new QLineEdit(baseWindow);
    baseWindow->edIsoExSystemIdValue->setMinimumWidth(130);
    baseWindow->edIsoExSystemIdValue->setMaximumWidth(130);

    isoExGridLayout->addWidget(baseWindow->edIsoExSystemIdValue,0,1);

    QLabel *wdgIsoExVolumeSetText = new QLabel();
    wdgIsoExVolumeSetText->setText(QObject::tr("Volume Set:"));
    isoExGridLayout->addWidget(wdgIsoExVolumeSetText,0,2);
    baseWindow->edIsoExVolumeSetValue = new QLineEdit(baseWindow);
    baseWindow->edIsoExVolumeSetValue->setMinimumWidth(130);
    baseWindow->edIsoExVolumeSetValue->setMaximumWidth(130);

    isoExGridLayout->addWidget(baseWindow->edIsoExVolumeSetValue,0,3);

    QLabel *wdgIsoExPublisherText = new QLabel();
    wdgIsoExPublisherText->setText(QObject::tr("Publisher:"));
    isoExGridLayout->addWidget(wdgIsoExPublisherText,0,4);
    baseWindow->edIsoExPublisherValue = new QLineEdit(baseWindow);
    baseWindow->edIsoExPublisherValue->setMinimumWidth(130);
    baseWindow->edIsoExPublisherValue->setMaximumWidth(130);

    isoExGridLayout->addWidget(baseWindow->edIsoExPublisherValue,0,5);

    QLabel *wdgIsoExDataPreparerText = new QLabel();
    wdgIsoExDataPreparerText->setText(QObject::tr("Data Preparer:"));
    isoExGridLayout->addWidget(wdgIsoExDataPreparerText,1,0);
    baseWindow->edIsoExDataPreparerValue = new QLineEdit(baseWindow);
    baseWindow->edIsoExDataPreparerValue->setMinimumWidth(130);
    baseWindow->edIsoExDataPreparerValue->setMaximumWidth(130);

    isoExGridLayout->addWidget(baseWindow->edIsoExDataPreparerValue,1,1);

    QLabel *wdgIsoExApplicationText = new QLabel();
    wdgIsoExApplicationText->setText(QObject::tr("Application:"));
    isoExGridLayout->addWidget(wdgIsoExApplicationText,1,2);
    baseWindow->edIsoExApplicationValue = new QLineEdit(baseWindow);
    baseWindow->edIsoExApplicationValue->setMinimumWidth(130);
    baseWindow->edIsoExApplicationValue->setMaximumWidth(130);

    isoExGridLayout->addWidget(baseWindow->edIsoExApplicationValue,1,3);

    QLabel *wdgIsoExCopyrightFileText = new QLabel();
    wdgIsoExCopyrightFileText->setText(QObject::tr("Copyright File:"));
    isoExGridLayout->addWidget(wdgIsoExCopyrightFileText,1,4);
    baseWindow->edIsoExCopyrightFileValue = new QLineEdit(baseWindow);
    baseWindow->edIsoExCopyrightFileValue->setMinimumWidth(130);
    baseWindow->edIsoExCopyrightFileValue->setMaximumWidth(130);

    isoExGridLayout->addWidget(baseWindow->edIsoExCopyrightFileValue,1,5);

    QLabel *wdgIsoExAbstractFileText = new QLabel();
    wdgIsoExAbstractFileText->setText(QObject::tr("Abstract File:"));
    isoExGridLayout->addWidget(wdgIsoExAbstractFileText,2,0);
    baseWindow->edIsoExAbstractFileValue = new QLineEdit(baseWindow);
    baseWindow->edIsoExAbstractFileValue->setMinimumWidth(130);
    baseWindow->edIsoExAbstractFileValue->setMaximumWidth(130);

    isoExGridLayout->addWidget(baseWindow->edIsoExAbstractFileValue,2,1);

    QLabel *wdgIsoExBibliographicFileText = new QLabel();
    wdgIsoExBibliographicFileText->setText(QObject::tr("Bibliographic File:"));
    isoExGridLayout->addWidget(wdgIsoExBibliographicFileText,2,2);
    baseWindow->edIsoExBibliographicFileValue = new QLineEdit(baseWindow);
    baseWindow->edIsoExBibliographicFileValue->setMinimumWidth(130);
    baseWindow->edIsoExBibliographicFileValue->setMaximumWidth(130);

    isoExGridLayout->addWidget(baseWindow->edIsoExBibliographicFileValue,2,3);

    isoExGridLayout->setContentsMargins(3,0,0,0);
    isoExGridLayout->setRowMinimumHeight(0,25);
    isoExGridLayout->setRowMinimumHeight(1,25);
    isoExGridLayout->setRowMinimumHeight(2,25);
    wdgSOExFeatues->setLayout(isoExGridLayout);
    wdgSOExFeatues->setFixedHeight(80);
    baseRibbon->addWidgetGroup(QObject::tr("ISO Extended"), QObject::tr("ISO Extended"), wdgSOExFeatues);

    baseWindow->importIsoExButton = new QToolButton;
    baseWindow->importIsoExButton->setText(QObject::tr("Import"));
    baseWindow->importIsoExButton->setToolTip(QObject::tr("Import IsoEx from software settings"));
    baseWindow->importIsoExButton->setIcon(QIcon(":/icons/extiso_import32.png"));
    baseWindow->importIsoExButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("ISO Extended"), QObject::tr("Data"), baseWindow->importIsoExButton,nullptr);

    baseWindow->resetIsoExButton = new QToolButton;
    baseWindow->resetIsoExButton->setText(QObject::tr("Reset"));
    baseWindow->resetIsoExButton->setToolTip(QObject::tr("Reset all IsoEx fields"));
    baseWindow->resetIsoExButton->setIcon(QIcon(":/icons/extiso_reset32.png"));
    baseWindow->resetIsoExButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("ISO Extended"), QObject::tr("Data"), baseWindow->resetIsoExButton,nullptr);

}

extern void ribbonBuilderBoot(Ribbon *baseRibbon, MainWindow *baseWindow){

    baseWindow->selBootImageButton = new QToolButton;
    baseWindow->selBootImageButton->setText(QObject::tr("Set"));
    baseWindow->selBootImageButton->setToolTip(QObject::tr("If the BootImage is set, Doiskbutler will create a bootable disk."));
    baseWindow->selBootImageButton->setIcon(QIcon(":/icons/bootimage32.png"));
    baseWindow->selBootImageButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Boot Disc"), QObject::tr("Boot Image"), baseWindow->selBootImageButton,nullptr);
    //connect(selBootImageButton, SIGNAL(clicked()),this, SLOT(clickedSelBootImagePath()));

    QVBoxLayout *bootVerticalLayout = new QVBoxLayout();
    baseRibbon->addVerticalLayout(QObject::tr("Boot Disc"), QObject::tr("Boot Image"), bootVerticalLayout);

    baseWindow->delBootImageButton = new QToolButton;
    baseWindow->delBootImageButton->setText(QObject::tr("Remove"));
    baseWindow->delBootImageButton->setToolTip(QObject::tr("Remove current boot image"));
    baseWindow->delBootImageButton->setIcon(QIcon(":/icons/delete32.png"));
    baseWindow->delBootImageButton->setEnabled(true);
    baseRibbon->addButton(QObject::tr("Boot Disc"), QObject::tr("Boot Image"), baseWindow->delBootImageButton, bootVerticalLayout);
    //connect(delBootImageButton, SIGNAL(clicked()), this, SLOT(deleteBootImage()));

    //bootVerticalLayout->addStretch();
    bootVerticalLayout->setAlignment(Qt::AlignTop);
    bootVerticalLayout->setContentsMargins(3,1,3,0);

    //Test, erst die widgets erstellen dann ins Grid packen.

    baseWindow->imagePathBootEdit = new QLineEdit(baseWindow);

    //line 1
    QLabel *labelDeveloperID = new QLabel();
    labelDeveloperID->setText(QObject::tr("Developer ID:"));

    QLabel *labelBootImage = new QLabel();
    labelBootImage->setText(QObject::tr("Boot Image:"));

    baseWindow->developerIDBootEdit = new QLineEdit(baseWindow);
    baseWindow->developerIDBootEdit->setMinimumWidth(100);
    baseWindow->developerIDBootEdit->setMaximumWidth(100);

    QLabel *labelSectos = new QLabel();
    labelSectos->setText(QObject::tr("Sectors:"));

    baseWindow->sectorsBootEdit = new QLineEdit(baseWindow);
    baseWindow->sectorsBootEdit->setMinimumWidth(80);
    baseWindow->sectorsBootEdit->setMaximumWidth(80);

    QLabel *labelLoadSegment = new QLabel();
    labelLoadSegment->setText(QObject::tr("Load segment:"));

    baseWindow->loadSegmentBootEdit = new QLineEdit(baseWindow);
    baseWindow->loadSegmentBootEdit->setMinimumWidth(80);
    baseWindow->loadSegmentBootEdit->setMaximumWidth(80);

    QLabel *labelEmulation = new QLabel();
    labelEmulation->setText(QObject::tr("Emulation Type:"));

    baseWindow->emulationTypeBootCombo = new QComboBox();
    baseWindow->emulationTypeBootCombo->setMinimumWidth(100);
    baseWindow->emulationTypeBootCombo->setMaximumWidth(100);
    baseWindow->emulationTypeBootCombo->addItem(QObject::tr("No emulaton (NT/2000/XP)"));
    baseWindow->emulationTypeBootCombo->addItem(QObject::tr("Floppy 1.2MB"));
    baseWindow->emulationTypeBootCombo->addItem(QObject::tr("Floppy 1.44MB (95/98/ME)"));
    baseWindow->emulationTypeBootCombo->addItem(QObject::tr("Floppy 2.88MB"));
    baseWindow->emulationTypeBootCombo->addItem(QObject::tr("Hard Disk"));

    QLabel *labelPlatfom = new QLabel();
    labelPlatfom->setText(QObject::tr("Platform:"));

    baseWindow->platformBootCombo = new QComboBox();
    baseWindow->platformBootCombo->setMinimumWidth(100);
    baseWindow->platformBootCombo->setMaximumWidth(100);
    baseWindow->platformBootCombo->addItem(QObject::tr("x86 - 32"));
    baseWindow->platformBootCombo->addItem(QObject::tr("x86 - 64"));
    baseWindow->platformBootCombo->addItem(QObject::tr("PowerPC"));
    baseWindow->platformBootCombo->addItem(QObject::tr("MAC"));

    QLabel *labelBootIsoLevel = new QLabel();
    labelBootIsoLevel ->setText(QObject::tr("ISO Level:"));

    baseWindow->bootISOLevelCombo = new QComboBox();
    baseWindow->bootISOLevelCombo->setToolTip(QObject::tr("ISO Level to use."));
    baseWindow->bootISOLevelCombo->setMinimumWidth(140);
    baseWindow->bootISOLevelCombo->setMaximumWidth(140);

    QGridLayout *myBootGridLayout = new QGridLayout();
    myBootGridLayout->setContentsMargins(3,3,3,0);
    //myBootGridLayout->setSpacing(5);
    myBootGridLayout->setRowMinimumHeight(0,25);
    myBootGridLayout->setRowMinimumHeight(1,25);
    myBootGridLayout->setRowMinimumHeight(2,25);

    myBootGridLayout->addWidget(labelBootImage,0,0);
    myBootGridLayout->addWidget(baseWindow->imagePathBootEdit,0,1,1,5);

    myBootGridLayout->addWidget(labelEmulation,1,0);
    myBootGridLayout->addWidget(baseWindow->emulationTypeBootCombo,1,1);
    myBootGridLayout->addWidget(labelPlatfom,1,2);
    myBootGridLayout->addWidget(baseWindow->platformBootCombo,1,3);
    myBootGridLayout->addWidget(labelBootIsoLevel,1,4);
    myBootGridLayout->addWidget(baseWindow->bootISOLevelCombo,1,5);



    myBootGridLayout->addWidget(labelSectos,2,0);
    myBootGridLayout->addWidget(baseWindow->sectorsBootEdit,2,1);
    myBootGridLayout->addWidget(labelLoadSegment,2,2);
    myBootGridLayout->addWidget(baseWindow->loadSegmentBootEdit,2,3);
    myBootGridLayout->addWidget(labelDeveloperID,2,4);
    myBootGridLayout->addWidget(baseWindow->developerIDBootEdit,2,5);


    QWidget * wdgBootFeatues = new QWidget();

    wdgBootFeatues->setLayout(myBootGridLayout);
    wdgBootFeatues->setFixedHeight(80);

    baseRibbon->addWidgetGroup(QObject::tr("Boot Disc"), QObject::tr("El-Torito"), wdgBootFeatues);

}

#endif // RIBBONBUILDER_H
