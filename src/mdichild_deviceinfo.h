#ifndef MDICHILD_DEVICEINFO_H
#define MDICHILD_DEVICEINFO_H

#include <QMainWindow>
#include <QtWidgets>
#include <QListView>
#include <QSplitter>
#include <QLabel>
#include "mdichild_base.h"
#include "FoxSDKBurningLib.h"
#include "vrulemanager.h"

struct pDevice {
  int isDevice;
  QString strVendor;
  QString strModel;
  QString strName;
  QString strRevision;
  QString strFirmwareDate;
  QString strSerialNumber;
  QString strLoader;
  QString strConnInterface;
  QString strDriveInterface;
  QString strBufferSize;
  QString strVolumeLevels;
  QString strReadRetry;
  QString strRegionCode;
  QString strUserChangesLeft;
  QString strVendorResetLeft;

};

class MdiChildDeviceInfo : public MdiChildBase
{
  Q_OBJECT

public:
  MdiChildDeviceInfo(QWidget* parent, const QString &device);
  RuleManager::ProjectType GetProjectType() {return mProjectType;}



  void updateDeviceInfo();

  void setShowToolTips(bool bValue) { bShowToolTips = bValue;}
  bool getShowToolTips() {return bShowToolTips;}
  void setHideEmptyFields(bool bValue) {bHideEmptyFields = bValue;}
  bool getHideEmptyFields() {return bHideEmptyFields;}
  void setRegionChanges(int nValue) {nRegionChanges = nValue;}
  int getRegionChanges() {return nRegionChanges;}
  void setCurrentRegion(int nValue) {nCurrentRegion = nValue;}
  int getCurrentRegion() {return nCurrentRegion;}

  QString getBurnDrive() {return strBurnDrive;}
  void setBurnDrive(QString strValue) {
      strBurnDrive=strValue;
      QString wTitle = tr("Device Info");
      wTitle += " (";
      wTitle += strBurnDrive;
      wTitle += ")";
      setWindowTitle(wTitle);}


protected:
  void closeEvent(QCloseEvent *event);

  RuleManager::ProjectType mProjectType;
  QTableWidget* deviceInfoTable;
  QTreeWidget *treeWidget;
  QSplitter *splitter;
  QString strBurnDrive;

  void readDeviceInfo();
private:
  bool bShowToolTips;
  bool bHideEmptyFields;
  int nRegionChanges;
  int nCurrentRegion;

  QAction *myRefreshMenus;

};

#endif
