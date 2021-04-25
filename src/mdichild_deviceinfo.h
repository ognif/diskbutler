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

#ifndef MDICHILD_DEVICEINFO_H
#define MDICHILD_DEVICEINFO_H

#include <QMainWindow>
#include <QtWidgets>
#include <QListView>
#include <QSplitter>
#include <QLabel>
#include "mdichild_base.h"
#include "utils_common.h"
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
    QString getlastSelectedTab(){ return strlastSelectedTab; }
    void setlastSelectedTab( QString nValue ) { strlastSelectedTab = nValue; }
    void setUIControls(Ribbon *baseRibbon, QWidget* parent);
    void setRibbonTabs(Ribbon *baseRibbon, QWidget* parent);

    QString getBurnDrive() {return strBurnDrive;}
    void setBurnDrive(QString strValue) {
        strBurnDrive=strValue;
        QString wTitle = tr("Device Info");
        wTitle += " (";
        wTitle += strBurnDrive;
        wTitle += ")";
        setWindowTitle(wTitle);}
    void setBurnDeviceList(QWidget* parent);


    void fillInfoPairPlain(QTreeWidgetItem* rootItem, TCHAR *itemName, QString itemText);
    void fillInfoPairPlainInt(QTreeWidgetItem* rootItem, int itemValue, QString itemText);
    void fillInfoPairPlainCaps(QTreeWidgetItem* rootItem, int32 capsHandle, int64 capsID, QString capsText, QString itemText);

protected:
    void closeEvent(QCloseEvent *event);

    RuleManager::ProjectType mProjectType;
    QTreeWidget *treeWidget;
    QSplitter *splitter;
    QString strBurnDrive;


    void readDeviceInfoEx();
private:
    bool bShowToolTips;
    bool bHideEmptyFields;
    int nRegionChanges;
    int nCurrentRegion;
    QString strlastSelectedTab;

    QAction *myRefreshMenus;

};

#endif
