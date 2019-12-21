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


/* Todos
 * Die Buttons für die Fenster Ansicht einbauen.
 * AddFile etc. nur Aktiv wenn Project aktiv ist. Folder Regeln beobachten.
 * Im Moment sind ja Gruppe inaktiv also auch die Gruppenbeueichnungen, ich finde es aber besse wenn ur die Buttons selber,
 * nicht die Gruppen inaktiv werden.
 * Icons für die Toolbar oben
 * Delete Buttons aktivieren
 * hasSelectedTreeItem() liefert falsch infos wegen der Auswahl von TRacks zurück
 * deleteAll liefert falschen vector, dateien scheinen nicht gelöscht zu werden.
 * AddFile Button darf nur auf Datatrack aktiv sein, nicht auf Session
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMenu>
#include "CommonTreeWidget.h"
#include "mdichild_dialog.h"
#include "configdialog.h"
#include "burndialog.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

//##############################################################################################################
//Important Information to work with the Foxburner SDK and QT
//##############################################################################################################
/*
 * tDeviceInfo.strVendor = QString::fromWCharArray(vendorId_);
 * FoxBurner String Conversion für Windows Linux und MAc
 * #if defined (WIN32)
 *  stISOInfo.strVolumeLabel = QString::fromUtf16(pISOVolumeInfo.chVolumeLabel);
 *#else
 *  stISOInfo.strVolumeLabel = QString::fromUtf8(pISOVolumeInfo.chVolumeLabel);
 *#endif
 */

void CheckVersionThread::run()
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    //qDebug() << QString("http://www.ifoerster.com/diskbutler/diskbutler_checkversion.php?v="+qApp->applicationVersion());

    QNetworkRequest req( QUrl( QString("http://www.ifoerster.com/diskbutler/diskbutler_checkversion.php?v="+qApp->applicationVersion()) ) );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        //success
        QString strReply = (QString)reply->readAll();
        qDebug("List: %s",strReply.toLatin1().constData());
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonObject jsonObj = jsonResponse.object();

        qDebug() << jsonObj["Version"].toString();

        if(jsonObj["Version"].toString().compare("no")!=0){
            emit tNewVersion(jsonObj["Version"].toString());
        }

        delete reply;
    }
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("Diskbutler"));


#if defined (LINUX)
    QString mLicense = "A2JTSM-V8L7AO-G2RA75-AU0L0A-KPM0GP";
#endif
#if defined (MAC)
    QString mLicense = "IXJNQ9-J9FYAD-KWDAX5-ATBLQA-9LMJVZ";
#endif
#if defined (WIN32)
    QString mLicense = "XLJB0G-A73TAK-KPIA0E-A5JP0A-CFM24B";
#endif

#if defined (WIN32)
    int res = ::Initialize((const TCHAR*)mLicense.utf16(), BS_ASPI_INTERNAL, BS_TRUE);
#else
    int res = ::Initialize((const TCHAR*)mLicense.toUtf8(), BS_ASPI_INTERNAL, BS_TRUE);
#endif

    if(res!=0){
        QString result = tr("FoxSDK reports: %1").arg(res);
        QMessageBox::information(this, tr("Information"),result);
    }

     ui->ribbonDockWidget->setTitleBarWidget(new QWidget());

     mdiArea = new QMdiArea;
     mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
     mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
     setCentralWidget(mdiArea);

     res = ::CreateProject(BS_BURNER_ISOUDF,BS_CONTINUE_NO_SESSION );
     if(res!=0){
         QMessageBox::information(this, tr("Information"),
                                  tr("Error in Create Projekt"));
     }

     //Test Versionchecker here. Will it block the UI?
     mCheckVersionThread  = new CheckVersionThread();
     connect(mCheckVersionThread, SIGNAL(tNewVersion(QString)), this, SLOT(onNewVersionAvail(QString)));
     mCheckVersionThread->start();


     //fillDriveList();

     // Add tabs to ribbon
     ui->ribbonTabWidget->addTab(QIcon(":/icons/blueprint.png"), tr("Project"));
     ui->ribbonTabWidget->addTab(QIcon(":/icons/general.png"), tr("Edit"));
     ui->ribbonTabWidget->addTab(QIcon(":/icons/device.png"), tr("Device"));
     ui->ribbonTabWidget->addTab(QIcon(":/icons/projectset.png"), tr("General"));
     ui->ribbonTabWidget->addTab(QIcon(":/icons/filesystem.png"), tr("File System"));
     ui->ribbonTabWidget->addTab(QIcon(":/icons/extiso.png"), tr("ISO Extended"));
     ui->ribbonTabWidget->addTab(QIcon(":/icons/boot.png"), tr("Boot Disc"));
     ui->ribbonTabWidget->addTab(QIcon(":/icons/025.png"), tr("View"));

     // Add 'Open project' button
     dataIsoProjectButton = new QToolButton;
     dataIsoProjectButton->setText(tr("ISO"));
     dataIsoProjectButton->setToolTip(tr("Create ISO Project"));
     dataIsoProjectButton->setIcon(QIcon(":/icons/32datacd.png"));
     dataIsoProjectButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Data"), dataIsoProjectButton,nullptr);
     connect(dataIsoProjectButton, &QToolButton::clicked,  [=] { newProject(RuleManager::TYPE_PROJECT_ISO); });

     dataUdfProjectButton = new QToolButton;
     dataUdfProjectButton->setText(tr("UDF"));
     dataUdfProjectButton->setToolTip(tr("Create UDF Project"));
     dataUdfProjectButton->setIcon(QIcon(":/icons/32dataudf.png"));
     dataUdfProjectButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Data"), dataUdfProjectButton,nullptr);
     connect(dataUdfProjectButton, &QToolButton::clicked,  [=] { newProject(RuleManager::TYPE_PROJECT_UDF); });

     dataIsoUdfProjectButton = new QToolButton;
     dataIsoUdfProjectButton->setText(tr("ISO/UDF"));
     dataIsoUdfProjectButton->setToolTip(tr("Create ISO/UDF Bridge Project"));
     dataIsoUdfProjectButton->setIcon(QIcon(":/icons/32dataudfiso.png"));
     dataIsoUdfProjectButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Data"), dataIsoUdfProjectButton,nullptr);
     connect(dataIsoUdfProjectButton, &QToolButton::clicked,  [=] { newProject(RuleManager::TYPE_PROJECT_ISOUDF); });

     mmAudioProjectButton = new QToolButton;
     mmAudioProjectButton->setText(tr("Audio"));
     mmAudioProjectButton->setToolTip(tr("Create AudioCD Project"));
     mmAudioProjectButton->setIcon(QIcon(":/icons/32audiocd.png"));
     mmAudioProjectButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Multimedia"), mmAudioProjectButton,nullptr);
     connect(mmAudioProjectButton, &QToolButton::clicked,  [=] { newProject(RuleManager::TYPE_PROJECT_AUDIOCD); });

     mmEnhancedProjectButton = new QToolButton;
     mmEnhancedProjectButton->setText(tr("MixedMode"));
     mmEnhancedProjectButton->setToolTip(tr("Create MixedModeCD Project"));
     mmEnhancedProjectButton->setIcon(QIcon(":/icons/32mixedmode.png"));
     mmEnhancedProjectButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Multimedia"), mmEnhancedProjectButton,nullptr);
     connect(mmEnhancedProjectButton, &QToolButton::clicked,  [=] { newProject(RuleManager::TYPE_PROJECT_MIXEDMODE); });

     vidSVCDProjectButton = new QToolButton;
     vidSVCDProjectButton->setText(tr("SVCD"));
     vidSVCDProjectButton->setToolTip(tr("Create SVCD Video Project"));
     vidSVCDProjectButton->setIcon(QIcon(":/icons/32svideocd.png"));
     vidSVCDProjectButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Video"), vidSVCDProjectButton,nullptr);
     connect(vidSVCDProjectButton, &QToolButton::clicked,  [=] { newProject(RuleManager::TYPE_PROJECT_SUPERVIDEO); });

     vidDVDProjectButton = new QToolButton;
     vidDVDProjectButton->setText(tr("Video DVD"));
     vidDVDProjectButton->setToolTip(tr("Create VideoDVD Project"));
     vidDVDProjectButton->setIcon(QIcon(":/icons/32videodvd.png"));
     vidDVDProjectButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Video"), vidDVDProjectButton,nullptr);
     connect(vidDVDProjectButton, &QToolButton::clicked,  [=] { newProject(RuleManager::TYPE_PROJECT_VIDEODVD); });

     vidBlueProjectButton = new QToolButton;
     vidBlueProjectButton->setText(tr("Blu-ray"));
     vidBlueProjectButton->setToolTip(tr("Create Blu-ray Video Project"));
     vidBlueProjectButton->setIcon(QIcon(":/icons/32bluray.png"));
     vidBlueProjectButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Video"), vidBlueProjectButton,nullptr);
     //connect(vidBlueProjectButton, SIGNAL(clicked()), mapperNewFile, SLOT(map()));
     connect(vidBlueProjectButton, &QToolButton::clicked,  [=] { newProject(RuleManager::TYPE_PROJECT_BLURAYVIDEO); });


    //Open & Save
     appOpenButton = new QToolButton;
     appOpenButton->setText(tr("Open"));
     appOpenButton->setToolTip(tr("Open DiskButler project"));
     appOpenButton->setIcon(QIcon(":/icons/open32.png"));
     appOpenButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Open & Save"), appOpenButton,nullptr);
     connect(appOpenButton, SIGNAL(clicked()), this, SLOT(open()));

     appSaveButton = new QToolButton;
     appSaveButton->setText(tr("Save"));
     appSaveButton->setToolTip(tr("Save disk to DiskButler project"));
     appSaveButton->setIcon(QIcon(":/icons/save32.png"));
     appSaveButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Open & Save"), appSaveButton,nullptr);
     connect(appSaveButton, SIGNAL(clicked()), this, SLOT(save()));

     appSaveAsButton = new QToolButton;
     appSaveAsButton->setText(tr("Save As"));
     appSaveAsButton->setToolTip(tr("Save disk to DiskButler project"));
     appSaveAsButton->setIcon(QIcon(":/icons/save32.png"));
     appSaveAsButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Project"), tr("Open & Save"), appSaveAsButton,nullptr);
     connect(appSaveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));

     //Project Things
     addFileEditButton = new QToolButton;
     addFileEditButton->setText(tr("Add File"));
     addFileEditButton->setToolTip(tr("Add File to the project"));
     addFileEditButton->setIcon(QIcon(":/icons/fileadd.png"));
     addFileEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Files & Folders"), addFileEditButton,nullptr);
     connect(addFileEditButton, SIGNAL(clicked()), this, SLOT(insertItem()));

     addFolderEditButton = new QToolButton;
     addFolderEditButton->setText(tr("Add Folder"));
     addFolderEditButton->setToolTip(tr("Add a folder and its content"));
     addFolderEditButton->setIcon(QIcon(":/icons/folderadd.png"));
     addFolderEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Files & Folders"), addFolderEditButton,nullptr);
     connect(addFolderEditButton, SIGNAL(clicked()), this, SLOT(insertNode()));

     createFolderEditButton = new QToolButton;
     createFolderEditButton->setText(tr("Create Folder"));
     createFolderEditButton->setToolTip(tr("Create a new empty folder"));
     createFolderEditButton->setIcon(QIcon(":/icons/createfolder.png"));
     createFolderEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Files & Folders"), createFolderEditButton,nullptr);
     connect(createFolderEditButton, SIGNAL(clicked()), this, SLOT(addNode()));

     delEditButton = new QToolButton;
     delEditButton->setText(tr("Delete Item"));
     delEditButton->setToolTip(tr("Delete selected item or folder"));
     delEditButton->setIcon(QIcon(":/icons/delete.png"));
     delEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Files & Folders"), delEditButton, nullptr);
     connect(delEditButton, SIGNAL(clicked()), this, SLOT(deleteItem()));
     //Vertical Buttons

     QVBoxLayout *myVerticalLayout = new QVBoxLayout;
     ui->ribbonTabWidget->addVerticalLayout(tr("Edit"), tr("Files & Folders"), myVerticalLayout);

     renameEditButton = new QToolButton;
     renameEditButton->setText(tr("Rename"));
     renameEditButton->setToolTip(tr("Rename selected item"));
     renameEditButton->setIcon(QIcon(":/icons/rename.png"));
     renameEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Files & Folders"), renameEditButton,myVerticalLayout);
     connect(renameEditButton, SIGNAL(clicked()), this, SLOT(renameItemEx()));

     delAllEditButton = new QToolButton;
     delAllEditButton->setText(tr("Delete all"));
     delAllEditButton->setToolTip(tr("Delete all content"));
     delAllEditButton->setIcon(QIcon(":/icons/deleteall.png"));
     delAllEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Files & Folders"), delAllEditButton, myVerticalLayout);
     connect(delAllEditButton, SIGNAL(clicked()), this, SLOT(resetFiles()));

     updtEditButton = new QToolButton;
     updtEditButton->setText(tr("Update project"));
     updtEditButton->setToolTip(tr("Update all file & folder info"));
     updtEditButton->setIcon(QIcon(":/icons/refresh_32.png"));
     updtEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Files & Folders"), updtEditButton, myVerticalLayout);
     connect(updtEditButton, SIGNAL(clicked()), this, SLOT(updateProject()));


     //TRacks
     dataTrackEditButton = new QToolButton;
     dataTrackEditButton->setText(tr("Data Track"));
     dataTrackEditButton->setToolTip(tr("Add a data track to the disc"));
     dataTrackEditButton->setIcon(QIcon(":/icons/datatrack.png"));
     dataTrackEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Tracks"), dataTrackEditButton, nullptr);
     connect(dataTrackEditButton, SIGNAL(clicked()), this, SLOT(addDataTrack()));

     audioTrackEditButton = new QToolButton;
     audioTrackEditButton->setText(tr("Audio Track"));
     audioTrackEditButton->setToolTip(tr("Add a audio track to the disc"));
     audioTrackEditButton->setIcon(QIcon(":/icons/audiotrack.png"));
     audioTrackEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Tracks"), audioTrackEditButton, nullptr);
     connect(audioTrackEditButton, SIGNAL(clicked()), this, SLOT(addAudioTrack()));


     //Selection
     allSelectEditButton = new QToolButton;
     allSelectEditButton->setText(tr("Select All"));
     allSelectEditButton->setToolTip(tr("Select All files and folders"));
     allSelectEditButton->setIcon(QIcon(":/icons/selall.png"));
     allSelectEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Selection"), allSelectEditButton,nullptr);
     connect(allSelectEditButton, SIGNAL(clicked()), this, SLOT(selectAll()));

     inverseSelectEditButton = new QToolButton;
     inverseSelectEditButton->setText(tr("Inverse Selection"));
     inverseSelectEditButton->setToolTip(tr("Selected all expect the selected"));
     inverseSelectEditButton->setIcon(QIcon(":/icons/inversesel.png"));
     inverseSelectEditButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Edit"), tr("Selection"), inverseSelectEditButton,nullptr);
     connect(inverseSelectEditButton, SIGNAL(clicked()), this, SLOT(reverseSelection()));




     QHBoxLayout *readdevices = new QHBoxLayout(this);
     QWidget * wdgReadDiskWidget = new QWidget(this);

     listReadDevicesWidget = new QListWidget(this);
     listReadDevicesWidget->setSelectionMode(QAbstractItemView::SingleSelection);
     listReadDevicesWidget->setMinimumWidth(250);
     //connect(listBurnDevicesWidget,
       //          SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
      //           this, SLOT(updateBurnDeviceSel(QListWidgetItem *)));
     connect(listReadDevicesWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(updateReadDeviceSel(QListWidgetItem*)));

     fillReadDriveList();

     readdevices->addWidget(listReadDevicesWidget);
     readdevices->setContentsMargins(3,0,3,0);
     wdgReadDiskWidget->setLayout(readdevices);
     wdgReadDiskWidget->setFixedHeight(80);

     ui->ribbonTabWidget->addWidgetGroup(tr("Device"), tr("Read Device"), wdgReadDiskWidget);

     readDeviceUpdateButton = new QToolButton;
     readDeviceUpdateButton->setText(tr("Refresh"));
     readDeviceUpdateButton->setToolTip(tr("Refresh device list"));
     readDeviceUpdateButton->setIcon(QIcon(":/icons/refresh_32.png"));
     readDeviceUpdateButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Device"), tr("Read Device"), readDeviceUpdateButton,nullptr);
     connect(readDeviceUpdateButton, SIGNAL(clicked()), this, SLOT(fillReadDriveList()));

     //Now we build Device Tab
     openDeviceButton = new QToolButton;
     openDeviceButton->setText(tr("Open tray"));
     openDeviceButton->setToolTip(tr("This will eject the disc from the drive"));
     openDeviceButton->setIcon(QIcon(":/icons/eject.png"));
     openDeviceButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Device"), tr("Device"), openDeviceButton,nullptr);
     //connect(inverseSelectEditButton, SIGNAL(clicked()), this, SLOT(reverseSelection()));

     closeDeviceButton = new QToolButton;
     closeDeviceButton->setText(tr("Close tray"));
     closeDeviceButton->setToolTip(tr("This will close the device tray"));
     closeDeviceButton->setIcon(QIcon(":/icons/load.png"));
     closeDeviceButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Device"), tr("Device"), closeDeviceButton,nullptr);
     //connect(inverseSelectEditButton, SIGNAL(clicked()), this, SLOT(reverseSelection()));

     infoDeviceButton = new QToolButton;
     infoDeviceButton->setText(tr("Device Info"));
     infoDeviceButton->setToolTip(tr("This will open the device information dialog."));
     infoDeviceButton->setIcon(QIcon(":/icons/Device_Infos_32.png"));
     infoDeviceButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Device"), tr("Device"), infoDeviceButton,nullptr);
     //connect(inverseSelectEditButton, SIGNAL(clicked()), this, SLOT(reverseSelection()));
     connect(infoDeviceButton, SIGNAL(clicked()), this, SLOT(newDeviceInfo()));


     infoMediaButton = new QToolButton;
     infoMediaButton->setText(tr("Media Info"));
     infoMediaButton->setToolTip(tr("Get the information about the current disk."));
     infoMediaButton->setIcon(QIcon(":/icons/Disco_Infos_32.png"));
     infoMediaButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Device"), tr("Media"), infoMediaButton,nullptr);
     //connect(inverseSelectEditButton, SIGNAL(clicked()), this, SLOT(reverseSelection()));
     connect(infoMediaButton, SIGNAL(clicked()), this, SLOT(newDiskInfo()));

     imageMediaButton = new QToolButton;
     imageMediaButton->setText(tr("2Image"));
     imageMediaButton->setToolTip(tr("Save the current disk to disk image"));
     imageMediaButton->setIcon(QIcon(":/icons/create_image.png"));
     imageMediaButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Device"), tr("Media"), imageMediaButton,nullptr);
     //connect(inverseSelectEditButton, SIGNAL(clicked()), this, SLOT(reverseSelection()));

     grabAudioMediaButton = new QToolButton;
     grabAudioMediaButton->setText(tr("Grab"));
     grabAudioMediaButton->setToolTip(tr("Grab audio tracks to audio files"));
     grabAudioMediaButton->setIcon(QIcon(":/icons/grabaudio.png"));
     grabAudioMediaButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Device"), tr("Media"), grabAudioMediaButton,nullptr);
     //connect(grabAudioediaButton, SIGNAL(clicked()), this, SLOT(reverseSelection()));

     scanMediaButton = new QToolButton;
     scanMediaButton->setText(tr("Scan"));
     scanMediaButton->setToolTip(tr("Scan disc about read errors"));
     scanMediaButton->setIcon(QIcon(":/icons/diskscan.png"));
     scanMediaButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Device"), tr("Media"), scanMediaButton,nullptr);
     connect(scanMediaButton, SIGNAL(clicked()), this, SLOT(openScanDialog()));

     hexMediaButton = new QToolButton;
     hexMediaButton->setText(tr("Sector Viewer"));
     hexMediaButton->setToolTip(tr("Show sectors in hexdecimal view"));
     hexMediaButton->setIcon(QIcon(":/icons/hexedit.png"));
     hexMediaButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Device"), tr("Media"), hexMediaButton,nullptr);
     connect(hexMediaButton, SIGNAL(clicked()), this, SLOT(openHexEditor()));


     //Widget Tets //Drives, backup Text for later
     QHBoxLayout *burndevices = new QHBoxLayout(this);
     QWidget * wdgDisk1Widget = new QWidget(this);

     listBurnDevicesWidget = new QListWidget(this);
     listBurnDevicesWidget->setSelectionMode(QAbstractItemView::SingleSelection);
     listBurnDevicesWidget->setMinimumWidth(250);
     //connect(listBurnDevicesWidget,
       //          SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
      //           this, SLOT(updateBurnDeviceSel(QListWidgetItem *)));
     connect(listBurnDevicesWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(updateBurnDeviceSel(QListWidgetItem*)));

     fillBurnDriveList();

     burndevices->addWidget(listBurnDevicesWidget);
     burndevices->setContentsMargins(3,0,3,0);
     wdgDisk1Widget->setLayout(burndevices);
     wdgDisk1Widget->setFixedHeight(80);

     ui->ribbonTabWidget->addWidgetGroup(tr("General"), tr("Burn Device(s)"), wdgDisk1Widget);

     burnDeviceUpdateButton = new QToolButton;
     burnDeviceUpdateButton->setText(tr("Refresh"));
     burnDeviceUpdateButton->setToolTip(tr("Refresh device list"));
     burnDeviceUpdateButton->setIcon(QIcon(":/icons/refresh_32.png"));
     burnDeviceUpdateButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("General"), tr("Burn Device(s)"), burnDeviceUpdateButton,nullptr);
     connect(burnDeviceUpdateButton, SIGNAL(clicked()), this, SLOT(fillBurnDriveList()));


     /*
      *
      *      QVBoxLayout *deviceVerticalLayout = new QVBoxLayout;
     ui->ribbonTabWidget->addVerticalLayout(tr("General"), tr("Device"), deviceVerticalLayout);

     burnDeviceUpdateButton = new QToolButton;
     //burnDeviceUpdateButton->setText(tr("Remove"));
     burnDeviceUpdateButton->setToolTip(tr("Refresh device list"));
     burnDeviceUpdateButton->setIcon(QIcon(":/icons/refresh_32.png"));
     burnDeviceUpdateButton->setEnabled(true);
     burnDeviceUpdateButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
     ui->ribbonTabWidget->addButton(tr("General"), tr("Device"), burnDeviceUpdateButton, deviceVerticalLayout);
     //connect(delBootImageButton, SIGNAL(clicked()), this, SLOT(deleteBootImage()));
     //deviceVerticalLayout->addStretch();
     burndevices->addWidget(listBurnDevicesWidget);

     burnDeviceUpdateGeneralButton = new QPushButton(QIcon(":/icons/refresh_32.png"),"");
     burnDeviceUpdateGeneralButton->setStyleSheet("QPushButton{border:none;background-color:rgba(255, 255, 255,100);}QPushButton:pressed {padding-top:0px;padding-left:2px;}"); // Style sheet
     burnDeviceUpdateGeneralButton->setIconSize(QSize(16,16));
     burnDeviceUpdateGeneralButton->setMinimumSize(16,16);
     //connect(burnDeviceUpdateGeneralButton, SIGNAL(clicked()), this, SLOT(fillBurnDriveList()));
     burndevices->addWidget(burnDeviceUpdateGeneralButton);


     QVBoxLayout *bootVerticalLayout = new QVBoxLayout;
     ui->ribbonTabWidget->addVerticalLayout(tr("Boot Disc"), tr("Boot Image"), bootVerticalLayout);

     delBootImageButton = new QToolButton;
     delBootImageButton->setText(tr("Remove"));
     delBootImageButton->setToolTip(tr("Remove current boot image"));
     delBootImageButton->setIcon(QIcon(":/icons/delete.png"));
     delBootImageButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Boot Disc"), tr("Boot Image"), delBootImageButton, bootVerticalLayout);
     connect(delBootImageButton, SIGNAL(clicked()), this, SLOT(deleteBootImage()));
     bootVerticalLayout->addStretch();



     burndevices->setContentsMargins(3,0,0,0);
     wdgDisk1Widget->setLayout(burndevices);
     wdgDisk1Widget->setFixedHeight(80);
     ui->ribbonTabWidget->addWidgetGroup(tr("General"), tr("Device"), wdgDisk1Widget);



     QGridLayout *generalDeviceGridLayout = new QGridLayout(this);
     QWidget * wdgDisk1Widget = new QWidget(this);

     QLabel *labelDevice = new QLabel(this);
     labelDevice->setText(tr("Device:"));
     generalDeviceGridLayout->addWidget(labelDevice,0,0);

     burnDeviceGeneralCombo = new QComboBox();
     burnDeviceGeneralCombo->setToolTip(tr("Burn drive selection"));
     burnDeviceGeneralCombo->setMinimumWidth(200);
     burnDeviceGeneralCombo->setMaximumWidth(200);
     burnDeviceGeneralCombo->setMaximumWidth(200);
     connect(burnDeviceGeneralCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(burnDriveChanged(int)));

     fillBurnDriveList();

     generalDeviceGridLayout->addWidget(burnDeviceGeneralCombo,0,1);

     burnDeviceUpdateGeneralButton = new QPushButton(QIcon(":/icons/refresh_32.png"),"");
     burnDeviceUpdateGeneralButton->setStyleSheet("QPushButton{border:none;background-color:rgba(255, 255, 255,100);}QPushButton:pressed {padding-top:0px;padding-left:2px;}"); // Style sheet
     burnDeviceUpdateGeneralButton->setIconSize(QSize(16,16));
     burnDeviceUpdateGeneralButton->setMinimumSize(16,16);
     //connect(burnDeviceUpdateGeneralButton, SIGNAL(clicked()), this, SLOT(fillBurnDriveList()));
     generalDeviceGridLayout->addWidget(burnDeviceUpdateGeneralButton,0,2);

     QLabel *labelSpeed = new QLabel(this);
     labelSpeed->setText(tr("Speed:"));
     generalDeviceGridLayout->addWidget(labelSpeed,1,0);

     speedGeneralCombo= new QComboBox();
     speedGeneralCombo->setToolTip(tr("Burn drive selection"));
     speedGeneralCombo->setMinimumWidth(120);
     speedGeneralCombo->setMaximumWidth(120);
     speedGeneralCombo->addItem(tr("max."));
     generalDeviceGridLayout->addWidget(speedGeneralCombo,1,1);

     speedUpdateGeneralButton = new QPushButton(QIcon(":/icons/refresh_32.png"),"");
     speedUpdateGeneralButton->setStyleSheet("QPushButton{border:none;background-color:rgba(255, 255, 255,100);}QPushButton:pressed {padding-top:0px;padding-left:2px;}"); // Style sheet
     speedUpdateGeneralButton->setIconSize(QSize(16,16));
     speedUpdateGeneralButton->setMinimumSize(16,16);
     //connect(speedUpdateGeneralButton, SIGNAL(clicked()), this, SLOT(fillBurnDriveList()));
     generalDeviceGridLayout->addWidget(speedUpdateGeneralButton,1,2);

     QLabel *labelCount = new QLabel(this);
     labelCount->setText(tr("Copies:"));
     generalDeviceGridLayout->addWidget(labelCount,2,0);

     copiesGeneralCombo= new QComboBox();
     copiesGeneralCombo->setToolTip(tr("Burn drive selection"));
     copiesGeneralCombo->setMinimumWidth(120);
     copiesGeneralCombo->setMaximumWidth(120);
     copiesGeneralCombo->addItem(tr("1"));
     copiesGeneralCombo->addItem(tr("2"));
     copiesGeneralCombo->addItem(tr("3"));
     copiesGeneralCombo->setEditable(true);
     generalDeviceGridLayout->addWidget(copiesGeneralCombo,2,1);
*/



     QGridLayout *myFirstGridLayout = new QGridLayout(this);
     QWidget * wdgGeneralFeatues = new QWidget(this);

     simulateBurnGeneralCheck = new QCheckBox(tr("Simulate Burning"));
     simulateBurnGeneralCheck->setToolTip(tr("Simulate the burn processs by hardware"));
     simulateBurnGeneralCheck->setCheckState(Qt::Unchecked);
     connect(simulateBurnGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectSimulateBurnChanged(int)));
     myFirstGridLayout->addWidget(simulateBurnGeneralCheck,0,0);

     ejectAfterGeneralCheck = new QCheckBox(tr("Eject after burn"));
     ejectAfterGeneralCheck->setToolTip(tr("Eject the disk or tray after burn process is finsihed or aborted."));
     ejectAfterGeneralCheck->setCheckState(Qt::Checked);
     connect(ejectAfterGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectEjectChanged(int)));
     myFirstGridLayout->addWidget(ejectAfterGeneralCheck,0,1);

     autoEraseGeneralCheck = new QCheckBox(tr("Auto erase"));
     autoEraseGeneralCheck->setToolTip(tr("Enable the automatically erase of a disk."));
     autoEraseGeneralCheck->setCheckState(Qt::Unchecked);
     connect(autoEraseGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectAutoEraseChanged(int)));
     myFirstGridLayout->addWidget(autoEraseGeneralCheck,0,2);

     burnProofGeneralCheck = new QCheckBox(tr("BURN-Proof / SafeBurn"));
     burnProofGeneralCheck->setToolTip(tr("Use buffer protection system"));
     burnProofGeneralCheck->setCheckState(Qt::Checked);
     connect(burnProofGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectBurnProofChanged(int)));
     myFirstGridLayout->addWidget(burnProofGeneralCheck,1,0);

     verifyAfterGeneralCheck = new QCheckBox(tr("Verify disc"));
     verifyAfterGeneralCheck->setToolTip(tr("Verify disk content and source content about write errors."));
     verifyAfterGeneralCheck->setCheckState(Qt::Unchecked);
     connect(verifyAfterGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectVerifyChanged(int)));
     myFirstGridLayout->addWidget(verifyAfterGeneralCheck,1,1);

     padDataGeneralCheck = new QCheckBox(tr("Pad Data Track"));
     padDataGeneralCheck->setToolTip(tr("Will add padding packets to data tracks."));
     padDataGeneralCheck->setCheckState(Qt::Unchecked);
     connect(padDataGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectPadDataChanged(int)));
     myFirstGridLayout->addWidget(padDataGeneralCheck,1,2);


     activeOPCGeneralCheck = new QCheckBox(tr("Active OPC"));
     activeOPCGeneralCheck ->setToolTip(tr("Enable Optimum Power Calibration before writing."));
     activeOPCGeneralCheck ->setCheckState(Qt::Unchecked);
     connect(activeOPCGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectOPCChanged(int)));
     myFirstGridLayout->addWidget(activeOPCGeneralCheck,2,0);


     finishDiscGeneralCheck = new QCheckBox(tr("Finish Medium"));
     finishDiscGeneralCheck->setToolTip(tr("Will close the disk to avoid further burning to the disk."));
     finishDiscGeneralCheck->setCheckState(Qt::Unchecked);
     connect(finishDiscGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectFinishMediumChanged(int)));
     myFirstGridLayout->addWidget(finishDiscGeneralCheck,2,1);


     avchdGeneralCheck = new QCheckBox(tr("Allow AVCHD"));
     avchdGeneralCheck->setToolTip(tr("Allow to burn easy Blu-ray structure to DVD"));
     avchdGeneralCheck->setCheckState(Qt::Unchecked);
     connect(avchdGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectAVCHDChanged(int)));
     myFirstGridLayout->addWidget(avchdGeneralCheck,2,2);

     myFirstGridLayout->setContentsMargins(3,0,0,0);
     wdgGeneralFeatues->setLayout(myFirstGridLayout);
     wdgGeneralFeatues->setFixedHeight(80);
     ui->ribbonTabWidget->addWidgetGroup(tr("General"), tr("Features"), wdgGeneralFeatues);

     //Execute
     burnGeneralButton = new QToolButton;
     burnGeneralButton->setText(tr("Burn"));
     burnGeneralButton->setToolTip(tr("Burn selected Project"));
     burnGeneralButton->setIcon(QIcon(":/icons/burn_32.png"));
     burnGeneralButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("General"), tr("Execute"), burnGeneralButton,nullptr);
     connect(burnGeneralButton, SIGNAL(clicked()), this, SLOT(doBurn()));

     eraseGeneralButton = new QToolButton;
     eraseGeneralButton->setText(tr("Erase"));
     eraseGeneralButton->setToolTip(tr("Erase RW disk"));
     eraseGeneralButton->setIcon(QIcon(":/icons/Erase_32.png"));
     eraseGeneralButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("General"), tr("Execute"), eraseGeneralButton,nullptr);
     //connect(dataUdfProjectButton, SIGNAL(clicked()), mapperNewFile, SLOT(map()));;

     //View
     viewBrowserButton = new QToolButton;
     viewBrowserButton->setText(tr("File Explorer"));
     viewBrowserButton->setToolTip(tr("Toggle disc explorer in project view"));
     viewBrowserButton->setIcon(QIcon(":/icons/filebrowser.png"));
     viewBrowserButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("View"), tr("Explorer"), viewBrowserButton,nullptr);
     connect(viewBrowserButton, SIGNAL(clicked()), this, SLOT(toggleFileExplorer()));

     viewTileButton = new QToolButton;
     viewTileButton->setText(tr("Tile"));
     viewTileButton->setToolTip(tr("Tile all windows in project view"));
     viewTileButton->setIcon(QIcon(":/icons/arrangeicons.png"));
     viewTileButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("View"), tr("Windows"), viewTileButton,nullptr);
     connect(viewTileButton, SIGNAL(clicked()), mdiArea, SLOT(tileSubWindows()));

     viewCascadeButton = new QToolButton;
     viewCascadeButton->setText(tr("Cascade"));
     viewCascadeButton->setToolTip(tr("Cascade all windows in project view"));
     viewCascadeButton->setIcon(QIcon(":/icons/cascade.png"));
     viewCascadeButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("View"), tr("Windows"), viewCascadeButton,nullptr);
     connect(viewCascadeButton, SIGNAL(clicked()), mdiArea, SLOT(cascadeSubWindows()));

     m_windowMenu = new QMenu();
     viewSwitchButton = new QToolButton;
     viewSwitchButton->setText(tr("Switch"));
     viewSwitchButton->setToolTip(tr("Switch to selected project in project view"));
     viewSwitchButton->setIcon(QIcon(":/icons/windowswitch.png"));
     viewSwitchButton->setEnabled(true);
     viewSwitchButton->setMenu(m_windowMenu);
     ui->ribbonTabWidget->addButton(tr("View"), tr("Windows"), viewSwitchButton,nullptr);
     connect(m_windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowSwitchMenu()));

     //File System
     QGridLayout *myFSGridLayout = new QGridLayout(this);
     QWidget * wdgFSFeatues = new QWidget(this);

     QHBoxLayout *isoFSLEvelFiller = new QHBoxLayout(this);

     QLabel *wdgISOLevelText = new QLabel(this);
     wdgISOLevelText->setText(tr("ISO Level:"));
     isoFSLEvelFiller->addWidget(wdgISOLevelText);
     //myFSGridLayout->addWidget(wdgISOLevelText,0,0);

     isoLevelFSCombo = new QComboBox();
     isoLevelFSCombo->setToolTip(tr("ISO Level to use."));
     isoLevelFSCombo->setMinimumWidth(140);
     isoLevelFSCombo->setMaximumWidth(140);
     isoFSLEvelFiller->addWidget(isoLevelFSCombo);

     //connect(isoLevelFSCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fsIsoTypeChanged(int)));
     //We use activated in case of a user click, no matter that it is always fired because the value keep the same.
     //But it will not get -1 if Combo is cleared.
     connect(isoLevelFSCombo, SIGNAL(activated(int)), this, SLOT(fsIsoTypeChanged(int)));
     myFSGridLayout->setContentsMargins(3,0,0,0);
     myFSGridLayout->addLayout(isoFSLEvelFiller,0,0);
     isoLowerNamesFSCheck = new QCheckBox(tr("Allow lowercase names"));
     isoLowerNamesFSCheck->setToolTip(tr("Allow to set lowercase file/folder names."));
     isoLowerNamesFSCheck->setCheckState(Qt::Unchecked);
     connect(isoLowerNamesFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsLowerCaseNamesChanged(int)));
     myFSGridLayout->addWidget(isoLowerNamesFSCheck,0,1);
     jolietLongNamesFSCheck = new QCheckBox(tr("Allow long Joliet names"));
     jolietLongNamesFSCheck->setToolTip(tr("Allow to use long Joliet file names."));
     jolietLongNamesFSCheck->setCheckState(Qt::Unchecked);
     connect(jolietLongNamesFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsLongJolietFileNamesChanged(int)));
     myFSGridLayout->addWidget(jolietLongNamesFSCheck,0,2);
     useJolietFSCheck = new QCheckBox(tr("Joliet Extension"));
     useJolietFSCheck->setToolTip(tr("Use the Joliet ISO9660 extension"));
     useJolietFSCheck->setCheckState(Qt::Unchecked);
     connect(useJolietFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsJolietChanged(int)));
     myFSGridLayout->addWidget(useJolietFSCheck,1,0);
     isoManyDirectoriesFSCheck = new QCheckBox(tr("Allow many directories"));
     isoManyDirectoriesFSCheck->setToolTip(tr("Allow to more directories than ISO9660 allow."));
     isoManyDirectoriesFSCheck->setCheckState(Qt::Unchecked);
     connect(isoManyDirectoriesFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsManyDirectories(int)));
     myFSGridLayout->addWidget(isoManyDirectoriesFSCheck,1,1);
     isoExtent1FSCheck = new QCheckBox(tr("Not write ISO ';1' Extension"));
     isoExtent1FSCheck ->setToolTip(tr("The application will not write the ISO ';1' Extension to files."));
     isoExtent1FSCheck ->setCheckState(Qt::Unchecked);
     connect(isoExtent1FSCheck , SIGNAL(stateChanged(int)), this, SLOT(fsNotWriteISO1Extension(int)));
     myFSGridLayout->addWidget(isoExtent1FSCheck,1,2);
     useRockRidgeFSCheck = new QCheckBox(tr("Rock Ridge Extension"));
     useRockRidgeFSCheck->setToolTip(tr("Use the Rock Ridge ISO9660 extension"));
     useRockRidgeFSCheck->setCheckState(Qt::Unchecked);
     connect(useRockRidgeFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsRockRidgeChanged(int)));
     myFSGridLayout->addWidget(useRockRidgeFSCheck,2,0);
     isoLongFileNamesFSCheck = new QCheckBox(tr("Allow long ISO9660 names"));
     isoLongFileNamesFSCheck->setToolTip(tr("Allow to use long file names."));
     isoLongFileNamesFSCheck->setCheckState(Qt::Unchecked);
     connect(isoLongFileNamesFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsLongFileNamesChanged(int)));
     myFSGridLayout->addWidget(isoLongFileNamesFSCheck,2,1);
     myFSGridLayout->setRowMinimumHeight(0,25);
     myFSGridLayout->setRowMinimumHeight(1,25);
     myFSGridLayout->setRowMinimumHeight(2,25);
     myFirstGridLayout->setContentsMargins(3,0,0,0);
     wdgFSFeatues->setLayout(myFSGridLayout);
     wdgFSFeatues->setFixedHeight(80);
     ui->ribbonTabWidget->addWidgetGroup(tr("File System"), tr("ISO9660"), wdgFSFeatues);

     QGridLayout *myFSUDFGridLayout = new QGridLayout(this);
     QWidget * wdgFSUDFFeatues = new QWidget(this);

     QHBoxLayout *udfFSVersionFiller = new QHBoxLayout(this);
     QLabel *wdgUDFVersonText = new QLabel(this);
     wdgUDFVersonText->setText(tr("Version:"));
     udfFSVersionFiller->addWidget(wdgUDFVersonText);
     udfVersionFSCombo = new QComboBox();
     udfVersionFSCombo->setToolTip(tr("UDF VErsion to use on disc."));
     udfVersionFSCombo->setMinimumWidth(140);
     udfVersionFSCombo->setMaximumWidth(140);
     connect(udfVersionFSCombo, SIGNAL(activated(int)), this, SLOT(fsUDFYTpeChanged(int)));
     udfFSVersionFiller->addWidget(udfVersionFSCombo);
     myFSUDFGridLayout->addLayout(udfFSVersionFiller,0,0);

     QHBoxLayout *udfFSPartitionFiller = new QHBoxLayout(this);
     QLabel *wdgUDFPartitionText = new QLabel(this);
     wdgUDFPartitionText->setText(tr("Partition:"));
     udfFSPartitionFiller->addWidget(wdgUDFPartitionText);
     udfPartitionFSCombo = new QComboBox();
     udfPartitionFSCombo->setToolTip(tr("UDF Parition type to use on disc."));
     udfPartitionFSCombo->setMinimumWidth(140);
     udfPartitionFSCombo->setMaximumWidth(140);
     connect(udfPartitionFSCombo, SIGNAL(activated(int)), this, SLOT(fsUDFPartitionChanged(int)));
     udfFSPartitionFiller->addWidget(udfPartitionFSCombo);
     myFSUDFGridLayout->addLayout(udfFSPartitionFiller,1,0);

     udfFileStreamFSCheck = new QCheckBox(tr("Write File Stream"));
     udfFileStreamFSCheck->setToolTip(tr("Write File Stream"));
     udfFileStreamFSCheck->setCheckState(Qt::Unchecked);
     connect(udfFileStreamFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsUDFWriteStreamChanged(int)));
     myFSUDFGridLayout->addWidget(udfFileStreamFSCheck,2,0);
     myFSUDFGridLayout->setContentsMargins(3,0,0,0);
     wdgFSUDFFeatues->setLayout(myFSUDFGridLayout);
     wdgFSUDFFeatues->setFixedHeight(80);
     ui->ribbonTabWidget->addWidgetGroup(tr("File System"), tr("UDF"), wdgFSUDFFeatues);


     QGridLayout *myISOExGridLayout = new QGridLayout(this);
     QWidget * wdgSOExFeatues = new QWidget(this);

     QLabel *wdgIsoExSystemIdText = new QLabel(this);
     wdgIsoExSystemIdText->setText(tr("System ID:"));
     myISOExGridLayout->addWidget(wdgIsoExSystemIdText,0,0);
     edIsoExSystemIdValue = new QLineEdit(this);
     edIsoExSystemIdValue->setMinimumWidth(130);
     edIsoExSystemIdValue->setMaximumWidth(130);
     connect(edIsoExSystemIdValue,SIGNAL(textChanged(QString)),this,SLOT(isoExSystemIdchanged(QString)));
     myISOExGridLayout->addWidget(edIsoExSystemIdValue,0,1);

     QLabel *wdgIsoExVolumeSetText = new QLabel(this);
     wdgIsoExVolumeSetText->setText(tr("Volume Set:"));
     myISOExGridLayout->addWidget(wdgIsoExVolumeSetText,0,2);
     edIsoExVolumeSetValue = new QLineEdit(this);
     edIsoExVolumeSetValue->setMinimumWidth(130);
     edIsoExVolumeSetValue->setMaximumWidth(130);
     connect(edIsoExVolumeSetValue,SIGNAL(textChanged(QString)),this,SLOT(isoExVolumechanged(QString)));
     myISOExGridLayout->addWidget(edIsoExVolumeSetValue,0,3);

     QLabel *wdgIsoExPublisherText = new QLabel(this);
     wdgIsoExPublisherText->setText(tr("Publisher:"));
     myISOExGridLayout->addWidget(wdgIsoExPublisherText,0,4);
     edIsoExPublisherValue = new QLineEdit(this);
     edIsoExPublisherValue->setMinimumWidth(130);
     edIsoExPublisherValue->setMaximumWidth(130);
     connect(edIsoExPublisherValue,SIGNAL(textChanged(QString)),this,SLOT(isoExPublisherchanged(QString)));
     myISOExGridLayout->addWidget(edIsoExPublisherValue,0,5);

     QLabel *wdgIsoExDataPreparerText = new QLabel(this);
     wdgIsoExDataPreparerText->setText(tr("Data Preparer:"));
     myISOExGridLayout->addWidget(wdgIsoExDataPreparerText,1,0);
     edIsoExDataPreparerValue = new QLineEdit(this);
     edIsoExDataPreparerValue->setMinimumWidth(130);
     edIsoExDataPreparerValue->setMaximumWidth(130);
     connect(edIsoExDataPreparerValue,SIGNAL(textChanged(QString)),this,SLOT(isoExDataPreparerchanged(QString)));
     myISOExGridLayout->addWidget(edIsoExDataPreparerValue,1,1);

     QLabel *wdgIsoExApplicationText = new QLabel(this);
     wdgIsoExApplicationText->setText(tr("Application:"));
     myISOExGridLayout->addWidget(wdgIsoExApplicationText,1,2);
     edIsoExApplicationValue = new QLineEdit(this);
     edIsoExApplicationValue->setMinimumWidth(130);
     edIsoExApplicationValue->setMaximumWidth(130);
     connect(edIsoExApplicationValue,SIGNAL(textChanged(QString)),this,SLOT(isoExApplicationchanged(QString)));
     myISOExGridLayout->addWidget(edIsoExApplicationValue,1,3);

     QLabel *wdgIsoExCopyrightFileText = new QLabel(this);
     wdgIsoExCopyrightFileText->setText(tr("Copyright File:"));
     myISOExGridLayout->addWidget(wdgIsoExCopyrightFileText,1,4);
     edIsoExCopyrightFileValue = new QLineEdit(this);
     edIsoExCopyrightFileValue->setMinimumWidth(130);
     edIsoExCopyrightFileValue->setMaximumWidth(130);
     connect(edIsoExCopyrightFileValue,SIGNAL(textChanged(QString)),this,SLOT(isoExCopyrightFilechanged(QString)));
     myISOExGridLayout->addWidget(edIsoExCopyrightFileValue,1,5);

     QLabel *wdgIsoExAbstractFileText = new QLabel(this);
     wdgIsoExAbstractFileText->setText(tr("Abstract File:"));
     myISOExGridLayout->addWidget(wdgIsoExAbstractFileText,2,0);
     edIsoExAbstractFileValue = new QLineEdit(this);
     edIsoExAbstractFileValue->setMinimumWidth(130);
     edIsoExAbstractFileValue->setMaximumWidth(130);
     connect(edIsoExAbstractFileValue,SIGNAL(textChanged(QString)),this,SLOT(isoExAbstractFilechanged(QString)));
     myISOExGridLayout->addWidget(edIsoExAbstractFileValue,2,1);

     QLabel *wdgIsoExBibliographicFileText = new QLabel(this);
     wdgIsoExBibliographicFileText->setText(tr("Bibliographic File:"));
     myISOExGridLayout->addWidget(wdgIsoExBibliographicFileText,2,2);
     edIsoExBibliographicFileValue = new QLineEdit(this);
     edIsoExBibliographicFileValue->setMinimumWidth(130);
     edIsoExBibliographicFileValue->setMaximumWidth(130);
     connect(edIsoExBibliographicFileValue,SIGNAL(textChanged(QString)),this,SLOT(isoExBibliographicFilechanged(QString)));
     myISOExGridLayout->addWidget(edIsoExBibliographicFileValue,2,3);

     myISOExGridLayout->setContentsMargins(3,0,0,0);
     myISOExGridLayout->setRowMinimumHeight(0,25);
     myISOExGridLayout->setRowMinimumHeight(1,25);
     myISOExGridLayout->setRowMinimumHeight(2,25);
     wdgSOExFeatues->setLayout(myISOExGridLayout);
     wdgSOExFeatues->setFixedHeight(80);
     ui->ribbonTabWidget->addWidgetGroup(tr("ISO Extended"), tr("ISO Extended"), wdgSOExFeatues);

     importIsoExButton = new QToolButton;
     importIsoExButton->setText(tr("Import"));
     importIsoExButton->setToolTip(tr("Import IsoEx from software settings"));
     importIsoExButton->setIcon(QIcon(":/icons/extiso_import.png"));
     importIsoExButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("ISO Extended"), tr("Data"), importIsoExButton,nullptr);
     connect(importIsoExButton, SIGNAL(clicked()), this, SLOT(isoExImportClicked()));

     resetIsoExButton = new QToolButton;
     resetIsoExButton->setText(tr("Reset"));
     resetIsoExButton->setToolTip(tr("Reset all IsoEx fields"));
     resetIsoExButton->setIcon(QIcon(":/icons/extiso_reset.png"));
     resetIsoExButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("ISO Extended"), tr("Data"), resetIsoExButton,nullptr);
     connect(resetIsoExButton, SIGNAL(clicked()), this, SLOT(isoExReset()));


     selBootImageButton = new QToolButton;
     selBootImageButton->setText(tr("Set"));
     selBootImageButton->setToolTip(tr("If the BootImage is set, Doiskbutler will create a bootable disk."));
     selBootImageButton->setIcon(QIcon(":/icons/bootimage.png"));
     selBootImageButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Boot Disc"), tr("Boot Image"), selBootImageButton,nullptr);
     connect(selBootImageButton, SIGNAL(clicked()),this, SLOT(clickedSelBootImagePath()));

     QVBoxLayout *bootVerticalLayout = new QVBoxLayout;
     ui->ribbonTabWidget->addVerticalLayout(tr("Boot Disc"), tr("Boot Image"), bootVerticalLayout);

     delBootImageButton = new QToolButton;
     delBootImageButton->setText(tr("Remove"));
     delBootImageButton->setToolTip(tr("Remove current boot image"));
     delBootImageButton->setIcon(QIcon(":/icons/delete.png"));
     delBootImageButton->setEnabled(true);
     ui->ribbonTabWidget->addButton(tr("Boot Disc"), tr("Boot Image"), delBootImageButton, bootVerticalLayout);
     connect(delBootImageButton, SIGNAL(clicked()), this, SLOT(deleteBootImage()));
     bootVerticalLayout->addStretch();
     //Test, erst die widgets erstellen dann ins Grid packen.

     imagePathBootEdit = new QLineEdit();

     //line 1
     QLabel *labelDeveloperID = new QLabel();
     labelDeveloperID->setText(tr("Developer ID:"));

     QLabel *labelBootImage = new QLabel();
     labelBootImage->setText(tr("Boot Image:"));

     developerIDBootEdit = new QLineEdit();
     developerIDBootEdit->setMinimumWidth(100);
     developerIDBootEdit->setMaximumWidth(100);
     connect(developerIDBootEdit,SIGNAL(textChanged(QString)),this,SLOT(bootDeveloperIDchanged(QString)));

     QLabel *labelSectos = new QLabel();
     labelSectos->setText(tr("Sectors:"));

     sectorsBootEdit = new QLineEdit();
     sectorsBootEdit->setMinimumWidth(80);
     sectorsBootEdit->setMaximumWidth(80);
     connect(sectorsBootEdit,SIGNAL(textChanged(QString)),this,SLOT(bootSectorschanged(QString)));

     QLabel *labelLoadSegment = new QLabel();
     labelLoadSegment->setText(tr("Load segment:"));

     loadSegmentBootEdit = new QLineEdit();
     loadSegmentBootEdit->setMinimumWidth(80);
     loadSegmentBootEdit->setMaximumWidth(80);
     connect(loadSegmentBootEdit,SIGNAL(textChanged(QString)),this,SLOT(bootLoadSegmentchanged(QString)));

     QLabel *labelEmulation = new QLabel();
     labelEmulation->setText(tr("Emulation Type:"));

     emulationTypeBootCombo = new QComboBox();
     emulationTypeBootCombo->setMinimumWidth(100);
     emulationTypeBootCombo->setMaximumWidth(100);
     emulationTypeBootCombo->addItem(tr("No emulaton (NT/2000/XP)"));
     emulationTypeBootCombo->addItem(tr("Floppy 1.2MB"));
     emulationTypeBootCombo->addItem(tr("Floppy 1.44MB (95/98/ME)"));
     emulationTypeBootCombo->addItem(tr("Floppy 2.88MB"));
     emulationTypeBootCombo->addItem(tr("Hard Disk"));
     connect(emulationTypeBootCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(bootEmulationTypeChanged(int)));

     QLabel *labelPlatfom = new QLabel();
     labelPlatfom->setText(tr("Platform:"));

     platformBootCombo = new QComboBox();
     platformBootCombo->setMinimumWidth(100);
     platformBootCombo->setMaximumWidth(100);
     platformBootCombo->addItem(tr("x86 - 32"));
     platformBootCombo->addItem(tr("x86 - 64"));
     platformBootCombo->addItem(tr("PowerPC"));
     platformBootCombo->addItem(tr("MAC"));
     connect(platformBootCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(bootPlatformChanged(int)));

     QLabel *labelBootIsoLevel = new QLabel();
     labelBootIsoLevel ->setText(tr("ISO Level:"));

     bootISOLevelCombo = new QComboBox();
     bootISOLevelCombo->setToolTip(tr("ISO Level to use."));
     bootISOLevelCombo->setMinimumWidth(140);
     bootISOLevelCombo->setMaximumWidth(140);
     connect(bootISOLevelCombo, SIGNAL(activated(int)), this, SLOT(fsIsoTypeChanged(int)));

     QGridLayout *myBootGridLayout = new QGridLayout();
     myBootGridLayout->setContentsMargins(3,0,0,0);
     myBootGridLayout->setSpacing(5);
     myBootGridLayout->setMargin(0);
     myBootGridLayout->setRowMinimumHeight(0,25);
     myBootGridLayout->setRowMinimumHeight(1,25);
     myBootGridLayout->setRowMinimumHeight(2,25);

     myBootGridLayout->addWidget(labelBootImage,0,0);
     myBootGridLayout->addWidget(imagePathBootEdit,0,1,1,5);

     myBootGridLayout->addWidget(labelEmulation,1,0);
     myBootGridLayout->addWidget(emulationTypeBootCombo,1,1);
     myBootGridLayout->addWidget(labelPlatfom,1,2);
     myBootGridLayout->addWidget(platformBootCombo,1,3);
     myBootGridLayout->addWidget(labelBootIsoLevel,1,4);
     myBootGridLayout->addWidget(bootISOLevelCombo,1,5);



     myBootGridLayout->addWidget(labelSectos,2,0);
     myBootGridLayout->addWidget(sectorsBootEdit,2,1);
     myBootGridLayout->addWidget(labelLoadSegment,2,2);
     myBootGridLayout->addWidget(loadSegmentBootEdit,2,3);
     myBootGridLayout->addWidget(labelDeveloperID,2,4);
     myBootGridLayout->addWidget(developerIDBootEdit,2,5);


     QWidget * wdgBootFeatues = new QWidget();

     //wdgBootFeatues->setLayout(myBootGridLayout);
     wdgBootFeatues->setLayout(myBootGridLayout);
     wdgBootFeatues->setFixedHeight(80);

     ui->ribbonTabWidget->addWidgetGroup(tr("Boot Disc"), tr("El-Torito"), wdgBootFeatues);

     //ISO Extended
     //Group Selection
     //Track & Session

     //SubWindowActivated ist die MEssage aus der MDI Area. Das hessit wenn ein SubWindow den focus bekommt.


     connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
             this, SLOT(updateProjectRibbon()));


     connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
             this, SLOT(on_active_child_changed()));



     createOwnMenuBar();

     createStatusBar();
     updateProjectRibbon();
     insertCustomRibbonTabs();

     //temporary fix for initilaize Tabs
     ui->ribbonTabWidget->hideTab(tr("File System"));
     ui->ribbonTabWidget->hideTab(tr("ISO Extended"));
     ui->ribbonTabWidget->hideTab(tr("Boot Disc"));
     ui->ribbonTabWidget->hideTab(tr("General"));
     ui->ribbonTabWidget->setTabIndex(0);

}



void MainWindow::createStatusBar()
{

  /*
   * Ich möchte gerne links Info Text und recht Status text, dzwischen einen Spacer
   * Horizontal Layout
   * Label
   * Spacer
   * Label
   * Und los
   */

  QHBoxLayout *statusbarLayout = new QHBoxLayout(this);
  QWidget * statusWidget = new QWidget(this);

  mStatusInfoLabel = new QLabel;
  mStatusInfoLabel->setText(tr("..."));
  mStatusInfoLabel->setAlignment(Qt::AlignLeft);
  mStatusInfoLabel->setSizePolicy(QSizePolicy::Expanding,
                       QSizePolicy::Expanding);

  mStatusTextLabel = new QLabel;
  mStatusTextLabel->setText(tr("Welcome to Diskbutler"));
  mStatusTextLabel->setSizePolicy(QSizePolicy::Expanding,
                       QSizePolicy::Expanding);

  QSpacerItem *item = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed);

  statusbarLayout->addWidget(mStatusInfoLabel);
  statusbarLayout->addSpacerItem(item);
  statusbarLayout->addWidget(mStatusTextLabel);


  statusbarLayout->setContentsMargins(0,0,0,0);
  statusbarLayout->setSpacing(0);
  statusbarLayout->setMargin(0);

  statusWidget->setLayout(statusbarLayout);

  statusBar()->addPermanentWidget(statusWidget,1);

}

void MainWindow::statusbar_changed(const QString &text)
{

    QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow();
    if ("" == text && activeSubWindow != nullptr) {
        MdiChildBase * child = qobject_cast<MdiChildBase *>(activeSubWindow->widget());
        //statusBar()->showMessage(child->updateStatus());
        updateStatusBarText(child->updateStatus());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

MdiChildDialog *MainWindow::createMdiChild(const RuleManager::ProjectType project_type)
{
    MdiChildDialog *child = new MdiChildDialog(project_type);
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(datatrack_changed()),
        this, SLOT(updateProjectRibbon()));

    connect(child, SIGNAL(audiotrack_changed(bool)),
        this, SLOT(updateProjectRibbon()));

    connect(child, SIGNAL(status_changed(const QString&)),
           this, SLOT(statusbar_changed(const QString&)));

    return child;
}

MdiChildDialog *MainWindow::createMdiChild()
{
    MdiChildDialog *child = new MdiChildDialog();
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(datatrack_changed()),
            this, SLOT(updateProjectRibbon()));

    connect(child, SIGNAL(audiotrack_changed(bool)),
        this, SLOT(updateProjectRibbon()));

    connect(child, SIGNAL(status_changed(const QString&)),
           this, SLOT(statusbar_changed(const QString&)));

  return child;
}

void MainWindow::newProject(int type)
{



    RuleManager::ProjectType projectType = static_cast<RuleManager::ProjectType>(type);
    QString str = tr("Project Type: ");
    MdiChildDialog *child = createMdiChild(projectType);
    //child->SetProjectType(projectType);
    child->newFile();
    //child->show();
    //child->showMaximized();
    //child->showNormal();

    if(isCascaded()==false){
        child->showMaximized();
    }else{
        child->showNormal();
    }

    str += RuleManager::GetProjectTypeStr(projectType);

    //str += "  Support: ";
    //str += RuleManager::GetOptionsStr(projectType);
    //statusBar()->showMessage(str);
    //updateStatusBarText(str);
    updateStatusBarText(child->updateStatus());

    //Switch to Tab Edit to start direktly with editing projekt.
    //Maybe we will do this editable.
    ui->ribbonTabWidget->currentTab(tr("Edit"));
    setDefaults();

    /*
    QMessageBox::information(this, tr("Information"),
                             tr("Button click!"));
                             */
}

void MainWindow::updateStatusBarText(const QString &text)
{
    mStatusTextLabel->setText(text);
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName, bool isFile)
{
    QString canonicalFilePath;
    if (isFile)
        canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    else
        canonicalFilePath = fileName;

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChildDialog *mdiChild = qobject_cast<MdiChildDialog *>(window->widget());
        if (nullptr != mdiChild){
            if (mdiChild->currentFile() == canonicalFilePath)
                return window;
        }
    }
    return nullptr;
}

MdiChildDialog *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow()){
        return qobject_cast<MdiChildDialog *>(activeSubWindow->widget());
    }
    return nullptr;
}

MdiChildDiskInfo *MainWindow::activeMdiInfoChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow()){
        return qobject_cast<MdiChildDiskInfo *>(activeSubWindow->widget());
    }
    return nullptr;
}

MdiChildDeviceInfo *MainWindow::activeMdiDeviceChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow()){
        return qobject_cast<MdiChildDeviceInfo *>(activeSubWindow->widget());
    }
    return nullptr;
}


MdiChildHex *MainWindow::activeMdiHexChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow()){
        return qobject_cast<MdiChildHex *>(activeSubWindow->widget());
    }
    return nullptr;
}

MdiChildScan *MainWindow::activeMdiScanChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow()){
        return qobject_cast<MdiChildScan *>(activeSubWindow->widget());
    }
    return nullptr;
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::on_active_child_changed()
{
    //First we check if there is a window available.
    QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow();
    if (activeSubWindow != nullptr) {
        //A Subwindow is available
        if(currentSubWindow!=activeSubWindow){
            //The window is not the the same as before. When this will happen ever inside this event?
            currentSubWindow = activeSubWindow;

            //Now we need to check the type of window.
            MdiChildBase * child = qobject_cast<MdiChildBase *>(activeSubWindow->widget());
            //We need to update the statusbar with DiskInfo and DeviceInfo
            updateStatusBarText(child->updateStatus());
            RuleManager::ProjectType childType = child->GetProjectType();

            if(childType == RuleManager::TYPE_PROJECT_DISKINFO || childType == RuleManager::TYPE_PROJECT_DEVICEINFO){
                getListWidgetInfo();
            }else{
                getListWidget();
            }
        }
    }

    insertCustomRibbonTabs();
    updateProjectRibbon();

}

void MainWindow::toggleFileExplorer()
{
    if (activeMdiChild() == nullptr)
        return;
    activeMdiChild()->toggleFileExplorer();
}

void MainWindow::showFullScreenMode()
{
    showFullScreen();
}

void MainWindow::mdiNextWindow()
{
    mdiArea->activateNextSubWindow();
}





void MainWindow::updateWindowSwitchMenu()
{
    m_windowMenu->clear();
    QList<QMdiSubWindow*> windows = mdiArea->subWindowList();

    for (int i = 0; i < windows.size(); ++i)
    {
        QWidget* child = windows.at(i)->widget();
        if(!child)
            continue;

        QString text;
        if (i < 9)
            text = tr("&%1 %2").arg(i + 1).arg(child->windowTitle());
        else
            text = tr("%1 %2").arg(i + 1).arg(child->windowTitle());

        QAction* action  = m_windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, &QAction::triggered, [=] { setActiveSubWindow(windows.at(i)); });
    }

}

void MainWindow::selectAll()
{
    activeMdiChild()->selectAll();
}

void MainWindow::reverseSelection()
{
    activeMdiChild()->reverseSelection();
}

void MainWindow::addNode()
{
    activeMdiChild()->addNewFolder();
    //updateProjectRibbon();
}

void MainWindow::insertNode()
{
    //  static int folderNumber = 1;
    //  QString name = tr("Folder%1").arg(folderNumber++);
    MdiChildDialog *mdi_child = activeMdiChild();
    if (nullptr == mdi_child)
        return;
    QFileDialog *dialog = new QFileDialog();
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
    //QString name = dialog->getOpenFileName(this);
    QString name = dialog->getExistingDirectory(this);
    delete dialog;
    if (name != "") {
        mdi_child->insertNode(name);
        //updateProjectRibbon();
        //activeMdiChild()->addFolder(name);
    }

}

void MainWindow::insertItem()
{
    //  static int itemNumber = 1;
    //  QString name = tr("Item%1").arg(itemNumber++);
    MdiChildDialog *mdi_child = activeMdiChild();
    if (nullptr == mdi_child)
        return;

    mdi_child->insertItem();
    //updateProjectRibbon();
}

void MainWindow::resetFiles()
{
    activeMdiChild()->triggerReset();
    //);
}

void MainWindow::updateProject()
{
    activeMdiChild()->updateProject();
}

void MainWindow::renameItemEx()
{
    if (activeMdiChild() == nullptr)
        return;
    activeMdiChild()->renameItem();
}

void MainWindow::deleteItem()
{
    if (activeMdiChild() == nullptr)
        return;
    activeMdiChild()->deleteItem();
}

bool MainWindow::isAudioTrack()
{
    if(activeMdiChild() == nullptr) return false;

    QDataItem *tempItem = activeMdiChild()->GetSelectedTreeItem();

    if(tempItem){

        if(tempItem->GetType()==QDataItem::Session || tempItem->GetType()==QDataItem::Disk || tempItem->GetType()==QDataItem::DataTrack)
        return false;

        QTreeWidgetItem* parentItem = tempItem->parent();
        if(tempItem->GetType()==QDataItem::AudioTrack || ((QDataItem*)parentItem)->GetType()==QDataItem::AudioTrack)
        return true;

    }

    return false;
}

void MainWindow::addDataTrack()
{
    activeMdiChild()->addDataTrack();
}

void MainWindow::addAudioTrack()
{
    activeMdiChild()->addAudioTrack();
}

void MainWindow::fillSourceDriveList()
{
    for (int i = 0; i < mDiskList.count(); i++) {
        burnDeviceGeneralCombo->addItem(mDiskList[i]);
        if(isBurnDevice(mDiskList[i])){
            burnDeviceGeneralCombo->setItemIcon(i,QIcon(":/icons/burndrive.png"));
        }else{
            burnDeviceGeneralCombo->setItemIcon(i,QIcon(":/icons/readdevice.png"));
        }
    }
}

void MainWindow::burnDriveChanged(int index)
{

    //if(isComboBurnDriveUpdate==true) return;
    //FIrst we save the new drive to the software settings
    //Then we send this "new drive" action to the MDI child windows.
    QString strDriveName = burnDeviceGeneralCombo->currentText();
    ConfigurationPage::mSettings.setValue("CurrentBurnDevice", strDriveName);

    //If project we need to send to project and return
    if(activeMdiChild() != nullptr){
        QDiskItem *diskItem = (QDiskItem *)activeMdiChild()->getTreeWidget()->topLevelItem(0);
        diskItem->setBurnDevice(strDriveName);
        return;
    }

    //Extract first letter

    int32 nError = ::SetBurnDevice(strDriveName.at(0).toLatin1());
    if(nError!=0){
        //What happen when SetBurnDevice report Error?
        QMessageBox::information(this, tr("Information"),tr("Error set Burn Device"));
        burnDeviceGeneralCombo->setCurrentIndex(0);
    }

    /*
    speedGeneralCombo->clear();
    //SpeedBox
    QString tmp;
    int32 nSize=20;
    SSpeed Speeds[20];
    ::GetPossibleBurnSpeeds(BS_CURRENT_DEVICE, Speeds, &nSize);
    speedGeneralCombo->addItem("max.");
    for(int i=0;i<nSize;++i)
    {
        QString tspeed = QString("%1 (%2 kb/s)").arg(QString::number(Speeds[i].fSpeed),QString::number(Speeds[i].nSpeedInKBPerSec));
        speedGeneralCombo->addItem(tspeed);

    }
    speedGeneralCombo->setCurrentIndex(0);
    */

}

void MainWindow::fillReadDriveList()
{

    mBurnDriveList.clear();
    ::RescanDevices();
    TCHAR chListAllDevices[26][50];
    int32 nUsed = 0;
    int32 res = ::GetDevices(chListAllDevices, &nUsed, false);
    if (BS_SDK_ERROR_NO == res) {
      for (int i = 0; i < nUsed; i++) {
    #if defined (WIN32)
          mBurnDriveList.append(QString::fromUtf16((const ushort*)chListAllDevices[i]));
    #else
          mBurnDriveList.append(QString::fromUtf8(chListAllDevices[i]));
    #endif

      }
    }

    listReadDevicesWidget->clear();

    for (int i = 0; i < mBurnDriveList.count(); i++) {

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(mBurnDriveList[i]);

        if(isBurnDevice(mBurnDriveList[i])){
             item->setIcon(QIcon(":/icons/burndrive.png"));
        }else{
             item->setIcon(QIcon(":/icons/readdevice.png"));
        }

        listReadDevicesWidget->addItem(item);
    }

    listReadDevicesWidget->setCurrentRow(0);

}

void MainWindow::fillBurnDriveList()
{

    mBurnDriveList.clear();
    ::RescanDevices();
    TCHAR chListAllDevices[26][50];
    int32 nUsed = 0;
    int32 res = ::GetDevices(chListAllDevices, &nUsed, false);
    if (BS_SDK_ERROR_NO == res) {
      for (int i = 0; i < nUsed; i++) {
    #if defined (WIN32)
          mBurnDriveList.append(QString::fromUtf16((const ushort*)chListAllDevices[i]));
    #else
          mBurnDriveList.append(QString::fromUtf8(chListAllDevices[i]));
    #endif

      }
    }

    listBurnDevicesWidget->clear();

    for (int i = 0; i < mBurnDriveList.count(); i++) {

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(mBurnDriveList[i]);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

        item->setCheckState(Qt::Unchecked);

        if(isBurnDevice(mBurnDriveList[i])){
             item->setIcon(QIcon(":/icons/burndrive.png"));
        }else{
             item->setIcon(QIcon(":/icons/readdevice.png"));
        }

        listBurnDevicesWidget->addItem(item);
    }

    //Set Burning Device(es) from project.
    //GetBurnDevices count = 0, den ersten selectieren
}

void MainWindow::updateReadDeviceSel(QListWidgetItem *current)
{
    //Currently there is no dynamic drive change needed.
    return;
}

void MainWindow::updateBurnDeviceSel(QListWidgetItem *current)
{
    setListWidget();
}

void MainWindow::setDefaults()
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){

        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        if(diskItem->getBurnDevices().count()==0){
            if(listBurnDevicesWidget->count()>0){
                listBurnDevicesWidget->item(0)->setCheckState(Qt::Checked);
            }
        }

        setListWidget();
    }
}

//SetListWidget will be called by changeing the item inside the listview.

void MainWindow::setListWidget()
{
    //We will fill the burndevice list of diskitem.
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){

        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));

        diskItem->clearDrives();
        for(int i = 0; i < listBurnDevicesWidget->count(); ++i)
        {
            QListWidgetItem* item = listBurnDevicesWidget->item(i);
            if(item->checkState()==Qt::Checked){
                diskItem->appendDrives(item->text());
            }
        }
    }

}

void MainWindow::getListWidgetInfo()
{
    //WEnn diese Funktion auftrauch, ist klar das es info ist.
    //Dann müssen wir eiegntlich nur das child window emulieren.
    QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow();
    MdiChildBase * child = qobject_cast<MdiChildBase *>(activeSubWindow->widget());

    QString readDevices = child->getBurnDrive();

    for(int i = 0; i < listReadDevicesWidget->count(); ++i)
    {

        if(listReadDevicesWidget->item(i)->text()==readDevices){
            listReadDevicesWidget->setCurrentRow(i);
            return;
        }
    }
}

//GEtListWidget will be called by changeing the activemdichild.

void MainWindow::getListWidget()
{
    //We will fill the burndevice list of diskitem.
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));

        for(int i = 0; i < listBurnDevicesWidget->count(); ++i)
        {
            bool checked = false;
            for (int y = 0; y < diskItem->getBurnDevices().count(); y++) {

                //qDebug("List: %d %s",i,listBurnDevicesWidget->item(i)->text().toLatin1().constData());
                //qDebug("Disk: %d %s",y,diskItem->getBurnDevices()[y].toLatin1().constData());

                if(listBurnDevicesWidget->item(i)->text()==diskItem->getBurnDevices()[y]){
                    checked = true;
                }
            }
            listBurnDevicesWidget->item(i)->setCheckState(checked==true?Qt::Checked:Qt::Unchecked);
        }


    }
}

void MainWindow::fillDriveList()
{

    mDiskList.clear();
    ::RescanDevices();
    TCHAR chListDevices[26][50];
    int32 nUsed = 0;
    int32 res = ::GetDevices(chListDevices, &nUsed, false);
    if (BS_SDK_ERROR_NO == res) {
      for (int i = 0; i < nUsed; i++) {
    #if defined (WIN32)
          mDiskList.append(QString::fromUtf16((const ushort*)chListDevices[i]));
    #else
          mDiskList.append(QString::fromUtf8(chListDevices[i]));
    #endif

      }
    }

    mBurnDriveList.clear();
    ::RescanDevices();
    TCHAR chListAllDevices[26][50];
    nUsed = 0;
    res = ::GetDevices(chListAllDevices, &nUsed, true);
    if (BS_SDK_ERROR_NO == res) {
      for (int i = 0; i < nUsed; i++) {
    #if defined (WIN32)
          mBurnDriveList.append(QString::fromUtf16((const ushort*)chListAllDevices[i]));
    #else
          mBurnDriveList.append(QString::fromUtf8(chListAllDevices[i]));
    #endif

      }
    }

}

bool MainWindow::isBurnDevice(QString toTest)
{
    for (int i = 0; i < mBurnDriveList.count(); i++) {
        if(mBurnDriveList[i]==toTest) return true;
    }
    return false;
}

void MainWindow::updateDiskList()
{

    //isComboBurnDriveUpdate = true;
    fillDriveList();

    //burnDeviceGeneralCombo->clear();

    fillSourceDriveList();

    //Select
    //isComboBurnDriveUpdate = false;
    //setBurnDeviceFromSettings();
}

bool MainWindow::isCascaded()
{
    //When we query this function, the window is created. So it is 1 not 0
    if(mdiArea->subWindowList().count()==1){
        return false;
    }

    foreach(QMdiSubWindow * window, mdiArea->subWindowList()) {
        if(window->windowState() & Qt::WindowMaximized){
            return false;
        }
    }

    return true;
}

bool MainWindow::hasProjectWindow()
{

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChildDialog *mdiChild = qobject_cast<MdiChildDialog *>(window->widget());
        if (nullptr != mdiChild){
            RuleManager::ProjectType tProject = mdiChild->GetProjectType();
            if(tProject == RuleManager::TYPE_PROJECT_AUDIOCD || tProject == RuleManager::TYPE_PROJECT_MIXEDMODE || tProject == RuleManager::TYPE_PROJECT_OPEN
                    || tProject == RuleManager::TYPE_PROJECT_VIDEODVD || tProject == RuleManager::TYPE_PROJECT_BLURAYVIDEO || tProject == RuleManager::TYPE_PROJECT_UDF
                    || tProject == RuleManager::TYPE_PROJECT_ISO || tProject == RuleManager::TYPE_PROJECT_ISOUDF || tProject == RuleManager::TYPE_PROJECT_BLURAYVIDEO
                    || tProject == RuleManager::TYPE_PROJECT_SUPERVIDEO){
                return true;
            }
        }
    }
    return false;

}

void MainWindow::insertCustomRibbonTabs()
{
    bool hasMdiChild = (activeMdiChild() != nullptr);
    bool hasMdiInfoChild = (activeMdiInfoChild() != nullptr);
    bool hasMdiDeviceChild = (activeMdiDeviceChild() != nullptr);
    bool hasMdiHexChild = (activeMdiHexChild() != nullptr);
    bool hasMdiScanChild = (activeMdiScanChild() != nullptr);

    //This call hinder us to hide all initilaized Tabs, because on statrup this dialog is not finished.
    if(QWidget::isActiveWindow()){

        if(hasMdiChild) // Secure check that a child is selected
        {
            ui->ribbonTabWidget->showTab(":/icons/open-disk.png","General");
            RuleManager::ProjectType tProject = activeMdiChild()->GetProjectType();

            if(RuleManager::TYPE_PROJECT_AUDIOCD==tProject || RuleManager::TYPE_PROJECT_MIXEDMODE==tProject || RuleManager::TYPE_PROJECT_OPEN==tProject){
                //pageAudioCD->setVisible(true);
                //ribbonBar()->setCurrentPage(getPageIndex(pageAudioCD));
                if(RuleManager::TYPE_PROJECT_AUDIOCD==tProject){
                   ui->ribbonTabWidget->hideTab(tr("File System"));
                }
                if(RuleManager::TYPE_PROJECT_MIXEDMODE==tProject || RuleManager::TYPE_PROJECT_OPEN==tProject){
                    ui->ribbonTabWidget->showTab(":/icons/filesystem.png","File System");
                    //insertFileSystemRibbonPage(tProject);
                    //insertDiskRibbonPage(tProject);
                }
            }else{
                ui->ribbonTabWidget->showTab(":/icons/filesystem.png","File System");
                ui->ribbonTabWidget->currentTab(tr("Edit"));
                //insertFileSystemRibbonPage(tProject);
                //insertDiskRibbonPage(tProject);
                updateDiskPage(tProject);
                //ribbonBar()->setCurrentPage(getPageIndex(pageProject));
            }
            if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SPECIALS_ISOEXTINFO)){
                ui->ribbonTabWidget->showTab(":/icons/extiso.png","ISO Extended");
            }else{
                ui->ribbonTabWidget->hideTab(tr("ISO Extended"));
            }
            if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SPECIALS_BootCD_DVD_BD)){
                ui->ribbonTabWidget->showTab(":/icons/boot.png","Boot Disc");
            }else{
                ui->ribbonTabWidget->hideTab(tr("Boot Disc"));
            }
        }else if(hasMdiInfoChild || hasMdiDeviceChild || hasMdiHexChild || hasMdiScanChild){
            ui->ribbonTabWidget->currentTab(tr("Device"));
            if(hasProjectWindow()==false){
                ui->ribbonTabWidget->hideTab(tr("File System"));
                ui->ribbonTabWidget->hideTab(tr("ISO Extended"));
                ui->ribbonTabWidget->hideTab(tr("Boot Disc"));
                ui->ribbonTabWidget->hideTab(tr("General"));
            }
        }else{
            ui->ribbonTabWidget->hideTab(tr("File System"));
            ui->ribbonTabWidget->hideTab(tr("ISO Extended"));
            ui->ribbonTabWidget->hideTab(tr("Boot Disc"));
            ui->ribbonTabWidget->hideTab(tr("General"));
            ui->ribbonTabWidget->setTabIndex(0);
        }

    }
}

void MainWindow::updateDiskPage(RuleManager::ProjectType tProject)
{
    simulateBurnGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_SIMULATE));
    burnProofGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_BURNPROOF));
    ejectAfterGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_EJECT));
    finishDiscGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_FINALIZE));
    verifyAfterGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_VERIFY));
    autoEraseGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_AUTOERASE));
    padDataGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_PADDATATRACKS));
    avchdGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SPECIALS_AVCHD));
    activeOPCGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_OPC));
}


void MainWindow::updateProjectRibbon()
{
    /*
     * Hier müssen wir die Inhalte der Ribbons an das aktuelle Projekt anpassen.
     * Wir müssen die Pointer auf die Elemnete hier gruppieren
     */

    bool hasMdiChild = (activeMdiChild() != nullptr);
    bool hasMdiInfoChild = (activeMdiInfoChild() != nullptr);
    bool hasMdiDeviceChild = (activeMdiDeviceChild() != nullptr);
    bool hasMdiHexChild = (activeMdiHexChild() != nullptr);
    bool hasMdiScanChild = (activeMdiScanChild() != nullptr);

    //if(QWidget::isActiveWindow()){


        //scanMediaButton->setEnabled(hasMdiScanChild);
        //hexMediaButton->setEnabled(hasMdiHexChild);
        imageMediaButton->setEnabled(hasMdiInfoChild);

        viewTileButton->setEnabled(hasMdiChild);
        viewCascadeButton->setEnabled(hasMdiChild);
        viewSwitchButton->setEnabled(hasMdiChild);

        menuViewTile->setEnabled(hasMdiChild);
        menuViewCascade->setEnabled(hasMdiChild);
        menuViewNext->setEnabled(hasMdiChild);

        appSaveButton->setEnabled(hasMdiChild);
        appSaveAsButton->setEnabled(hasMdiChild);
        menuNewSaveProject->setEnabled(hasMdiChild);
        menuNewSaveAsProject->setEnabled(hasMdiChild);

        grabAudioMediaButton->setEnabled(false);

        dataTrackEditButton->setEnabled(activeMdiChild() != nullptr
            && !activeMdiChild()->isDataTrackExist()
            //&& activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
            && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_VIDEODVD
            && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_BLURAYVIDEO
            );
        audioTrackEditButton->setEnabled(activeMdiChild() != nullptr
            && (activeMdiChild()->GetProjectType() == RuleManager::TYPE_PROJECT_OPEN
                || activeMdiChild()->GetProjectType() == RuleManager::TYPE_PROJECT_MIXEDMODE
                || activeMdiChild()->GetProjectType() == RuleManager::TYPE_PROJECT_AUDIOCD));

        addFileEditButton->setEnabled(activeMdiChild() != nullptr
            && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
            );
        menuEditAddFile->setEnabled(activeMdiChild() != nullptr
                && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
                );
        addFolderEditButton->setEnabled(activeMdiChild() != nullptr
            && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
            );
        menuEditAddFolder->setEnabled(activeMdiChild() != nullptr
                && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
                );
        addFolderEditButton->setEnabled(activeMdiChild() != nullptr
            && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_AUDIOCD);
        menuEditAddFolder->setEnabled(activeMdiChild() != nullptr
                && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_AUDIOCD);


        bool hasSelection = (activeMdiChild() && activeMdiChild()->hasSelectedTreeItem());

        renameEditButton->setEnabled(hasSelection
                              && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
            );

        menuEditRenameItem->setEnabled(hasSelection
                                       && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
                     );

        allSelectEditButton->setEnabled(activeMdiChild() != nullptr
                && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
                );

        inverseSelectEditButton->setEnabled(activeMdiChild() != nullptr
                && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
                );

        delAllEditButton->setEnabled(activeMdiChild() != nullptr);
        menuEditDeleteAll->setEnabled(activeMdiChild() != nullptr);

        updtEditButton->setEnabled(activeMdiChild() != nullptr);
        menuEditUpdateProject->setEnabled(activeMdiChild() != nullptr);

        QDataItem *tempItem = nullptr;

        if(activeMdiChild() != nullptr){

            if(activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_DISKINFO
                    && activeMdiChild()->GetProjectType() != RuleManager::TYPE_PROJECT_DEVICEINFO)
            {
                tempItem = activeMdiChild()->GetSelectedTreeItem();
                if(tempItem){
                    if(tempItem->GetType()==QDataItem::Disk || tempItem->GetType()==QDataItem::Session || tempItem->GetType()==QDataItem::DataTrack){
                        delEditButton->setEnabled(false);
                        menuEditDeleteItem->setEnabled(false);
                        renameEditButton->setEnabled(false);
                        menuEditRenameItem->setEnabled(false);
                    }else{
                        delEditButton->setEnabled(hasSelection);
                        menuEditDeleteItem->setEnabled(hasSelection);
                        renameEditButton->setEnabled(hasSelection);
                        menuEditRenameItem->setEnabled(hasSelection);
                    }
                }
            }

            viewBrowserButton->setEnabled(true);
            menuToggleExplorer->setEnabled(true);
            if(activeMdiChild()->GetProjectType() == RuleManager::TYPE_PROJECT_EXPLORE){
               viewBrowserButton->setEnabled(false);
               menuToggleExplorer->setEnabled(false);
            }

            if(activeMdiChild()->GetProjectType() == RuleManager::TYPE_PROJECT_EXPLORE){

                tempItem = activeMdiChild()->GetSelectedTreeItem();

                if(!tempItem)activeMdiChild()->getTreeWidget()->setRootItem();

                tempItem = activeMdiChild()->GetSelectedTreeItem();


            }

            if(activeMdiChild()->GetProjectType() == RuleManager::TYPE_PROJECT_AUDIOCD || activeMdiChild()->GetProjectType() == RuleManager::TYPE_PROJECT_MIXEDMODE || activeMdiChild()->GetProjectType() == RuleManager::TYPE_PROJECT_OPEN)
            {
                int nCounter = (activeMdiChild()->isDataTrackExist()==true)?1:0;
                int nChildCount = activeMdiChild()->getTreeWidget()->getSessionItem()->childCount();

                tempItem = activeMdiChild()->GetSelectedTreeItem();

                int index; // = activeMdiChild()->getTreeWidget()->getSessionItem()->indexOfChild(tempItem);

                if(tempItem){
                    if(nChildCount > nCounter){
                        if(tempItem->GetType()==QDataItem::Disk || tempItem->GetType()==QDataItem::Session || tempItem->GetType()==QDataItem::DataTrack){
                            //groupAudioOrder->setEnabled(false);
                        }else{
                            //groupAudioOrder->setEnabled(true);
                        }
                    }else{
                        //groupAudioOrder->setEnabled(false);
                    }
                    //Why not show CDText dialog also if the audio item is selectd? It is also a track? Isn't it.
                    //(QDataItem*)(tempItem->parent())->GetType()
                    if(tempItem->GetType()==QDataItem::Disk || isAudioTrack()==true){
                        //actionCDText->setEnabled(true);
                    }else{
                        //actionCDText->setEnabled(false);
                    }
                    if(isAudioTrack()==true){
                        QTreeWidgetItem* parentItem = tempItem->parent();
                        if(parentItem){
                            if(((QDataItem*)parentItem)->GetType()==QDataItem::AudioTrack){
                                //actionAudioIndexies->setEnabled(true);
                            }
                        }

                        if(tempItem->GetType()==QDataItem::AudioTrack){
                            if(tempItem->childCount()==1){
                                //actionCDText->setEnabled(true);
                                //actionAudioIndexies->setEnabled(true);
                            }else{
                                //actionCDText->setEnabled(false);
                                //actionAudioIndexies->setEnabled(false);
                            }
                        }

                        if(tempItem->GetType()==QDataItem::AudioTrack){
                            index = activeMdiChild()->getTreeWidget()->getSessionItem()->indexOfChild(tempItem);
                        }else{
                            QTreeWidgetItem* parentItem = tempItem->parent();
                            index = activeMdiChild()->getTreeWidget()->getSessionItem()->indexOfChild(parentItem);
                        }

                        if ((activeMdiChild()->isDataTrackExist() && index == 1) || (!activeMdiChild()->isDataTrackExist() && index == 0)) {
                          //actionAudioUp->setEnabled(false);
                        } else {
                          //actionAudioUp->setEnabled(true);
                        }
                        if (nChildCount-1 == index) {
                          //actionAudioDown->setEnabled(false);
                        } else {
                          //actionAudioDown->setEnabled(true);
                        }
                    }else{
                        //actionAudioIndexies->setEnabled(false);
                    }
                }else{
                    //actionCDText->setEnabled(false);
                    //actionAudioIndexies->setEnabled(false);
                    //actionAudioDown->setEnabled(false);
                    //actionAudioUp->setEnabled(false);
                }

            }

            QDiskItem *diskItem = (QDiskItem *)activeMdiChild()->getTreeWidget()->topLevelItem(0);
            //Wir fangen an die Sachen zu lesen.
            RuleManager::ProjectType tProject = activeMdiChild()->GetProjectType();
            if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SPECIALS_BootCD_DVD_BD)){
                //Has Boot
                if(diskItem->getDoBootDisk()==true){
                    ui->ribbonTabWidget->disableButtonGroup(tr("Boot Disc"), tr("El-Torito"), true);
                    delBootImageButton->setEnabled(true);
                }else{
                    ui->ribbonTabWidget->disableButtonGroup(tr("Boot Disc"), tr("El-Torito"), false);
                    delBootImageButton->setEnabled(false);
                }
                    //void setDoBootDisk(bool value) {doBootDisk=value;}
                    //void setBootDiskFile(QString str) {mBootDiskFile=str;}
                //updateBootDisk(diskItem->getDoBootDisk());
                //enableBootDisk->setChecked(diskItem->getDoBootDisk());
                imagePathBootEdit->setText(diskItem->getBootDiskFile());
                emulationTypeBootCombo->setCurrentIndex(diskItem->getBootEmulationType());
                platformBootCombo->setCurrentIndex(diskItem->getBootPlatformID());
                developerIDBootEdit->setText(diskItem->getBootDeveloperID());
                sectorsBootEdit->setText(diskItem->getBootSectors());
                bootISOLevelCombo->setCurrentIndex(diskItem->getISOFsType());
                loadSegmentBootEdit->setText(diskItem->getBootLoadSegment());
            }

            if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SPECIALS_ISOEXTINFO)){
                edIsoExSystemIdValue->setText(diskItem->getSystemId());
                edIsoExVolumeSetValue->setText(diskItem->getVolumeSet());
                edIsoExPublisherValue->setText(diskItem->getPublisher());
                edIsoExDataPreparerValue->setText(diskItem->getDatapreparer());
                edIsoExApplicationValue->setText(diskItem->getApplication());
                edIsoExCopyrightFileValue->setText(diskItem->getCoprightFile());
                edIsoExAbstractFileValue->setText(diskItem->getAbstractFile());
                edIsoExBibliographicFileValue->setText(diskItem->getBibliographicFile());
            }


            if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_UDF))
            {
                ui->ribbonTabWidget->disableButtonGroup(tr("File System"), tr("UDF"), true);
                udfVersionFSCombo->clear();
                if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_UDF102)) udfVersionFSCombo->addItem(tr("UDF 1.02"));
                if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_UDF15)) udfVersionFSCombo->addItem(tr("UDF 1.5"));
                if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_UDF20)) udfVersionFSCombo->addItem(tr("UDF 2.0"));
                if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_UDF201)) udfVersionFSCombo->addItem(tr("UDF 2.01"));
                if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_UDF25)) udfVersionFSCombo->addItem(tr("UDF 2.5"));
                if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_UDF26)) udfVersionFSCombo->addItem(tr("UDF 2.6"));

                udfPartitionFSCombo->clear();
                if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_UDF_PARTITION))
                {
                    udfPartitionFSCombo->setEnabled(true);
                    udfPartitionFSCombo->addItem(tr("Physical"));
                    udfPartitionFSCombo->addItem(tr("Virtual"));
                    udfPartitionFSCombo->addItem(tr("Sparable"));
                }else{
                    udfPartitionFSCombo->setEnabled(false);
                }

                udfFileStreamFSCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_UDF_WRITE_FILE_STREAM));

            }else{
                ui->ribbonTabWidget->disableButtonGroup(tr("File System"), tr("UDF"), false);
            }


            if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_ISO9660))
            {
                ui->ribbonTabWidget->disableButtonGroup(tr("File System"), tr("ISO9660"), true);

                if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SPECIALS_BootCD_DVD_BD)){
                    bootISOLevelCombo->clear();
                    if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL1)) bootISOLevelCombo->addItem(tr("ISO Level 1"));
                    if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL2)) bootISOLevelCombo->addItem(tr("ISO Level 2"));
                    if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL3)) bootISOLevelCombo->addItem(tr("ISO Level 3"));
                    if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ROMEO)) bootISOLevelCombo->addItem(tr("Romeo"));

                }

               isoLevelFSCombo->clear();
               if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL1)) isoLevelFSCombo->addItem(tr("ISO Level 1"));
               if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL2)) isoLevelFSCombo->addItem(tr("ISO Level 2"));
               if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL3)) isoLevelFSCombo->addItem(tr("ISO Level 3"));
               if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ROMEO)) isoLevelFSCombo->addItem(tr("Romeo"));

               isoLongFileNamesFSCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ALLOW_LONGISO_FILENAMES));
               isoManyDirectoriesFSCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ALLOW_MANY_DIRECTORIES));
               isoLowerNamesFSCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ALLOW_LOWERCASE_FILENAMES));
               useJolietFSCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_JOLIET));
               useRockRidgeFSCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMEXT_ROCK_RIDGE));
            } else{

                ui->ribbonTabWidget->disableButtonGroup(tr("File System"), tr("ISO9660"), false);
            }

            isoLevelFSCombo->setCurrentIndex(diskItem->getISOFsType());
            bootISOLevelCombo->setCurrentIndex(diskItem->getISOFsType());

            useJolietFSCheck->setChecked(diskItem->getUseJoliet());
            useRockRidgeFSCheck->setChecked(diskItem->getUseRockRidge());
            isoLowerNamesFSCheck->setChecked(diskItem->getAllowLowerCaseNames());
            isoManyDirectoriesFSCheck->setChecked(diskItem->getAllowManyDirectories());
            isoLongFileNamesFSCheck->setChecked(diskItem->getAllowLongFileNames());
            jolietLongNamesFSCheck->setChecked(diskItem->getAllowLongJolietFileNames());
            udfVersionFSCombo->setCurrentIndex(diskItem->getUDFType());
            udfPartitionFSCombo->setCurrentIndex(diskItem->getUDFPartition());
            udfFileStreamFSCheck->setChecked(diskItem->getUDFWriteStream());
            isoExtent1FSCheck->setChecked(diskItem->getNotWriteISO1Extension());

            simulateBurnGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_SIMULATE));
            simulateBurnGeneralCheck->setChecked(diskItem->getFeatureSimulate());
            activeOPCGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_OPC));
            activeOPCGeneralCheck->setChecked(diskItem->getFeatureOPC());
            finishDiscGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_FINALIZE));
            finishDiscGeneralCheck->setChecked(diskItem->getFeatureFinishDisk());
            padDataGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_PADDATATRACKS));
            padDataGeneralCheck->setChecked(diskItem->getFeaturePadDataTrack());
            avchdGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SPECIALS_AVCHD));
            avchdGeneralCheck->setChecked(diskItem->getFeatureAVCHD());
            verifyAfterGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_SETTINGS_VERIFY));
            verifyAfterGeneralCheck->setChecked(diskItem->getFeatureVerify());

            //Always allowed
            ejectAfterGeneralCheck->setChecked(diskItem->getFeatureEject());
            burnProofGeneralCheck->setChecked(diskItem->getFeatureBurnProof());
            autoEraseGeneralCheck->setChecked(diskItem->getFeatureAutoErase());

            createFolderEditButton->setEnabled(true);
            menuEditCreateFolder->setEnabled(true);
            if(activeMdiChild()->GetProjectType() == RuleManager::TYPE_PROJECT_AUDIOCD){
                addFolderEditButton->setEnabled(false);
                createFolderEditButton->setEnabled(false);
                menuEditCreateFolder->setEnabled(false);
                dataTrackEditButton->setEnabled(false);
                renameEditButton->setEnabled(false);
                menuEditRenameItem->setEnabled(false);
            }

            tempItem = activeMdiChild()->GetSelectedTreeItem();
            if(tempItem){
                if(tempItem->GetType()==QDataItem::Disk || tempItem->GetType()==QDataItem::Session || tempItem->GetType()==QDataItem::AudioTrack){
                    //Kein Folder, Kein File, Kein Track
                    createFolderEditButton->setEnabled(false);
                    menuEditCreateFolder->setEnabled(false);
                    addFolderEditButton->setEnabled(false);
                    addFileEditButton->setEnabled(false);
                    menuEditAddFile->setEnabled(false);

                    if(tempItem->GetType()!=QDataItem::AudioTrack){
                        createFolderEditButton->setEnabled(false);
                        menuEditCreateFolder->setEnabled(false);
                        delEditButton->setEnabled(false);
                        menuEditDeleteItem->setEnabled(false);
                    }

                }
            }

        }else{
            viewBrowserButton->setEnabled(false);
            menuToggleExplorer->setEnabled(false);
            createFolderEditButton->setEnabled(false);
            menuEditCreateFolder->setEnabled(false);
            delEditButton->setEnabled(false);

            //Alle Menus aus schalten.
            menuEditDeleteItem->setEnabled(false);
            menuEditCreateFolder->setEnabled(false);
            menuEditAddFile->setEnabled(false);
            menuEditRenameItem->setEnabled(false);
            menuEditAddFolder->setEnabled(false);
            menuEditDeleteAll->setEnabled(false);
            menuEditUpdateProject->setEnabled(false);
        }

        //Betrifft nur Aktive
        if(hasMdiInfoChild==true || hasMdiDeviceChild==true || hasMdiHexChild==true){
            viewTileButton->setEnabled(true);
            viewCascadeButton->setEnabled(true);
            viewSwitchButton->setEnabled(true);

            menuViewTile->setEnabled(true);
            menuViewCascade->setEnabled(true);
            menuViewNext->setEnabled(true);
        }

   // }

}

void MainWindow::doBurn()
{
    //We will give now the job to the window.
    //Jobs
    //1: Burn
    //2: Create Image
    burnDialog *dialog = new burnDialog(1,activeMdiChild()->getTreeWidget(),nullptr,tr(""),nullptr,this);
    dialog->exec(); //show();
    //Show non modal, exec is modal
}


//##########################################################################################################
//TOPIC: Update DiskItem structure
//Here we put now the change Events for DiskItem structure.
//##########################################################################################################
//ISOEx
void MainWindow::isoExSystemIdchanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setSystemId(strText);
    }
}

void MainWindow::isoExVolumechanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setVolumeSet(strText);
    }
}

void MainWindow::isoExPublisherchanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setPublisher(strText);
    }
}

void MainWindow::isoExDataPreparerchanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setDatapreparer(strText);
    }
}

void MainWindow::isoExApplicationchanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setApplication(strText);
    }
}

void MainWindow::isoExCopyrightFilechanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setCoprightFile(strText);
    }
}

void MainWindow::isoExAbstractFilechanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setAbstractFile(strText);
    }
}

void MainWindow::isoExBibliographicFilechanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setBibliographicFile(strText);
    }
}

//FileSystem
void MainWindow::fsIsoTypeChanged(int nIndex)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setISOFsType(nIndex);
        //Because we set it in FSSystem and Bootimage, we need to uodate all.
        updateProjectRibbon();
    }
}

void MainWindow::fsJolietChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setUseJoliet(nState?1:0);
    }
}

void MainWindow::fsRockRidgeChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setUseRockRidge(nState?1:0);
    }
}

void MainWindow::fsLowerCaseNamesChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setAllowLowerCaseNames(nState?1:0);
    }
}

void MainWindow::fsManyDirectories(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setAllowManyDirectories(nState?1:0);
    }
}

void MainWindow::fsLongFileNamesChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setAllowLongFileNames(nState?1:0);
    }
}

void MainWindow::fsNotWriteISO1Extension(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setAllowLongJolietFileNames(nState?1:0);
    }
}

void MainWindow::fsLongJolietFileNamesChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setNotWriteISO1Extension(nState?1:0);
    }
}

//UDF
void MainWindow::fsUDFYTpeChanged(int nIndex)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setUDFType(nIndex);
    }
}

void MainWindow::fsUDFPartitionChanged(int nIndex)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setUDFPartition(nIndex);
    }
}

void MainWindow::fsUDFWriteStreamChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setUDFWriteStream(nState?1:0);
    }
}

//Projectoptions
void MainWindow::projectSimulateBurnChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setFeatureSimulate(nState?1:0);
    }
}

void MainWindow::projectBurnProofChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setFeatureBurnProof(nState?1:0);
    }
}

void MainWindow::projectOPCChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setFeatureOPC(nState?1:0);
    }
}

void MainWindow::projectEjectChanged(int nState)
{

    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setFeatureEject(nState?1:0);
    }
}

void MainWindow::projectFinishMediumChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setFeatureFinishDisk(nState?1:0);
    }
}

void MainWindow::projectVerifyChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setFeatureVerify(nState?1:0);
    }
}

void MainWindow::projectAutoEraseChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setFeatureAutoErase(nState?1:0);
    }
}

void MainWindow::projectPadDataChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setFeaturePadDataTrack(nState?1:0);
    }
}

void MainWindow::projectAVCHDChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setFeatureAVCHD(nState?1:0);
    }
}

void MainWindow::updateBootDisk(bool bState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
    }

    if(diskItem!=nullptr) diskItem->setDoBootDisk(bState);
}

void MainWindow::bootEmulationTypeChanged(int iIndex)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setBootEmulationType(iIndex);
    }

}

void MainWindow::bootPlatformChanged(int iIndex)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setBootPlatformID(iIndex);
    }
}

void MainWindow::bootDeveloperIDchanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setBootDeveloperID(strText);
    }

}

void MainWindow::bootSectorschanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setBootSectors(strText);
    }
}

void MainWindow::bootLoadSegmentchanged(QString strText)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setBootLoadSegment(strText);
    }
}

void MainWindow::deleteBootImage()
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){

        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));

        if(diskItem!=nullptr){
            imagePathBootEdit->setText("");
            diskItem->setBootDiskFile("");
            diskItem->setDoBootDisk(false); //Trigger update
            updateProjectRibbon();
        }


    }
}


void MainWindow::clickedSelBootImagePath()
{

    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){

        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));

    }

    QString fileName = QFileDialog::getOpenFileName(
               this,
               tr("Open Boot Image"),
               QDir::currentPath(),
               tr("Boot Image (*.img)") );

    if (fileName.isEmpty())return;

    //QMessageBox::information(this, tr("Information"),fileName);

    if(diskItem!=nullptr){
        imagePathBootEdit->setText(fileName);
        diskItem->setBootDiskFile(fileName);
        diskItem->setDoBootDisk(true); //Trigger update
        updateProjectRibbon();
    }

}

//##############################################################################################################
//File Open & Save
//##############################################################################################################

void MainWindow::open()
{
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Load Project File"),
                                   QDir::currentPath(),
                                   tr("XBEL Files (*.xbel *.xml)"));
    if (fileName.isEmpty())
        return;

    QMdiSubWindow *existing = findMdiChild(fileName);
    if (existing) {
        mdiArea->setActiveSubWindow(existing);
        return;
    }
    MdiChildDialog *child = createMdiChild();
    if (child->loadFile(fileName)) {
        updateStatusBarText(tr("File loaded"));
        child->show();
        child->showMaximized();
    } else {
        child->close();
    }
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save()){
        //statusBar()->showMessage(tr("File saved"), 2000);
        updateStatusBarText(tr("File saved"));
    }
}

void MainWindow::saveAs()
{
    if (activeMdiChild() && activeMdiChild()->saveAs()){
        //statusBar()->showMessage(tr("File saved"), 2000);
        updateStatusBarText(tr("File saved"));
    }
}

//ISOEx Data
void MainWindow::isoExImportClicked()
{
    edIsoExSystemIdValue->setText(ConfigurationPage::mSettings.value("systemId","").toString());
    edIsoExVolumeSetValue->setText(ConfigurationPage::mSettings.value("volumeSet","").toString());
    edIsoExPublisherValue->setText(ConfigurationPage::mSettings.value("publisher","").toString());
    edIsoExDataPreparerValue->setText(ConfigurationPage::mSettings.value("datapreparer","").toString());
    edIsoExApplicationValue->setText(ConfigurationPage::mSettings.value("app","").toString());
    edIsoExCopyrightFileValue->setText(ConfigurationPage::mSettings.value("corightfile","").toString());
    edIsoExAbstractFileValue->setText(ConfigurationPage::mSettings.value("abstractfile","").toString());
    edIsoExBibliographicFileValue->setText(ConfigurationPage::mSettings.value("bibliographicfile","").toString());
}

void MainWindow::isoExReset()
{
    edIsoExSystemIdValue->setText("");
    edIsoExVolumeSetValue->setText("");
    edIsoExPublisherValue->setText("");
    edIsoExDataPreparerValue->setText("");
    edIsoExApplicationValue->setText("");
    edIsoExCopyrightFileValue->setText("");
    edIsoExAbstractFileValue->setText("");
    edIsoExBibliographicFileValue->setText("");
}

void MainWindow::openSettings()
{
    ConfigDialog *dialog = new ConfigDialog;
    dialog->show();
}

void MainWindow::closeDiskbutler()
{
    QApplication::quit();
}

//##############################################################################################################
//Create a QmenuBAr in the Top. Check what Windows and Linxu will say according to this.
//##############################################################################################################

void MainWindow::createOwnMenuBar()
{
    //It is needed to add actions to the menu, else it will not visible.
    //IF: Move to own function, not leave here

    //Check for updates?
    //Settings

    //Dynamisch
    //Insert file
    //Insert folder
    //Create Folder
    //Delete item

    //Save Pojekt
    //Open Projekt

    //QT is detecting names of QMenuBAr items. So a name like "Setting" is moved to the predefined menu "Preferences"
    //on the Mac. So we have to take care about our namings inside the QMenu bar.

    //File
    //->New Project
    //->->ISO
    //->->ISO/UDF
    //--
    //Open Project
    //Save Pproject
    //Save

    //Edit
    //Add File
    //Add Folder
    //Create Folder
    //Delete Selected
    //Rename Selected
    //--
    //Update All

    //Actions
    //Burn
    //Erase

    setUnifiedTitleAndToolBarOnMac(true);

    //Part of "File" Menu
    menuNewISOProject = new QAction(tr("&ISO"), this);
    menuNewISOProject->setStatusTip(tr("Create a new ISO project"));
    connect(menuNewISOProject, &QAction::triggered,  [=] { newProject(RuleManager::TYPE_PROJECT_UDF); });

    menuNewISOUDFProject = new QAction(tr("&ISOUDF"), this);
    menuNewISOUDFProject->setStatusTip(tr("Create a new ISOUDF project"));
    connect(menuNewISOUDFProject, &QAction::triggered,  [=] { newProject(RuleManager::TYPE_PROJECT_ISOUDF); });

    menuNewOpenProject = new QAction(tr("Open"), this);
    menuNewOpenProject->setShortcuts(QKeySequence::Open);
    menuNewOpenProject->setStatusTip(tr("Open a Diskbutler project"));
    connect(menuNewOpenProject, SIGNAL(triggered()), this, SLOT(open()));

    menuNewSaveProject = new QAction(tr("Save"), this);
    menuNewSaveProject->setShortcuts(QKeySequence::Save);
    menuNewSaveProject->setStatusTip(tr("Save a Diskbutler project"));
    connect(menuNewSaveProject, SIGNAL(triggered()), this, SLOT(save()));

    menuNewSaveAsProject = new QAction(tr("Save as"), this);
    menuNewSaveAsProject->setShortcuts(QKeySequence::SaveAs);
    menuNewSaveAsProject->setStatusTip(tr("Save a Diskbutler project with new name"));
    connect(menuNewSaveAsProject, SIGNAL(triggered()), this, SLOT(saveAs()));

    menuNewExit = new QAction(tr("Exit"), this);
    menuNewExit->setShortcuts(QKeySequence::Quit);
    menuNewExit->setStatusTip(tr("Close Diskbutler"));
    connect(menuNewExit, SIGNAL(triggered()), this, SLOT(closeDiskbutler()));

    //Part of Edit Menu
    menuEditAddFile = new QAction(tr("Add file"), this);
    menuEditAddFile->setStatusTip(tr("Add file to the project"));
    connect(menuEditAddFile, SIGNAL(triggered()), this, SLOT(insertItem()));

    menuEditAddFolder = new QAction(tr("Add folder"), this);
    menuEditAddFolder ->setStatusTip(tr("Add folder to the project"));
    connect(menuEditAddFolder , SIGNAL(triggered()), this, SLOT(insertNode()));

    menuEditCreateFolder = new QAction(tr("Create folder"), this);
    menuEditCreateFolder ->setStatusTip(tr("Create empty folder to the project"));
    connect(menuEditCreateFolder , SIGNAL(triggered()), this, SLOT(addNode()));

    menuEditRenameItem = new QAction(tr("Rename item"), this);
    menuEditRenameItem ->setStatusTip(tr("Rename the selected item in project"));
    connect(menuEditRenameItem , SIGNAL(triggered()), this, SLOT(renameItemEx()));

    menuEditDeleteItem = new QAction(tr("Delete item"), this);
    menuEditDeleteItem->setShortcuts(QKeySequence::Delete);
    menuEditDeleteItem ->setStatusTip(tr("Delete selected item"));
    connect(menuEditDeleteItem , SIGNAL(triggered()), this, SLOT(deleteItem()));

    menuEditDeleteAll = new QAction(tr("Delete all"), this);
    menuEditDeleteAll->setShortcuts(QKeySequence::DeleteCompleteLine);
    menuEditDeleteAll ->setStatusTip(tr("Delete all content from project"));
    connect(menuEditDeleteAll , SIGNAL(triggered()), this, SLOT(resetFiles()));

    menuEditUpdateProject = new QAction(tr("Update project"), this);
    menuEditUpdateProject->setShortcuts(QKeySequence::Refresh);
    menuEditUpdateProject ->setStatusTip(tr("Re-read all project data (files and folder)"));
    connect(menuEditUpdateProject , SIGNAL(triggered()), this, SLOT(updateProject()));

    //Part of Action Menu
    menuActionBurn = new QAction(tr("Burn now"), this);
    menuActionBurn ->setStatusTip(tr("Write project to disc or ISO image"));
    connect(menuActionBurn , SIGNAL(triggered()), this, SLOT(doBurn()));

    menuActionErase = new QAction(tr("Erase disc"), this);
    menuActionErase ->setStatusTip(tr("Erase current disc fast or complete"));
    connect(menuActionErase, SIGNAL(triggered()), this, SLOT(doBurn()));

    menuEditSettings = new QAction(tr("&Settings"), this);
    menuEditSettings->setShortcuts(QKeySequence::Preferences);
    menuEditSettings->setStatusTip(tr("Open Diskbutler properties"));
    connect(menuEditSettings, SIGNAL(triggered()), this, SLOT(openSettings()));

    menuViewNext = new QAction(tr("&Next Window"), this);
    menuViewNext->setShortcuts(QKeySequence::NextChild);
    menuViewNext->setStatusTip(tr("Switch to next window"));
    connect(menuViewNext, SIGNAL(triggered()), this, SLOT(mdiNextWindow()));

    menuViewTile = new QAction(tr("&Tile"), this);
    //menuViewTile->setShortcuts(QKeySequence::Preferences);
    menuViewTile->setStatusTip(tr("Switch to next window"));
    connect(menuViewTile, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    menuViewCascade = new QAction(tr("&Cascade"), this);
    //menuViewCascade->setShortcuts(QKeySequence::Preferences);
    menuViewCascade->setStatusTip(tr("Switch to next window"));
    connect(menuViewCascade, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    menuMaximize = new QAction(tr("&Full screen"), this);
    menuMaximize->setShortcuts(QKeySequence::FullScreen);
    menuMaximize->setStatusTip(tr("Switch to full screen"));
    connect(menuMaximize, SIGNAL(triggered()), this, SLOT(showFullScreenMode()));

    menuToggleExplorer = new QAction(tr("&Toggle disc explorer in project view"), this);
    //menuToggleExplorer->setShortcuts(QKeySequence::Preferences);
    menuToggleExplorer->setStatusTip(tr("Enable / disable the explorer view in project window"));
    connect(menuToggleExplorer, SIGNAL(triggered()), this, SLOT(toggleFileExplorer()));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QMenu *projectMenu = fileMenu->addMenu(tr("&Projects"));
    projectMenu->addAction(menuNewISOProject);
    projectMenu->addAction(menuNewISOUDFProject);

    fileMenu->addAction(menuNewOpenProject);
    fileMenu->addAction(menuNewSaveProject);
    fileMenu->addAction(menuNewSaveAsProject);
    fileMenu->addSeparator();
    fileMenu->addAction(menuNewExit);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(menuEditAddFile);
    editMenu->addAction(menuEditAddFolder);
    editMenu->addAction(menuEditCreateFolder);
    editMenu->addSeparator();
    editMenu->addAction(menuEditDeleteItem);
    editMenu->addAction(menuEditDeleteAll);
    editMenu->addSeparator();
    editMenu->addAction(menuEditRenameItem);
    editMenu->addSeparator();
    editMenu->addAction(menuEditSettings);

    QMenu *actionMenu = menuBar()->addMenu(tr("&Action"));
    actionMenu->addAction(menuActionBurn);
    actionMenu->addAction(menuActionErase);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(menuViewNext);
    viewMenu->addAction(menuViewTile);
    viewMenu->addAction(menuViewCascade);
    //viewMenu->addAction(menuMaximize);
    viewMenu->addAction(menuToggleExplorer);

}

//##############################################################################################################
//Check actual version o Network
//##############################################################################################################

void MainWindow::onNewVersionAvail(QString newVersion)
{
    //QMessageBox::information(this, tr("New Version"),newVersion);
    QMessageBox::information(this, tr("New Version"),tr("Pupsi version"));

    //----------------------------------------------------------------------------------------------
    //Additional Infos PHP Version file with json
    /*
     *     silentFoxSDKActions(9);
    <?php
        $version = $_GET['v'];



        $x=0;



        $newversion = "2.3.0.2";

        $a = explode(".", $version);
        $b = explode(".", $newversion);

        for($i=0;$i<count($b);$i++) {
            if($b[$i] > $a[$i]){
                $array = array('Version'=> $newversion);
                echo json_encode($array);
                $x=1;
                break;
            } else if($b[$i] < $a[$i]){
                break;
            }
        }

        if($x==0){
            $newversion = "no";
            $array = array('Version'=> $newversion);
            echo json_encode($array);
        }

        exit();



    ?>
     */
}

//##############################################################################################################
//Additional Disk & Media Functions
//##############################################################################################################
void MainWindow::openHexEditor()
{
    if(listReadDevicesWidget->count()==0) return;

    //Then we check if there is a selction
    QListWidgetItem* itemFromList = listReadDevicesWidget->currentItem();
    if(itemFromList==nullptr) return;

    QString strDriveName = itemFromList->text();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
      MdiChildHex *mdiChild = qobject_cast<MdiChildHex *>(window->widget());
      if (nullptr != mdiChild){
          //We have now a DeviceInfo child.
          //We will check now the drive. If the drive is the same, we pop up the mdichild to top.
          //And return
          //else we will create a new one
          //RuleManager::ProjectType myType = mdiChild->GetProjectType();
          if(strDriveName.at(0)==mdiChild->getBurnDrive().at(0)){
            mdiArea->setActiveSubWindow(window);
            return;
          }

      }
    }

    MdiChildHex *child = new MdiChildHex(this,strDriveName);
    mdiArea->addSubWindow(child);

    if(isCascaded()==false){
        child->showMaximized();
    }else{
        child->showNormal();
    }
    
    //active child changed an der Position. Daher springt er im Tab. 
    //Müssen wir dasa active subfenster definieren des Hex Editors.
}

void MainWindow::openScanDialog()
{
    if(listReadDevicesWidget->count()==0) return;

    //Then we check if there is a selction
    QListWidgetItem* itemFromList = listReadDevicesWidget->currentItem();
    if(itemFromList==nullptr) return;

    QString strDriveName = itemFromList->text();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
      MdiChildScan *mdiChild = qobject_cast<MdiChildScan *>(window->widget());
      if (nullptr != mdiChild){
          //We have now a DeviceInfo child.
          //We will check now the drive. If the drive is the same, we pop up the mdichild to top.
          //And return
          //else we will create a new one
          //RuleManager::ProjectType myType = mdiChild->GetProjectType();
          if(strDriveName.at(0)==mdiChild->getBurnDrive().at(0)){
            mdiArea->setActiveSubWindow(window);
            return;
          }

      }
    }

    MdiChildScan *child = new MdiChildScan(this,strDriveName);
    mdiArea->addSubWindow(child);

    if(isCascaded()==false){
        child->showMaximized();
    }else{
        child->showNormal();
    }
}



void MainWindow::newDiskInfo()
{

    //First we check if there is something to view
    if(listReadDevicesWidget->count()==0) return;

    //Then we check if there is a selction
    QListWidgetItem* itemFromList = listReadDevicesWidget->currentItem();
    if(itemFromList==nullptr) return;

    QString strDriveName = itemFromList->text();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
      MdiChildDiskInfo *mdiChild = qobject_cast<MdiChildDiskInfo *>(window->widget());
      if (nullptr != mdiChild){
          //We have now a DeviceInfo child.
          //We will check now the drive. If the drive is the same, we pop up the mdichild to top.
          //And return
          //else we will create a new one
          //RuleManager::ProjectType myType = mdiChild->GetProjectType();
          if(strDriveName.at(0)==mdiChild->getBurnDrive().at(0)){
            mdiArea->setActiveSubWindow(window);
            return;
          }

      }
    }

    MdiChildDiskInfo *child = new MdiChildDiskInfo(this,strDriveName);
    //We will create the child and set the errorhandle inside the class.
    //If success, we call addSubWindow, if not we will delete it.
    if(child->thisSuccessfullCreated==true){
        mdiArea->addSubWindow(child);

        if(isCascaded()==false){
            child->showMaximized();
        }else{
            child->showNormal();
        }
    }else{
        child->close();
    }


    //insertCustomRibbonPages();
    //specialActionDiskInfo();



}

void MainWindow::newDeviceInfo()
{
    //First we check if there is something to view
    if(listReadDevicesWidget->count()==0) return;

    //Then we check if there is a selction
    QListWidgetItem* itemFromList = listReadDevicesWidget->currentItem();
    if(itemFromList==nullptr) return;

    QString strDriveName = itemFromList->text();

        foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
          MdiChildDeviceInfo *mdiChild = qobject_cast<MdiChildDeviceInfo *>(window->widget());
          if (nullptr != mdiChild){
              //We have now a DeviceInfo child.
              //We will check now the drive. If the drive is the same, we pop up the mdichild to top.
              //And return
              //else we will create a new one
              //RuleManager::ProjectType myType = mdiChild->GetProjectType();
              if(strDriveName.at(0)==mdiChild->getBurnDrive().at(0)){
                mdiArea->setActiveSubWindow(window);
                return;
              }

          }
        }

        MdiChildDeviceInfo *child = new MdiChildDeviceInfo(this,strDriveName);
        mdiArea->addSubWindow(child);

        if(isCascaded()==false){
            child->showMaximized();
        }else{
            child->showNormal();
        }
        //insertCustomRibbonPages();
        //specialActionDeviceInfo();



}
