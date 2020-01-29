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

#include "burndialog.h"
#include "ui_burndialog.h"
#include "settingspages.h"
#include "utils_common.h"
#include "qaudiotrackitem.h"
#include "xbelgenerator.h"
#include "xbelhandler.h"

#include "qtrackitem.h"
#include "qfilesysitem.h"
#include "qfilesys_udfitem.h"
#include "qfilesys_iso9660item.h"

//For all actions we have 3 debug levels
//1. Info
//2. Error
//3. Debug


//ACHTUNG! Der Thread hat noch MixedMode Fehler.

void ContentThread::run()
{

    int nAudioTrack = 0;

    QTreeWidgetItemIterator it(myTree);
    while (*it) {

        //Static cast
        if (((QDataItem*)(*it))->GetType()==QDataItem::AudioTrack){


            //Wir müssen anfangen mit Reinerpretcast. Stürzt er dann ab?
            QAudioTrackItem *audio_track = (QAudioTrackItem*)(QDataItem*)(*it);
            //QAudioTrackItem *audio_track = (QAudioTrackItem*)(QDataItem*)(*it);

            nAudioTrack++;

            //CDTExt is part of the Audiodatatrack not of files so we need to read before we go further with file.

            SFileAudioProperty data;

#if defined (WIN32)
      data.lpszSourceFilePath = (const wchar_t *)QString::number(nAudioTrack).utf16();
      data.lpszTitle = (const wchar_t *)audio_track->getTitle().utf16();
      data.Arranger = (const wchar_t *)audio_track->getArranger().utf16();
      data.Composer = (const wchar_t *)audio_track->getComposer().utf16();
      data.Message = (const wchar_t *)audio_track->getMessage().utf16();
      data.Performer = (const wchar_t *)audio_track->getPerformer().utf16();
      data.SongWriter = (const wchar_t *)audio_track->getSongWriter().utf16();
      data.lpszMCN_ISRC = (const wchar_t *)audio_track->getUPCEAN().utf16();
#else
      data.lpszSourceFilePath = QString::number(nAudioTrack).toUtf8();
      data.lpszTitle = audio_track->getTitle().toUtf8();
      data.Arranger = audio_track->getArranger().toUtf8();
      data.Composer = audio_track->getComposer().toUtf8();
      data.Message = audio_track->getMessage().toUtf8();
      data.Performer = audio_track->getPerformer().toUtf8();
      data.SongWriter = audio_track->getSongWriter().toUtf8();
      data.lpszMCN_ISRC = audio_track->getUPCEAN().toUtf8();
#endif


            data.nPause = audio_track->getPause();
            data.bPauseInFrames = audio_track->getPauseInFrames();


            ++it;

            if(((QDataItem*)(*it))->GetType()==QDataItem::File){

                SFileToAdd file;
                QString strSavePath = QDir::toNativeSeparators(((QDataItem*)(*it))->GetFullPath());
#if defined (WIN32)
    file.lpszSourceFilePath = (const wchar_t *)strSavePath.utf16();
#else
    QByteArray isoPath = QDir::cleanPath(QDir::toNativeSeparators(((QDataItem*)(*it))->GetDiskPath())).toUtf8();
    isoPath.append('\0');
    file.lpszSourceFilePath = isoPath;

#endif
                if(myTree->GetProjectType()==RuleManager::TYPE_PROJECT_MIXEDMODE){
#if defined (WIN32)
    file.lpszDestinationPath = L"\\audio";
#else
    file.lpszDestinationPath = "/audio";
#endif
                }else{
#if defined (WIN32)
    file.lpszDestinationPath = L"\\";
#else
    file.lpszDestinationPath = "/";
#endif
                }
                file.bVideoFile = BS_FALSE;
                file.nSavePath = BS_DONT_SAVE_PATH;
                int32 res = ::AddFile(file);
                if (res != BS_SDK_ERROR_NO)
                {
                   emit tHandleError(res);
                }
                //CDText
                res = ::SetAudioFileProperty(data);
                if (res != BS_SDK_ERROR_NO)
                {
                   emit tHandleError(res);
                }


            }else{
                //Fatal Error
                qDebug("Fatal Error");
            }


        }else{

            if (((QDataItem*)(*it))->GetType()==QDataItem::Folder){

                //addLogItem(((QDataItem*)(*it))->GetDiskPath(),2);
                SDirToCreate dir;

                QString strSavePath = QDir::toNativeSeparators(((QDataItem*)(*it))->GetDiskPath());
#if defined (WIN32)
                dir.lpszDestinationPath = (const wchar_t *)strSavePath.utf16();
                dir.lpszDir = (const wchar_t *)((QDataItem*)(*it))->GetName().utf16();
#else
                QByteArray folderPath = QDir::cleanPath(QDir::toNativeSeparators(((QDataItem*)(*it))->GetDiskPath())).toUtf8();
                folderPath.append('\0');
                QByteArray folderName = ((QDataItem*)(*it))->GetName().toUtf8();
                folderName.append('\0');
                dir.lpszDestinationPath = folderPath;
                dir.lpszDir = folderName;

#endif


                int32 res = ::CreateDir(dir);
                if (res != BS_SDK_ERROR_NO)
                {
                   emit tHandleError(res);
                }
            }else if(((QDataItem*)(*it))->GetType()==QDataItem::File){

                //addLogItem(((QDataItem*)(*it))->GetDiskPath(),2);
                SFileToAdd file;
                //WE fixed another bug in the SDK. Because QT delivers C:/ instead of C:\
                //We removed now "/"from SDK validation code
                QString strSavePath1 = QDir::toNativeSeparators(((QDataItem*)(*it))->GetFullPath());
                QString strSavePath2 = QDir::toNativeSeparators(((QDataItem*)(*it))->GetDiskPath());
#if defined (WIN32)
    //file.lpszFileName = (const wchar_t *)((QDataItem*)(*it))->GetName().utf16();
    file.lpszSourceFilePath = (const wchar_t *)strSavePath1.utf16();
    file.lpszDestinationPath = (const wchar_t *)strSavePath2.utf16();
#else
    QByteArray sourcePath = QDir::cleanPath(QDir::toNativeSeparators(((QDataItem*)(*it))->GetFullPath())).toUtf8();
    sourcePath.append('\0');
    QByteArray destPath = QDir::cleanPath(QDir::toNativeSeparators(((QDataItem*)(*it))->GetDiskPath())).toUtf8();
    destPath.append('\0');
    file.lpszSourceFilePath = sourcePath;
    file.lpszDestinationPath = destPath;
#endif
                file.bVideoFile = BS_FALSE;
                file.nSavePath = BS_DONT_SAVE_PATH;
                int32 res = ::AddFile(file);
                if (res != BS_SDK_ERROR_NO)
                {
                   emit tHandleError(res);
                }

                SFileDateTime tAdded;



                tAdded.nYear = static_cast<uint8>(dynamic_cast<QDataItem*>(*it)->GetDateAdded().date().year());
                tAdded.nMonth = ((QDataItem*)(*it))->GetDateAdded().date().year();
                tAdded.nDay = ((QDataItem*)(*it))->GetDateAdded().date().day();
                tAdded.nHour  = ((QDataItem*)(*it))->GetDateAdded().time().hour();
                tAdded.nMinute  = ((QDataItem*)(*it))->GetDateAdded().time().minute();
                tAdded.nSecond  = ((QDataItem*)(*it))->GetDateAdded().time().second();

                SFileDateTime tCreated;

                tAdded.nYear = static_cast<uint8>(dynamic_cast<QDataItem*>(*it)->GetDateAdded().date().year());
                tCreated.nYear = ((QDataItem*)(*it))->GetDateCreated().date().year();
                tCreated.nMonth = ((QDataItem*)(*it))->GetDateCreated().date().year();
                tCreated.nDay = ((QDataItem*)(*it))->GetDateCreated().date().day();
                tCreated.nHour  = ((QDataItem*)(*it))->GetDateCreated().time().hour();
                tCreated.nMinute  = ((QDataItem*)(*it))->GetDateCreated().time().minute();
                tCreated.nSecond  = ((QDataItem*)(*it))->GetDateCreated().time().second();

                SFileDateTime tModified;
                tModified.nYear = ((QDataItem*)(*it))->GetDateModified().date().year();
                tModified.nMonth = ((QDataItem*)(*it))->GetDateModified().date().year();
                tModified.nDay = ((QDataItem*)(*it))->GetDateModified().date().day();
                tModified.nHour  = ((QDataItem*)(*it))->GetDateModified().time().hour();
                tModified.nMinute  = ((QDataItem*)(*it))->GetDateModified().time().minute();
                tModified.nSecond  = ((QDataItem*)(*it))->GetDateModified().time().second();

                QString strSavePath3 = QDir::toNativeSeparators(((QDataItem*)(*it))->GetFullPath());
#if defined (WIN32)
    res = ::SetFileTimes((const wchar_t *)strSavePath3.utf16(), &tCreated, &tModified, &tAdded);
#else
    res = ::SetFileTimes(sourcePath, &tCreated, &tModified, &tAdded);
#endif
                if (res != BS_SDK_ERROR_NO)
                {
                    emit tHandleError(res);
                }

                int32 nAttrib = 0;
                if(((QDataItem*)(*it))->GetHidden()==true) nAttrib |= BS_FA_HIDDEN;
                if(((QDataItem*)(*it))->GetHiddenExt()==true) nAttrib |= BS_FA_ADVANCED_HIDDEN;
#if defined (WIN32)
    res = ::SetFileAttr((const wchar_t *)strSavePath3.utf16(), nAttrib);
#else
    res = ::SetFileAttr(sourcePath, nAttrib);
#endif

                if (res != BS_SDK_ERROR_NO)
                {
                    emit tHandleError(res);
                }
            }

        }



        ++it;
    }



  emit tContentDone();
}

void burnDialog::OnCreateDirEvent(const TCHAR *pcFullName, const TCHAR *, const TCHAR*, const TCHAR*, burnDialog *pUserData)
{
#if defined (WIN32)
    QString strOut = QString("Directory created: %1").arg(QString::fromUtf16(pcFullName));
#else
    QString strOut = QString("Directory created: %1").arg(QString::fromUtf8(pcFullName));
#endif
    pUserData->addLogItem(strOut,1);
}

void burnDialog::OnAddFileEvent(const TCHAR *pcFullPath, const TCHAR *, const TCHAR *, const TCHAR* , double , double dFileSize, burnDialog *pUserData)
{
#if defined (WIN32)
    QString strOut = QString("File added: %1 (%2MB)").arg(QString::fromUtf16(pcFullPath), QString::number(dFileSize/1024/1024));
#else
    QString strOut = QString("File added: %1 (%2MB)").arg(QString::fromUtf8(pcFullPath), QString::number(dFileSize/1024/1024));
#endif
    pUserData->addLogItem(strOut,1);
}

void burnDialog::OnFinalizeEvent(burnDialog *pUserData)
{
    emit pUserData->messageUpdated(tr("Finalize disk..."));
}

void burnDialog::OnBurnDoneEvent(const TCHAR* pcError, burnDialog *pUserData)
{
#if defined (WIN32)
    QString strError = QString::fromUtf16(pcError);
#else
    QString strError = QString::fromUtf8(pcError);
#endif

    QString msg;
    bool isError = false;
    if(strError.isEmpty()){
        msg = tr("Disk created successfully.");
    } else {
        isError = true;
        msg = tr("A burn error occoured: ")+strError;
    }
    emit pUserData->actionAfterBurn(msg,isError);
}

void burnDialog::OnJobDoneEvent(burnDialog *pUserData)
{
    emit pUserData->jobDone(tr("Job completed."));
}

void burnDialog::OnBurnFileEvent(const TCHAR* pcFileName, burnDialog *pUserData)
{
#if defined (WIN32)
    emit pUserData->fileUpdated(QString::fromUtf16(pcFileName));
#else
    emit pUserData->fileUpdated(QString::fromUtf8(pcFileName));
#endif

}

void burnDialog::OnProcessEvent(float fPercent, float fDeviceBuffer, float fCache, double dBytesWritten, double dImageSize, burnDialog *pUserData)
{
    emit pUserData->progressChanged((int)fPercent, (int)fCache);
}

void burnDialog::OnInfoTextEvent(const TCHAR *pcInfoText, int32 nLevel, burnDialog *pUserData)
{

}

void burnDialog::OnStartVerifyEvent(burnDialog *pUserData)
{
    emit pUserData->messageUpdated(tr("Start verify disk content."));
}

void burnDialog::OnVerifyFileEvent(const TCHAR *pcFileName, burnDialog *pUserData)
{
#if defined (WIN32)
    QString strFile = QString::fromUtf16(pcFileName);
#else
    QString strFile = QString::fromUtf8(pcFileName);
#endif
    QString msg = tr("Verify file: ")+strFile;
    emit pUserData->messageUpdated(msg);
}

void burnDialog::OnVerifyErrorEvent(const TCHAR *pcFileName, const TCHAR *pcError, burnDialog *pUserData)
{
#if defined (WIN32)
    QString strError = QString::fromUtf16(pcError);
    QString strFile = QString::fromUtf16(pcFileName);
#else
    QString strError = QString::fromUtf8(pcError);
    QString strFile = QString::fromUtf8(pcFileName);
#endif

    QString msg = QString("Verify error: %1 with file %2").arg(strError,strFile);
    emit pUserData->messageUpdated(msg);
}

void burnDialog::OnVerifyDoneEvent(int32 nNumErrors, burnDialog *pUserData)
{
    QString msg = QString("Verify done with %1 errors").arg(QString::number(nNumErrors));
    emit pUserData->messageUpdated(msg);
}

void burnDialog::OnEraseDoneEvent(const TCHAR *pcError, burnDialog *pUserData)
{
#if defined (WIN32)
    QString strError = QString::fromUtf16(pcError);
#else
    QString strError = QString::fromUtf8(pcError);
#endif

    QString msg;
    if(strError.isEmpty()){
        msg = tr("Disk erased successfully.");
    } else {
        msg = tr("A erase error occoured: ")+strError;
    }
    emit pUserData->messageUpdated(msg);
}

burnDialog::burnDialog(int sdkJobType, CommonTreeWidget *contentOwner, QString imagePath, MdiChildDialog *sourceDialog, SAudioGrabbingParams *xParams, QWidget *parent) :
    QDialog(parent),
    thisJobType(sdkJobType),
    strImageFilePath(imagePath),
    ui(new Ui::burnDialog),
    projectTree(contentOwner),
    projectParams(xParams),
    dlgSource(sourceDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    buildCommonDialog(sdkJobType);


}

burnDialog::burnDialog(int sdkJobType, CommonTreeWidget *contentOwner, MdiChildDiskInfo *contentSource, QString imagePath, MdiChildDialog *sourceDialog, QWidget *parent) :
    QDialog(parent),
    thisJobType(sdkJobType),
    strImageFilePath(imagePath),
    ui(new Ui::burnDialog),
    projectTree(contentOwner),
    jobSource(contentSource),
    dlgSource(sourceDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    buildCommonDialog(sdkJobType);




}

void burnDialog::buildCommonDialog(int sdkJobType)
{
    logLevel = 0;

    timerId = 0;

    m_enState = ST_NONE;
    mContentThread = nullptr;

    ui->totalProgress->setRange(0,100);
    ui->bufferValue->setRange(0,100);
    ui->currentSpeed->setText(tr(""));
    ui->currentTask->setText(tr(""));

    QString emptyValue = "";
#if defined (WIN32)
    ::SetImageFilePath((const wchar_t *)emptyValue.utf16());
#else
    ::SetImageFilePath(emptyValue.toUtf8());
#endif

    myStartBurnCommand = new QAction(this);
    connect(myStartBurnCommand, SIGNAL(triggered()), this, SLOT(startBurnNow()));




    connect(this, SIGNAL(progressChanged(int,int)), this, SLOT(UpdateMyUI(int,int)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(this, SIGNAL(messageUpdated(QString)), this, SLOT(myPostMessage(QString)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(this, SIGNAL(fileUpdated(QString)), this, SLOT(myPostFileEvent(QString)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(this, SIGNAL(jobDone(QString)), this, SLOT(myJobDoneEvent(QString)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(this, SIGNAL(actionAfterBurn(QString,bool)), this, SLOT(myActionAfterBurn(QString,bool)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));


    //For Debug we set to 2
    logLevel = 2;
    ui->logLevel->addItem(tr("Information"));
    ui->logLevel->addItem(tr("Details/Error"));
    ui->logLevel->addItem(tr("Debug"));
    ui->logLevel->setCurrentIndex(logLevel);



    connect(ui->logLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(debugStateChanged(int)));

    SetAddFileEventCallback((AddFileEvent) burnDialog::OnAddFileEvent, this);
    SetCreateDirEventCallback((CreateDirEvent) burnDialog::OnCreateDirEvent, this);
    SetBurnDoneEventCallback((BurnDoneEvent) burnDialog::OnBurnDoneEvent, this);
    SetBurnFileEventCallback((BurnFileEvent) burnDialog::OnBurnFileEvent, this);
    SetEraseDoneEventCallback((EraseDoneEvent) burnDialog::OnEraseDoneEvent, this);
    SetFinalizeEventCallback((FinalizeEvent) burnDialog::OnFinalizeEvent, this);
    SetInfoTextEventCallback((InfoTextEvent) burnDialog::OnInfoTextEvent, BS_IL_INFO, this);
    SetJobDoneEventCallback((JobDoneEvent) burnDialog::OnJobDoneEvent, this);
    SetProcessEventCallback((ProcessEvent) burnDialog::OnProcessEvent, this);
    SetStartVerifyEventCallback((StartVerifyEvent) burnDialog::OnStartVerifyEvent, this);
    SetVerifyDoneEventCallback((VerifyDoneEvent) burnDialog::OnVerifyDoneEvent, this);
    SetVerifyErrorEventCallback((VerifyErrorEvent) burnDialog::OnVerifyErrorEvent, this);
    SetVerifyFileEventCallback((VerifyFileEvent) burnDialog::OnVerifyFileEvent, this);

    QMenu *menu = new QMenu();
    QAction *testAction = new QAction("Change burning device", this);
    menu->addAction(testAction);




    ui->testmenu->setText("New burn");
    ui->testmenu->setMenu(menu);

    if(sdkJobType==1){
        //This is the tree where the project will get splitted
        diskItem = nullptr;
        diskItem = dynamic_cast<QDiskItem *>(projectTree->topLevelItem(0));


        if(diskItem==nullptr)return; //Bad error

        QString strWindowTitle = QString("Burn disc [%1]").arg(diskItem->getDisktitle());
        setWindowTitle(strWindowTitle);

        addLogItem(tr("Start burn project"),0);

        step1Project();
    }
    if(sdkJobType==2){

        QString strWindowTitle = QString("Read disc to image: %1").arg(jobSource->getImagePath());
        setWindowTitle(strWindowTitle);

        addLogItem(tr("Start create disk image"),0);
        step2Project();
    }

    if(sdkJobType==3){

        QString strDriveName = ConfigurationPage::mSettings.value("CurrentBurnDevice","").toString();
        QString strWindowTitle = QString("Copy disc to image: %1").arg(strDriveName);
        setWindowTitle(strWindowTitle);

        addLogItem(tr("Start copy disk"),0);
        step3Project();
    }

    if(sdkJobType==4){

        //Erase Disk

        addLogItem(tr("Start erase disk"),0);
        step4Project();
    }

    if(sdkJobType==5){

        QString strWindowTitle = QString("Burn disc image: %1").arg(strImageFilePath);
        setWindowTitle(strWindowTitle);
        //Burn Disk Image
        addLogItem(tr("Start burning disk image"),0);
        step5Project();
    }

    if(sdkJobType==6){ //Extract File
        QString strWindowTitle = QString("Extract: %1").arg(strImageFilePath);
        setWindowTitle(strWindowTitle);
        addLogItem(tr("Start extract file/folder"),0);
        step6Project();
    }
    if(sdkJobType==8){ //Extract File
        QString strWindowTitle = "Extract track";
        setWindowTitle(strWindowTitle);
        addLogItem(tr("Start extract track"),0);
        step8Project();
    }
    if(sdkJobType==7){ //Extract File
        QString strWindowTitle = "Extract disc to image";
        setWindowTitle(strWindowTitle);
        addLogItem(tr("Start extract disk"),0);
        step7Project();
    }
    if(sdkJobType==9){ //Extract File
        QString strWindowTitle = "Grab audio track";
        setWindowTitle(strWindowTitle);
        addLogItem(tr("Grab track to audio file"),0);
        step9Project();
    }
}

burnDialog::~burnDialog()
{
    qDebug("End Dialog");
    SetAddFileEventCallback(nullptr, nullptr);
    SetCreateDirEventCallback(nullptr, nullptr);
    SetBurnDoneEventCallback(nullptr, nullptr);
    SetBurnFileEventCallback(nullptr, nullptr);
    SetEraseDoneEventCallback(nullptr, nullptr);
    SetFinalizeEventCallback(nullptr, nullptr);
    SetInfoTextEventCallback(NULL ,NULL, NULL);
    SetJobDoneEventCallback(nullptr, nullptr);
    SetProcessEventCallback(nullptr, nullptr);
    SetStartVerifyEventCallback(nullptr, nullptr);
    SetVerifyDoneEventCallback(nullptr, nullptr);
    SetVerifyErrorEventCallback(nullptr, nullptr);
    SetVerifyFileEventCallback(nullptr, nullptr);
    qDebug("killTimer");
    killTimer(timerId);
    delete ui;
}

void burnDialog::step9Project()
{
    ui->bufferValue->setValue(0);
    ui->totalProgress->setValue(0);

    QDataItem *tSelected = projectTree->GetSelectedItem();

    //Es ist zwingend notwendig auch SetImage zu setzen weil das ja schon aus sein kann.
    //strSource
    QString strSourceX = dlgSource->currentFile();
    QString strSource = QDir::toNativeSeparators(strSourceX);

    bool isImageFile = dlgSource->isImagefile();
    int32 nError;

    addLogItem(tr("SourcePath: ")+strSource,0);

    if(isImageFile){
        nError = ::SetImageFilePath((const TCHAR*)strSource.utf16());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
        QString strDriveName = "#";
        nError = ::SetReadDevice(strDriveName.at(0).toLatin1());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
    }else{
        nError = ::SetReadDevice(strSource.at(0).toLatin1());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
    }






    QTrackItem *track = dynamic_cast<QTrackItem*>(tSelected);
    STrackInfo info = track->GetTrackInfo();



    SAudioGrabbingParams params;
    //params.nNetworkTagsHandle = tagInfo.getHandle();
    params.nBitrate = projectParams->nBitrate;
    params.nBitrateType = projectParams->nBitrateType;
    params.nEncoderType = projectParams->nEncoderType;
    params.nMaxBitrate = projectParams->nMaxBitrate;
    params.nMinBitrate = projectParams->nMinBitrate;
    params.nNetworkTagsHandle = projectParams->nNetworkTagsHandle;
    params.nTagChoice = projectParams->nTagChoice;

    //If pattern then we build te name according to the pattern.

    QString strSavePath = QDir::toNativeSeparators(strImageFilePath);

    addLogItem(tr("Target Path: ")+strSavePath,0);

#if defined (WIN32)
    nError = ::GrabAudioTrack(params, info.nTrackNumber, (const wchar_t *)strSavePath.utf16());
#else
    nError = ::GrabAudioTrack(params, info.nTrackNumber, strSavePath.toUtf8());
#endif

    if (nError != BS_SDK_ERROR_NO)
    {
       onHandleError(nError);
       return;
    }

    m_enState = ST_BURN;
    ui->logLevel->setEnabled(false);
    ui->saveLog->setEnabled(false);
    ui->testmenu->setEnabled(false);
}

void burnDialog::step7Project()
{
    ui->bufferValue->setValue(0);
    ui->totalProgress->setValue(0);

    QString strSourceX = dlgSource->currentFile();
    QString strSource = QDir::toNativeSeparators(strSourceX);

    bool isImageFile = dlgSource->isImagefile();
    int32 nError;

    addLogItem(tr("SourcePath: ")+strSource,0);

    if(isImageFile){
        nError = ::SetImageFilePath((const TCHAR*)strSource.utf16());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
        QString strDriveName = "#";
        nError = ::SetReadDevice(strDriveName.at(0).toLatin1());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
    }else{
        nError = ::SetReadDevice(strSource.at(0).toLatin1());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
    }

    SCreateImageParams cCreateImageParams;
    cCreateImageParams.cErrorParams.bErrorCorrection = true;
    cCreateImageParams.cErrorParams.bBlankBadSectors = false;

    QString strSavePath = QDir::toNativeSeparators(strImageFilePath);


#if defined (WIN32)
    wcsncpy(cCreateImageParams.lpszImagePath, (const wchar_t *)strSavePath.utf16(), 128*sizeof(TCHAR));
#else
    _tcsncpy(cCreateImageParams.lpszImagePath,strSavePath.toLocal8Bit(),128*sizeof(TCHAR));
#endif

    QFileInfo strTarget(strSavePath);

    //qDebug() << strSavePath;

    if(strTarget.suffix().compare("iso", Qt::CaseInsensitive)==0){
        cCreateImageParams.nImageType = BS_IMG_ISO;
    }else{
        cCreateImageParams.nImageType = BS_IMG_BIN;
    }


    cCreateImageParams.cErrorParams.nHardwareRetryCount = 7;
    cCreateImageParams.cErrorParams.nSoftwareRetryCount = 1;


    nError = ::CreateImage(cCreateImageParams, BS_IMGTASK_CREATE);
    if (nError != BS_SDK_ERROR_NO)
    {
       onHandleError(nError);
       return;
    }

    m_enState = ST_BURN;
    ui->logLevel->setEnabled(false);
    ui->saveLog->setEnabled(false);
    ui->testmenu->setEnabled(false);
}

void burnDialog::step8Project()
{
    ui->bufferValue->setValue(0);
    ui->totalProgress->setValue(0);

    QDataItem *tSelected = projectTree->GetSelectedItem();

    //Es ist zwingend notwendig auch SetImage zu setzen weil das ja schon aus sein kann.
    //strSource
    QString strSourceX = dlgSource->currentFile();
    QString strSource = QDir::toNativeSeparators(strSourceX);

    bool isImageFile = dlgSource->isImagefile();
    int32 nError;

    addLogItem(tr("SourcePath: ")+strSource,0);

    if(isImageFile){
        nError = ::SetImageFilePath((const TCHAR*)strSource.utf16());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
        QString strDriveName = "#";
        nError = ::SetReadDevice(strDriveName.at(0).toLatin1());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
    }else{
        nError = ::SetReadDevice(strSource.at(0).toLatin1());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
    }

    QTrackItem *track = dynamic_cast<QTrackItem*>(tSelected);
    STrackInfo info = track->GetTrackInfo();
    int nTRack = info.nTrackNumber;

    QString strSavePath = QDir::toNativeSeparators(strImageFilePath);

    if(tSelected->GetType()==QDataItem::AudioTrack){

        QFileInfo strTarget(strSavePath);

        int32 nFormat;
        if(strTarget.suffix().compare("bin", Qt::CaseInsensitive)==0){
            nFormat = BS_FF_BIN;
        }else{
            nFormat = BS_FF_WAVE;
        }

#if defined (WIN32)
        nError = ::SaveTrackToFile(BS_READ_DEVICE, nTRack,
                (const wchar_t *)strSavePath.utf16(), nFormat);
#else
        nError = ::SaveTrackToFile(BS_READ_DEVICE, nTRack,
                strSavePath.toUtf8(), nFormat);
#endif
        m_enState = ST_BURN;
        ui->logLevel->setEnabled(false);
        ui->saveLog->setEnabled(false);
        ui->testmenu->setEnabled(false);
    }

    if(tSelected->GetType()==QDataItem::DataTrack){

        QFileInfo strTarget(strSavePath);
        int32 nFormat;
        if(strTarget.suffix().compare("iso", Qt::CaseInsensitive)==0){
            nFormat = BS_FF_ISO;
        }else if(strTarget.suffix().compare("bin", Qt::CaseInsensitive)==0){
            nFormat = BS_FF_BIN;
        }else if(strTarget.suffix().compare("wav", Qt::CaseInsensitive)==0){
            nFormat = BS_FF_WAVE;
        }else{
            nFormat = BS_FF_MPEG;
        }

#if defined (WIN32)
        nError = ::SaveTrackToFile(BS_READ_DEVICE, nTRack,
                (const wchar_t *)strSavePath.utf16(), nFormat);
#else
        nError = ::SaveTrackToFile(BS_READ_DEVICE, nTRack,
                strSavePath.toUtf8(), nFormat);
#endif

        m_enState = ST_BURN;
        ui->logLevel->setEnabled(false);
        ui->saveLog->setEnabled(false);
        ui->testmenu->setEnabled(false);


    }



}

void burnDialog::step6Project()
{

    ui->bufferValue->setValue(0);
    ui->totalProgress->setValue(0);
    //QMessageBox::about(this, tr("Information"), strImageFilePath);
    //projectTree
    int nTRack = 0;
    int32 nFSSystem = 0;


    QDataItem *tSelected = projectTree->GetSelectedItem();

    //Es ist zwingend notwendig auch SetImage zu setzen weil das ja schon aus sein kann.
    //strSource



    QString strSourceX = dlgSource->currentFile();
    QString strSource = QDir::toNativeSeparators(strSourceX);

    bool isImageFile = dlgSource->isImagefile();
    int32 nError;

    addLogItem(tr("SourcePath: ")+strSource,0);

    if(isImageFile){
#if defined (WIN32)
        nError = ::SetImageFilePath((const wchar_t *)strSource.utf16());
#else
        nError = ::SetImageFilePath(strSource.toUtf8());
#endif

        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
        QString strDriveName = "#";
        nError = ::SetReadDevice(strDriveName.at(0).toLatin1());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
    }else{
        nError = ::SetReadDevice(strSource.at(0).toLatin1());
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }
    }


    if(tSelected->GetType()==QDataItem::File){
        addLogItem(tr("Extract File Operation."),0);
        //Ok, it is a file so continue
        QDataItem *current_item = tSelected;
        QDataItem *parent_item = dynamic_cast<QDataItem*>(tSelected->parent());
        while (parent_item != nullptr && QDataItem::DataTrack != parent_item->GetType()) {
          current_item = parent_item;
          parent_item = dynamic_cast<QDataItem*>(parent_item->parent());
        }

        if(parent_item != nullptr){
            if (QDataItem::DataTrack == parent_item->GetType()) {
              QTrackItem *track = dynamic_cast<QTrackItem*>(parent_item);
              STrackInfo info = track->GetTrackInfo();
              nTRack = info.nTrackNumber;

              if (info.nFormat != BS_TF_AUDIO) {
                // data track
                QFileSysItem *fs = dynamic_cast<QFileSysItem*>(current_item);
                nFSSystem = fs->fsTypeInt();

              }
            }
        }

        if(nTRack == 0 || nFSSystem == 0){
            addLogItem(tr("Error: Track or FileSystem NULL."),0);
            return;
        }

        QString strSavePath = QDir::toNativeSeparators(strImageFilePath);

        addLogItem(tr("Target Path: ")+strSavePath,0);


        addLogItem(tr("OpenDiskSession"),0);
        HSESSION hSession;
        nError = ::OpenDiskSession(BS_READ_DEVICE, nTRack, &hSession, nFSSystem);
        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }

        addLogItem(tr("ImportFile: ")+tSelected->GetFullPath(),0);
#if defined (WIN32)
        nError = ::ImportFile(hSession, (const wchar_t *)tSelected->GetFullPath().utf16(), (const wchar_t *)strSavePath.utf16());
#else
        nError = ::ImportFile(hSession, tSelected->GetFullPath().toUtf8(), strSavePath.toUtf8());
#endif

        if (nError != BS_SDK_ERROR_NO)
        {
           onHandleError(nError);
           return;
        }

        ::CloseDiskSession(hSession);
        //And it continue

        m_enState = ST_BURN;
        ui->logLevel->setEnabled(false);
        ui->saveLog->setEnabled(false);
        ui->testmenu->setEnabled(false);
    }

}

void burnDialog::step5Project()
{
    //All infos are in tree->disk
    ui->bufferValue->setValue(0);
    ui->totalProgress->setValue(0);

    int32 res;

    QString strSavePath = QDir::toNativeSeparators(strImageFilePath);

    addLogItem(tr("Image file path: ")+strSavePath,0);

    QFileInfo fi(strSavePath);

    QString strDriveName = ConfigurationPage::mSettings.value("CurrentBurnDevice","").toString();
    if(strDriveName.at(0)=='#')
    {
        addLogItem(tr("Fatal error. Image writer selected."),0);
        return;
    }


    addLogItem(tr("Set burn device: ")+strDriveName,0);
    res = ::SetBurnDevice(strDriveName.at(0).toLatin1());
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    int32 nSpeed = 0;
    ::GetBurnSpeed(BS_CURRENT_DEVICE,&nSpeed);

    if (nSpeed == BS_MAX_SPEED){
        ::GetMaxBurnSpeed(BS_CURRENT_DEVICE, &nSpeed);
    }

    ui->currentSpeed->setText(QString("%1 KB/s").arg(QString::number(nSpeed)));

    if(fi.suffix().compare("iso", Qt::CaseInsensitive)==0){
        //ISO
        //ISO Options
        SOptions opt;
        ::GetOptions(&opt);

        addLogItem(tr("Start BurnISO process now (::BurnISO(stringPath, SOptions))"),0);

        m_enState = ST_BURN;
        ui->logLevel->setEnabled(false);
        ui->saveLog->setEnabled(false);
        ui->testmenu->setEnabled(false);

#if defined (WIN32)
        res = ::BurnISO((const wchar_t *)strSavePath.utf16(),opt);
#else
        res = ::BurnISO(strSavePath.toUtf8(),opt);
#endif
        if (res != BS_SDK_ERROR_NO)
        {
           onHandleError(res);
           return;
        }


    }else{
        //CUE
        addLogItem(tr("Create project: BS_BURNER_CUE"),0);
        res = ::CreateProject(BS_BURNER_CUE);
        if (res != BS_SDK_ERROR_NO)
        {
           onHandleError(res);
           return;
        }

        SFileToAdd fileToAdd;
#if defined (WIN32)
        fileToAdd.lpszSourceFilePath = (const wchar_t *)strSavePath.utf16();
#else
        fileToAdd.lpszSourceFilePath = strSavePath.toUtf8();
#endif
        res = ::AddFile(fileToAdd);
        if (res != BS_SDK_ERROR_NO)
        {
           (res);
           return;
        }
        addLogItem(tr("Start preparing disk now (::Prepare())"),0);
        res = ::Prepare();
        if (res != BS_SDK_ERROR_NO)
        {
           onHandleError(res);
           return;
        }

        m_enState = ST_BURN;
        ui->logLevel->setEnabled(false);
        ui->saveLog->setEnabled(false);
        ui->testmenu->setEnabled(false);

        addLogItem(tr("Start Burn process now (::Burn())"),0);
        res = ::Burn();
        if (res != BS_SDK_ERROR_NO)
        {
           onHandleError(res);
           return;
        }
    }

}

//Set Callbacks and Remove Callbacks
void burnDialog::step2Project()
{
    //All infos are in tree->disk
    ui->bufferValue->setValue(0);
    ui->totalProgress->setValue(0);
    //Set read device fpr image creation. THe source drive is the main drive in general settings.
    //It is not #Imagedrive
    QString strDriveName = jobSource->getBurnDrive();

    addLogItem(QString("Set read device %1").arg(strDriveName),0);

    int32 res = ::SetReadDevice(strDriveName.at(0).toLatin1());
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    QString strSavePath = QDir::toNativeSeparators(jobSource->getImagePath());

    SCreateImageParams cCreateImageParams;
    cCreateImageParams.cErrorParams.bErrorCorrection = jobSource->getErrorCorrection();
    cCreateImageParams.cErrorParams.bBlankBadSectors = false;

    addLogItem(QString("Error Correction %1").arg(QString::number(jobSource->getErrorCorrection())),0);

#if defined (WIN32)
    wcsncpy(cCreateImageParams.lpszImagePath, (const wchar_t *)strSavePath.utf16(), 128*sizeof(TCHAR));
#else
    _tcsncpy(cCreateImageParams.lpszImagePath,strSavePath.toLocal8Bit(),128*sizeof(TCHAR));
#endif
    addLogItem(QString("Set image file %1").arg(strSavePath),0);


    cCreateImageParams.nImageType = (jobSource->getImageCreateMethod()==1)?BS_IMG_ISO:BS_IMG_BIN;
    cCreateImageParams.cErrorParams.nHardwareRetryCount = jobSource->getImageCreateHardRetry();
    cCreateImageParams.cErrorParams.nSoftwareRetryCount = jobSource->getImageCreateSoftRetry();

    int32 nSpeed = 0;
    ::GetReadSpeed(BS_READ_DEVICE,&nSpeed);

    if (nSpeed == BS_MAX_SPEED){
        ::GetMaxReadSpeed(BS_READ_DEVICE, &nSpeed);
    }
    ui->currentSpeed->setText(QString("%1 KB/s").arg(QString::number(nSpeed)));
    addLogItem(QString("Readspeed %1 KB/s").arg(QString::number(nSpeed)),0);


    //Wir müssen hier beide Als Maske übergeben.
    int8 dwOperation = 0;
    if(jobSource->getImageJobCreate()!=0){
        dwOperation = BS_IMGTASK_CREATE;
        addLogItem(tr("Set job type: BS_IMGTASK_CREATE"),0);
    }
    if(jobSource->getImageJobVerify()!=0){
        dwOperation |= BS_IMGTASK_VERIFY;
        addLogItem(tr("Set job type: BS_IMGTASK_VERIFY"),0);
    }

    res = ::CreateImage(cCreateImageParams, dwOperation);
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    //Aufgefallen:
    //Die Verify Medlungen kommen nicht im Log an.
    //Verify Start
    //Verify End


    m_enState = ST_BURN;
    ui->logLevel->setEnabled(false);
    ui->saveLog->setEnabled(false);
    ui->testmenu->setEnabled(false);

}

void burnDialog::step4Project()
{
    //All infos are in tree->disk
    ui->bufferValue->setValue(0);
    ui->totalProgress->setValue(0);

    QString strDriveName = ConfigurationPage::mSettings.value("CurrentBurnDevice","").toString();

    addLogItem(tr("Set burn device: ")+strDriveName,0);

    int32 res = ::SetBurnDevice(strDriveName.at(0).toLatin1());
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    if(jobSource->getFastErase()){
        addLogItem(tr("Set fast erase option"),0);
    }else{
        addLogItem(tr("Set complete erase option"),0);
    }

    if(jobSource->getEjectAfterErase()){
        addLogItem(tr("Set eject after erase option"),0);
    }

    m_enState = ST_BURN;
    ui->logLevel->setEnabled(false);
    ui->saveLog->setEnabled(false);
    ui->testmenu->setEnabled(false);

    res = ::Erase(jobSource->getFastErase(), jobSource->getEjectAfterErase());
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }
}

void burnDialog::step3Project()
{
    //All infos are in tree->disk
    ui->bufferValue->setValue(0);
    ui->totalProgress->setValue(0);
    //Set read device fpr image creation. THe source drive is the main drive in general settings.
    //It is not #Imagedrive
    QString strDriveName = ConfigurationPage::mSettings.value("CurrentBurnDevice","").toString();
    QString strBurnerName = jobSource->getBurnDrive();

    addLogItem(tr("Set read device: ")+strDriveName,0);

    int32 res = ::SetReadDevice(strDriveName.at(0).toLatin1());
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    addLogItem(tr("Set burn device: ")+strBurnerName,0);

    res = ::SetBurnDevice(strBurnerName.at(0).toLatin1());
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    if(strDriveName.at(0)==strBurnerName.at(0)){
        addLogItem(tr("Fatal error. Source and target device are teh same"),0);
        return;
    }

    SDiskCopyOptions cDiskCopyOptions;
    cDiskCopyOptions.bVerifyAfterBurn = jobSource->getCopyVerify();
    cDiskCopyOptions.bEjectAfterBurn = jobSource->getCopyEject();
    cDiskCopyOptions.nReadMode = jobSource->getCopyReadMethod();
    cDiskCopyOptions.nWriteMethod = jobSource->getCopyWriteMethod();

    //BS_RM_USERDATA; BS_RM_RAW; BS_RM_RAW_SUBCHANNEL
    //BS_WM_DAO; BS_WM_DAO96

    SReadErrorCorrectionParams cCorrectionParams;
    cCorrectionParams.bErrorCorrection = jobSource->getErrorCorrection();
    cCorrectionParams.nHardwareRetryCount = jobSource->getErrorHarwareRetry();
    cCorrectionParams.nSoftwareRetryCount = jobSource->getErrorSoftwareRetry();

    cDiskCopyOptions.cErrorParams = cCorrectionParams;

    res = ::CopyDisk(cDiskCopyOptions);
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    m_enState = ST_BURN;
    ui->logLevel->setEnabled(false);
    ui->saveLog->setEnabled(false);
    ui->testmenu->setEnabled(false);
}

void burnDialog::step1Project()
{
    //All infos are in tree->disk
    ui->bufferValue->setValue(0);
    ui->totalProgress->setValue(0);



    int32 res = 0;

    //Set Project
    RuleManager::ProjectType project_type = diskItem->getProjectType();
    switch(project_type){
        case RuleManager::TYPE_PROJECT_OPEN:
            res = ::CreateProject(BS_BURNER_ISOUDF, BS_CONTINUE_NO_SESSION);
            addLogItem(tr("Set project type: BS_BURNER_ISOUDF"),0);
            break;
        case RuleManager::TYPE_PROJECT_AUDIOCD:
            res = ::CreateProject(BS_BURNER_AUDIO, BS_CONTINUE_NO_SESSION);
            addLogItem(tr("Set project type: BS_BURNER_AUDIO"),0);
            break;
        case RuleManager::TYPE_PROJECT_MIXEDMODE:
            res = ::CreateProject(BS_BURNER_MIXEDMODE, BS_CONTINUE_NO_SESSION);
            addLogItem(tr("Set project type: BS_BURNER_MIXEDMODE"),0);
            break;
        case RuleManager::TYPE_PROJECT_VIDEODVD:
            res = ::CreateProject(BS_BURNER_VIDEODVD, BS_CONTINUE_NO_SESSION);
            addLogItem(tr("Set project type: BS_BURNER_VIDEODVD"),0);
            break;
        case RuleManager::TYPE_PROJECT_BLURAYVIDEO:
            res = ::CreateProject(BS_BURNER_BLURAY, BS_CONTINUE_NO_SESSION);
            addLogItem(tr("Set project type: BS_BURNER_BLURAY"),0);
            break;
        case RuleManager::TYPE_PROJECT_UDF:
            res = ::CreateProject(BS_BURNER_UDFDVD, BS_CONTINUE_NO_SESSION);
            addLogItem(tr("Set project type: BS_BURNER_UDFDVD"),0);
            break;
        case RuleManager::TYPE_PROJECT_ISO:
            res = ::CreateProject(BS_BURNER_DATA, BS_CONTINUE_NO_SESSION);
            addLogItem(tr("Set project type: BS_BURNER_DATA"),0);
            break;
        case RuleManager::TYPE_PROJECT_ISOUDF:
            res = ::CreateProject(BS_BURNER_ISOUDF, BS_CONTINUE_NO_SESSION);
            addLogItem(tr("Set project type: BS_BURNER_ISOUDF"),0);
            break;
        case RuleManager::TYPE_PROJECT_VIDEOCD:
            res = ::CreateProject(BS_BURNER_VCD, BS_CONTINUE_NO_SESSION);
            addLogItem(tr("Set project type: BS_BURNER_VCD"),0);
            break;
        case RuleManager::TYPE_PROJECT_SUPERVIDEO:
            res = ::CreateProject(BS_BURNER_SVCD, BS_CONTINUE_NO_SESSION);
            addLogItem(tr("Set project type: BS_BURNER_SVCD"),0);
            break;

    }
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    QString strDriveName = "";
    bImageWriter = false;
    if(diskItem->getBurnDevices().length()==1){
        qDebug("Ein Laufwerk");
        strDriveName = diskItem->getBurnDevices()[0];
        if(strDriveName.at(0)=='#'){
            bImageWriter = true;
            addLogItem(tr("Imagewriter activated"),0);
        }
    }else{
        qDebug("Mehr Laufwerke, wir steigen erstmal aus");
        return;
    }

    qDebug("Set Burn Device");

    res = ::SetBurnDevice(strDriveName.at(0).toLatin1());
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }



    addLogItem(tr("Set burn device: ")+strDriveName,0);

    addLogItem(tr("Set options"),0);
    SOptions opt;
    ::GetOptions(&opt);

#if defined (WIN32)
    wcsncpy(opt.chVolumeLabel, (const wchar_t *)diskItem->getDisktitle().utf16(), 128*sizeof(TCHAR));
#else
    _tcsncpy(opt.chVolumeLabel, diskItem->getDisktitle().toLocal8Bit(),128*sizeof(TCHAR));
#endif

    opt.bEjectAfterBurn = diskItem->getFeatureEject();
    addLogItem(tr("Option eject after burn: ")+QString::number(diskItem->getFeatureEject()),1);
    opt.bFinalize = diskItem->getFeatureFinishDisk();
    addLogItem(tr("Option finalize disk: ")+QString::number(diskItem->getFeatureFinishDisk()),1);
    opt.bJolietFileSystem = diskItem->getUseJoliet();
    addLogItem(tr("Option joliet extension: ")+QString::number(diskItem->getUseJoliet()),1);
    opt.bRockRidgeFileSystem = diskItem->getUseRockRidge();
    addLogItem(tr("Option rock ridge: ")+QString::number(diskItem->getUseRockRidge()),1);
    opt.bVerifyAfterBurn = diskItem->getFeatureVerify();
    addLogItem(tr("Option verify: ")+QString::number(diskItem->getFeatureVerify()),1);
    opt.bPerformOPC = diskItem->getFeatureOPC();
    addLogItem(tr("Option optimum power calibration: ")+QString::number(diskItem->getFeatureOPC()),1);
    opt.bAutoErase = diskItem->getFeatureAutoErase();
    addLogItem(tr("Option auto erase: ")+QString::number(diskItem->getFeatureAutoErase()),1);
    opt.bPadDataTracks = diskItem->getFeaturePadDataTrack();
    addLogItem(tr("Option pad data track: ")+QString::number(diskItem->getFeaturePadDataTrack()),1);
    opt.bTestBurn = diskItem->getFeatureSimulate();
    addLogItem(tr("Option simulate burn: ")+QString::number(diskItem->getFeatureSimulate()),1);



    if(diskItem->getDoBootDisk()==true){
        addLogItem(tr("Bootdisk enabled"),1);
        opt.bBootable = 1;

#if defined (WIN32)
    wcsncpy(opt.chBootImage, (const wchar_t *)diskItem->getBootDiskFile().utf16(), 260*sizeof(TCHAR));
#else
    _tcsncpy(opt.chBootImage, diskItem->getBootDiskFile().toLocal8Bit(),260*sizeof(TCHAR));
#endif
        addLogItem(tr("Set boot image: ")+diskItem->getBootDiskFile(),1);
    }
    res = ::SetOptions(opt);
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }



    //SetOptions infoEx
    SISOInfoEx isoInfo;
    memset(&isoInfo, 0, sizeof(isoInfo));
    //::GetISOInfoEx(&isoInfo);


    if( (project_type==RuleManager::TYPE_PROJECT_OPEN)
            || (project_type==RuleManager::TYPE_PROJECT_MIXEDMODE)
            || (project_type==RuleManager::TYPE_PROJECT_ISO)
            || (project_type==RuleManager::TYPE_PROJECT_ISOUDF))
    {
        addLogItem(tr("Set options ISOInfoEx"),1);
        isoInfo.ISOAllowLowercaseNames = diskItem->getAllowLowerCaseNames();
        addLogItem(tr("Set allow lower case names: ")+QString::number(diskItem->getAllowLowerCaseNames()),1);
        isoInfo.ISOAllowManyDirectories = diskItem->getAllowManyDirectories();
        addLogItem(tr("Set allow many directories: ")+QString::number(diskItem->getAllowManyDirectories()),1);
        isoInfo.ISOAllowLongISO9660Names = diskItem->getAllowLongFileNames();
        addLogItem(tr("Set allow long ISO names: ")+QString::number(diskItem->getAllowLongFileNames()),1);
        isoInfo.ISOAllowLongJolietNames = diskItem->getAllowLongJolietFileNames();
        addLogItem(tr("Set allow long joliet names: ")+QString::number(diskItem->getAllowLongJolietFileNames()),1);
        isoInfo.ISOLevel = BS_ISO_LEVEL_1 + diskItem->getISOFsType();
        addLogItem(tr("Set ISO level: ")+QString::number(diskItem->getISOFsType()),1);
        isoInfo.ISOAddSuffix = ((diskItem->getAllowLongJolietFileNames()==0)?1:0);
        addLogItem(tr("Set not write ISO Extension: ")+QString::number((diskItem->getAllowLongJolietFileNames()==0)?1:0),1);

    }

    if( (project_type==RuleManager::TYPE_PROJECT_OPEN)
            || (project_type==RuleManager::TYPE_PROJECT_MIXEDMODE)
            || (project_type==RuleManager::TYPE_PROJECT_ISO)
            || (project_type==RuleManager::TYPE_PROJECT_VIDEODVD)
            || (project_type==RuleManager::TYPE_PROJECT_ISOUDF))
    {

        //wcsncpy(info.ISOAbstractFileIdentifier, diskItem->getAbstractFile().toStdWString().c_str(), 36*sizeof(TCHAR));
        //wcsncpy(info.ISOApplicationIdentifier, diskItem->getApplication().toStdWString().c_str(), 128*sizeof(TCHAR));
        //wcsncpy(info.ISOBiblioIdentifier, diskItem->getBibliographicFile().toStdWString().c_str(), 36*sizeof(TCHAR));
        //wcsncpy(info.ISOCopyrightFileIdentifier, diskItem->getCoprightFile().toStdWString().c_str(), 36*sizeof(TCHAR));
        //wcsncpy(info.ISODataPreparerIdentifier, diskItem->getDatapreparer().toStdWString().c_str(), 128*sizeof(TCHAR));
        //wcsncpy(info.ISOPublisherIdentifier, diskItem->getPublisher().toStdWString().c_str(), 128*sizeof(TCHAR));
        //wcsncpy(info.ISOSystemIdentifier, diskItem->getSystemId().toStdWString().c_str(), 31*sizeof(TCHAR));
        //wcsncpy(info.ISOSetIdentifier, diskItem->getVolumeSet().toStdWString().c_str(), 128*sizeof(TCHAR));

         _tcsncpy(isoInfo.ISOAbstractFileIdentifier, diskItem->getAbstractFile().toLocal8Bit(), 36*sizeof(TCHAR));
         _tcsncpy(isoInfo.ISOApplicationIdentifier, diskItem->getApplication().toLocal8Bit(), 128*sizeof(TCHAR));
         _tcsncpy(isoInfo.ISOBiblioIdentifier, diskItem->getBibliographicFile().toLocal8Bit(), 36*sizeof(TCHAR));
         _tcsncpy(isoInfo.ISOCopyrightFileIdentifier, diskItem->getCoprightFile().toLocal8Bit(), 36*sizeof(TCHAR));
         _tcsncpy(isoInfo.ISODataPreparerIdentifier, diskItem->getDatapreparer().toLocal8Bit(), 128*sizeof(TCHAR));
         _tcsncpy(isoInfo.ISOPublisherIdentifier, diskItem->getPublisher().toLocal8Bit(), 128*sizeof(TCHAR));
         _tcsncpy(isoInfo.ISOSystemIdentifier, diskItem->getSystemId().toLocal8Bit(), 31*sizeof(TCHAR));
         _tcsncpy(isoInfo.ISOSetIdentifier, diskItem->getVolumeSet().toLocal8Bit(), 128*sizeof(TCHAR));

         qDebug("Datum bauen");

        //TimeEx infos
         SFileDateTime tCreated;
         tCreated.nYear = static_cast<uint8>(diskItem->getDiskDateCreation().date().year()- 1900);
         tCreated.nMonth = static_cast<uint8>(diskItem->getDiskDateCreation().date().month());
         tCreated.nDay = static_cast<uint8>(diskItem->getDiskDateCreation().date().day());
         tCreated.nHour  = static_cast<uint8>(diskItem->getDiskDateCreation().time().hour());
         tCreated.nMinute  = static_cast<uint8>(diskItem->getDiskDateCreation().time().minute());
         tCreated.nSecond  = static_cast<uint8>(diskItem->getDiskDateCreation().time().second());

         SFileDateTime tModified;
         tModified.nYear = static_cast<uint8>(diskItem->getDiskDateMdification().date().year()- 1900);
         tModified.nMonth = static_cast<uint8>(diskItem->getDiskDateMdification().date().month());
         tModified.nDay = static_cast<uint8>(diskItem->getDiskDateMdification().date().day());
         tModified.nHour  = static_cast<uint8>(diskItem->getDiskDateMdification().time().hour());
         tModified.nMinute  = static_cast<uint8>(diskItem->getDiskDateMdification().time().minute());
         tModified.nSecond  = static_cast<uint8>(diskItem->getDiskDateMdification().time().second());

         SFileDateTime tEffective;
         tEffective.nYear = static_cast<uint8>(diskItem->getDiskDateEffective().date().year()- 1900);
         tEffective.nMonth = static_cast<uint8>(diskItem->getDiskDateEffective().date().month());
         tEffective.nDay = static_cast<uint8>(diskItem->getDiskDateEffective().date().day());
         tEffective.nHour  = static_cast<uint8>(diskItem->getDiskDateEffective().time().hour());
         tEffective.nMinute  = static_cast<uint8>(diskItem->getDiskDateEffective().time().minute());
         tEffective.nSecond  = static_cast<uint8>(diskItem->getDiskDateEffective().time().second());

         SFileDateTime tExpiration;
         tExpiration.nYear = static_cast<uint8>(diskItem->getDiskDateExpiration().date().year()- 1900);
         tExpiration.nMonth = static_cast<uint8>(diskItem->getDiskDateExpiration().date().month());
         tExpiration.nDay = static_cast<uint8>(diskItem->getDiskDateExpiration().date().day());
         tExpiration.nHour  = static_cast<uint8>(diskItem->getDiskDateExpiration().time().hour());
         tExpiration.nMinute  = static_cast<uint8>(diskItem->getDiskDateExpiration().time().minute());
         tExpiration.nSecond  = static_cast<uint8>(diskItem->getDiskDateExpiration().time().second());

         if(diskItem->getIsoExUseDates()==true){
             qDebug("Datum setzen");
             isoInfo.ISOUseCreationDateTime = BS_TRUE;
             isoInfo.ISOUseModificationDateTime = BS_TRUE;
             isoInfo.ISOUseExpirationDateTime = BS_TRUE;
             isoInfo.ISOUseEffectiveDateTime = BS_TRUE;

             isoInfo.ISOCreationDateTime = tCreated;
             isoInfo.ISOModificationDateTime = tModified;
             isoInfo.ISOExpirationDateTime = tExpiration;
             isoInfo.ISOEffectiveDateTime = tEffective;
         }


    }

    res = ::SetISOInfoEx(isoInfo);
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    //return;

    if(diskItem->getDoBootDisk()==true){
        addLogItem(tr("Set options SBootInfoEx"),0);
        SBootInfoEx info;
        ::GetBootInfoEx(&info);
#if defined (WIN32)
    wcsncpy(info.DeveloperID, (const wchar_t *)diskItem->getBootDeveloperID().utf16(), 24*sizeof(TCHAR));
#else
    _tcsncpy(info.DeveloperID, diskItem->getBootDeveloperID().toLocal8Bit(),24*sizeof(TCHAR));
#endif
        addLogItem(tr("Set boot developer ID: ")+diskItem->getBootDeveloperID(),1);
        info.BootIndicator	= 1; //true
        info.Emulation		=  diskItem->getBootEmulationType();
        addLogItem(tr("Set boot emulation type: ")+QString::number(diskItem->getBootEmulationType()),1);
        info.LoadSegment	= wcstol(diskItem->getBootLoadSegment().toStdWString().c_str(), NULL, 16);
        addLogItem(tr("Set boot load segment: ")+diskItem->getBootLoadSegment(),1);
        info.PlatformID		= diskItem->getBootPlatformID();
        addLogItem(tr("Set boot platform ID: ")+QString::number(diskItem->getBootPlatformID()),1);
        info.SectorCount	= diskItem->getBootSectors().toInt();
        addLogItem(tr("Set boot sectors: ")+diskItem->getBootSectors(),1);
        res = ::SetBootInfoEx(info);
        if (res != BS_SDK_ERROR_NO)
        {
           onHandleError(res);
           return;
        }
    }
    if( (project_type==RuleManager::TYPE_PROJECT_AUDIOCD)
            || (project_type==RuleManager::TYPE_PROJECT_MIXEDMODE))
    {
        //CDText

        SFileAudioProperty data;

        if(project_type==RuleManager::TYPE_PROJECT_MIXEDMODE){
#if defined (WIN32)
    data.lpszSourceFilePath = L"\\audio";
#else
    data.lpszSourceFilePath = "/audio";
#endif
        }else{
#if defined (WIN32)
    data.lpszSourceFilePath = L"\\";
#else
    data.lpszSourceFilePath = "/";
#endif
        }

#if defined (WIN32)
        data.lpszTitle = (const wchar_t *)diskItem->getTitle().utf16();
        data.Arranger = (const wchar_t *)diskItem->getArranger().utf16();
        data.Composer = (const wchar_t *)diskItem->getComposer().utf16();
        data.Message = (const wchar_t *)diskItem->getMessage().utf16();
        data.Performer = (const wchar_t *)diskItem->getPerformer().utf16();
        data.SongWriter = (const wchar_t *)diskItem->getSongWriter().utf16();
        data.lpszMCN_ISRC = (const wchar_t *)diskItem->getUPCEAN().utf16();
#else
        data.lpszTitle = diskItem->getTitle().toUtf8();
        data.Arranger = diskItem->getArranger().toUtf8();
        data.Composer = diskItem->getComposer().toUtf8();
        data.Message = diskItem->getMessage().toUtf8();
        data.Performer = diskItem->getPerformer().toUtf8();
        data.SongWriter = diskItem->getSongWriter().toUtf8();
        data.lpszMCN_ISRC = diskItem->getUPCEAN().toUtf8();
#endif

        //data.nPause = audio_track->getPause().toInt(); //NOt allowed on disk
        //data.bPauseInFrames = audio_track->get

        res = ::SetAudioFileProperty(data);
        if (res != BS_SDK_ERROR_NO)
        {
           onHandleError(res);
           return;
        }
    }





    //VideoDVD and Blu-ray have fixed UDF
    if( (project_type==RuleManager::TYPE_PROJECT_OPEN)
            || (project_type==RuleManager::TYPE_PROJECT_ISOUDF)
            || (project_type==RuleManager::TYPE_PROJECT_UDF))
    {

        const int32 UDF_VERSIONS[] =
        {
            BS_UDF_VERSION_102,
            BS_UDF_VERSION_150,
            BS_UDF_VERSION_200,
            BS_UDF_VERSION_201,
            BS_UDF_VERSION_250,
            BS_UDF_VERSION_260
        };

        addLogItem(tr("Set options SUDFOptions"),0);

        SUDFOptions cUDFOptions;
        GetUDFOptions(&cUDFOptions);

        //wcsncpy(cUDFOptions.ImplementationID, diskItem->getDisktitle().toStdWString().c_str(), 22*sizeof(TCHAR));

        cUDFOptions.Version = UDF_VERSIONS[diskItem->getUDFType()];
        addLogItem(tr("Set UDF version: ")+QString::number(diskItem->getUDFType()),1);
        cUDFOptions.PartitionType = diskItem->getUDFPartition();
        addLogItem(tr("Set UDF parition: ")+QString::number(diskItem->getUDFPartition()),1);
        cUDFOptions.WriteFileStreams = diskItem->getUDFWriteStream();
        addLogItem(tr("Set UDF write file streams: ")+QString::number(diskItem->getUDFWriteStream()),1);
        res = ::SetUDFOptions(cUDFOptions);
        if (res != BS_SDK_ERROR_NO)
        {
           onHandleError(res);
           return;
        }
    }

    addLogItem(tr("Ready set options"),0);

    if(diskItem->getProjectType() == RuleManager::TYPE_PROJECT_BLURAYVIDEO){
        checkBluRayProject();
    }

    //Also irgednwas stimmt da nicht.
    //Ordner mit Leerzeichen?

    //First we try to set AddFile Event and look what happen
    if(!mContentThread){
        mContentThread  = new ContentThread(projectTree);
        connect(mContentThread, SIGNAL(tContentDone()), this, SLOT(onContentDone()));
        connect(mContentThread, SIGNAL(tHandleError(int32)), this, SLOT(onHandleError(int32)));
    }

    mContentThread->start();

}

void burnDialog::startBurnNow()
{

    int32 res;
    //We will set the image path here
    //We need to check if image path is full, but only if imageWriter is on.
    if(bImageWriter==true){
        if(selectImage()==false){
            addLogItem(tr("Fatal Error, no image path available."),0);
            return;
        }
    }


    addLogItem(tr("Start preparing disk now (::Prepare())"),0);
    res = ::Prepare();
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    //Now we check the size of the disk to set the cache.

    double nImageSize;
    res = ::GetImageSize(&nImageSize);
    if (res == BS_SDK_ERROR_NO)
    {
        QString nIngo = QString::number(nImageSize/1024/1024);
        addLogItem(tr("Start dynamic data cache."),0);
        SOptions opt;
        ::GetOptions(&opt);
        opt.nCacheSize = 16*1024*1024;
        //No need to check error. If error ignore dynamic cache.
        if((nImageSize/1024/1024) < 2000){
            opt.nCacheSize = 8*1024*1024;
            addLogItem(tr("Data size lower than 2GB. Set Cache to 8 MB."),0);
        }
        if((nImageSize/1024/1024) < 100){ //smaller than 100 MB
            opt.nCacheSize = 4*1024*1024;
            addLogItem(tr("Data size lower than 100MB. Set Cache to 4 MB."),0);
        }
        if((nImageSize/1024/1024) < 10){ //smaller than 10 MB
            opt.nCacheSize = 1*1024*1024;
            ::SetBurnSpeed(BS_CURRENT_DEVICE,5540);
            addLogItem(tr("Data size lower than 10MB. Set Cache to 1 MB and Speed to 5540."),0);
        }
        res = ::SetOptions(opt);
    }

    int32 nSpeed = 0;
    ::GetBurnSpeed(BS_CURRENT_DEVICE,&nSpeed);

    if (nSpeed == BS_MAX_SPEED){
        ::GetMaxBurnSpeed(BS_CURRENT_DEVICE, &nSpeed);
    }
    ui->currentSpeed->setText(QString("%1 KB/s").arg(QString::number(nSpeed)));

    addLogItem(tr("Start Burn process now (::Burn())"),0);
    m_enState = ST_BURN;
    ui->logLevel->setEnabled(false);
    ui->saveLog->setEnabled(false);
    ui->testmenu->setEnabled(false);

    res = ::Burn();
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }
}

void burnDialog::addLogItem(QString strText, int logLevel)
{
    if(logLevel<=ui->logLevel->currentIndex()){
        QString strAusgabe = QString("%1 :: %2").arg(QTime::currentTime().toString("HH:mm:ss"), strText);
        QListWidgetItem *item = new QListWidgetItem(strAusgabe);
        ui->listWidget->insertItem(0,item);
    }


}



void burnDialog::onContentDone()
{
    addLogItem(tr("Check writeable media now."),0);
    timerId = startTimer(1000);
}

void burnDialog::onHandleError(int32 res)
{
    TCHAR chError[2048];
    int32 nLength = 2048;

    // Get error description
    ::GetText(res, chError, &nLength);

#if defined (WIN32)
      QString strError = QString("SDK Error: %1").arg(QString::fromUtf16(chError));
#else
      QString strError = QString("SDK Error: %1").arg(QString::fromUtf8(chError));
#endif
    addLogItem(strError,0);
    qDebug("Error: %s",strError.toLatin1().constData());
}

void burnDialog::debugStateChanged(int nState)
{
    logLevel = nState;
}

void burnDialog::checkBluRayProject()
{

    SMediumInfo mi;
    int32 res = GetMediumInformation(BS_CURRENT_DEVICE,&mi);
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
    }

    SUDFOptions mOptions;
    ::GetUDFOptions(&mOptions);
    //Ok, now we have the long way to check the mediumtype for AVCHD
    if(mi.wMediumTypeCode==BS_DVD_PLUSRW || mi.wMediumTypeCode==BS_DVD_RW_RO || mi.wMediumTypeCode==BS_DVD_RW
        || mi.wMediumTypeCode==BS_DVD_RW_SR || mi.wMediumTypeCode==BS_DVD_PLUSR || mi.wMediumTypeCode==BS_DVD_RDL_PLUS
        || mi.wMediumTypeCode==BS_DVD_RWDL_PLUS || mi.wMediumTypeCode==BS_DVD_MRDL || mi.wMediumTypeCode==BS_DVD_R){

            if(diskItem->getFeatureAVCHD()==true){

                mOptions.AvchdDisc = 1;
                addLogItem(tr("Bluray Project: DVD Media detected. Set AVCHD Flag because allowed"),1);

            }

    }
    res = ::SetUDFOptions(mOptions);
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
    }

    SOptions cOptions;
    ::GetOptions(&cOptions);
    cOptions.bFinalize = 1;
    addLogItem(tr("Blu-ray Project: Force disk finalize flag."),1);
    res = ::SetOptions(cOptions);
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
    }

}

void burnDialog::timerEvent(QTimerEvent *event)
{

    killTimer(timerId);
    timerId = 0;

    SMediumInfo mi;
    int32 res = GetMediumInformation(BS_CURRENT_DEVICE,&mi);
    if (res != BS_SDK_ERROR_NO)
    {
        onHandleError(res);
        //Avoid never ending loop:
        //if(res != BS_SCSI_ERROR_DISK_31){
            timerId = startTimer(4000);
        //}
        return;

    }
    /*
    //We have to test this first in case of the string problems
    if(mi.chMediumType == "BS_NDEF"){

    }
    */

    if(mi.nMediumStatus!=BS_MS_EMPTY_DISK){
        //Disk is not empty
        if(mi.wMediumTypeCode == BS_DVD_PLUSRW || mi.wMediumTypeCode == BS_DVD_RW_RO || mi.wMediumTypeCode == BS_BLURAY_RE || mi.wMediumTypeCode == BS_DVD_RAM  || mi.wMediumTypeCode == BS_DVD_RW_SR){
            //The disk is not Empty and it is a RW media.
            if(diskItem->getFeatureAutoErase()==true){
                //Send Message, because we will erase
                addLogItem(tr("Full RW disk detected. Auto erase activated. So disk will erased."),0);
                myStartBurnCommand->trigger();
                return;
            }
        }
        addLogItem(tr("No writeable disk. Please insert disk."),0);
        timerId = startTimer(4000);
        return;

    }else{
        addLogItem(tr("Writeable disk found. Will continue..."),0);
        myStartBurnCommand->trigger();
        return;
        //Is Empty, send messge
    }
}

void burnDialog::UpdateMyUI(int percent, int cache)
{
    //ui->bufferValue->setValue(progBuffer);
    //ui->totalProgress->setValue(progPercent);
    ui->bufferValue->setValue(cache);
    ui->totalProgress->setValue(percent);
}

void burnDialog::myJobDoneEvent(QString msg)
{
    ui->logLevel->setEnabled(true);
    ui->saveLog->setEnabled(true);
    if(thisJobType==1 || thisJobType==5)ui->testmenu->setEnabled(true);
    m_enState = ST_NONE;
    addLogItem(msg,0);
}

void burnDialog::myPostMessage(QString msg)
{
    addLogItem(msg, 0);
}

void burnDialog::myPostFileEvent(QString file)
{
    //addLogItem(strFile); //We will not log here because it is overlapped and some files are missed.
    ui->currentTask->setText(file);
}

void burnDialog::myActionAfterBurn(QString msg, bool isError)
{
    addLogItem(msg, 0);
    qDebug() << msg;
    if(isError==false){ //isok
        if(ConfigurationPage::mSettings.value("AutoSaveProjects", false).toBool()==true){
            //Enumerate Paths
            //Build from EAN Code
            //WRite
            //Title
            qDebug() << "start";
            QDateTime dt = QDateTime::currentDateTime();
            QString strDateTime = dt.toString("hhmmssyyyyMMdd");
            QString strSave = QString("%1(%2).xml").arg(strDateTime, diskItem->getDisktitle());

            //QString strPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory);
            QString strPath = QString("%1foxarchive/").arg(QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory));

            QDir dir(strPath);
            if (!dir.exists()) {
                dir.mkpath(".");
            }

            QString strFile = strPath + strSave;

            qDebug() << strFile;

            QFile file(strFile);
            if (!file.open(QFile::WriteOnly | QFile::Text)) {
              QMessageBox::warning(this, tr(""),
                                   tr("Cannot write file %1:\n%2.")
                                   .arg(strFile)
                                   .arg(file.errorString()));
              return;
            }

            QApplication::setOverrideCursor(Qt::WaitCursor);
            XbelGenerator generator(projectTree);
            bool res = generator.write(&file);
            QApplication::restoreOverrideCursor();
        }
    }
}

void burnDialog::on_saveLog_clicked()
{
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Log file"),
                                     nullptr,
                                     tr("Log Files (*.log)"));
    if (fileName.isEmpty())
      return;

#if defined (WIN32)
    ::SaveLogToFile((const wchar_t *)fileName.utf16());
#else
    ::SaveLogToFile(fileName.toUtf8());
#endif

}

void burnDialog::on_cancel_clicked()
{
    if(m_enState == ST_NONE){
        close();
    }else{
        ::Abort();
    }
}

bool burnDialog::selectImage()
{
    QString fileName;

    if(diskItem->getProjectType()==RuleManager::TYPE_PROJECT_MIXEDMODE || diskItem->getProjectType()==RuleManager::TYPE_PROJECT_AUDIOCD || diskItem->getProjectType()==RuleManager::TYPE_PROJECT_VIDEOCD || diskItem->getProjectType()==RuleManager::TYPE_PROJECT_SUPERVIDEO){
        //fileName = tr("E:/test.bin");
        fileName = QFileDialog::getSaveFileName(this, tr("Save to BIN/CUE image file"), nullptr, tr("BIN Files (*.bin)"));
        if (fileName.isEmpty())
          return false;
    }else{
        //fileName = tr("E:/test.iso");
        fileName = QFileDialog::getSaveFileName(this, tr("Save to ISO image file"), nullptr, tr("ISO Files (*.iso)"));
        if (fileName.isEmpty())
          return false;

    }

#if defined (WIN32)
    int32 res = ::SetImageFilePath((const wchar_t *)fileName.utf16());
#else
    int32 res = ::SetImageFilePath(fileName.toUtf8());
#endif

    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return false;
    }
    addLogItem(tr("Set image path: ")+fileName,0);

    return true;
}







//Problems
/*
    UDF ImplemationID seems not the UDF name, it seems the perparer. Wrong from Alex?

    /*

    SFileToAdd file;


        file.lpszFileName = convertToFoxValue("IFoerserDev.spc");
        file.lpszSourceFilePath = convertToFoxValue("E:\\IFoerserDev.spc");
        file.lpszDestinationPath = convertToFoxValue("\\");
        file.bVideoFile = BS_FALSE;
        file.nSavePath = BS_DONT_SAVE_PATH;


    res = ::AddFile(file);
    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }


    SDirToCreate dir;
        dir.lpszDestinationPath = convertToFoxValue("\\");
        dir.lpszDir = convertToFoxValue("Ingo");

    res = ::CreateDir(dir);

    if (res != BS_SDK_ERROR_NO)
    {
       onHandleError(res);
       return;
    }

    qApp->processEvents();
    Will update the UI


    logModel = new QStringListModel(this);
    QStringList list;
    list.append(tr("start"));
    logModel->setStringList(list);
    ui->listView->setModel(logModel);

    //ListView flickert genauso scheisse
    //logModel->insertRow(0);
    //QModelIndex index = logModel->index(0);
    //logModel->setData(index, strAusgabe);

    */




void burnDialog::on_testmenu_clicked()
{
    //Start new bun session
    QString initEmpty = "";
#if defined (WIN32)
    ::SetImageFilePath((const wchar_t *)initEmpty.utf16());
#else
    ::SetImageFilePath(initEmpty.toUtf8());
#endif
    addLogItem(tr("Check writeable media now."),0);
    timerId = startTimer(1000);
}
