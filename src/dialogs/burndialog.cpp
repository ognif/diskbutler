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

#include "burndialog.h"
#include "ui_burndialog.h"
#include "settingspages.h"
#include "utils_common.h"
#include "qaudiotrackitem.h"
#include "xbelgenerator.h"
#include "xbelhandler.h"

#include "qdataitem.h"
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

    QTreeWidgetItemIterator it( myTree );
    while ( *it ) {

        //Static cast
        if ( ( (QDataItem* )( *it ) )->GetType() == QDataItem::AudioTrack ){


            //Wir müssen anfangen mit Reinerpretcast. Stürzt er dann ab?
            QAudioTrackItem *audio_track = ( QAudioTrackItem* )( QDataItem* )( *it );

            nAudioTrack++;

            //CDTExt is part of the Audiodatatrack not of files so we need to read before we go further with file.

            SFileAudioProperty data;

            QString trackId = QString("%1").arg(nAudioTrack, 2, 10, QLatin1Char('0'));

            const TCHAR *pAudioTrack = nullptr;
            pAudioTrack = convertToFoxValue(trackId);
            data.lpszSourceFilePath = pAudioTrack;

            const TCHAR *pTitle = nullptr;
            if(audio_track->getTitle().length()>0){
                pTitle = convertToFoxValue(audio_track->getTitle());
                data.lpszTitle = pTitle;
            }

            const TCHAR *pArranger = nullptr;
            if(audio_track->getArranger().length()>0){
                pArranger = convertToFoxValue(audio_track->getArranger());
                data.Arranger = pArranger;
            }

            const TCHAR *pComposer = nullptr;
            if(audio_track->getComposer().length()>0){
                pComposer = convertToFoxValue(audio_track->getComposer());
                data.Composer = pComposer;
            }

            const TCHAR *pMessage = nullptr;
            if(audio_track->getMessage().length()>0){
                pMessage = convertToFoxValue(audio_track->getMessage());
                data.Message = pMessage;
            }

            const TCHAR *pPerformer = nullptr;
            if(audio_track->getMessage().length()>0){
                pPerformer = convertToFoxValue(audio_track->getPerformer());
                data.Performer = pPerformer;
            }

            const TCHAR *pSongWriter = nullptr;
            if(audio_track->getMessage().length()>0){
                pSongWriter = convertToFoxValue(audio_track->getSongWriter());
                data.SongWriter = pSongWriter;
            }

            const TCHAR *pUPCEAN = nullptr;
            if(audio_track->getMessage().length()>0){
                pUPCEAN = convertToFoxValue(audio_track->getUPCEAN());
                data.lpszMCN_ISRC = pUPCEAN;
            }

            data.nPause = audio_track->getPause();
            data.bPauseInFrames = audio_track->getPauseInFrames();


            ++it;

            if(((QDataItem*)(*it))->GetType()==QDataItem::File){

                SFileToAdd file;
                const TCHAR *pFullPath = convertToFoxValue(QDir::toNativeSeparators(((QDataItem*)(*it))->GetFullPath()));
                file.lpszSourceFilePath = pFullPath;

                /*
                 * We need to add the current root path here.
                 * But only for the mixed mode, in TRack mode not.
                 */

                QString  diskRoot = PATHSEPSTRING;
                if( myTree->GetProjectType()==RuleManager::TYPE_PROJECT_MIXEDMODE ){
                    diskRoot += "audio";
                }

                const TCHAR *pRootPath = convertToFoxValue(diskRoot);
                file.lpszDestinationPath = pRootPath;

                file.bVideoFile = BS_FALSE;
                file.nSavePath = BS_DONT_SAVE_PATH;
                int32 res = ::AddFile(file);

                delete [] pFullPath;
                delete [] pRootPath;

                if (res != BS_SDK_ERROR_NO)
                {
                    emit tHandleError(res);
                    return;
                }
                //CDText
                res = ::SetAudioFileProperty(data);

                if(pAudioTrack) delete [] pAudioTrack;
                if(pTitle) delete [] pTitle;
                if(pArranger) delete [] pArranger;
                if(pComposer) delete [] pComposer;
                if(pMessage) delete [] pMessage;
                if(pPerformer) delete [] pPerformer;
                if(pSongWriter) delete [] pSongWriter;
                if(pUPCEAN) delete [] pUPCEAN;

                if (res != BS_SDK_ERROR_NO)
                {
                    emit tHandleError(res);
                    return;
                }


            }else{
                //Fatal Error
                qDebug("Fatal Error");
                return;
            }


        }else{ // NO AudoTRack

            if (((QDataItem*)(*it))->GetType()==QDataItem::Folder){

                //addLogItem(((QDataItem*)(*it))->GetDiskPath(),2);
                SDirToCreate dir;

                const TCHAR *pRootPath = convertToFoxValue(QDir::toNativeSeparators(((QDataItem*)(*it))->GetDiskPath()));
                dir.lpszDestinationPath = pRootPath;

                const TCHAR *pDir = convertToFoxValue(QDir::toNativeSeparators(((QDataItem*)(*it))->GetName()));
                dir.lpszDir = pDir;

                int32 res = ::CreateDir(dir);

                delete [] pRootPath;
                delete [] pDir;

                if (res != BS_SDK_ERROR_NO)
                {
                    qDebug() << "Create Dir";
                    emit tHandleError(res);
                    return;
                }
            }else if(((QDataItem*)(*it))->GetType()==QDataItem::File){

                //addLogItem(((QDataItem*)(*it))->GetDiskPath(),2);
                SFileToAdd file;
                /*
                 * WE fixed another bug in the SDK. Because QT delivers C:/ instead of C:\
                 * We removed now "/"from SDK validation code
                 *
                 * Wie gehen wir vor. Also das FileAdd wird mit dem Original und Zielpfad ausgeführt.
                 * Filename und Pfad auf Disk müssten ja nach dem Rename sein.
                 *
                 */

                QFileInfo strSource(QDir::toNativeSeparators(((QDataItem*)(*it))->GetFullPath()));

                const TCHAR *pFilePath = convertToFoxValue(QDir::toNativeSeparators(((QDataItem*)(*it))->GetFullPath()));
                file.lpszSourceFilePath = pFilePath;

                QString destFilePath = QDir::toNativeSeparators(((QDataItem*)(*it))->GetDiskPath());

                QString  diskRoot = PATHSEPSTRING;
                if( myTree->GetProjectType()==RuleManager::TYPE_PROJECT_MIXEDMODE ){
                    diskRoot += "data";
                    diskRoot += destFilePath;
                }else{
                    diskRoot = QDir::toNativeSeparators(((QDataItem*)(*it))->GetDiskPath());
                }

                const TCHAR *pDiskPath = convertToFoxValue(diskRoot);
                file.lpszDestinationPath = pDiskPath;

                //If source and target do not have the same name we give the different filename to the AddFile. No Rename needed.
                const TCHAR *pFileName = nullptr;
                if(strSource.fileName() != ((QDataItem*)(*it))->GetName()){
                    pFileName = convertToFoxValue(((QDataItem*)(*it))->GetName());
                    file.lpszFileName = pFileName;
                }

                file.bVideoFile = BS_FALSE;
                file.nSavePath = BS_DONT_SAVE_PATH;
                int32 res = ::AddFile(file);

                delete [] pFilePath;
                delete [] pDiskPath;
                if(pFileName) delete [] pFileName;

                if (res != BS_SDK_ERROR_NO)
                {
                    qDebug() << "AddFile";
                    qDebug() << ((QDataItem*)(*it))->GetDiskFilePath();
                    //qDebug() << destFilePath;
                    qDebug() << QDir::toNativeSeparators(((QDataItem*)(*it))->GetDiskPath());
                    qDebug() << QDir::toNativeSeparators(((QDataItem*)(*it))->GetFullPath());

                    emit tHandleError(res);
                    return;
                }

                SFileDateTime tAdded;

                tAdded.nYear = static_cast<uint8>(((QDataItem*)(*it))->GetDateAdded().date().year()- 1900); //static_cast<uint8>(dynamic_cast<QDataItem*>(*it)->GetDateAdded().date().year()- 1900);
                tAdded.nMonth = static_cast<uint8>(((QDataItem*)(*it))->GetDateAdded().date().month());
                tAdded.nDay = static_cast<uint8>(((QDataItem*)(*it))->GetDateAdded().date().day());
                tAdded.nHour  = static_cast<uint8>(((QDataItem*)(*it))->GetDateAdded().time().hour());
                tAdded.nMinute  = static_cast<uint8>(((QDataItem*)(*it))->GetDateAdded().time().minute());
                tAdded.nSecond  = static_cast<uint8>(((QDataItem*)(*it))->GetDateAdded().time().second());

                SFileDateTime tCreated;
                //There is no creation Date on Linux.
                //So we map it to modification date
                //OK, Attribute geht auch. Also muss es in der SDK sein.
#if defined (WIN32)
                tCreated.nYear =static_cast<uint8>(((QDataItem*)(*it))->GetDateCreated().date().year()- 1900);
                tCreated.nMonth = static_cast<uint8>(((QDataItem*)(*it))->GetDateCreated().date().month());
                tCreated.nDay = static_cast<uint8>(((QDataItem*)(*it))->GetDateCreated().date().day());
                tCreated.nHour  = static_cast<uint8>(((QDataItem*)(*it))->GetDateCreated().time().hour());
                tCreated.nMinute  = static_cast<uint8>(((QDataItem*)(*it))->GetDateCreated().time().minute());
                tCreated.nSecond  = static_cast<uint8>(((QDataItem*)(*it))->GetDateCreated().time().second());
#else
                tCreated.nYear = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().date().year()- 1900);
                tCreated.nMonth = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().date().month());
                tCreated.nDay = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().date().day());
                tCreated.nHour  = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().time().hour());
                tCreated.nMinute  = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().time().minute());
                tCreated.nSecond  = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().time().second());

#endif

                SFileDateTime tModified;
                tModified.nYear = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().date().year()- 1900);
                tModified.nMonth = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().date().month());
                tModified.nDay = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().date().day());
                tModified.nHour  = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().time().hour());
                tModified.nMinute  = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().time().minute());
                tModified.nSecond  = static_cast<uint8>(((QDataItem*)(*it))->GetDateModified().time().second());


                const TCHAR *pdestFilePath = convertToFoxValue(destFilePath);
                res = ::SetFileTimes(pdestFilePath, &tCreated, &tModified, &tAdded);
                delete [] pdestFilePath;

                if (res != BS_SDK_ERROR_NO)
                {
                    qDebug() << "SetFileTimes";

                    qDebug() << destFilePath;

                    emit tHandleError(res);
                    return;
                }

                int32 nAttrib = 0;
                if(((QDataItem*)(*it))->GetHidden()==true) nAttrib |= BS_FA_HIDDEN;
                if(((QDataItem*)(*it))->GetHiddenExt()==true) nAttrib |= BS_FA_ADVANCED_HIDDEN;
                if(((QDataItem*)(*it))->GetArchive()==true) nAttrib |= BS_FA_ARCHIVE;
                if(((QDataItem*)(*it))->GetDirectory()==true) nAttrib |= BS_FA_DIRECTORY;

                const TCHAR *pAttrFilePath = convertToFoxValue(destFilePath);
                res = ::SetFileAttr(pAttrFilePath, nAttrib);
                delete [] pAttrFilePath;

                if (res != BS_SDK_ERROR_NO)
                {
                    qDebug() << "SetAttributes";
                    emit tHandleError(res);
                    return;
                }
            }

        }



        ++it;
    }



    emit tContentDone();
}

void burnDialog::OnCreateDirEvent(const TCHAR *pcFullName, const TCHAR *, const TCHAR*, const TCHAR*, burnDialog *pUserData)
{

    QString strOut = QString("Directory created: %1").arg(convertToQT(pcFullName));

    pUserData->addLogItem(strOut,1);
}

void burnDialog::OnAddFileEvent(const TCHAR *pcFullPath, const TCHAR *, const TCHAR *, const TCHAR* , double , double dFileSize, burnDialog *pUserData)
{

    QString strOut = QString("File added: %1 (%2MB)").arg( convertToQT( pcFullPath ), QString::number( dFileSize/1024/1024 ));

    pUserData->addLogItem(strOut,2);
}

void burnDialog::OnFinalizeEvent(burnDialog *pUserData)
{
    emit pUserData->messageUpdated(tr("Finalize disk..."));
}

void burnDialog::OnBurnDoneEvent(const TCHAR* pcError, burnDialog *pUserData)
{

    QString strError = convertToQT( pcError );

    QString msg;
    bool isError = false;
    if(strError.isEmpty()){
        msg = tr("Disk created successfully.");
    } else {
        isError = true;
        msg = tr( "A burn error occoured: " ) + strError;
    }
    emit pUserData->actionAfterBurn( msg, isError );
}

void burnDialog::OnJobDoneEvent( burnDialog *pUserData )
{
    emit pUserData->jobDone( tr( "Job completed." ) );
}

void burnDialog::OnBurnFileEvent( const TCHAR* pcFileName, burnDialog *pUserData )
{

    emit pUserData->fileUpdated( convertToQT( pcFileName ) );

}

void burnDialog::OnProcessEvent( float fPercent, float, float fCache, double, double, burnDialog *pUserData )
{
    emit pUserData->progressChanged( ( int ) fPercent, ( int ) fCache);
}

void burnDialog::OnInfoTextEvent( const TCHAR *, int32, burnDialog *)
{

}

void burnDialog::OnStartVerifyEvent( burnDialog *pUserData )
{
    emit pUserData->messageUpdated( tr( "Start verify disk content." ) );
}

void burnDialog::OnVerifyFileEvent( const TCHAR *pcFileName, burnDialog *pUserData )
{

    QString msg = tr( "Verify file: " ) + convertToQT( pcFileName );
    emit pUserData->messageUpdated( msg );
}

void burnDialog::OnVerifyErrorEvent( const TCHAR *pcFileName, const TCHAR *pcError, burnDialog *pUserData )
{

    QString msg = QString("Verify error: %1 with file %2").arg( convertToQT( pcError ), convertToQT( pcFileName ) );
    emit pUserData->messageUpdated( msg );
}

void burnDialog::OnVerifyDoneEvent( int32 nNumErrors, burnDialog *pUserData )
{
    QString msg = QString("Verify done with %1 errors").arg( QString::number( nNumErrors ) );
    emit pUserData->messageUpdated( msg );
}

void burnDialog::OnEraseDoneEvent( const TCHAR *pcError, burnDialog *pUserData )
{

    QString strError = convertToQT( pcError );

    QString msg;
    if(strError.isEmpty()){
        msg = tr( "Disk erased successfully." );
    } else {
        msg = tr( "A erase error occoured: " ) + strError;
    }
    emit pUserData->messageUpdated(msg);
}

burnDialog::burnDialog( int sdkJobType, DBJobCommands *jobCommands, CommonTreeWidget *contentOwner, QWidget *parent ) :
    QDialog( parent ),
    thisJobType( sdkJobType ),
    ui( new Ui::burnDialog ),
    projectTree( contentOwner ),
    controlParams( jobCommands )
{
    setAttribute( Qt::WA_DeleteOnClose );
    ui->setupUi( this );
    buildCommonDialog();




    if( thisJobType == DB_JOB_BURN )
    {

        diskItem = nullptr;
        diskItem = dynamic_cast<QDiskItem *>( projectTree->topLevelItem( 0 ) );
        if( diskItem == nullptr ) return; //Bad error

        QString strWindowTitle = QString( "Burn disc [%1]" ).arg( jobCommands->burnDevice );
        setWindowTitle( strWindowTitle );

        addLogItem( tr( "Start burn project" ), 0 );

        startBurnProject(jobCommands->burnDevice);

    }
    if( thisJobType == DB_JOB_ERASE ){

        QString strWindowTitle = QString( "Erase disc [%1]" ).arg( jobCommands->burnDevice );
        setWindowTitle( strWindowTitle );

        addLogItem( tr( "Set burn device: " ) + controlParams->burnDevice, 0 );

        int32 res = ::SetBurnDevice(controlParams->burnDevice.at(0).toLatin1());
        if (res != BS_SDK_ERROR_NO)
        {
            onHandleError( res );
            return;
        }

        addLogItem( tr( "Set fast erase option: " ) + QString( controlParams->eraseFast ? "true" : "false" ), 0 );
        addLogItem( tr( "Set eject after erase option: " ) + QString( controlParams->ejectErase ? "true" : "false" ), 0 );

        switchControls(false);

        res = ::Erase( controlParams->eraseFast, controlParams->ejectErase );
        if ( res != BS_SDK_ERROR_NO )
        {
            onHandleError( res );
            return;
        }

    }

}

burnDialog::burnDialog( int sdkJobType, QString imagePath, MdiChildDiskInfo *sourceDialog, QWidget *parent ) :
    QDialog( parent ),
    thisJobType( sdkJobType ),
    strImageFilePath( imagePath ),
    ui( new Ui::burnDialog ),
    jobSource( sourceDialog )
{
    setAttribute( Qt::WA_DeleteOnClose );
    ui->setupUi( this );

    buildCommonDialog();

    if( thisJobType == DB_JOB_IMAGECREATE ){

        QString strWindowTitle = QString( "Create image from [%1]" ).arg( jobSource->getBurnDrive() );
        setWindowTitle( strWindowTitle );

        addLogItem( tr( "Start create disc image" ), 0 );

        createImageJob();

    }


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

    buildCommonDialog();

}

void burnDialog::buildCommonDialog()
{
    qRegisterMetaType<int32>("int32");

    ui->bufferValue->setValue( 0 );
    ui->totalProgress->setValue( 0 );

    logLevel = 0;
    timerId = 0;
    m_enState = ST_NONE;
    mContentThread = nullptr;

    ui->totalProgress->setRange( 0, 100 );
    ui->bufferValue->setRange( 0, 100 );
    ui->currentSpeed->setText( tr( "" ) );
    ui->currentTask->setText( tr( "" ) );

    const TCHAR *pEmptyPath = convertToFoxValue("");
    ::SetImageFilePath(pEmptyPath);
    delete [] pEmptyPath;

    myStartBurnCommand = new QAction( this );
    connect( myStartBurnCommand, SIGNAL( triggered() ), this, SLOT( startBurnNow() ) );

    connect( this, SIGNAL(progressChanged(int,int)), this, SLOT(UpdateMyUI(int,int)), static_cast<Qt::ConnectionType>(Qt::QueuedConnection|Qt::UniqueConnection) );
    connect( this, SIGNAL( messageUpdated(QString) ), this, SLOT( myPostMessage(QString)), static_cast<Qt::ConnectionType>( Qt::QueuedConnection | Qt::UniqueConnection ) );
    connect( this, SIGNAL( fileUpdated(QString) ), this, SLOT( myPostFileEvent(QString) ), static_cast<Qt::ConnectionType>( Qt::QueuedConnection | Qt::UniqueConnection ) );
    connect( this, SIGNAL( jobDone(QString) ), this, SLOT( myJobDoneEvent(QString) ), static_cast<Qt::ConnectionType>( Qt::QueuedConnection | Qt::UniqueConnection ) );
    connect( this, SIGNAL( actionAfterBurn(QString,bool) ), this, SLOT( myActionAfterBurn(QString,bool) ), static_cast<Qt::ConnectionType>( Qt::QueuedConnection | Qt::UniqueConnection ) );


    //For Debug we set to 2
    logLevel = 0;
    ui->logLevel->addItem( tr( "Information" ) );
    ui->logLevel->addItem( tr( "Details/Error" ) );
    ui->logLevel->addItem( tr( "Debug" ) );
    ui->logLevel->setCurrentIndex( logLevel );

    connect( ui->logLevel, SIGNAL( currentIndexChanged(int) ), this, SLOT( debugStateChanged(int) ) );

    SetAddFileEventCallback( ( AddFileEvent ) burnDialog::OnAddFileEvent, this );
    SetCreateDirEventCallback( ( CreateDirEvent ) burnDialog::OnCreateDirEvent, this );

    //if(thisJobType==DB_JOB_ERASE)
    SetBurnDoneEventCallback( ( BurnDoneEvent ) burnDialog::OnBurnDoneEvent, this );
    SetBurnFileEventCallback( ( BurnFileEvent ) burnDialog::OnBurnFileEvent, this );
    SetEraseDoneEventCallback( ( EraseDoneEvent ) burnDialog::OnEraseDoneEvent, this );
    SetFinalizeEventCallback( ( FinalizeEvent ) burnDialog::OnFinalizeEvent, this );
    SetInfoTextEventCallback( ( InfoTextEvent ) burnDialog::OnInfoTextEvent, BS_IL_INFO, this );
    SetJobDoneEventCallback( ( JobDoneEvent ) burnDialog::OnJobDoneEvent, this );
    SetProcessEventCallback( ( ProcessEvent ) burnDialog::OnProcessEvent, this );
    SetStartVerifyEventCallback( ( StartVerifyEvent ) burnDialog::OnStartVerifyEvent, this );
    SetVerifyDoneEventCallback( ( VerifyDoneEvent ) burnDialog::OnVerifyDoneEvent, this );
    SetVerifyErrorEventCallback( ( VerifyErrorEvent ) burnDialog::OnVerifyErrorEvent, this );
    SetVerifyFileEventCallback ( ( VerifyFileEvent ) burnDialog::OnVerifyFileEvent, this );

    QMenu *menu = new QMenu();
    QAction *testAction = new QAction( "Change burning device" , this );
    menu->addAction( testAction );

    ui->testmenu->setText( "New burn" );
    ui->testmenu->setMenu( menu );



    /*

    if(sdkJobType==3){

        QString strDriveName = ConfigurationPage::mSettings.value("CurrentBurnDevice","").toString();
        QString strWindowTitle = QString("Copy disc to image: %1").arg(strDriveName);
        setWindowTitle(strWindowTitle);

        addLogItem(tr("Start copy disk"),0);
        step3Project();
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
        extractAudioTrack();
    }

    if(sdkJobType==9){ //Grab audio File
        QString strWindowTitle = "Grab audio track";
        setWindowTitle(strWindowTitle);
        addLogItem(tr("Grab track to audio file"),0);
        step9Project();
    }
    */
}

burnDialog::~burnDialog()
{
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

    const TCHAR *pSaveFilePath = convertToFoxValue(strSavePath);
    nError = ::GrabAudioTrack(params, info.nTrackNumber, pSaveFilePath);
    delete [] pSaveFilePath;


    if (nError != BS_SDK_ERROR_NO)
    {
        onHandleError(nError);
        return;
    }

    switchControls(false);
}

//Creat Image


void burnDialog::extractAudioTrack()
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

        const TCHAR *pSavePath = convertToFoxValue(strSavePath);
        nError = ::SaveTrackToFile(BS_READ_DEVICE, nTRack,
                                   pSavePath, nFormat);
        delete [] pSavePath;

        switchControls(false);
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

        const TCHAR *pSavePath = convertToFoxValue(strSavePath);
        nError = ::SaveTrackToFile(BS_READ_DEVICE, nTRack,
                                   pSavePath, nFormat);
        delete [] pSavePath;

        switchControls(false);


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

        const TCHAR *pSourceFilePath = convertToFoxValue(strSource);
        nError = ::SetImageFilePath(pSourceFilePath);
        delete [] pSourceFilePath;

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

        const TCHAR *pFullPath = convertToFoxValue(tSelected->GetFullPath());
        const TCHAR *pDestPath = convertToFoxValue(strSavePath);
        nError = ::ImportFile(hSession, pFullPath, pDestPath);
        delete [] pFullPath;
        delete [] pDestPath;

        if (nError != BS_SDK_ERROR_NO)
        {
            onHandleError(nError);
            return;
        }

        ::CloseDiskSession(hSession);
        //And it continue

        switchControls(false);
    }

}

//BUrn DIsk Image
void burnDialog::burnDiskImage()
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

        switchControls(false);

        const TCHAR *pFilePath = convertToFoxValue(strSavePath);
        res = ::BurnISO(pFilePath, opt);
        delete [] pFilePath;

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

        const TCHAR *pFilePath = convertToFoxValue(strSavePath);
        fileToAdd.lpszSourceFilePath = pFilePath;

        res = ::AddFile(fileToAdd);

        delete [] pFilePath;

        if (res != BS_SDK_ERROR_NO)
        {
            onHandleError(res);
            return;
        }
        addLogItem(tr("Start preparing disk now (::Prepare())"),0);
        res = ::Prepare();
        if (res != BS_SDK_ERROR_NO)
        {
            onHandleError(res);
            return;
        }

        switchControls(false);

        addLogItem(tr("Start Burn process now (::Burn())"),0);
        res = ::Burn();
        if (res != BS_SDK_ERROR_NO)
        {
            onHandleError(res);
            return;
        }
    }

}

//copy
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

    switchControls(false);
}

void burnDialog::startBurnProject(QString burnDrive)
{
    //All infos are in tree->disk
    ui->bufferValue->setValue( 0 );
    ui->totalProgress->setValue( 0 );

    int32 res = 0;

    //Set Project
    RuleManager::ProjectType project_type = diskItem->getProjectType();
    switch( project_type ){
    case RuleManager::TYPE_PROJECT_OPEN:
        res = ::CreateProject( BS_BURNER_ISOUDF, BS_CONTINUE_NO_SESSION );
        addLogItem( tr( "Set project type: BS_BURNER_ISOUDF" ), 0 );
        break;
    case RuleManager::TYPE_PROJECT_AUDIOCD:
        res = ::CreateProject( BS_BURNER_AUDIO, BS_CONTINUE_NO_SESSION );
        addLogItem( tr( "Set project type: BS_BURNER_AUDIO" ), 0 );
        break;
    case RuleManager::TYPE_PROJECT_MIXEDMODE:
        res = ::CreateProject( BS_BURNER_MIXEDMODE, BS_CONTINUE_NO_SESSION );
        addLogItem( tr( "Set project type: BS_BURNER_MIXEDMODE" ), 0 );
        break;
    case RuleManager::TYPE_PROJECT_VIDEODVD:
        res = ::CreateProject( BS_BURNER_VIDEODVD, BS_CONTINUE_NO_SESSION );
        addLogItem( tr( "Set project type: BS_BURNER_VIDEODVD" ), 0 );
        break;
    case RuleManager::TYPE_PROJECT_BLURAYVIDEO:
        res = ::CreateProject( BS_BURNER_BLURAY, BS_CONTINUE_NO_SESSION );
        addLogItem( tr( "Set project type: BS_BURNER_BLURAY" ), 0 );
        break;
    case RuleManager::TYPE_PROJECT_UDF:
        res = ::CreateProject( BS_BURNER_UDFDVD, BS_CONTINUE_NO_SESSION );
        addLogItem( tr( "Set project type: BS_BURNER_UDFDVD" ), 0 );
        break;
    case RuleManager::TYPE_PROJECT_ISO:
        res = ::CreateProject( BS_BURNER_DATA, BS_CONTINUE_NO_SESSION );
        addLogItem( tr( "Set project type: BS_BURNER_DATA" ), 0 );
        break;
    case RuleManager::TYPE_PROJECT_ISOUDF:
        res = ::CreateProject( BS_BURNER_ISOUDF, BS_CONTINUE_NO_SESSION );
        addLogItem( tr( "Set project type: BS_BURNER_ISOUDF" ), 0 );
        break;
    case RuleManager::TYPE_PROJECT_VIDEOCD:
        res = ::CreateProject( BS_BURNER_VCD, BS_CONTINUE_NO_SESSION );
        addLogItem( tr( "Set project type: BS_BURNER_VCD" ), 0 );
        break;
    case RuleManager::TYPE_PROJECT_SUPERVIDEO:
        res = ::CreateProject( BS_BURNER_SVCD, BS_CONTINUE_NO_SESSION );
        addLogItem( tr( "Set project type: BS_BURNER_SVCD" ), 0 );
        break;
    default:
        addLogItem( tr( "Unsupported project type" ), 0 );
        return;

    }
    if ( res != BS_SDK_ERROR_NO )
    {
        onHandleError( res );
        return;
    }

    bImageWriter = false;
    QString strDriveName = burnDrive;
    if( strDriveName.at( 0 ) == '#' ){
        bImageWriter = true;
        addLogItem( tr( "Imagewriter activated" ), 0 );
    }

    qDebug( "Set Burn Device" );

    res = ::SetBurnDevice( strDriveName.at( 0 ).toLatin1() );
    if ( res != BS_SDK_ERROR_NO )
    {
        onHandleError( res );
        return;
    }

    addLogItem( tr( "Set burn device: " ) + strDriveName, 0 );

    addLogItem( tr( "Set options" ), 0 );
    SOptions opt;
    ::GetOptions( &opt );

    const TCHAR *pLabel = convertToFoxValue(diskItem->getDisktitle());
    _tcsncpy( opt.chVolumeLabel, pLabel, 128 );
    delete [] pLabel;

    opt.bEjectAfterBurn = diskItem->getFeatureEject();
    addLogItem( tr( "Option eject after burn: " ) + QString::number( diskItem->getFeatureEject() ), 1 );
    opt.bFinalize = diskItem->getFeatureFinishDisk();
    addLogItem( tr( "Option finalize disk: " ) + QString::number( diskItem->getFeatureFinishDisk() ), 1 );
    opt.bJolietFileSystem = diskItem->getUseJoliet();
    addLogItem( tr( "Option joliet extension: " ) + QString::number( diskItem->getUseJoliet() ), 1 );
    opt.bRockRidgeFileSystem = diskItem->getUseRockRidge();
    addLogItem( tr( "Option rock ridge: " ) + QString::number( diskItem->getUseRockRidge() ), 1 );
    opt.bVerifyAfterBurn = diskItem->getFeatureVerify();
    addLogItem( tr( "Option verify: " ) + QString::number( diskItem->getFeatureVerify() ), 1 );
    opt.bPerformOPC = diskItem->getFeatureOPC();
    addLogItem( tr( "Option optimum power calibration: " ) + QString::number( diskItem->getFeatureOPC() ), 1 );
    opt.bAutoErase = diskItem->getFeatureAutoErase();
    addLogItem( tr( "Option auto erase: " ) + QString::number( diskItem->getFeatureAutoErase() ), 1 );
    opt.bPadDataTracks = diskItem->getFeaturePadDataTrack();
    addLogItem( tr( "Option pad data track: " ) + QString::number( diskItem->getFeaturePadDataTrack() ), 1 );
    opt.bTestBurn = diskItem->getFeatureSimulate();
    addLogItem( tr( "Option simulate burn: " ) + QString::number( diskItem->getFeatureSimulate() ), 1 );

    if( diskItem->getDoBootDisk() == true ){
        addLogItem( tr( "Bootdisk enabled" ), 1 );
        opt.bBootable = 1;

        const TCHAR *pBootImage = convertToFoxValue(diskItem->getBootDiskFile());
        _tcsncpy( opt.chBootImage, pBootImage, 260 );
        delete [] pBootImage;

        addLogItem( tr( "Set boot image: " ) + diskItem->getBootDiskFile(), 1 );
    }
    res = ::SetOptions( opt );
    if (res != BS_SDK_ERROR_NO)
    {
        onHandleError( res );
        return;
    }



    //SetOptions infoEx
    SISOInfoEx isoInfo;
    memset( &isoInfo, 0, sizeof( isoInfo ) );
    //::GetISOInfoEx(&isoInfo);


    if( ( project_type==RuleManager::TYPE_PROJECT_OPEN )
            || ( project_type==RuleManager::TYPE_PROJECT_MIXEDMODE )
            || ( project_type==RuleManager::TYPE_PROJECT_ISO )
            || ( project_type==RuleManager::TYPE_PROJECT_ISOUDF ) )
    {
        addLogItem( tr( "Set options ISOInfoEx" ), 1 );
        isoInfo.ISOAllowLowercaseNames = diskItem->getAllowLowerCaseNames();
        addLogItem( tr( "Set allow lower case names: " ) + QString::number( diskItem->getAllowLowerCaseNames() ), 1 );
        isoInfo.ISOAllowManyDirectories = diskItem->getAllowManyDirectories();
        addLogItem( tr( "Set allow many directories: " ) + QString::number( diskItem->getAllowManyDirectories() ), 1 );
        isoInfo.ISOAllowLongISO9660Names = diskItem->getAllowLongFileNames();
        addLogItem( tr( "Set allow long ISO names: " ) + QString::number( diskItem->getAllowLongFileNames() ), 1 );
        isoInfo.ISOAllowLongJolietNames = diskItem->getAllowLongJolietFileNames();
        addLogItem( tr( "Set allow long joliet names: " ) + QString::number( diskItem->getAllowLongJolietFileNames() ), 1 );
        isoInfo.ISOLevel = BS_ISO_LEVEL_1 + diskItem->getISOFsType();
        addLogItem( tr( "Set ISO level: " ) + QString::number( diskItem->getISOFsType() ), 1 );
        isoInfo.ISOAddSuffix = ((diskItem->getAllowLongJolietFileNames()==0)?1:0);
        addLogItem( tr( "Set not write ISO Extension: " ) + QString::number( ( diskItem->getAllowLongJolietFileNames() == 0 ) ? 1 : 0 ), 1 );

    }

    if( ( project_type==RuleManager::TYPE_PROJECT_OPEN )
            || ( project_type==RuleManager::TYPE_PROJECT_MIXEDMODE )
            || ( project_type==RuleManager::TYPE_PROJECT_ISO )
            || ( project_type==RuleManager::TYPE_PROJECT_VIDEODVD )
            || ( project_type==RuleManager::TYPE_PROJECT_ISOUDF ) )
    {

        const TCHAR *pISOAbstractFileIdentifier = convertToFoxValue(diskItem->getAbstractFile());
        _tcsncpy( isoInfo.ISOAbstractFileIdentifier, pISOAbstractFileIdentifier, 36 );
        delete [] pISOAbstractFileIdentifier;
        const TCHAR *pISODataPreparerIdentifier = convertToFoxValue(diskItem->getDatapreparer());
        _tcsncpy( isoInfo.ISODataPreparerIdentifier, pISODataPreparerIdentifier, 128 );
        delete [] pISODataPreparerIdentifier;
        const TCHAR *pISOSetIdentifier = convertToFoxValue(diskItem->getVolumeSet());
        _tcsncpy( isoInfo.ISOSetIdentifier, pISOSetIdentifier, 128 );
        delete [] pISOSetIdentifier;
        const TCHAR *pISOSystemIdentifier = convertToFoxValue(diskItem->getSystemId());
        _tcsncpy( isoInfo.ISOSystemIdentifier, pISOSystemIdentifier, 31 );
        delete [] pISOSystemIdentifier;
        const TCHAR *pISOPublisherIdentifier = convertToFoxValue(diskItem->getPublisher());
        _tcsncpy( isoInfo.ISOPublisherIdentifier, pISOPublisherIdentifier, 36 );
        delete [] pISOPublisherIdentifier;
        const TCHAR *pISOCopyrightFileIdentifier = convertToFoxValue(diskItem->getCoprightFile());
        _tcsncpy( isoInfo.ISOCopyrightFileIdentifier, pISOCopyrightFileIdentifier, 36 );
        delete [] pISOCopyrightFileIdentifier;
        const TCHAR *pISOApplicationIdentifier = convertToFoxValue(diskItem->getApplication());
        _tcsncpy( isoInfo.ISOApplicationIdentifier, pISOApplicationIdentifier, 128 );
        delete [] pISOApplicationIdentifier;
        const TCHAR *pISOBiblioIdentifier = convertToFoxValue(diskItem->getBibliographicFile());
        _tcsncpy( isoInfo.ISOBiblioIdentifier, pISOBiblioIdentifier, 36 );
        delete [] pISOBiblioIdentifier;

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

        res = ::SetISOInfoEx( isoInfo );
        if ( res != BS_SDK_ERROR_NO )
        {
            onHandleError( res );
            return;
        }
    }

    if( diskItem->getDoBootDisk() == true ){
        addLogItem( tr( "Set options SBootInfoEx" ), 0 );
        SBootInfoEx info;
        ::GetBootInfoEx( &info );

        const TCHAR *pDevID = convertToFoxValue(diskItem->getBootDeveloperID());
        _tcsncpy( info.DeveloperID, pDevID, 24 );
        delete [] pDevID;

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
    //Ich versteh nicht ganz was hier passiert
    //Ich geh davon aus, dass er hier versucht die CD zu setzen.
    /*
    if( ( project_type == RuleManager::TYPE_PROJECT_AUDIOCD )
            || ( project_type == RuleManager::TYPE_PROJECT_MIXEDMODE ) )
    {
        //CDText

        SFileAudioProperty data;

        QString  diskRoot = PATHSEPSTRING;
        if( project_type == RuleManager::TYPE_PROJECT_MIXEDMODE ){
            diskRoot += "audio"; //Es gibt ja zwei Roots dort "data"
        }

        //Was passiert mit dem Diskroot?

        const TCHAR *pSourceFilePath = nullptr;
        if( diskItem->getBootDiskFile().length() > 0 ){
            pSourceFilePath = convertToFoxValue(diskItem->getBootDiskFile());
            data.lpszSourceFilePath = pSourceFilePath;
        }


        const TCHAR *pTitle = nullptr;
        if( diskItem->getTitle().length() > 0 ){
            pTitle = convertToFoxValue(diskItem->getTitle());
            data.lpszTitle = pTitle;
        }


        const TCHAR *pArranger = nullptr;
        if( diskItem->getArranger().length() > 0 ){
            pArranger = convertToFoxValue(diskItem->getArranger());
            data.Arranger = pArranger;
        }


        const TCHAR *pComposer = nullptr;
        if( diskItem->getComposer().length() > 0 ){
            pComposer = convertToFoxValue(diskItem->getComposer());
            data.Composer = pComposer;
        }


        const TCHAR *pMessage = nullptr;
        if( diskItem->getMessage().length() > 0 ){
            pMessage = convertToFoxValue(diskItem->getMessage());
            data.Message = pMessage;
        }


        const TCHAR *pPerformer = nullptr;
        if( diskItem->getPerformer().length() > 0 ){
            pPerformer = convertToFoxValue(diskItem->getPerformer());
            data.Performer = pPerformer;
        }


        const TCHAR *pSongWriter = nullptr;
        if( diskItem->getSongWriter().length() > 0 ){
            pSongWriter = convertToFoxValue(diskItem->getSongWriter());
            data.SongWriter = pSongWriter;
        }


        const TCHAR *pMCN_ISRC = nullptr;
        if( diskItem->getUPCEAN().length() > 0 ){
            pMCN_ISRC = convertToFoxValue(diskItem->getUPCEAN());
            data.lpszMCN_ISRC = pMCN_ISRC;
        }



        //data.nPause = audio_track->getPause().toInt(); //NOt allowed on disk
        //data.bPauseInFrames = audio_track->get

        res = ::SetAudioFileProperty( data );

        if(pSourceFilePath) delete [] pSourceFilePath;
        if(pTitle) delete [] pTitle;
        if(pArranger) delete [] pArranger;
        if(pComposer) delete [] pComposer;
        if(pMessage) delete [] pMessage;
        if(pPerformer) delete [] pPerformer;
        if(pSongWriter) delete [] pSongWriter;
        if(pMCN_ISRC) delete [] pMCN_ISRC;

        if ( res != BS_SDK_ERROR_NO )
        {
            onHandleError( res );
            return;
        }
    }
    */





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

        //Crash in case of wrong structure
        //wcsncpy(cUDFOptions.ImplementationID, diskItem->getDisktitle().toStdWString().c_str(), 22*sizeof(TCHAR));

        cUDFOptions.Version = UDF_VERSIONS[diskItem->getUDFType()];
        addLogItem(tr("Set UDF version: ")+QString::number(diskItem->getUDFType()),1);
        cUDFOptions.PartitionType = diskItem->getUDFPartition();
        addLogItem(tr("Set UDF parition: ")+QString::number(diskItem->getUDFPartition()),1);
        cUDFOptions.WriteFileStreams = diskItem->getUDFWriteStream();
        addLogItem(tr("Set UDF write file streams: ")+QString::number(diskItem->getUDFWriteStream()),1);

        res = ::SetUDFOptions( cUDFOptions );
        if ( res != BS_SDK_ERROR_NO )
        {
            onHandleError( res );
            return;
        }
    }

    addLogItem( tr( "Ready set options" ), 0 );

    if( diskItem->getProjectType() == RuleManager::TYPE_PROJECT_BLURAYVIDEO ){
        checkBluRayProject();
    }

    //Also irgednwas stimmt da nicht.
    //Ordner mit Leerzeichen?

    //First we try to set AddFile Event and look what happen
    if( !mContentThread ){
        mContentThread  = new ContentThread( projectTree );
        connect( mContentThread, SIGNAL( tContentDone() ), this, SLOT( onContentDone() ) );
        connect( mContentThread, SIGNAL( tHandleError(int32) ), this, SLOT( onHandleError(int32) ) );
    }

    mContentThread->start();

}

void burnDialog::createImageJob()
{
    //All infos are in tree->disk
    ui->bufferValue->setValue( 0 );
    ui->totalProgress->setValue( 0 );
    //Set read device fpr image creation. THe source drive is the main drive in general settings.
    //It is not #Imagedrive
    QString strDriveName = jobSource->getBurnDrive();

    addLogItem( QString( "Set read device %1" ).arg( strDriveName ), 0 );

    int32 res = ::SetReadDevice( strDriveName.at(0).toLatin1() );
    if ( res != BS_SDK_ERROR_NO )
    {
        onHandleError( res );
        return;
    }

    QString strSavePath = QDir::toNativeSeparators( jobSource->getImagePath() );

    SCreateImageParams cCreateImageParams;
    cCreateImageParams.cErrorParams.bErrorCorrection = jobSource->getErrorCorrection();
    cCreateImageParams.cErrorParams.bBlankBadSectors = false;

    addLogItem( QString( "Error Correction %1" ).arg( QString::number( jobSource->getErrorCorrection() ) ), 0 );

    //OK, how to copy the stringvalue to it?
    const TCHAR *pFilePath = convertToFoxValue(strSavePath);
    _tcsncpy( cCreateImageParams.lpszImagePath, pFilePath, 260 );
    delete [] pFilePath;

    addLogItem( QString( "Set image file %1" ).arg( strSavePath ), 0 );


    cCreateImageParams.nImageType = ( jobSource->getImageCreateMethod() == 1 ) ? BS_IMG_ISO : BS_IMG_BIN;
    cCreateImageParams.cErrorParams.nHardwareRetryCount = jobSource->getImageCreateHardRetry();
    cCreateImageParams.cErrorParams.nSoftwareRetryCount = jobSource->getImageCreateSoftRetry();

    ::SetReadSpeed(BS_READ_DEVICE, BS_MAX_SPEED);

    int32 nSpeed = 0;
    ::GetReadSpeed( BS_READ_DEVICE, &nSpeed );

    if ( nSpeed == BS_MAX_SPEED ){
        ::GetMaxReadSpeed( BS_READ_DEVICE, &nSpeed );
    }
    ui->currentSpeed->setText( QString( "%1 KB/s" ).arg( QString::number( nSpeed ) ) );
    addLogItem( QString( "Readspeed %1 KB/s" ).arg( QString::number( nSpeed ) ), 0 );


    //Wir müssen hier beide Als Maske übergeben.
    int8 dwOperation = 0;
    if( jobSource->getImageJobCreate() != 0 ){
        dwOperation = BS_IMGTASK_CREATE;
        addLogItem( tr( "Set job type: BS_IMGTASK_CREATE" ), 0 );
    }
    if( jobSource->getImageJobVerify() != 0 ){
        dwOperation |= BS_IMGTASK_VERIFY;
        addLogItem( tr( "Set job type: BS_IMGTASK_VERIFY" ), 0 );
    }

    res = ::CreateImage( cCreateImageParams, dwOperation );
    if ( res != BS_SDK_ERROR_NO )
    {
        onHandleError( res );
        return;
    }

    switchControls(false);

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
    switchControls(false);

    res = ::Burn();
    if (res != BS_SDK_ERROR_NO)
    {
        onHandleError(res);
        return;
    }
}

void burnDialog::addLogItem(QString strText, int logLevel)
{
    //Debug = 2
    if(logLevel==ui->logLevel->currentIndex()){
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

void burnDialog::onHandleError( int32 res )
{
    TCHAR chError[2048];
    int32 nLength = 2048;

    // Get error description
    ::GetText( res, chError, &nLength );

    QString strError = QString( "SDK Error: %1" ).arg(convertToQT( chError ) );

    addLogItem( strError, 0 );
    qDebug("Error: %s",strError.toLatin1().constData());
    switchControls(true);
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
        return;
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
        return;
    }

    SOptions cOptions;
    ::GetOptions(&cOptions);
    cOptions.bFinalize = 1;
    addLogItem(tr("Blu-ray Project: Force disk finalize flag."),1);
    res = ::SetOptions(cOptions);
    if (res != BS_SDK_ERROR_NO)
    {
        onHandleError(res);
        return;
    }

}

void burnDialog::timerEvent(QTimerEvent *)
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
    switchControls(true);
    addLogItem(msg,0);
}

void burnDialog::myPostMessage(QString msg)
{
    addLogItem(msg, 0);
}

void burnDialog::myPostFileEvent(QString file)
{
    //addLogItem(strFile); //We will not log here because it is overlapped and some files are missed.    QFontMetrics fm;
    QFont f = ui->currentTask->font();
    QFontMetrics fm = QFontMetrics(f);
    int max_w = ui->currentTask->width();
    QString elided_text = fm.elidedText(file, Qt::ElideMiddle, max_w);
    ui->currentTask->setText(elided_text); //need change
}

void burnDialog::myActionAfterBurn(QString msg, bool isError)
{
    addLogItem(msg, 0);
    qDebug() << msg;
    if(isError==false){ //isok
        if(ConfigurationPage::mSettings.value("AutoSaveProjects", false).toBool()==true){
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
            generator.write(&file);
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

    const TCHAR *pFilePath = convertToFoxValue(QDir::toNativeSeparators(fileName));
    ::SaveLogToFile(pFilePath);
    delete [] pFilePath;


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

    const TCHAR *pFilePath = convertToFoxValue(QDir::toNativeSeparators(fileName));
    int32 res = ::SetImageFilePath(pFilePath);
    delete [] pFilePath;


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

    const TCHAR *pFilePath = convertToFoxValue(initEmpty);
    ::SetImageFilePath(pFilePath);
    delete [] pFilePath;

    addLogItem(tr("Check writeable media now."),0);
    timerId = startTimer(1000);
}

void burnDialog::switchControls(bool restore)
{

    if(restore==true){
        m_enState = ST_NONE;
        ui->cancel->setText(tr("Close"));
        ui->logLevel->setEnabled(true);
        ui->saveLog->setEnabled(true);
        if(thisJobType==1 || thisJobType==5)ui->testmenu->setEnabled(true);
    }else{
        m_enState = ST_BURN;
        ui->cancel->setText(tr("Stop"));
        ui->logLevel->setEnabled(false);
        ui->saveLog->setEnabled(false);
        ui->testmenu->setEnabled(false);
    }

}
