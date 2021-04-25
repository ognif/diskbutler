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

#ifndef MDICHILD_HEX_H
#define MDICHILD_HEX_H

#include <QMainWindow>
#include <QtWidgets>
#include <QListView>
#include <QSplitter>
#include <QThread>
#include <QLabel>
#include "mdichild_base.h"
#include "FoxSDKBurningLib.h"
#include "vrulemanager.h"
#include "qhexview.h"

typedef std::vector<char> SectorData_t;
class MdiChildHex;

class ThreadHex : public QThread
{
    Q_OBJECT
public:
    ThreadHex(MdiChildHex *mSubWindow)
    {
        myChild = mSubWindow;
    }
protected:
    void run();
    MdiChildHex *myChild;
};

class MdiChildHex : public MdiChildBase
{
    Q_OBJECT

    struct pData {
      int mSector;
      int mHasData;
      double maxSectors;
      int bufferSize;
      QString strlastSelectedTab;
    };

public:
    MdiChildHex(QWidget* parent, const QString &device);
    RuleManager::ProjectType GetProjectType() {return mProjectType;}

    QString getlastSelectedTab(){ return thisData.strlastSelectedTab; }
    void setlastSelectedTab( QString nValue ) { thisData.strlastSelectedTab = nValue; }
    QString getBurnDrive() {return strBurnDrive;}
    void setBurnDrive(QString strValue) {
      strBurnDrive=strValue;
      QString wTitle = tr("Device Info");
      wTitle += " (";
      wTitle += strBurnDrive;
      wTitle += ")";
      setWindowTitle(wTitle);
      startUpdate();
    }
    void setUIControls(Ribbon *baseRibbon, QWidget* parent);
    void setBurnDeviceList(QWidget* parent);
    void setRibbonTabs(Ribbon *baseRibbon, QWidget* parent);

    //I prefer to work with sturctures.
    int getSector() {return thisData.mSector;}
    void setSector(int newSector) {thisData.mSector = newSector;}
    int getDataState() {return thisData.mHasData;}
    void setDataState(int newData) {thisData.mHasData = newData;}

    int getMaxSector() {return thisData.maxSectors;}
    void setMaxSector(int newSector) {thisData.maxSectors = newSector;}

    int getBufferSize() {return thisData.bufferSize;}
    void setBufferSize(int newBuffer) {thisData.bufferSize = newBuffer;}
    void readSectorEx(int32 sectorIndex) {readSector(sectorIndex,getBufferSize());}


    void saveBlock();
    void updateHex();
    void startHexThread();
    //Const, so the buffer will keep the same


Q_SIGNALS:
    void datatrack_changed();
    void updatedHexFinished(int mMaxSize, int bufferSize);
    void startSpinner();
    void stopSpinner();
private:
    void qDebugAusgabeSDK(int32 errCode, const QString &customMessage);


protected Q_SLOTS:
    void on_hex_completed(int mMaxSize, int bufferSize);
    void startUpdate();

private:
    QHexView *hexView;
    ThreadHex *mWorkThread;
protected:
    RuleManager::ProjectType mProjectType;
    QString strBurnDrive;
    int32 readSector(int sectorIndex, int buffersize);

    pData thisData;

};

#endif
