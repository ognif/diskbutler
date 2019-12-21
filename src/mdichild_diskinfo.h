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

class MdiChildDiskInfo : public MdiChildBase
{
  Q_OBJECT

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

public:
  MdiChildDiskInfo(QWidget* parent, const QString &device);
  int32 ExtractTextFromHandle(int32 handle,int32 nCDTCI, QString& rText);
  int32 ExtractTrackTextFromHandle(int32 handle, int32 nTrack, int32 nCDTCI, QString& rText);
  RuleManager::ProjectType GetProjectType() {return mProjectType;}
  QString GetDeviceLetter() {return deviceLetter;}
  bool thisSuccessfullCreated;

  bool GetEraseable() {return isEraseable;}
  bool GetOpenDisk() {return isOpen;}
  bool GetEmptyDisk() {return bIsEmptyDisk;}
  bool GetIsIsoDisk() {return bIsISO;}
  bool GetIsBinDisk() {return bIsBIN;}
  void setHideEmptyFields(bool bValue) {hideEmptyFields=bValue;}
  void updateDiskInfo();
  void setCopyVerify(bool bValue) {bCopyVerify=bValue;}
  bool getCopyVerify() {return bCopyVerify;}
  void setImageVerify(bool bValue) {bImageVerify=bValue;}
  bool getImageVerify() {return bImageVerify;}
  void setCopyEject(bool bValue) {bCopyEject=bValue;}
  bool getCopyEject() {return bCopyEject;}
  void setUseErrorCorrection(bool bValue) {bUseErrorCorrection=bValue;}
  bool getErrorCorrection() {return bUseErrorCorrection;}
  void setBlankBadSectors(bool bValue) {bBlankBadSectors=bValue;}
  bool getBlankBadSectors() {return bBlankBadSectors;}
  void setCopyReadMethod(int nValue) {nReadMethod=nValue;}
  int getCopyReadMethod() {return nReadMethod;}
  void setImageCreateMethod(int nValue) {nImageCreateMethode=nValue;}
  int getImageCreateMethod() {return nImageCreateMethode;}

  void setCopyWriteMethod(int nValue) {nWriteMethod=nValue;}
  int getCopyWriteMethod() {return nWriteMethod;}
  void setErrorHarwareRetry(int nValue) {nHardwareRetry=nValue;}
  int getErrorHarwareRetry() {return nHardwareRetry;}
  void setErrorSoftwareRetry(int nValue) {nSoftwareRetry=nValue;}
  int getErrorSoftwareRetry() {return nSoftwareRetry;}

  void setEjectAfterErase(bool bValue) {bEraseEject=bValue;}
  bool getEjectAfterErase() {return bEraseEject;}
  void setFastErase(bool bValue) {bEraseFast=bValue;}
  bool getFastErase() {return bEraseFast;}

  QString getImagePath() {return strImagePath;}
  void setImagePath(QString strValue) {strImagePath=strValue;}
  QString getBurnDrive() {return strBurnDrive;}
  void setBurnDrive(QString strValue) {
      strBurnDrive=strValue;
      QString wTitle = tr("Disk Info");
      wTitle += " (";
      wTitle += strBurnDrive;
      wTitle += ")";
      setWindowTitle(wTitle);
      }

protected:
  void closeEvent(QCloseEvent *event);
  RuleManager::ProjectType mProjectType;
  QTableWidget* diskInfoTable;
  QTreeWidget *treeWidget;
  QSplitter *splitter;
  bool isEraseable;
  bool isOpen;
  bool hideEmptyFields;
  bool bCopyVerify;
  bool bImageVerify;
  bool bCopyEject;
  bool bUseErrorCorrection;
  bool bBlankBadSectors;
  bool bIsEmptyDisk;
  bool bIsISO;
  bool bIsBIN;
  bool bEraseEject;
  bool bEraseFast;
  int nImageCreateMethode;
  int nReadMethod; //0=ISO;1=RAW;2=RAW+SUB
  int nWriteMethod; //0=DAO;1=DAO96
  int nHardwareRetry;
  int nSoftwareRetry;
  QString strImagePath;
  QString strBurnDrive;
  QString deviceLetter;

  void readDiskInfo();

private:
  QAction *myUpdateMenus;
  QAction *myRefreshMenus;

};

#endif
