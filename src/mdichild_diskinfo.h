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

#ifndef MDICHILD_DISKINFO_H
#define MDICHILD_DISKINFO_H

#include <QMainWindow>
#include <QtWidgets>
#include <QListView>
#include <QSplitter>
#include <QLabel>
#include "mdichild_base.h"
#include "FoxSDKBurningLib.h"
#include "vrulemanager.h"
#include "QDummyTextTree.h"

class MdiChildDiskInfo;

class ThreadInfo : public QThread
{
    Q_OBJECT
public:
    ThreadInfo(MdiChildDiskInfo *mSubWindow)
    {
        myChild = mSubWindow;
    }
signals:
    void completed(int maxCount, int hasData);
protected:
    void run();
    MdiChildDiskInfo *myChild;

};

class MdiChildDiskInfo : public MdiChildBase
{
  Q_OBJECT

  struct pTableItem {
      QString columnText;
      int itemColumn;
      int itemRow;
      bool rowSpan;
      bool rowBold;
  };

  struct pInformations {
    QString strMediaType;
    QString strMediaID;
    QString strMediaSpeeds;
    QString strMediaStatus;
    QString strMediaSize;
    QString strMediaUsedSpace;
    QString strMediaFreeSpace;
    QString strMediaExType;
    QString strMediaUPC;
    QString strMediaMaxWSpeed;
    QString strBootable;
    QString strBridgeFileSystem;
  };

  struct pUDF {
    int isUDF;
    QString strUDFVersion;
    QString strUDFFiles;
    QString strUDFFolders;
    QString strUDFPartition;
    QString strVolumeLabel;
    QString strPerparer;
    QString strRootAddress;
  };

  struct pISO {
    int isISO;
    QString strVolumeLabel;
    QString strCreationTime;
    QString strModificationTime;
    QString strExpirationTime;
    QString strEffectiveTime;
    QString strAbstract;
    QString strApplication;
    QString strBiblio;
    QString strCopyright;
    QString strDataPreparer;
    QString strPublisher;
    QString strSet;
    QString strSystem;
    QString strISOLevel;
    QString strISOExtension;
    QString strRootAddress;
  };

  struct pAudio {
    int isAudio;
    QString strTitle;
    QString strPerformer;
    QString strSongWriter;
    QString strComposer;
    QString strArranger;
    QString strMessage;
    QString strISRC;
  };

  struct pData {
    int mFormat;
    int imageHardRetry;
    int imageSoftRetry;
    int imageCorrSwitch;
    int imageJobVerify;
    int imageJobCreate;

    int mHasData;
    int16 nImageFormats;

    int nImageCreateMethode;
    int nReadMethod; //0=ISO;1=RAW;2=RAW+SUB
    bool bBlankBadSectors;
    bool bUseErrorCorrection;
    bool bCopyEject;
    int bImageVerify;
    int bCopyVerify;
    bool hideEmptyFields;
    bool bIsISO;
    bool bIsBIN;
    int nWriteMethod; //0=DAO;1=DAO96
    int nHardwareRetry;
    int nSoftwareRetry;
    bool bIsEmptyDisk;
    bool bEraseEject;
    bool bEraseFast;
    bool isEraseable;
    bool isOpen;
    QString strImagePath;
    QString strBurnDrive;
    QString deviceLetter;
  };

public:
  MdiChildDiskInfo(QWidget* parent, const QString &device);
  int32 ExtractTextFromHandle(int32 handle,int32 nCDTCI, QString& rText);
  int32 ExtractTrackTextFromHandle(int32 handle, int32 nTrack, int32 nCDTCI, QString& rText);
  RuleManager::ProjectType GetProjectType() {return mProjectType;}
  QString GetDeviceLetter() {return thisData.deviceLetter;}

  int hasData;


  bool GetEraseable() {return thisData.isEraseable;}
  void setEraseable(bool newValue) {thisData.isEraseable = newValue;}

  bool GetOpenDisk() {return thisData.isOpen;}
  void setOpenDisk(bool newValue) {thisData.isOpen = newValue;}

  bool GetEmptyDisk() {return thisData.bIsEmptyDisk;}
  void setEmptyDisk(bool newValue) {thisData.bIsEmptyDisk = newValue;}

  bool getHideEmptyFields() {return thisData.hideEmptyFields;}
  void setHideEmptyFields(bool bValue) {thisData.hideEmptyFields=bValue;}
  void updateDiskInfo();

  void setCopyVerify(int bValue) {thisData.bCopyVerify=bValue;}
  int getCopyVerify() {return thisData.bCopyVerify;}

  void setImageVerify(int bValue) {thisData.bImageVerify=bValue;}
  int getImageVerify() {return thisData.bImageVerify;}

  void setCopyEject(bool bValue) {thisData.bCopyEject=bValue;}
  bool getCopyEject() {return thisData.bCopyEject;}
  void setUseErrorCorrection(bool bValue) {thisData.bUseErrorCorrection=bValue;}
  bool getErrorCorrection() {return thisData.bUseErrorCorrection;}
  void setBlankBadSectors(bool bValue) {thisData.bBlankBadSectors=bValue;}
  bool getBlankBadSectors() {return thisData.bBlankBadSectors;}
  void setCopyReadMethod(int nValue) {thisData.nReadMethod=nValue;}
  int getCopyReadMethod() {return thisData.nReadMethod;}

  void setImageFormats(int nValue) {thisData.nImageFormats=nValue;}
  int getImageFormats() {return thisData.nImageFormats;}

  //I prefer to work with sturctures.
  //Image Create Values
  int getImageCreateSoftRetry() {return thisData.imageSoftRetry;}
  void setImageCreateSoftRetry(int newValue) {thisData.imageSoftRetry = newValue;}
  int getImageCreateHardRetry() {return thisData.imageHardRetry;}
  void setImageCreateHardRetry(int newValue) {thisData.imageHardRetry = newValue;}
  int getImageCorrSwitch() {return thisData.imageCorrSwitch;}
  void setImageCorrSwitch(int newValue) {thisData.imageCorrSwitch = newValue;}
  int getImageJobCreate() {return thisData.imageJobCreate;}
  void setImageJobCreate(int newValue) {thisData.imageJobCreate = newValue;}
  int getImageJobVerify() {return thisData.imageJobVerify;}
  void setImageJobVerify(int newValue) {thisData.imageJobVerify = newValue;}
  int getImageCreateMethod() {return thisData.nImageCreateMethode;}
  void setImageCreateMethod(int newValue) {thisData.nImageCreateMethode = newValue;}
  QString getImagePath() {return thisData.strImagePath;}
  void setImagePath(QString strValue) {thisData.strImagePath=strValue;}

  bool getIsIsoDisk() {return thisData.bIsISO;}
  void setIsIsoDisk(bool newValue) {thisData.bIsISO = newValue;}
  bool getIsBinDisk() {return thisData.bIsBIN;}
  void setIsBinDisk(bool newValue) {thisData.bIsBIN = newValue;}

  bool getDataState() {return thisData.mHasData;}
  void setDataState(bool newData) {thisData.mHasData = newData;}

  void setCopyWriteMethod(int nValue) {thisData.nWriteMethod=nValue;}
  int getCopyWriteMethod() {return thisData.nWriteMethod;}

  void setErrorHarwareRetry(int nValue) {thisData.nHardwareRetry=nValue;}
  int getErrorHarwareRetry() {return thisData.nHardwareRetry;}
  void setErrorSoftwareRetry(int nValue) {thisData.nSoftwareRetry=nValue;}
  int getErrorSoftwareRetry() {return thisData.nSoftwareRetry;}

  void setEjectAfterErase(bool bValue) {thisData.bEraseEject=bValue;}
  bool getEjectAfterErase() {return thisData.bEraseEject;}
  void setFastErase(bool bValue) {thisData.bEraseFast=bValue;}
  bool getFastErase() {return thisData.bEraseFast;}


  QString getBurnDrive() {return thisData.strBurnDrive;}
  void setBurnDrive(QString strValue) {
      thisData.strBurnDrive=strValue;
      QString wTitle = tr("Disk Info");
      wTitle += " (";
      wTitle += thisData.strBurnDrive;
      wTitle += ")";
      setWindowTitle(wTitle);
      }

  void readDiskInfo();

protected:
  void closeEvent(QCloseEvent *event);
  RuleManager::ProjectType mProjectType;
  QTableWidget* diskInfoTable;
  //QTreeWidget *treeWidget;
  QDummyTextTree *treeWidget;
  QSplitter *splitter;
  pData thisData;
  ThreadInfo *mWorkThread;
Q_SIGNALS:
    void thread_finished(QList<QTreeWidgetItem *>, QVector<pTableItem *>);
    void startSpinner();
    void stopSpinner();
protected Q_SLOTS:
    void on_thread_completed(QList<QTreeWidgetItem *> items, QVector<pTableItem *> myTable);
    void startUpdateInfo();

private:
  QAction *myUpdateMenus;
  QAction *myRefreshMenus;
  void qDebugAusgabeSDK(int32 errCode, const QString &customMessage);
};

#endif


/*
 * Warum das Treewidget nicht in dem Thread gefüllt werden kann, verstehe ich nicht.
 * Aber den Table können wir aussehralb des Threads füllen lassen.
 * qRegisterMetaType<QVector<int> >("QVector<int>");
 */
