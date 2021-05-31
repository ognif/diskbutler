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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QListWidgetItem>
#include <QtWidgets>


#include "vrulemanager.h"
#include "mdichild_diskinfo.h"
#include "mdichild_deviceinfo.h"
#include "mdichild_hex.h"
#include "mdichild_scan.h"
#include "qtwaitingspinner.h"
#include "devicecomboex.h"
#include "dynbasslib.h"
#include "qaudiotrackitem.h"

class QtWaitingSpinner;

class MdiChildDialog;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiSubWindow;
QT_END_NAMESPACE

class CheckVersionThread : public QThread
{
  Q_OBJECT
public:
  //CheckVersionThread();
signals:
  void tNewVersion(QString);
protected:
  void run();
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QStringList mDiskList;
    QStringList mBurnDriveList;

private:
    Ui::MainWindow *ui;
    QMdiArea *mdiArea;

    MdiChildDialog *activeMdiChild();
    MdiChildDiskInfo *activeMdiInfoChild();
    MdiChildDeviceInfo *activeMdiDeviceChild();
    MdiChildHex *activeMdiHexChild();
    MdiChildScan *activeMdiScanChild();


    QMdiSubWindow *currentSubWindow;
    QMdiSubWindow *findMdiChild(const QString &fileName, bool isFile = true);
    bool hasProjectWindow();
    bool isCascaded();
    RuleManager::ProjectType actProject;
    QString lastSelectedTab;

    CheckVersionThread *mCheckVersionThread;

    void createStatusBar();
    QLabel *mStatusTextLabel;
    QLabel *mStatusInfoLabel;

    void updateStatusBarText(const QString &text, bool isInfo);
    QString checkBurnDriveSelection();

public:
    QToolButton *viewSwitchButton;
    QToolButton *viewBrowserButton;
    QToolButton *addFileEditButton;
    QToolButton *addFolderEditButton;
    QToolButton *createFolderEditButton;
    QToolButton *viewTileButton;
    QToolButton *viewCascadeButton;
    QToolButton *viewCloseAllbutton;
    QToolButton *allSelectEditButton;
    QToolButton *inverseSelectEditButton;
    QToolButton *delEditButton;
    QToolButton *delAllEditButton;
    QToolButton *dataIsoProjectButton;
    QToolButton *dataUdfProjectButton;
    QToolButton *dataIsoUdfProjectButton;
    QToolButton *vidBlueProjectButton;
    QToolButton *mmAudioProjectButton;
    QToolButton *mmEnhancedProjectButton;
    QToolButton *vidDVDProjectButton;
    QToolButton *vidSVCDProjectButton;
    QToolButton *updtEditButton;
    QToolButton *dataTrackEditButton;
    QToolButton *audioTrackEditButton;
    QToolButton *burnGeneralButton;
    QToolButton *eraseGeneralButton;
    QToolButton *importIsoExButton;
    QToolButton *resetIsoExButton;
    QToolButton *selBootImageButton;
    QToolButton *renameEditButton;
    QToolButton *openDeviceButton;
    QToolButton *closeDeviceButton;
    QToolButton *infoDeviceButton;
    QToolButton *infoMediaButton;
    QToolButton *imageMediaButton;
    QToolButton *grabAudioMediaButton;
    QToolButton *scanMediaButton;
    QToolButton *hexMediaButton;
    QToolButton *startImageCreateJob;
    QToolButton *updateHexEditor;
    QToolButton *saveHexEditor;
    QToolButton *navFirstHexEditor;
    QToolButton *navLastHexEditor;
    QToolButton *navStepBackHexEditor;
    QToolButton *navStepForwardHexEditor;
    QToolButton *startScanEditor;
    QToolButton *stopScanEditor;
    QToolButton *saveScanEditor;
    QToolButton *burnDiskImage;
    QToolButton *unLockDevice;

    QToolButton *appOpenButton;
    QToolButton *appSaveButton;
    QToolButton *appSaveAsButton;
    QToolButton *appSettings;
    QToolButton *delBootImageButton;
    QToolButton *burnDeviceUpdateButton;
    QToolButton *readDeviceUpdateButton;
    QToolButton *updateScanEditor;
    QToolButton *resetAudioData;
    QToolButton *viewPropertyGridButton;

    QComboBox *udfVersionFSCombo;
    QComboBox *udfPartitionFSCombo;
    QComboBox *isoLevelFSCombo;
    QComboBox *imageFormatCombo;
    QCheckBox *imageReadErrorSwitch;
    QDoubleSpinBox *imageReadHardRetry;
    QDoubleSpinBox *imageReadSoftRetry;
    QCheckBox *imageReadJobCreate;
    QCheckBox *imageReadJobVerify;

    QDoubleSpinBox *hexSector;
    QDoubleSpinBox *scanStartOffset;
    QDoubleSpinBox *scanReadOffset;

    QComboBox *speedGeneralCombo;
    QComboBox *copiesGeneralCombo;
    QComboBox *emulationTypeBootCombo;
    QComboBox *platformBootCombo;
    QComboBox *bootISOLevelCombo;
    QCheckBox *simulateBurnGeneralCheck;
    QCheckBox *autoEraseGeneralCheck;
    QCheckBox *burnProofGeneralCheck;
    QCheckBox *ejectAfterGeneralCheck;
    QCheckBox *padDataGeneralCheck;
    QCheckBox *verifyAfterGeneralCheck;
    QCheckBox *activeOPCGeneralCheck;
    QCheckBox *finishDiscGeneralCheck;
    QCheckBox *avchdGeneralCheck;
    QCheckBox *jolietLongNamesFSCheck;
    QCheckBox *useJolietFSCheck;
    QCheckBox *isoManyDirectoriesFSCheck;
    QCheckBox *isoExtent1FSCheck;
    QCheckBox *isoLongPathFSCheck;
    QCheckBox *useRockRidgeFSCheck;
    QCheckBox *isoLongFileNamesFSCheck;
    QCheckBox *udfFileStreamFSCheck;
    QCheckBox *eraseFast;
    QCheckBox *ejectAfterErase;
    QCheckBox *burnISOAutoErase;
    QCheckBox *burnISOEjectAfterBurn;
    QCheckBox *burnISOUnderrunProtection;

    QLineEdit *edIsoExSystemIdValue;
    QLineEdit *edIsoExVolumeSetValue;
    QLineEdit *edIsoExPublisherValue;
    QLineEdit *edIsoExDataPreparerValue;
    QLineEdit *edIsoExApplicationValue;
    QLineEdit *edIsoExCopyrightFileValue;
    QLineEdit *edIsoExAbstractFileValue;
    QLineEdit *edIsoExBibliographicFileValue;
    QLineEdit *developerIDBootEdit;
    QLineEdit *sectorsBootEdit;
    QLineEdit *loadSegmentBootEdit;
    QLineEdit *imagePathBootEdit;

    QLineEdit *cdTextArrangerEdit;
    QLineEdit *cdTextComposerEdit;
    QLineEdit *cdTextSongWriterEdit;
    QLineEdit *cdTextPerformerEdit;
    QLineEdit *cdTextMessageEdit;
    QLineEdit *cdTextTitleEdit;
    QLineEdit *cdTextEANEdit;

    QDoubleSpinBox *indexesMinSelector;
    QDoubleSpinBox *indexesSecSelector;
    QDoubleSpinBox *indexesFrameSelector;
    QListWidget * indexesCollectionList;
    QListWidget * fileFilterList;
    QDoubleSpinBox *audioPauseLength;
    QCheckBox *audioAddPause;
    QCheckBox *filterByDate;

    QToolButton *addIndexToList;
    QToolButton *delIndexFromList;
    QToolButton *addFilterToList;
    QToolButton *delFilterFromList;
    QToolButton *importFilterList;
    QToolButton *resetFilterList;

    QComboBox *listBurnDevicesWidget;
    QComboBox *listReadDevicesWidget;

    QMenu* m_diskInfoMenu;
    QMenu* m_EjectTrayMenu;
    QMenu* m_CloseTrayMenu;
    QMenu* m_windowMenu;
    QMenu* m_imagetMenu;

    QDateEdit *filterFromDate;
    QDateEdit *filterToDate;


private:
    void fillSourceDriveList();
    void onHandleError( int32 res );
    void checkButtonsSelBurnDevice();
    QStringList *getFilterList();
    void setNewFilterValues();

    void fillDriveList();
    bool isBurnDevice(QString toTest);
    void mdiAreaActivationTemplate();
    void checkFileSystemNaming();
    void loadPlugins();


private slots:
    MdiChildDialog *createMdiChild(const RuleManager::ProjectType project_type);
    MdiChildDialog *createMdiChild();

    void selectAll();
    void reverseSelection();
    void fillBurnDriveList(bool configCheck = false);
    void fillReadDriveList(bool configCheck = false);
    void insertNode();
    void insertItem();
    void addNode();
    void resetFiles();
    void updateProject();
    void deleteItem();
    void renameItemEx();
    void addDataTrack();
    void addAudioTrack();
    void filterAddToList();
    void filterDelFromList();
    void filterImport();
    void filterReset();
    void filterTextChanged(QWidget*);

    void unLockBurnDevice();
    void closeTray(bool readDevice = false);
    void ejectTray(bool readDevice = false);

    void setDefaults();

    void newDiskInfo();
    void openHexEditor();
    void openScanDialog();
    void newDeviceInfo();
    void onCreateDiscImage();

    void release2ImageTab();
    void changeReadSoftRetry(double nValue);
    void changeReadHardRetry(double nValue);

    void newProject(int type);
    void onActiveChildChanged(QMdiSubWindow *activeSubWindow);
    void onRibbonChanged(int nIndex);
    void setActiveSubWindow(QWidget *window);
    void updateDiskInfo();
    void toggleFileExplorer();
    void togglePropertyGrid();
    void mdiNextWindow();
    void showFullScreenMode();
    void updateDiskList();

    void doBurn();
    void doErase();
    void doBurnImage();

    //Checkversion
    void onNewVersionAvail(QString newVersion);

    //Open & Save
    void open();
    void save();
    void saveAs();

    //Update DiskItem structure ISOEx
    void isoExSystemIdchanged(QString strText);
    void isoExVolumechanged(QString strText);
    void isoExPublisherchanged(QString strText);
    void isoExDataPreparerchanged(QString strText);
    void isoExApplicationchanged(QString strText);
    void isoExCopyrightFilechanged(QString strText);
    void isoExAbstractFilechanged(QString strText);
    void isoExBibliographicFilechanged(QString strText);
    //Update DiskItem structure Filesystem
    void fsIsoTypeChanged(int nIndex);
    void fsJolietChanged(int nState);
    void fsRockRidgeChanged(int nState);
    void fsManyDirectories(int nState);
    void fsLongFileNamesChanged(int nState);
    void fsNotWriteISO1Extension(int nState);
    void fsLongJolietFileNamesChanged(int nState);
    void fsUDFYTpeChanged(int nIndex);
    void fsUDFPartitionChanged(int nIndex);
    void fsUDFWriteStreamChanged(int nState);
    //Update Projectsettings
    void projectSimulateBurnChanged(int nState);
    void projectBurnProofChanged(int nState);
    void projectOPCChanged(int nState);
    void projectEjectChanged(int nState);
    void projectFinishMediumChanged(int nState);
    void projectVerifyChanged(int nState);
    void projectAutoEraseChanged(int nState);
    void projectPadDataChanged(int nState);
    void projectAVCHDChanged(int nState);
    void errorCorrectionChanged(int nState);
    void jobCreateChanged(int nState);
    void jobVerifyChanged(int nState);    
    //Update Bootdisk
    void bootEmulationTypeChanged(int iIndex);
    void imageCreateMethod(int iIndex);
    void bootPlatformChanged(int iIndex);
    void bootDeveloperIDchanged(QString strText);
    void bootSectorschanged(QString strText);
    void bootLoadSegmentchanged(QString strText);
    void clickedSelBootImagePath();
    void deleteBootImage();

    void isoExImportClicked();
    void isoExReset();
    void openSettings();
    void closeDiskbutler();

    //Hex
    void actHexNextSector();
    void actHexFirstSector();
    void actHexLastSector();
    void actHexPrevSector();
    void actHexUpdate();
    void actHexSaveLog();
    void hexReadSector(double nValue);

    //Scan
    void scanStartScan();
    void scanStopScan();
    void scanSaveLog();
    void scanUpdateDiskInfo();
    void scanStartOffsetValue(double nValue);
    void scanReadOffsetValue(double nValue);

    //View
    void viewTileMdi();
    void viewCascadeView();

    //Show
    void loadDefaultProject();

    //CDText
    void onEnterCDTextText(QLineEdit* edit);
    void onAudioPauseLength(double nValue);
    void onAudioAddPause(int nState);
    void onActionIndexes(bool isAdd);
    void onActionAudioReset();

    //Filter
    void filterByDateTrigger(int state);


public slots:
    void startWaitSpinner();
    void stopWaitSpinner();
    void statusbar_changed(QString text, bool isRight);
    void updateBurnDeviceSel(int index);
    void updateReadDeviceSel(int index);
    void updatedHexData();
    void disableScanTabControls();
    void enableScanTabControls();
    void onUpdateRibbonControls();

protected Q_SLOTS:
    void updateWindowSwitchMenu();
    void updateDiskInfoMenu();

protected:
    QtWaitingSpinner *mBlockSpinner;

};

#endif // MAINWINDOW_H
