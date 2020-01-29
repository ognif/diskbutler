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

#ifndef BURNDIALOG_H
#define BURNDIALOG_H

#include <QDialog>
#include "CommonTreeWidget.h"
#include "qdiskitem.h"
#include "mdichild_diskinfo.h"
#include "mdichild_dialog.h"
#include <QThread>



class ContentThread : public QThread
{
  Q_OBJECT
public:
  ContentThread(CommonTreeWidget *contentOwner) {myTree = contentOwner;}
signals:
  void tContentDone();
  void tHandleError(int32);
protected:
  void run();
  CommonTreeWidget *myTree;
};

namespace Ui {
class burnDialog;
}

class burnDialog : public QDialog
{
    Q_OBJECT

    enum State
    {
        ST_NONE,
        ST_BURN,
        ST_FINALIZE,
        ST_VERIFY
    };

public:
    explicit burnDialog(int sdkJobType, CommonTreeWidget *contentOwner, MdiChildDiskInfo *contentSource = NULL, QString imagePath = tr(""), MdiChildDialog *sourceDialog = NULL, QWidget *parent = 0);
    explicit burnDialog(int sdkJobType, CommonTreeWidget *contentOwner, QString imagePath, MdiChildDialog *sourceDialog, SAudioGrabbingParams *xParams, QWidget *parent = 0);
    ~burnDialog();
    //int progPercent;
    //int progBuffer;

    int thisJobType;

    QString strError;
    QString strImageFilePath;
    bool bImageWriter;
    bool selectImage();
    //QString strMessage;
    //QString strFile;

public:
signals:
    void progressChanged(int percent, int cache);
    void messageUpdated(QString str);
    void fileUpdated(QString file);
    void jobDone(QString str);
    void actionAfterBurn(QString msg, bool isError);

private slots:
    void debugStateChanged(int nState);
    void startBurnNow();
    void UpdateMyUI(int percent, int cache);
    void myPostMessage(QString msg);
    void myJobDoneEvent(QString msg);
    void myPostFileEvent(QString file);
    void myActionAfterBurn(QString msg, bool isError);
    void on_saveLog_clicked();
    void on_cancel_clicked();
    void step1Project();
    void step2Project();
    void step3Project();
    void step4Project();
    void step5Project();
    void step6Project();
    void step7Project();
    void step8Project();
    void step9Project();
    void onContentDone();


    void on_testmenu_clicked();

private:
    Ui::burnDialog *ui;
    CommonTreeWidget *projectTree;
    SAudioGrabbingParams *projectParams;
    QDiskItem *diskItem;
    void addLogItem(QString strText, int logLevel);
    void onHandleError(int32 res);
    void buildCommonDialog(int sdkJobType);
    MdiChildDiskInfo *jobSource;
    MdiChildDialog *dlgSource;

    int logLevel;
    void readingTree();
    void checkBluRayProject();
    int timerId;
    int startTimerId;
    QAction *myStartBurnCommand;
    ContentThread *mContentThread;
    int m_enState;


    //QStringListModel *logModel;

protected:
    void timerEvent(QTimerEvent *event);
    static void OnCreateDirEvent(const TCHAR *pcFullName, const TCHAR *pcJolietName, const TCHAR* pcISOName, const TCHAR* pcUDFName, burnDialog *pUserData);
    static void OnAddFileEvent(const TCHAR *pcFullPath, const TCHAR *pcJolietName, const TCHAR *pcISOName, const TCHAR* pcUDFName , double dDateTime, double dFileSize, burnDialog *pUserData);
    static void OnFinalizeEvent(burnDialog *pUserData);
    static void OnBurnDoneEvent(const TCHAR* pcError, burnDialog *pUserData);
    static void OnJobDoneEvent(burnDialog *pUserData);
    static void OnBurnFileEvent(const TCHAR* pcFileName, burnDialog *pUserData);
    static void OnProcessEvent(float fPercent, float fDeviceBuffer, float fCache, double dBytesWritten, double dImageSize, burnDialog *pUserData);
    static void OnInfoTextEvent(const TCHAR *pcInfoText, int32 nLevel, burnDialog *pUserData);

    static void OnStartVerifyEvent(burnDialog *pUserData);
    static void OnVerifyFileEvent(const TCHAR *pcFileName, burnDialog *pUserData);
    static void OnVerifyErrorEvent(const TCHAR *pcFileName, const TCHAR *pcError, burnDialog *pUserData);
    static void OnVerifyDoneEvent(int32 nNumErrors, burnDialog *pUserData);

    static void OnEraseDoneEvent(const TCHAR *pcError, burnDialog *pUserData);
};

#endif // BURNDIALOG_H
