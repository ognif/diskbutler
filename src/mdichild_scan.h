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

#ifndef MDICHILD_SCAN_H
#define MDICHILD_SCAN_H

#include <QMainWindow>
#include <QtWidgets>
#include <QListView>
#include <QSplitter>
#include <QLabel>
#include "mdichild_base.h"
#include "FoxSDKBurningLib.h"
#include "vrulemanager.h"
#include "QScanBoard.h"

class QScrollArea;
class QScrollBar;
class MdiChildScan;

class ThreadAnalyze : public QThread
{
  Q_OBJECT
public:
    ThreadAnalyze(MdiChildScan *mSubWindow)
    {
        myChild = mSubWindow;
    }
protected:
  void run();
  MdiChildScan *myChild;
};


class readDiskSectors : public QThread
{
  Q_OBJECT

public:
  //CheckVersionThread();
    int maxSectors;
    int readSectors;
    int startSectors;
    bool threadStop;
    int bufferSize;
signals:
  void tCurrentSector(int, bool);
  void tStopScan();
protected:
  void run();
};

class MdiChildScan : public MdiChildBase
{
    Q_OBJECT

    struct pData {
      int mSectorOffset;
      int mReadOffset;
      int mHasData;
      int mBufferSize;
      bool mScanIsRunning;
      int mMaxSectors;
    };

public:
    MdiChildScan(QWidget* parent, const QString &device);
    RuleManager::ProjectType GetProjectType() {return mProjectType;}

    //I prefer to work with sturctures.
    bool getScanState() {return thisData.mScanIsRunning;}
    void setScanState(bool newState) {thisData.mScanIsRunning = newState;}
    int getSectorOffset() {return thisData.mSectorOffset;}
    void setSectorOffset(int newOffset) {thisData.mSectorOffset = newOffset;}
    int getReadOffset() {return thisData.mReadOffset;}
    void setReadOffset(int newOffset) {thisData.mReadOffset = newOffset;}
    int getDataState() {return thisData.mHasData;}
    void setDataState(int newData) {thisData.mHasData = newData;}
    int getBufferSize() {return thisData.mBufferSize;}
    void setBufferSize(int newBufferSize) {thisData.mBufferSize = newBufferSize;}
    int getMaxSector() {return thisData.mMaxSectors;}
    void setMaxSector(int newMaxSector) {thisData.mMaxSectors = newMaxSector;}

    void analyzeDisc();
    void onStop();
    void onUpdateSectors();
    void onStartScan(int offsetSectors, int readSectors);
    void onWriteReport();


    QString getBurnDrive() {return strBurnDrive;}
    void setBurnDrive(QString strValue) {
      strBurnDrive=strValue;
      QString wTitle = tr("Device Info");
      wTitle += " (";
      wTitle += strBurnDrive;
      wTitle += ")";
      setWindowTitle(wTitle);
    }

protected:
    RuleManager::ProjectType mProjectType;

    QString strBurnDrive;
    double maxSectors;
    double currentSector;

    void ScanJob();
    pData thisData;

private:
    QScanBoard* scanBoard;
    QScrollArea *scrollArea;


    readDiskSectors *mreadDiskSectors;
    ThreadAnalyze * manalyzeDisk;


    void startScanThread(int offsetSectors, int readSectors);
    void qDebugAusgabeSDK(int32 errCode, const QString &customMessage);

Q_SIGNALS:
    void datatrack_changed();
    void thread_finished(int mMaxSize, int bufferSize, int dataState);
    void enableControls();
    void disableControls();
    void startSpinner();
    void stopSpinner();

private slots:
    void onCurrentSector(int newSector, bool isBad);
    void startUpdateInfo();

protected Q_SLOTS:
    void on_thread_completed(int mMaxSize, int bufferSize, int dataState);
    void onStopScan();

};

#endif
