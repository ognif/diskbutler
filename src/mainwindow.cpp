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


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMenu>
#include "CommonTreeWidget.h"
#include "mdichild_dialog.h"
#include "configdialog.h"
#include "burndialog.h"
#include "ribbonbuilder.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


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
    ui->setupUi( this );

    setWindowTitle( tr("Diskbutler") );
    setWindowIcon( QIcon( ":/icons/diskbutler32.png" ) );
    mBlockSpinner = new QtWaitingSpinner( Qt::ApplicationModal, this, true );

    currentSubWindow = nullptr;


#if defined (LINUX)
    QString mLicense = "IQJH1V-6Y1ZA2-5NIAGR-ANJQNA-BMBNSO";
#endif
#if defined (MAC)
    QString mLicense = "IXJNQ9-J9FYAD-KWDAX5-ATBLQA-9LMJVZ";
#endif
#if defined (WIN32)
    QString mLicense = "KLJWVC-IXPJAP-35LAUI-AZ0G5A-2LB5KU";
#endif

    const TCHAR *pLicense = convertToFoxValue( mLicense );
    int res = ::Initialize( pLicense, BS_ASPI_INTERNAL, BS_TRUE );
    delete [] pLicense;

    if(res!=0){
        QString result = tr("FoxSDK reports: %1").arg( res );
        QMessageBox::information( this, tr("Information"), result );
    }

    ui->ribbonDockWidget->setTitleBarWidget( new QWidget() );

    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    mdiArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    setCentralWidget( mdiArea );

     res = ::CreateProject( BS_BURNER_ISOUDF, BS_CONTINUE_NO_SESSION );
     if(res!=0){
         QMessageBox::information( this, tr("Diskbutler information"),
                                  tr("Error in Create Projekt") );
     }

     //Test Versionchecker here. Will it block the UI?
     /*
     mCheckVersionThread  = new CheckVersionThread();
     connect(mCheckVersionThread, SIGNAL(tNewVersion(QString)), this, SLOT(onNewVersionAvail(QString)));
     mCheckVersionThread->start();
    */

     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start Project Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     ribbonBuilderAll( ui->ribbonTabWidget, this );

     connect( dataIsoProjectButton, &QToolButton::clicked,  [=] { newProject( RuleManager::TYPE_PROJECT_ISO ); } );
     connect( dataUdfProjectButton, &QToolButton::clicked,  [=] { newProject( RuleManager::TYPE_PROJECT_UDF ); } );
     connect( dataIsoUdfProjectButton, &QToolButton::clicked,  [=] { newProject( RuleManager::TYPE_PROJECT_ISOUDF ); } );
     connect( mmAudioProjectButton, &QToolButton::clicked,  [=] { newProject( RuleManager::TYPE_PROJECT_AUDIOCD ); } );
     connect( mmEnhancedProjectButton, &QToolButton::clicked,  [=] { newProject( RuleManager::TYPE_PROJECT_MIXEDMODE ); } );
     connect( vidSVCDProjectButton, &QToolButton::clicked,  [=] { newProject( RuleManager::TYPE_PROJECT_SUPERVIDEO ); } );
     connect( vidDVDProjectButton, &QToolButton::clicked,  [=] { newProject( RuleManager::TYPE_PROJECT_VIDEODVD ); } );
     connect( vidBlueProjectButton, &QToolButton::clicked,  [=] { newProject( RuleManager::TYPE_PROJECT_BLURAYVIDEO ); } );
     connect( appOpenButton, SIGNAL( clicked() ), this, SLOT( open() ) );
     connect( appSaveButton, SIGNAL( clicked() ), this, SLOT( save() ) );
     connect( appSaveAsButton, SIGNAL( clicked() ), this, SLOT( saveAs() ) );
     connect( appSettings, SIGNAL( clicked() ), this, SLOT( openSettings() ) );

     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start Edit Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     ribbonBuilderProject( ui->ribbonTabWidget, this );

     connect( addFileEditButton, SIGNAL( clicked() ), this, SLOT( insertItem() ) );
     connect( addFolderEditButton, SIGNAL( clicked() ), this, SLOT( insertNode() ) );
     connect( createFolderEditButton, SIGNAL( clicked() ), this, SLOT( addNode() ) );
     connect( delEditButton, SIGNAL( clicked() ), this, SLOT( deleteItem()) );
     connect( renameEditButton, SIGNAL( clicked() ), this, SLOT( renameItemEx() ) );
     connect( delAllEditButton, SIGNAL( clicked() ), this, SLOT( resetFiles() ) );
     connect( updtEditButton, SIGNAL( clicked() ), this, SLOT( updateProject() ));
     connect( dataTrackEditButton, SIGNAL( clicked() ), this, SLOT( addDataTrack() ));
     connect( audioTrackEditButton, SIGNAL( clicked() ), this, SLOT( addAudioTrack() ) );
     connect( allSelectEditButton, SIGNAL( clicked() ), this, SLOT( selectAll() ));
     connect( inverseSelectEditButton, SIGNAL( clicked() ), this, SLOT( reverseSelection() ) );

     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start General Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     ribbonBuilderGeneral( ui->ribbonTabWidget, this );

     connect(simulateBurnGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectSimulateBurnChanged(int)));
     connect(ejectAfterGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectEjectChanged(int)));
     connect(autoEraseGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectAutoEraseChanged(int)));
     connect(burnProofGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectBurnProofChanged(int)));
     connect(verifyAfterGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectVerifyChanged(int)));
     connect(padDataGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectPadDataChanged(int)));
     connect(activeOPCGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectOPCChanged(int)));
     connect(finishDiscGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectFinishMediumChanged(int)));
     connect(avchdGeneralCheck, SIGNAL(stateChanged(int)), this, SLOT(projectAVCHDChanged(int)));
     connect(burnGeneralButton, &QToolButton::clicked,  [=] { doBurn(); });
     connect(eraseGeneralButton, &QToolButton::clicked,  [=] { doErase(); });
     connect(burnDiskImage, &QToolButton::clicked,  [=] { doBurnImage(); });

     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start Device Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     ribbonBuilderDevice( ui->ribbonTabWidget, this );

     connect(listReadDevicesWidget, QOverload<int>::of(&QComboBox::currentIndexChanged),
         [=](int index){ updateReadDeviceSel(index);});
     fillReadDriveList(true);
     connect(listBurnDevicesWidget, QOverload<int>::of(&QComboBox::currentIndexChanged),
         [=](int index){ updateBurnDeviceSel(index);});
     fillBurnDriveList(true);

     connect(readDeviceUpdateButton, SIGNAL(clicked()), this, SLOT(fillReadDriveList()));

     QAction* actionEjectReadDevice  = m_EjectTrayMenu->addAction( tr("Read device") );
     actionEjectReadDevice->setCheckable( false );
     connect( actionEjectReadDevice, &QAction::triggered, [=] { ejectTray(true); } );

     QAction* actionCloseReadDevice  = m_CloseTrayMenu->addAction( tr("Read device") );
     actionCloseReadDevice->setCheckable( false );
     connect( actionCloseReadDevice, &QAction::triggered, [=] { closeTray(true); } );
     connect(openDeviceButton, SIGNAL(clicked()), this, SLOT(ejectTray()));
     connect(closeDeviceButton, SIGNAL(clicked()), this, SLOT(closeTray()));
     connect(infoDeviceButton, SIGNAL(clicked()), this, SLOT(newDeviceInfo()));
     connect(m_diskInfoMenu, SIGNAL(aboutToShow()), this, SLOT(updateDiskInfoMenu()));
     connect(infoMediaButton, SIGNAL(clicked()), this, SLOT(newDiskInfo()));
     connect(imageMediaButton, SIGNAL(clicked()), this, SLOT(release2ImageTab()));
     //connect(grabAudioediaButton, SIGNAL(clicked()), this, SLOT(reverseSelection()));
     connect(scanMediaButton, SIGNAL(clicked()), this, SLOT(openScanDialog()));
     connect(hexMediaButton, SIGNAL(clicked()), this, SLOT(openHexEditor()));

     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start View Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     ribbonBuilderView( ui->ribbonTabWidget, this );

     connect(viewBrowserButton, &QToolButton::clicked,  [=] { toggleFileExplorer(); });
     connect(viewTileButton, SIGNAL(clicked()), this, SLOT(viewTileMdi()));
     connect(viewCascadeButton, SIGNAL(clicked()), this, SLOT(viewCascadeView()));
     connect(m_windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowSwitchMenu()));

     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start 2Image Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     ribbonBuilder2Image( ui->ribbonTabWidget, this );

     connect(imageFormatCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(imageCreateMethod(int)));
     connect(imageReadErrorSwitch, SIGNAL(stateChanged(int)), this, SLOT(errorCorrectionChanged(int)));
     connect(imageReadJobCreate, SIGNAL(stateChanged(int)), this, SLOT(jobCreateChanged(int)));
     connect(imageReadJobVerify, SIGNAL(stateChanged(int)), this, SLOT(jobVerifyChanged(int)));
     connect(startImageCreateJob, SIGNAL(clicked()), this, SLOT(onCreateDiscImage()));

     //NEW
     connect( imageReadSoftRetry, QOverload<double>::of( &QDoubleSpinBox::valueChanged ), this, &MainWindow::changeReadSoftRetry );
     connect( imageReadHardRetry, QOverload<double>::of( &QDoubleSpinBox::valueChanged ), this, &MainWindow::changeReadHardRetry );


     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start Hex Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     ribbonBuilderHex( ui->ribbonTabWidget, this );

     connect(hexSector, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
         [=](double d){ hexReadSector(d);});
     connect(updateHexEditor, SIGNAL(clicked()), this, SLOT(actHexUpdate()));
     connect(saveHexEditor, SIGNAL(clicked()), this, SLOT(actHexSaveLog()));
     connect(navFirstHexEditor, SIGNAL(clicked()), this, SLOT(actHexFirstSector()));
     connect(navStepBackHexEditor, SIGNAL(clicked()), this, SLOT(actHexPrevSector()));
     connect(navStepForwardHexEditor, SIGNAL(clicked()), this, SLOT(actHexNextSector()));
     connect(navLastHexEditor, SIGNAL(clicked()), this, SLOT(actHexLastSector()));


     //End Hex Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start Scan Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     ribbonBuilderScan( ui->ribbonTabWidget, this );

     connect(scanStartOffset, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
         [=](double d){ scanStartOffsetValue(d);});
     connect(scanReadOffset, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
         [=](double d){ scanReadOffsetValue(d);});
     connect(updateScanEditor, SIGNAL(clicked()), this, SLOT(scanUpdateDiskInfo()));
     connect(startScanEditor, SIGNAL(clicked()), this, SLOT(scanStartScan()));
     connect(stopScanEditor, SIGNAL(clicked()), this, SLOT(scanStopScan()));
     connect(saveScanEditor, SIGNAL(clicked()), this, SLOT(scanSaveLog()));

     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start File System Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      ribbonBuilderFileSystem( ui->ribbonTabWidget, this );

     //connect(isoLevelFSCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fsIsoTypeChanged(int)));
     //We use activated in case of a user click, no matter that it is always fired because the value keep the same.
     //But it will not get -1 if Combo is cleared.
     connect(isoLevelFSCombo, SIGNAL(activated(int)), this, SLOT(fsIsoTypeChanged(int)));
     connect(jolietLongNamesFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsLongJolietFileNamesChanged(int)));
     connect(isoExtent1FSCheck , SIGNAL(stateChanged(int)), this, SLOT(fsNotWriteISO1Extension(int)));
     connect(useJolietFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsJolietChanged(int)));
     connect(isoManyDirectoriesFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsManyDirectories(int)));
     connect(useRockRidgeFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsRockRidgeChanged(int)));
     connect(isoLongFileNamesFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsLongFileNamesChanged(int)));
     connect(udfVersionFSCombo, SIGNAL(activated(int)), this, SLOT(fsUDFYTpeChanged(int)));
     connect(udfPartitionFSCombo, SIGNAL(activated(int)), this, SLOT(fsUDFPartitionChanged(int)));
     connect(udfFileStreamFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsUDFWriteStreamChanged(int)));
     //connect(udfFileStreamFSCheck, SIGNAL(stateChanged(int)), this, SLOT(fsUDFWriteStreamChanged(int)));


     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start ISOEx Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     ribbonBuilderISOExtended( ui->ribbonTabWidget, this );

     connect(edIsoExSystemIdValue,SIGNAL(textChanged(QString)),this,SLOT(isoExSystemIdchanged(QString)));
     connect(edIsoExVolumeSetValue,SIGNAL(textChanged(QString)),this,SLOT(isoExVolumechanged(QString)));
     connect(edIsoExPublisherValue,SIGNAL(textChanged(QString)),this,SLOT(isoExPublisherchanged(QString)));
     connect(edIsoExDataPreparerValue,SIGNAL(textChanged(QString)),this,SLOT(isoExDataPreparerchanged(QString)));
     connect(edIsoExApplicationValue,SIGNAL(textChanged(QString)),this,SLOT(isoExApplicationchanged(QString)));
     connect(edIsoExCopyrightFileValue,SIGNAL(textChanged(QString)),this,SLOT(isoExCopyrightFilechanged(QString)));
     connect(edIsoExAbstractFileValue,SIGNAL(textChanged(QString)),this,SLOT(isoExAbstractFilechanged(QString)));
     connect(edIsoExBibliographicFileValue,SIGNAL(textChanged(QString)),this,SLOT(isoExBibliographicFilechanged(QString)));
     connect(importIsoExButton, &QToolButton::clicked,  [=] { isoExImportClicked(); });
     connect(resetIsoExButton, &QToolButton::clicked,  [=] { isoExReset(); });


     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //Start BootImage Tab
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     ribbonBuilderBoot( ui->ribbonTabWidget, this );

     connect(selBootImageButton, &QToolButton::clicked,  [=] { clickedSelBootImagePath(); });
     connect(delBootImageButton, &QToolButton::clicked,  [=] { deleteBootImage(); });
     connect(developerIDBootEdit,SIGNAL(textChanged(QString)),this,SLOT(bootDeveloperIDchanged(QString)));
     connect(sectorsBootEdit,SIGNAL(textChanged(QString)),this,SLOT(bootSectorschanged(QString)));
     connect(loadSegmentBootEdit,SIGNAL(textChanged(QString)),this,SLOT(bootLoadSegmentchanged(QString)));
     connect(emulationTypeBootCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(bootEmulationTypeChanged(int)));
     connect(platformBootCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(bootPlatformChanged(int)));
     connect(bootISOLevelCombo, SIGNAL(activated(int)), this, SLOT(fsIsoTypeChanged(int)));


     //Finish Build Ribbon
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

     connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(onActiveChildChanged(QMdiSubWindow*)));
     connect(ui->ribbonTabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(onRibbonChanged(int)));

     //For Macs
     setUnifiedTitleAndToolBarOnMac( true );

     //temporary fix for initilaize Tabs
     ui->ribbonTabWidget->hideAll();
     ui->ribbonTabWidget->setTabIndex( 0 );

     lastSelectedTab = "";

     createStatusBar();
     emit onActiveChildChanged( nullptr );

     QTimer::singleShot( 0, this, SLOT( loadDefaultProject() ) );

}

MainWindow::~MainWindow()
{
    mBlockSpinner->deleteLater();
    delete ui;
}

void MainWindow::onRibbonChanged( int nIndex )
{

    if( activeMdiChild() != nullptr ){
        activeMdiChild()->setlastSelectedTab( ui->ribbonTabWidget->tabText( nIndex ) );
        return;
    }
    if( activeMdiInfoChild() != nullptr ){
        activeMdiInfoChild()->setlastSelectedTab( ui->ribbonTabWidget->tabText( nIndex ) );
        return;
    }
    if( activeMdiScanChild() != nullptr ){
        activeMdiScanChild()->setlastSelectedTab( ui->ribbonTabWidget->tabText( nIndex ) );
        return;
    }
    if( activeMdiHexChild() != nullptr ){
        activeMdiHexChild()->setlastSelectedTab( ui->ribbonTabWidget->tabText( nIndex ) );
        return;
    }
    if( activeMdiDeviceChild() != nullptr ){
        activeMdiDeviceChild()->setlastSelectedTab( ui->ribbonTabWidget->tabText( nIndex ) );
        return;
    }

}

void MainWindow::loadDefaultProject()
{
    int defaultProject = ConfigurationPage::mSettings.value( tr("DefaultProject" ), -1 ).toInt();
    if( defaultProject > -1 ){
        RuleManager::ProjectType myValue = ( RuleManager::ProjectType )defaultProject;
        newProject( myValue );
    }
}

void MainWindow::viewTileMdi()
{

    if ( QMdiSubWindow *activeSubWindow = mdiArea->currentSubWindow() ){
        activeSubWindow->blockSignals( true );
        mdiArea->tileSubWindows();
        activeSubWindow->blockSignals( false );
    }

}

void MainWindow::viewCascadeView()
{
    if ( QMdiSubWindow *activeSubWindow = mdiArea->currentSubWindow() ){
        activeSubWindow->blockSignals( true );
        mdiArea->cascadeSubWindows();
        activeSubWindow->blockSignals( false );
    }

}


void MainWindow::createStatusBar()
{

  QHBoxLayout *statusbarLayout = new QHBoxLayout();
  QWidget * statusWidget = new QWidget();

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

  statusWidget->setLayout(statusbarLayout);

  statusBar()->addPermanentWidget(statusWidget,1);

}

void MainWindow::statusbar_changed(const QString &text)
{
    //return;
    //BEcause a dilog (Selection) is active there is no activeSubwindow
    QMdiSubWindow *activeSubWindow = mdiArea->currentSubWindow();
    if ("" == text && activeSubWindow != nullptr) {
        MdiChildBase * child = qobject_cast<MdiChildBase *>(activeSubWindow);
        //statusBar()->showMessage(child->updateStatus());
        updateStatusBarText(child->updateStatus(),true);
    }
}



MdiChildDialog *MainWindow::createMdiChild(const RuleManager::ProjectType project_type)
{

    MdiChildDialog *child = new MdiChildDialog(project_type);
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(datatrack_changed()),
        this, SLOT(onUpdateRibbonControls()));

    connect(child, SIGNAL(audiotrack_changed(bool)),
        this, SLOT(onUpdateRibbonControls()));

    connect(child, SIGNAL(status_changed(const QString&)),
           this, SLOT(statusbar_changed(const QString&)));

    return child;
}

MdiChildDialog *MainWindow::createMdiChild()
{

    MdiChildDialog *child = new MdiChildDialog();
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(datatrack_changed()),
        this, SLOT(onUpdateRibbonControls()));

    connect(child, SIGNAL(audiotrack_changed(bool)),
       this, SLOT(onUpdateRibbonControls()));

    connect(child, SIGNAL(status_changed(const QString&)),
           this, SLOT(statusbar_changed(const QString&)));

  return child;
}

void MainWindow::onUpdateRibbonControls()
{
    if(activeMdiChild() != nullptr){
        activeMdiChild()->setUIControls( ui->ribbonTabWidget, this );
        //Filetree changed so check tree
        checkFileSystemNaming();
        return;
    }
    if( activeMdiInfoChild() != nullptr ){
        activeMdiInfoChild()->setUIControls( ui->ribbonTabWidget, this );
        return;
    }
    if( activeMdiScanChild() != nullptr ){
        activeMdiScanChild()->setUIControls( ui->ribbonTabWidget, this );
        return;
    }
    if( activeMdiHexChild() != nullptr ){
        activeMdiHexChild()->setUIControls( ui->ribbonTabWidget, this );
        return;
    }

}


void MainWindow::updateStatusBarText(const QString &text, bool isInfo)
{
    if(isInfo == true){
        mStatusTextLabel->setText(text);
    }else{
        mStatusInfoLabel->setText(text);
    }

}

//##############################################################################################################
//Window Display Functions
//##############################################################################################################

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
        QWidget* child = windows.at(i);
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

//##############################################################################################################
//End Window Display Functions
//##############################################################################################################

//##############################################################################################################
//Disk Content Functions
//##############################################################################################################

void MainWindow::newProject(int type)
{
    RuleManager::ProjectType projectType = static_cast<RuleManager::ProjectType>(type);
    QString str = tr("Project Type: ");
    MdiChildDialog *child = createMdiChild(projectType);

    //child->SetProjectType(projectType);
    child->newFile();

    if(isCascaded()==false){
        child->showMaximized();
    }else{
        child->showNormal();
    }

    str += RuleManager::GetProjectTypeStr(projectType);
    updateStatusBarText(child->updateStatus(),true);


    //Switch to Tab Edit to start direktly with editing projekt.
    //Maybe we will do this editable.
    ui->ribbonTabWidget->currentTab(tr("Edit"));
    setDefaults();

    mdiArea->setActiveSubWindow(child);

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

void MainWindow::checkFileSystemNaming()
{
    /*
     * Wir sortieren zuerst nach ISO LEVEL
     * ISO LEVEK ist ein template.
     * Switches legen wir auserhalb der WHile Schelife fest
     *
     * Pfadlänge max. 255 Zeichen
     * Dateinamen: 8.3
     * Dateinamen Joliet kur/lang 64/103
     * Lange ISO Namen: 207
     * QDataItem *tempItem = GetSelectedTreeItem();
     *
     * What todo with UDF
     * UDF is 255 file/folder
     * pathlength 1023
     * pathdepth = 99
     */

    int borderDir = 8;
    int borderISO = 11;
    int borderJoliet = 64;
    int bordeRockridge = 255;
    bool checkJoliet = false;
    bool checkRockRidge = false;
    int borderLength = 255;
    bool checkFSBorders = false;

    RuleManager::ProjectType tProject = activeMdiChild()->GetProjectType();
    if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_ISO9660)){

        checkFSBorders = true;

        if(useJolietFSCheck->isChecked()==true){
            checkJoliet = true;
        }
        if(useRockRidgeFSCheck->isChecked()==true){
            checkRockRidge = true;
        }

        int isoLevel = isoLevelFSCombo->itemData(isoLevelFSCombo->currentIndex()).toInt();
        switch(isoLevel){
            case BS_ISO_LEVEL_1:
                if(isoManyDirectoriesFSCheck->isChecked()==true){
                    borderDir = 120; //More than 999 is crazy
                }
                if(isoLongFileNamesFSCheck->isChecked()==true){
                    borderISO = 207;
                }
                if(jolietLongNamesFSCheck->isChecked()==true){
                    borderJoliet = 103;
                }
                if(useRockRidgeFSCheck->isChecked()==true){
                    bordeRockridge = 255;
                    borderDir = 120;
                }
            break;
            case BS_ISO_LEVEL_2:
                borderISO = 31;
                if(isoManyDirectoriesFSCheck->isChecked()==true){
                    borderDir = 120; //More than 999 is crazy
                }
                if(isoLongFileNamesFSCheck->isChecked()==true){
                    borderISO = 207;
                }
                if(jolietLongNamesFSCheck->isChecked()==true){
                    borderJoliet = 103;
                }
                if(useRockRidgeFSCheck->isChecked()==true){
                    bordeRockridge = 255;
                    borderDir = 120;
                }
            break;
            case BS_ISO_LEVEL_3:
                borderDir = 120;
                borderISO = 207;
                borderJoliet = 103;
                bordeRockridge = 255;
            break;
            case BS_ISO_LEVEL_ROMEO:
                borderDir = 8;
                borderISO = 128;
                //No Extension allowed
                checkJoliet = false;
                checkRockRidge = false;
            break;
        }

    }else{
        if(RuleManager::IsOptionAllowed(tProject, RuleManager::OPTION_FILESYSTEMS_UDF)){

            checkFSBorders = true;

            borderDir = 120;
            borderISO = 255;
            borderJoliet = 64;
            bordeRockridge = 255;
            borderLength = 1023;
            checkJoliet = false;
            checkRockRidge = false;

        }
    }


    if(checkFSBorders == true){

        QTreeWidgetItemIterator it( activeMdiChild()->getTreeWidget() );
        while ( *it ) {


            //((QDataItem*)(*it))->setIconWarning(false,"");
            QString strWarning = "";

            if( ((QDataItem*)(*it))->GetType()==QDataItem::File ){
                int fileLength = ((QDataItem*)(*it))->GetName().length();
                if( fileLength > borderISO ){
                    strWarning = QString( "File lenght exceed limit: %1 " ).arg( borderISO );
                }
                if( checkJoliet == true ){
                    if( fileLength > borderJoliet ){
                       strWarning = QString( "Jolie file lenght exceed limit: %1 " ).arg( borderJoliet );
                    }
                }
                if( checkRockRidge == true ){
                    if( fileLength > bordeRockridge ){
                       strWarning = QString( "Rockridge file lenght exceed limit: %1 " ).arg( bordeRockridge );
                    }
                }
                //Pathmax
                int fileMaxLength = ((QDataItem*)(*it))->GetDiskFilePath().length();
                if( fileMaxLength > borderLength ){
                    strWarning = QString( "Max path lenght exceed limit: %1 " ).arg( borderLength );
                }
                //Path depth
                QStringList dirList = ((QDataItem*)(*it))->GetDiskPath().split( PATHSEPSTRING );
                if( dirList.size() > borderDir ){
                    strWarning += "; ";
                    strWarning += QString( "Border dir level exceed limit: %1 " ).arg( borderDir );
                }
             }

            if ( ((QDataItem*)(*it))->GetType()==QDataItem::Folder ){

                int folderLength = ((QDataItem*)(*it))->GetName().length();
                if( folderLength > borderISO ){
                    strWarning = QString( "Folder lenght exceed limit: %1 " ).arg( borderISO );
                }
                if( checkJoliet == true ){
                    if( folderLength > borderJoliet ){
                       strWarning = QString( "Joliet folder lenght exceed limit: %1 " ).arg( borderJoliet );
                    }
                }
                if( checkRockRidge == true ){
                    if( folderLength > bordeRockridge ){
                       strWarning = QString( "Rockridge folder lenght exceed limit: %1 " ).arg( bordeRockridge );
                    }
                }
                QStringList dirList = ((QDataItem*)(*it))->GetDiskFilePath().split( PATHSEPSTRING );
                if( dirList.size() > borderDir ){
                    strWarning += "; ";
                    strWarning += QString( "Border dir level exceed limit: %1 " ).arg( borderDir );
                }

            }

            if(strWarning.length()>0){
                ((QDataItem*)(*it))->setIconWarning( true, strWarning );
            }else{
                ((QDataItem*)(*it))->setIconWarning( false, strWarning );
            }


            ++it;
        }

    }


    //What is the best way to autocorrect? The lowest possible filename.
    //Unblocksignals
}

void MainWindow::resetFiles()
{
    activeMdiChild()->triggerReset();
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

void MainWindow::addDataTrack()
{
    activeMdiChild()->addDataTrack();
}

void MainWindow::addAudioTrack()
{
    activeMdiChild()->addAudioTrack();
}

void MainWindow::selectAll()
{
    activeMdiChild()->selectAll();
}

void MainWindow::reverseSelection()
{
    activeMdiChild()->reverseSelection();
}

//##############################################################################################################
//End Disk Content Functions
//##############################################################################################################

void MainWindow::fillSourceDriveList()
{
    for (int i = 0; i < mDiskList.count(); i++) {
        listBurnDevicesWidget->addItem(mDiskList[i]);
        if(isBurnDevice(mDiskList[i])){
            listBurnDevicesWidget->setItemIcon(i,QIcon(":/icons/burndrive16.png"));
        }else{
            listBurnDevicesWidget->setItemIcon(i,QIcon(":/icons/readdevice16.png"));
        }
    }
}

QString MainWindow::checkBurnDriveSelection()
{
    QString strDriveName = listBurnDevicesWidget->currentText();

    if(activeMdiChild() != nullptr){
        QDiskItem *diskItem = (QDiskItem *)activeMdiChild()->getTreeWidget()->topLevelItem(0);
        if( diskItem->getBurnDevice() == "" ){
            diskItem->setBurnDevice(strDriveName);
        }else{
            return diskItem->getBurnDevice();
        }
    }
    return strDriveName;

}

void MainWindow::fillReadDriveList(bool configCheck)
{

    fillBurnDriveList();

    mBurnDriveList.clear();
    ::EnableImageDevice(BS_FALSE);
    ::RescanDevices();
    TCHAR chListAllDevices[26][50];
    int32 nUsed = 0;
    int32 res = ::GetDevices(chListAllDevices, &nUsed, false);
    if (BS_SDK_ERROR_NO == res) {
      for (int i = 0; i < nUsed; i++) {
          mBurnDriveList.append(convertToQT(chListAllDevices[i]));
      }
    }

    listReadDevicesWidget->blockSignals(true);

    listReadDevicesWidget->clear();

    for (int i = 0; i < mBurnDriveList.count(); i++) {

        listReadDevicesWidget->addItem(QIcon(":/icons/readdevice16.png"),mBurnDriveList[i]);

    }

    if(configCheck == true){
        QString startUpDevice = ConfigurationPage::mSettings.value("CurrentReadDevice", tr("")).toString();
        int driveIndex = 0;
        if(startUpDevice.length()>0){
            driveIndex = listReadDevicesWidget->findText(startUpDevice, Qt::MatchExactly);
            if(driveIndex == -1){
                driveIndex = 0;
            }
        }
        listReadDevicesWidget->setCurrentIndex(driveIndex);
    }else{
        listReadDevicesWidget->setCurrentIndex(0);
    }

    listReadDevicesWidget->blockSignals(false);

}

void MainWindow::fillBurnDriveList(bool configCheck)
{
    mBurnDriveList.clear();
    ::EnableImageDevice(BS_TRUE);
    ::RescanDevices();
    TCHAR chListAllDevices[26][50];
    int32 nUsed = 0;
    int32 res = ::GetDevices(chListAllDevices, &nUsed, true);
    if (BS_SDK_ERROR_NO == res) {
      for (int i = 0; i < nUsed; i++) {
          mBurnDriveList.append(convertToQT(chListAllDevices[i]));
      }
    }

    listBurnDevicesWidget->blockSignals(true);

    listBurnDevicesWidget->clear();
    //QStandardItemModel *mod = new QStandardItemModel();

    for (int i = 0; i < mBurnDriveList.count(); i++) {

        listBurnDevicesWidget->addItem(QIcon(":/icons/burndrive16.png"),mBurnDriveList[i]);
    }

    //Set Burning Device(es) from project.
    //GetBurnDevices count = 0, den ersten selectieren
    if(configCheck == true){
        QString startUpDevice = ConfigurationPage::mSettings.value("CurrentBurnDevice", tr("")).toString();
        int driveIndex = 0;
        if(startUpDevice.length()>0){
            driveIndex = listBurnDevicesWidget->findText(startUpDevice, Qt::MatchExactly);
            if(driveIndex == -1){
                driveIndex = 0;
            }
        }
        listBurnDevicesWidget->setCurrentIndex(driveIndex);
    }else{
        listBurnDevicesWidget->setCurrentIndex(0);
    }

    listBurnDevicesWidget->blockSignals(false);

}

void MainWindow::updateReadDeviceSel(int index)
{
    /*
     * A chnage of the Read Device must not reach Subwindows.
     * We can handle it this way: Each drive can have a scan, or one scan and we change the drive.
     * We cannot run a scan in parallel mode, only just one.
     * So it makes more sense to open just one drive.
     */
    //Currently there is no dynamic drive change needed.

    //Scan und HEx senden Daten an Childs
    //MEdiaInfo und DeviceInfos senden keine Daten an die Childs

    QString strDriveName = listReadDevicesWidget->itemText(index);
    ConfigurationPage::mSettings.setValue("CurrentReadDevice", strDriveName);
    ConfigurationPage::mSettings.sync();

    if( activeMdiHexChild() != nullptr ){
        activeMdiHexChild()->setBurnDrive(strDriveName);
    }
    if( activeMdiScanChild() != nullptr ){
        activeMdiScanChild()->setBurnDrive(strDriveName);
    }

}

void MainWindow::updateBurnDeviceSel(int index)
{
    QString strDriveName = listBurnDevicesWidget->itemText(index);
    ConfigurationPage::mSettings.setValue("CurrentBurnDevice", strDriveName);
    ConfigurationPage::mSettings.sync();

    if(activeMdiChild() != nullptr){
        QDiskItem *diskItem = (QDiskItem *)activeMdiChild()->getTreeWidget()->topLevelItem(0);
        if(diskItem != nullptr)
            diskItem->setBurnDevice(strDriveName);
      }

    if(listBurnDevicesWidget->count()==0){
        eraseGeneralButton->setEnabled(false);
        burnDiskImage->setEnabled(false);
    }else{
        if(strDriveName.at(0) == '#'){
            eraseGeneralButton->setEnabled(false);
            burnDiskImage->setEnabled(false);
        }else{
            eraseGeneralButton->setEnabled(true);
            burnDiskImage->setEnabled(true);
        }
    }

    if(activeMdiChild() != nullptr){
        QDiskItem *diskItem = (QDiskItem *)activeMdiChild()->getTreeWidget()->topLevelItem(0);
        diskItem->setBurnDevice(strDriveName);
    }

}

//Set Default is done by newProject. Means, nothing is set.
void MainWindow::setDefaults()
{

    QString strDriveName = listBurnDevicesWidget->currentText();

    //If project we need to send to project and return
    if(activeMdiChild() != nullptr){
        QDiskItem *diskItem = (QDiskItem *)activeMdiChild()->getTreeWidget()->topLevelItem(0);
        if(diskItem != nullptr)
            diskItem->setBurnDevice(strDriveName);
        return;
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
          mDiskList.append(convertToQT(chListDevices[i]));
      }
    }

    mBurnDriveList.clear();
    ::RescanDevices();
    TCHAR chListAllDevices[26][50];
    nUsed = 0;
    res = ::GetDevices(chListAllDevices, &nUsed, true);
    if (BS_SDK_ERROR_NO == res) {
      for (int i = 0; i < nUsed; i++) {
          mBurnDriveList.append(convertToQT(chListAllDevices[i]));
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

    //listBurnDevicesWidget->clear();

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

//##############################################################################################################
//Helper Functions
//##############################################################################################################

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName, bool isFile)
{
    QString canonicalFilePath;
    if (isFile)
        canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    else
        canonicalFilePath = fileName;

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChildDialog *mdiChild = qobject_cast<MdiChildDialog *>(window);
        if (nullptr != mdiChild){
            if (mdiChild->currentFile() == canonicalFilePath)
                return window;
        }
    }
    return nullptr;
}

MdiChildDialog *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->currentSubWindow()){
        return qobject_cast<MdiChildDialog *>(activeSubWindow);
    }
    return nullptr;
}

MdiChildDiskInfo *MainWindow::activeMdiInfoChild()
{
    //QMdiSubWindow *activeSubWindow = mdiArea->currentSubWindow();

    if (QMdiSubWindow *activeSubWindow = mdiArea->currentSubWindow()){
        return qobject_cast<MdiChildDiskInfo *>(activeSubWindow);
    }
    return nullptr;
}

MdiChildDeviceInfo *MainWindow::activeMdiDeviceChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->currentSubWindow()){
        return qobject_cast<MdiChildDeviceInfo *>(activeSubWindow);
    }
    return nullptr;
}


MdiChildHex *MainWindow::activeMdiHexChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->currentSubWindow()){
        return qobject_cast<MdiChildHex *>(activeSubWindow);
    }
    return nullptr;
}

MdiChildScan *MainWindow::activeMdiScanChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->currentSubWindow()){
        return qobject_cast<MdiChildScan *>(activeSubWindow);
    }
    return nullptr;
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;

    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

bool MainWindow::hasProjectWindow()
{

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChildDialog *mdiChild = qobject_cast<MdiChildDialog *>(window);
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

//##############################################################################################################
//End Helper Functions
//##############################################################################################################

//##############################################################################################################
//UI Management Functions
//##############################################################################################################

void MainWindow::onActiveChildChanged(QMdiSubWindow *activeSubWindow)
{
    qDebug() << "ActiveChildChanged";
    //Eigentlich müssen wir hier erst aktiv werden wenn die Fenster sich geänder tahebn.
    //Wenn last und new gleich dann sollten wir aussteigen.



    viewCloseAllbutton->setEnabled(mdiArea->subWindowList().count()==0?false:true);

    if(mdiArea->subWindowList().count()<=1){
        viewTileButton->setEnabled(false);
        viewCascadeButton->setEnabled(false);
        viewSwitchButton->setEnabled(false);
    }else{
        viewTileButton->setEnabled(true);
        viewCascadeButton->setEnabled(true);
        viewSwitchButton->setEnabled(true);
    }

    if(mdiArea->subWindowList().count()==0){
        ui->ribbonTabWidget->hideAll();
        ui->ribbonTabWidget->currentTab(tr("Project"));
        delEditButton->setEnabled(false);
        renameEditButton->setEnabled(false);
        viewBrowserButton->setEnabled(false);

        delAllEditButton->setEnabled(false);
        updtEditButton->setEnabled(false);
        inverseSelectEditButton->setEnabled(false);
        allSelectEditButton->setEnabled(false);
        addFolderEditButton->setEnabled(false);
        addFileEditButton->setEnabled(false);
        createFolderEditButton->setEnabled(false);
        dataTrackEditButton->setEnabled(false);
        audioTrackEditButton->setEnabled(false);
        appSaveButton->setEnabled(false);
        appSaveAsButton->setEnabled(false);
        imageMediaButton->setEnabled(false);
        grabAudioMediaButton->setEnabled(false);
        burnGeneralButton->setEnabled(false);

        return;
    }

    if( activeSubWindow == nullptr ) {
        return;
    }

    mdiArea->blockSignals(true);

    if(activeMdiChild() != nullptr){
        activeMdiChild()->setRibbonTabs(ui->ribbonTabWidget,this);
        mdiArea->blockSignals(false);
        return;
    }
    if( activeMdiInfoChild() != nullptr ){
        activeMdiInfoChild()->setRibbonTabs(ui->ribbonTabWidget,this);
        mdiArea->blockSignals(false);
        return;
    }
    if( activeMdiScanChild() != nullptr ){
        activeMdiScanChild()->setRibbonTabs(ui->ribbonTabWidget,this);
        mdiArea->blockSignals(false);
        return;
    }
    if( activeMdiHexChild() != nullptr ){
        activeMdiHexChild()->setRibbonTabs(ui->ribbonTabWidget,this);
        mdiArea->blockSignals(false);
        return;
    }
    if( activeMdiDeviceChild() != nullptr ){
        activeMdiDeviceChild()->setRibbonTabs(ui->ribbonTabWidget,this);
        mdiArea->blockSignals(false);
        return;
    }

    mdiArea->blockSignals(false);

}


//##############################################################################################################
//End UI Management Functions
//##############################################################################################################

//##############################################################################################################
//Burn Disc image Functions
//##############################################################################################################

void MainWindow::doBurnImage()
{

    QString fileName = QFileDialog::getOpenFileName(
               this,
               tr("Disc image file"),
               QDir::currentPath(),
               tr("All files (*.iso *.cue );;ISO Image (*.iso *.iso);;Bin/Cue Image (*.cue)") );

    if (fileName.isEmpty())return;

    DBJobCommands burnImageCommands;
    burnImageCommands.bAutoErase = autoEraseGeneralCheck->isChecked();
    burnImageCommands.bEjectAfterBurn = ejectAfterGeneralCheck->isChecked();
    burnImageCommands.bUnderrunProtection = burnProofGeneralCheck->isChecked();
    burnImageCommands.bPerformOPC = activeOPCGeneralCheck->isChecked();
    burnImageCommands.imagePath = fileName;

    burnDialog *dialog = new burnDialog( DB_JOB_BURNIMAGE, &burnImageCommands, nullptr, this );
    dialog->exec();
}


//##############################################################################################################
//Erase Functions
//##############################################################################################################

void MainWindow::doErase()
{

    DBJobCommands eraseCommands;
    eraseCommands.ejectErase = ejectAfterErase->isChecked();
    eraseCommands.eraseFast = eraseFast->isChecked();
    eraseCommands.burnDevice = checkBurnDriveSelection();


    burnDialog *dialog = new burnDialog( DB_JOB_ERASE, &eraseCommands, nullptr, this );
    dialog->exec();

}

//##############################################################################################################
//End Burn Functions
//#############################################################################################################

//##############################################################################################################
//Burn Functions
//##############################################################################################################

void MainWindow::doBurn()
{

    DBJobCommands eraseCommands;
    eraseCommands.burnDevice = checkBurnDriveSelection();

    burnDialog *dialog = new burnDialog( DB_JOB_BURN, &eraseCommands, activeMdiChild()->getTreeWidget(), this );
    dialog->exec(); //show();
    //Show non modal, exec is modal
}

//##############################################################################################################
//End Burn Functions
//##############################################################################################################

//##############################################################################################################
//Disc FS Functions
//##############################################################################################################
void MainWindow::fsIsoTypeChanged(int nIndex)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setISOFsType(nIndex);
        //Because we set it in FSSystem and Bootimage, we need to uodate all.
        //updateProjectRibbon();

        int isoLevel = isoLevelFSCombo->itemData(isoLevelFSCombo->currentIndex()).toInt();
        switch(isoLevel){
            case BS_ISO_LEVEL_1:
            case BS_ISO_LEVEL_2:
                isoManyDirectoriesFSCheck->setEnabled(true);
                isoManyDirectoriesFSCheck->setChecked(isoManyDirectoriesFSCheck->isChecked()==true?1:0);

                isoLongFileNamesFSCheck->setEnabled(true);
                isoLongFileNamesFSCheck->setChecked(isoLongFileNamesFSCheck->isChecked()==true?1:0);

                useJolietFSCheck->setEnabled(true);
                useJolietFSCheck->setChecked(useJolietFSCheck->isChecked()==true?1:0);

                jolietLongNamesFSCheck->setEnabled(true);
                if(useJolietFSCheck->isChecked()==true){
                    jolietLongNamesFSCheck->setEnabled(true);
                }else{
                    jolietLongNamesFSCheck->setEnabled(false);
                }
                jolietLongNamesFSCheck->setChecked(jolietLongNamesFSCheck->isChecked()==true?1:0);

                useRockRidgeFSCheck->setEnabled(true);
                useRockRidgeFSCheck->setChecked(useRockRidgeFSCheck->isChecked()==true?1:0);

                isoLongPathFSCheck->setEnabled(true);
                isoLongPathFSCheck->setChecked(isoLongPathFSCheck->isChecked()==true?1:0);
               break;
            case BS_ISO_LEVEL_3:
                isoManyDirectoriesFSCheck->setEnabled(true);
                isoManyDirectoriesFSCheck->setChecked(true);
                isoLongFileNamesFSCheck->setEnabled(true);
                isoLongFileNamesFSCheck->setChecked(true);
                useJolietFSCheck->setEnabled(true);
                useJolietFSCheck->setChecked(useJolietFSCheck->isChecked()==true?1:0);
                if(useJolietFSCheck->isChecked()==true){
                    jolietLongNamesFSCheck->setEnabled(true);
                }else{
                    jolietLongNamesFSCheck->setEnabled(false);
                }
                jolietLongNamesFSCheck->setChecked(true);

                useRockRidgeFSCheck->setEnabled(true);
                useRockRidgeFSCheck->setChecked(useRockRidgeFSCheck->isChecked()==true?1:0);
                isoLongPathFSCheck->setEnabled(true);
                isoLongPathFSCheck->setChecked(true);
            break;
            case BS_ISO_LEVEL_ROMEO:
                isoManyDirectoriesFSCheck->setEnabled(false);
                isoManyDirectoriesFSCheck->setChecked(false);
                jolietLongNamesFSCheck->setEnabled(false);
                jolietLongNamesFSCheck->setChecked(false);
                isoLongFileNamesFSCheck->setEnabled(false);
                isoLongFileNamesFSCheck->setChecked(false);
                isoLongPathFSCheck->setEnabled(false);
                isoLongPathFSCheck->setChecked(false);
                //No Extension allowed
                useJolietFSCheck->setEnabled(false);
                useJolietFSCheck->setChecked(false);
                useRockRidgeFSCheck->setEnabled(false);
                useRockRidgeFSCheck->setChecked(false);
            break;
        }

        checkFileSystemNaming();


    }
}

void MainWindow::fsJolietChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setUseJoliet(nState?1:0);
        jolietLongNamesFSCheck->setEnabled(nState?true:false);
        checkFileSystemNaming();
    }
}

void MainWindow::fsRockRidgeChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setUseRockRidge(nState?1:0);
        checkFileSystemNaming();
    }
}

void MainWindow::fsManyDirectories(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setAllowManyDirectories(nState?1:0);
        checkFileSystemNaming();
    }
}

void MainWindow::fsLongFileNamesChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setAllowLongFileNames(nState?1:0);
        checkFileSystemNaming();
    }
}

void MainWindow::fsNotWriteISO1Extension(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setNotWriteISO1Extension(nState?1:0);
    }
}

void MainWindow::fsLongJolietFileNamesChanged(int nState)
{
    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){
        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));
        diskItem->setAllowLongJolietFileNames(nState?1:0);
        checkFileSystemNaming();
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

//##############################################################################################################
//End Disc FS Functions
//##############################################################################################################

//##############################################################################################################
//Burn Feature Functions
//##############################################################################################################
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

//##############################################################################################################
//End Burn Feature Functions
//##############################################################################################################

//##############################################################################################################
//Boot Functions
//##############################################################################################################


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
            emit onActiveChildChanged( activeMdiChild() );
        }


    }
}


void MainWindow::clickedSelBootImagePath()
{

    QDiskItem *diskItem = nullptr;
    if(activeMdiChild() != nullptr){

        diskItem = static_cast<QDiskItem *>(activeMdiChild()->getTreeWidget()->topLevelItem(0));

        QString fileName = QFileDialog::getOpenFileName(
                   this,
                   tr("Open Boot Image"),
                   QDir::currentPath(),
                   tr("Boot Image (*.img *.ima)") );

        if (fileName.isEmpty())return;

        if(diskItem!=nullptr){
            imagePathBootEdit->setText(fileName);
            diskItem->setBootDiskFile(fileName);
            diskItem->setDoBootDisk(true); //Trigger update
            emit onActiveChildChanged( activeMdiChild() );
        }

    }

}

//##############################################################################################################
//End Boot Functions
//##############################################################################################################

//##############################################################################################################
//FoxSDK Functions
//##############################################################################################################

void MainWindow::closeTray(bool readDevice)
{
    QString strDriveName = "";

    int32 res = -1;
    if( readDevice == true ){
        strDriveName = listReadDevicesWidget->currentText();
        ::SetReadDevice(strDriveName.at(0).toLatin1());
        res = ::CloseDevice( BS_READ_DEVICE );
    }else{
        strDriveName = listBurnDevicesWidget->currentText();
        ::SetBurnDevice(strDriveName.at(0).toLatin1());
        res = ::CloseDevice( BS_CURRENT_DEVICE );
    }

    if ( res != BS_SDK_ERROR_NO ){
        onHandleError(res);
    }
}

void MainWindow::ejectTray(bool readDevice)
{
    QString strDriveName = "";

    int32 res = -1;
    if( readDevice == true ){
        strDriveName = listReadDevicesWidget->currentText();
        ::SetReadDevice(strDriveName.at(0).toLatin1());
        res = ::EjectDevice( BS_READ_DEVICE );
    }else{
        strDriveName = listBurnDevicesWidget->currentText();
        ::SetBurnDevice(strDriveName.at(0).toLatin1());
        res = ::EjectDevice( BS_CURRENT_DEVICE );
    }

    if ( res != BS_SDK_ERROR_NO ){
        onHandleError(res);
    }

}

void MainWindow::onHandleError( int32 res )
{
    TCHAR chError[2048];
    int32 nLength = 2048;

    // Get error description
    ::GetText( res, chError, &nLength );

    QString strError = QString( "SDK Error: %1" ).arg(convertToQT( chError ) );

    QMessageBox::information( this, tr("Diskbutler Error"), strError );
}

//##############################################################################################################
//End FoxSDK Functions
//##############################################################################################################

//##############################################################################################################
//Default Software Functions
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
        updateStatusBarText(tr("File loaded"),false);
        child->show();
        child->showMaximized();
    } else {
        child->close();
    }

     mdiArea->setActiveSubWindow(child);
}

void MainWindow::save()
{
    if ( activeMdiChild() && activeMdiChild()->save() ){
        updateStatusBarText( tr("File saved"), false );
    }
}

void MainWindow::saveAs()
{

    if ( activeMdiChild() && activeMdiChild()->saveAs() ){
        updateStatusBarText( tr("File saved"), false );
    }

}

void MainWindow::openSettings()
{
    ConfigDialog *dialog = new ConfigDialog;
    dialog->exec();

}

void MainWindow::closeDiskbutler()
{

    QApplication::quit();

}

//##############################################################################################################
//End Default Software Functions
//##############################################################################################################

//##############################################################################################################
//ISO Ex Settings Functions
//##############################################################################################################
void MainWindow::isoExImportClicked()
{

    edIsoExSystemIdValue->setText( ConfigurationPage::mSettings.value( "systemId", "" ).toString() );
    edIsoExVolumeSetValue->setText( ConfigurationPage::mSettings.value( "volumeSet", "" ).toString() );
    edIsoExPublisherValue->setText( ConfigurationPage::mSettings.value( "publisher", "" ).toString() );
    edIsoExDataPreparerValue->setText( ConfigurationPage::mSettings.value( "datapreparer", "" ).toString() );
    edIsoExApplicationValue->setText( ConfigurationPage::mSettings.value( "app", "" ).toString() );
    edIsoExCopyrightFileValue->setText( ConfigurationPage::mSettings.value( "corightfile", "" ).toString() );
    edIsoExAbstractFileValue->setText( ConfigurationPage::mSettings.value( "abstractfile", "" ).toString() );
    edIsoExBibliographicFileValue->setText( ConfigurationPage::mSettings.value( "bibliographicfile", "" ).toString() );

}

void MainWindow::isoExReset()
{

    edIsoExSystemIdValue->setText( "" );
    edIsoExVolumeSetValue->setText( "" );
    edIsoExPublisherValue->setText( "" );
    edIsoExDataPreparerValue->setText( "" );
    edIsoExApplicationValue->setText( "" );
    edIsoExCopyrightFileValue->setText( " ");
    edIsoExAbstractFileValue->setText( "" );
    edIsoExBibliographicFileValue->setText( "" );

}

void MainWindow::isoExSystemIdchanged( QString strText )
{

    QDiskItem *diskItem = nullptr;
    if( activeMdiChild() != nullptr ){
        diskItem = static_cast<QDiskItem *>( activeMdiChild()->getTreeWidget()->topLevelItem( 0 ) );
        diskItem->setSystemId( strText );
    }

}

void MainWindow::isoExVolumechanged( QString strText )
{

    QDiskItem *diskItem = nullptr;
    if( activeMdiChild() != nullptr ){
        diskItem = static_cast<QDiskItem *>( activeMdiChild()->getTreeWidget()->topLevelItem( 0 ) );
        diskItem->setVolumeSet( strText );
    }

}

void MainWindow::isoExPublisherchanged( QString strText )
{

    QDiskItem *diskItem = nullptr;
    if( activeMdiChild() != nullptr ){
        diskItem = static_cast<QDiskItem *>( activeMdiChild()->getTreeWidget()->topLevelItem( 0 ) );
        diskItem->setPublisher( strText );
    }

}

void MainWindow::isoExDataPreparerchanged( QString strText )
{

    QDiskItem *diskItem = nullptr;
    if( activeMdiChild() != nullptr ){
        diskItem = static_cast<QDiskItem *>( activeMdiChild()->getTreeWidget()->topLevelItem( 0 ) );
        diskItem->setDatapreparer( strText );
    }

}

void MainWindow::isoExApplicationchanged( QString strText )
{

    QDiskItem *diskItem = nullptr;
    if( activeMdiChild() != nullptr ){
        diskItem = static_cast<QDiskItem *>( activeMdiChild()->getTreeWidget()->topLevelItem( 0 ) );
        diskItem->setApplication( strText );
    }

}

void MainWindow::isoExCopyrightFilechanged( QString strText )
{

    QDiskItem *diskItem = nullptr;
    if( activeMdiChild() != nullptr ){
        diskItem = static_cast<QDiskItem *>( activeMdiChild()->getTreeWidget()->topLevelItem( 0 ) );
        diskItem->setCoprightFile( strText );
    }

}

void MainWindow::isoExAbstractFilechanged( QString strText )
{

    QDiskItem *diskItem = nullptr;
    if( activeMdiChild() != nullptr ){
        diskItem = static_cast<QDiskItem *>( activeMdiChild()->getTreeWidget()->topLevelItem( 0 ) );
        diskItem->setAbstractFile( strText );
    }

}

void MainWindow::isoExBibliographicFilechanged( QString strText )
{
    QDiskItem *diskItem = nullptr;
    if( activeMdiChild() != nullptr ){
        diskItem = static_cast<QDiskItem *>( activeMdiChild()->getTreeWidget()->topLevelItem( 0 ) );
        diskItem->setBibliographicFile( strText );
    }

}

//##############################################################################################################
//End ISO Ex Settings
//##############################################################################################################

//##############################################################################################################
//Check actual version o Network
//##############################################################################################################

void MainWindow::onNewVersionAvail(QString newVersion)
{
    //QMessageBox::information(this, tr("New Version"),newVersion);
    QMessageBox::information(this, tr("Diskbutler new nersion"),tr("Pupsi version"));

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
//Disk Viewer (Hex) Functions
//##############################################################################################################
void MainWindow::hexReadSector( double nValue )
{

    if( activeMdiHexChild() != nullptr ){
        activeMdiHexChild()->readSectorEx( nValue );
        activeMdiHexChild()->setSector( nValue );

        if( nValue == 0 ){
            navFirstHexEditor->setEnabled( false );
            navStepBackHexEditor->setEnabled( false );
        }else{
            navFirstHexEditor->setEnabled( true );
            navStepBackHexEditor->setEnabled( true );
        }
        if( nValue == activeMdiHexChild()->getMaxSector() ){
            navLastHexEditor->setEnabled( false );
            navStepForwardHexEditor->setEnabled( false );
        }else{
            navLastHexEditor->setEnabled( true );
            navStepForwardHexEditor->setEnabled( true );
        }
    }

}

void MainWindow::actHexNextSector()
{

    if( activeMdiHexChild() != nullptr ){
        double cSector = hexSector->value();
        cSector++;
        hexSector->setValue( cSector );
    }

}

void MainWindow::actHexFirstSector()
{

    if( activeMdiHexChild() != nullptr ){
        hexSector->setValue( 0 );
    }

}

void MainWindow::actHexLastSector()
{

    if( activeMdiHexChild() != nullptr ){
        hexSector->setValue( activeMdiHexChild()->getMaxSector() );
    }

}

void MainWindow::actHexPrevSector()
{

    if( activeMdiHexChild() != nullptr ){
        double cSector = hexSector->value();
        cSector--;
        hexSector->setValue( cSector );
    }

}

void MainWindow::actHexUpdate()
{

    if( activeMdiHexChild() != nullptr ){
        hexSector->blockSignals( true ); //Shush, block your signals
        hexSector->setValue( 0 );


        activeMdiHexChild()->startHexThread();
        hexSector->blockSignals( false ); //OK, emit from now on
    }

}

void MainWindow::actHexSaveLog()
{

    if( activeMdiHexChild() != nullptr ){
        activeMdiHexChild()->saveBlock();
    }

}

void MainWindow::updatedHexData()
{

    if( activeMdiHexChild() != nullptr ){
        hexSector->setRange( 0, activeMdiHexChild()->getMaxSector() );
        hexSector->setValue( 0 );
    }

}

void MainWindow::openHexEditor()
{
    if( listReadDevicesWidget->count() == 0 ) return;

    QString strDriveName = listReadDevicesWidget->itemText( listReadDevicesWidget->currentIndex() );

    foreach ( QMdiSubWindow *window, mdiArea->subWindowList() ) {
      MdiChildHex *mdiChild = qobject_cast<MdiChildHex *>( window );
      if ( nullptr != mdiChild ){
          //We have now a DeviceInfo child.
          //We will check now the drive. If the drive is the same, we pop up the mdichild to top.
          //And return
          //else we will create a new one
          //RuleManager::ProjectType myType = mdiChild->GetProjectType();
          //if(strDriveName.at(0)==mdiChild->getBurnDrive().at(0)){
            mdiArea->setActiveSubWindow( window );
            return;
          //}

      }
    }

    MdiChildHex *child = new MdiChildHex( this, strDriveName );

    connect( child, SIGNAL( startSpinner() ), this, SLOT( startWaitSpinner() ) );
    connect( child, SIGNAL( stopSpinner() ), this, SLOT( stopWaitSpinner() ) );
    connect( child, SIGNAL( datatrack_changed() ), this, SLOT( onUpdateRibbonControls() ) );


    mdiArea->addSubWindow( child );


    if( isCascaded() == false ){
        child->showMaximized();
    }else{
        child->showNormal();
    }

    mdiArea->setActiveSubWindow( child );
    
}

//##############################################################################################################
//End Disk Viewer (Hex) Functions
//##############################################################################################################

//##############################################################################################################
//Disk Scan Functions
//##############################################################################################################

void MainWindow::enableScanTabControls()
{
    startScanEditor->setEnabled( true );
    stopScanEditor->setEnabled( false );
    saveScanEditor->setEnabled( true );
    updateScanEditor->setEnabled( true );
    scanStartOffset->setEnabled( true );
    scanReadOffset->setEnabled( true );

}

void MainWindow::disableScanTabControls()
{
    startScanEditor->setEnabled( false );
    stopScanEditor->setEnabled( true );
    saveScanEditor->setEnabled( false );
    updateScanEditor->setEnabled( false );
    scanStartOffset->setEnabled( false );
    scanReadOffset->setEnabled( false );

    //Wir müssen der UI den status mitteilen
    /*
     * Also, wenn der scan läuft, dann müssen wir das Fenster WEchsel abfangen und ggf. verindern.
     * Wenn das Fenster gewechselt wird stoppen wir den Scan.
     *
     * Wie geben wir den Status an die UI:
     * Also wir sollten den Analyse auf jedenfall mit dem SPinner blockieren.
     * Also während des Analyze VOrgangs müssen wir die Elemente nicht steuern nur den Satus setzen.
     *
     */
}

void MainWindow::scanStartOffsetValue( double nValue )
{
    if( activeMdiScanChild() != nullptr ){
        activeMdiScanChild()->setSectorOffset( nValue );
    }
}

void MainWindow::scanReadOffsetValue( double nValue )
{
    if( activeMdiScanChild() != nullptr ){
        activeMdiScanChild()->setReadOffset( nValue );
    }
}

void MainWindow::scanStartScan()
{
    if( activeMdiScanChild() != nullptr ){

        activeMdiScanChild()->onStartScan( scanStartOffset->value(), scanReadOffset->value() );
        disableScanTabControls();
    }
}

void MainWindow::scanStopScan()
{
    if( activeMdiScanChild() != nullptr ){

        activeMdiScanChild()->onStop();
        enableScanTabControls();

    }
}

void MainWindow::scanSaveLog()
{
    if( activeMdiScanChild() != nullptr ){

        activeMdiScanChild()->onWriteReport();
    }
}

void MainWindow::scanUpdateDiskInfo()
{
    if( activeMdiScanChild() != nullptr ){

        activeMdiScanChild()->onUpdateSectors();
    }
}

void MainWindow::openScanDialog()
{
    if( listReadDevicesWidget->count() == 0 ) return;

    //Then we check if there is a selction
    //QListWidgetItem* itemFromList = listReadDevicesWidget->currentItem();
    //if(itemFromList==nullptr) return;

    QString strDriveName = listReadDevicesWidget->itemText(listReadDevicesWidget->currentIndex());

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
      MdiChildScan *mdiChild = qobject_cast<MdiChildScan *>(window);
      if (nullptr != mdiChild){
          //We have now a DeviceInfo child.
          //We will check now the drive. If the drive is the same, we pop up the mdichild to top.
          //And return
          //else we will create a new one
          //RuleManager::ProjectType myType = mdiChild->GetProjectType();
          //if(strDriveName.at(0)==mdiChild->getBurnDrive().at(0)){
          mdiArea->setActiveSubWindow(window);
          return;
          //}

      }
    }

    MdiChildScan *child = new MdiChildScan(strDriveName);

    connect( child, SIGNAL( startSpinner() ), this, SLOT( startWaitSpinner() ) );
    connect( child, SIGNAL( stopSpinner() ), this, SLOT( stopWaitSpinner() ) );
    connect( child, SIGNAL( enableControls() ), this, SLOT( enableScanTabControls() ) );
    connect( child, SIGNAL( datatrack_changed2() ), this, SLOT( onUpdateRibbonControls() ) );

    mdiArea->addSubWindow( child );

    if( isCascaded() == false ){
        child->showMaximized();
    }else{
        child->showNormal();
    }

    mdiArea->setActiveSubWindow( child );
}

//##############################################################################################################
//End Disk Scan Functions
//##############################################################################################################

//##############################################################################################################
//Disk Info Functions / DiscImage
//##############################################################################################################
void MainWindow::updateDiskInfo()
{

    if( activeMdiInfoChild() ){
        activeMdiInfoChild()->updateDiskInfo();
    }
}

void MainWindow::updateDiskInfoMenu()
{
    m_diskInfoMenu->clear();

    if( activeMdiInfoChild() ){
        QAction* action  = m_diskInfoMenu->addAction( tr("Update") );
        action->setCheckable( false );
        connect( action, &QAction::triggered, [=] { updateDiskInfo(); } );
    }

}

void MainWindow::release2ImageTab()
{
    qDebug() << "Release2ImageTab";
    ui->ribbonTabWidget->showTab(":/icons/create_image32.png","Create image");
    ui->ribbonTabWidget->currentTab(tr("Create image"));

    //Build the Tab well.
    //Dieses Tab wird im Vergleich zu den anderen nur hier gebaut und nicht beim Wechsel.
    //Beim wechsel wird das Tab versteckt.
    if(activeMdiInfoChild()){
        imageReadHardRetry->setValue(activeMdiInfoChild()->getImageCreateHardRetry());
        imageReadSoftRetry->setValue(activeMdiInfoChild()->getImageCreateSoftRetry());

        imageReadErrorSwitch->setCheckState(activeMdiInfoChild()->getImageCorrSwitch()? Qt::Checked : Qt::Unchecked);
        if(activeMdiInfoChild()->getImageCorrSwitch() == 0){
            imageReadHardRetry->setEnabled(false);
            imageReadSoftRetry->setEnabled(false);
        }else{
            imageReadHardRetry->setEnabled(true);
            imageReadSoftRetry->setEnabled(true);
        }

        imageReadJobVerify->setCheckState(activeMdiInfoChild()->getImageJobVerify()? Qt::Checked : Qt::Unchecked);
        imageReadJobCreate->setCheckState(activeMdiInfoChild()->getImageJobCreate()? Qt::Checked : Qt::Unchecked);

        imageFormatCombo->clear();
        if(activeMdiInfoChild()->getIsIsoDisk()==true){
            imageFormatCombo->addItem("ISO Image",BS_IMG_ISO);
        }
        if(activeMdiInfoChild()->getIsBinDisk()==true){
            imageFormatCombo->addItem("Bin Image",BS_IMG_BIN);
        }

    }
}

void MainWindow::jobVerifyChanged( int nState )
{
    if( activeMdiInfoChild() ){
        activeMdiInfoChild()->setImageJobVerify( nState );
    }
}

void MainWindow::jobCreateChanged( int nState )
{
    if( activeMdiInfoChild() ){
        activeMdiInfoChild()->setImageJobCreate( nState );
    }
}

void MainWindow::changeReadHardRetry( double nValue )
{
    if( activeMdiInfoChild() ){
        activeMdiInfoChild()->setImageCreateHardRetry( nValue );
    }
}

void MainWindow::changeReadSoftRetry( double nValue )
{
    if( activeMdiInfoChild() ){
        activeMdiInfoChild()->setImageCreateSoftRetry( nValue );
    }
}

void MainWindow::errorCorrectionChanged( int nState )
{
    if( activeMdiInfoChild() ){
        activeMdiInfoChild()->setImageCorrSwitch( nState );
        if( activeMdiInfoChild()->getImageCorrSwitch() == 0 ){
            imageReadHardRetry->setEnabled( false );
            imageReadSoftRetry->setEnabled( false );
        }else{
            imageReadHardRetry->setEnabled( true );
            imageReadSoftRetry->setEnabled( true );
        }
    }
}

void MainWindow::imageCreateMethod( int iIndex )
{
    if( activeMdiInfoChild() ){
        activeMdiInfoChild()->setImageCreateMethod( imageFormatCombo->itemData( iIndex ).toInt() );
    }
}

void MainWindow::onCreateDiscImage()
{
    //connect(imageFormatCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(imageCreateMethod(int)));
    //connect(imageReadErrorSwitch, SIGNAL(stateChanged(int)), this, SLOT(errorCorrectionChanged(int)));
    //connect(imageReadJobCreate, SIGNAL(stateChanged(int)), this, SLOT(jobCreateChanged(int)));
    //connect(imageReadJobVerify, SIGNAL(stateChanged(int)), this, SLOT(jobVerifyChanged(int)));
    //connect(startImageCreateJob, SIGNAL(clicked()), this, SLOT(onCreateDiscImage()));

    //Wir müssen  zuerst die Daten hier sammeln.
    //AM Besten aus dem MDIFInfo, so dass alle Änderungen direkt dort gespeichert werden.


    if(activeMdiInfoChild()){
        QString mFilter;
        if(activeMdiInfoChild()->getImageCreateMethod()==1){
            mFilter = tr("ISO file (*.iso)");
        }
        if(activeMdiInfoChild()->getImageCreateMethod()==2){
            mFilter = tr("BIN file (*.bin)");
        }
        //Collect Data and start.

        QString fileName =  QFileDialog::getSaveFileName(
                        this,
                        tr("Disc Image"),
                        QDir::currentPath(),
                        mFilter,&mFilter );

        if (fileName.isEmpty())
          return;

        //Ok, the filedialog is good, if another suffic is added, it tel the user not this way.
        //So we need only handle the issue that the default suffix is set
        activeMdiInfoChild()->setImagePath(fileName);

        //activeMdiInfoChild()->setImageCreateMethod(imageFormatCombo->itemData(imageFormatCombo->currentIndex()).toInt());
        burnDialog *dialog = new burnDialog( DB_JOB_IMAGECREATE, tr(""), activeMdiInfoChild(), this);
        dialog->exec(); //show();
    }
}

void MainWindow::newDiskInfo()
{
    //First we check if there is something to view
    if(listReadDevicesWidget->count()==0) return;

    //Then we check if there is a selction
    //QListWidgetItem* itemFromList = listReadDevicesWidget->currentItem();
    //if(itemFromList==nullptr) return;

    //QString strDriveName = itemFromList->text();
    QString strDriveName = listReadDevicesWidget->itemText(listReadDevicesWidget->currentIndex());


    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
      MdiChildDiskInfo *mdiChild = qobject_cast<MdiChildDiskInfo *>(window);
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

    //OK wor müssen aufpassen, die Fenster sind noch nicht erstellt, also kann das Fenster auch nicht auslösen.
    connect(child, SIGNAL(startSpinner()), this, SLOT(startWaitSpinner()));
    connect(child, SIGNAL(stopSpinner()), this, SLOT(stopWaitSpinner()));
    connect(child, SIGNAL(datatrack_changed()), this, SLOT(onUpdateRibbonControls()));


    //We will create the child and set the errorhandle inside the class.
    //If success, we call addSubWindow, if not we will delete it.
    mdiArea->addSubWindow(child);

    if(isCascaded()==false){
        child->showMaximized();
     }else{
        child->showNormal();
     }

    mdiArea->setActiveSubWindow(child);

}

//##############################################################################################################
//End Disk Info Functions / DiscImage
//##############################################################################################################

//##############################################################################################################
//Device Info Functions
//##############################################################################################################

void MainWindow::newDeviceInfo()
{

    //First we check if there is something to view
    if( listReadDevicesWidget->count() == 0 ) return;

    //Then we check if there is a selction
    //QListWidgetItem* itemFromList = listReadDevicesWidget->currentItem();
    //if(itemFromList==nullptr) return;

    //QString strDriveName = itemFromList->text();
    QString strDriveName = listReadDevicesWidget->itemText( listReadDevicesWidget->currentIndex() );

    foreach ( QMdiSubWindow *window, mdiArea->subWindowList() ) {
        MdiChildDeviceInfo *mdiChild = qobject_cast<MdiChildDeviceInfo *>( window );
        if ( nullptr != mdiChild ){
            //We have now a DeviceInfo child.
            //We will check now the drive. If the drive is the same, we pop up the mdichild to top.
            //And return
            //else we will create a new one
            //RuleManager::ProjectType myType = mdiChild->GetProjectType();
            if( strDriveName.at( 0 ) == mdiChild->getBurnDrive().at( 0 ) ){
                mdiArea->setActiveSubWindow( window );
                return;
            }

        }
    }

    MdiChildDeviceInfo *child = new MdiChildDeviceInfo( this, strDriveName );
    mdiArea->addSubWindow( child );

    if( isCascaded() == false ){
        child->showMaximized();
    }else{
        child->showNormal();
    }

    mdiArea->setActiveSubWindow( child );

}

//##############################################################################################################
//End Device Info Functions
//##############################################################################################################

//##############################################################################################################
//Additional UI Assets
//##############################################################################################################

void MainWindow::startWaitSpinner()
{
    mBlockSpinner->start();
}

void MainWindow::stopWaitSpinner()
{
    mBlockSpinner->stop();
}

//##############################################################################################################
//End Additional UI Assets
//##############################################################################################################


void MainWindow::mdiAreaActivationTemplate()
{
    if( mdiArea->subWindowList().count() == 0 ) {
        return;
    }

    QMdiSubWindow *activeSubWindow = mdiArea->currentSubWindow();
    if( activeSubWindow != nullptr ) {
        mdiArea->setActiveSubWindow( activeSubWindow );
    }

}


/*
 * Todo inside this
 * Import Session, wie managen wir das?
 *
 * Die eigenen TExtelemente bei Scan etc. auf Linux und Mac prüfen.
 *
 *
 * 2 Image: Default Werte aus den DriveInfoEx eintragen, Create als Default.
 *
 *
 *Also, FileProperties werden wohl beim Hinzufügen von Dateien direkt geschrieben.
 *Diese müssen im Splitter neben die Dateien der Disk angezeigt werden, weil wir die Info
 *nicht im Explorer haben. Warum auch.
 *
 *2Image geht weg wenn dailog wieder nach vorne kommt.
 *
 *
 *Noch die Settings aus den Projekten in die Tabs übernhemen. //LAufwerke
 *
 *
 *WEnn man das Media öffnet mit Disk, dann sollte ja hasdata = 1 kommen und 2 image an, geht aber nicht an
 *Emit kommt nicht an. WElches emit wird da gebraucht.
 *
 *Wenn ich von Project auf Diskinfo wechsel ist  Diskimage weg.
 *
 *Funktionen die gehen sollen
 *Burn von Data Projekten
 *Burn von ISO / Cue Images
 *Speichern einer Disk zu einem Image (CreateImage)
 *Disc Scan
 *Hex Editor
 *Erase
 *Open / Close Tray
 *
 *
 *Tab General bleibt als Dauerbrenner und nur Die Optionen werden an usn aus geschaltet.
 *Ebenso die Buttons.
 *
 *Import von Dateien aus einem anderen Projekt
 *
 *
 *###################################
 *Usage of Read and Burn Devices
 *The FoxSDK delivers an array of drives.
 *ReadDevices and BurnDevices are an different array but currently the same inside the SDK
 *So we need to set the drive names for each Interaction.
 *
 *###################################
 *ISO9660 - > Diskbutler is using Level3 as default until the community chaneg it.
 *In all it makes more sense for modern systems to use Level3.
 *
 *Irgendwie hinbekommen, wenn letztes Devices ar und dann kein Fenster mehr, dass er dann in Devices bleibt.
 *
 *EraseDone Event und BurnDone Event werden gleichzeitig  ausgelöset.
 *Also je nach durchgang Deaktiviern und aktivieren
 *
 *Bisher sieht es so aus als wenn der IMGBurn 4 Sektoren lädt und dass er auch bei Boot Level3 nimmt.
 *
 *Zuletzt gewähltes Laufwerk speichern.
 *
 *Unlock Drive Funktion falls abschuss
 *
 *
 *
 */
