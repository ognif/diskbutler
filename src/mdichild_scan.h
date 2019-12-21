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

class readDiskSectors : public QThread
{
  Q_OBJECT
public:
  //CheckVersionThread();
    int maxSectors;
    int readSectors;
    int startSectors;
    bool threadStop;
signals:
  void tCurrentSector(int, bool);
  void tStopScan();
protected:
  void run();
};

class MdiChildScan : public MdiChildBase
{
    Q_OBJECT

public:
    MdiChildScan(QWidget* parent, const QString &device);
    RuleManager::ProjectType GetProjectType() {return mProjectType;}
    bool thisSuccessfullCreated;

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
    int bufferSize;

    void ScanJob();

private:
    QScanBoard* scanBoard;
    QScrollArea *scrollArea;
    QPushButton *stopButton;
    QPushButton *startButton;
    QPushButton *rescanButton;
    QPushButton *reportButton;
    QDoubleSpinBox *offsetSectors;
    QDoubleSpinBox *readSectors;

    readDiskSectors *mreadDiskSectors;

    void analyzeDisc();
    void startScanThread();
    void setDisabled();


private slots:
    void onCurrentSector(int newSector, bool isBad);
    void onStopScan();
    void onStop();
    void onUpdateSectors();
    void onStartScan();
    void onWriteReport();

};

#endif
