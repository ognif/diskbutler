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

#include <QtWidgets>
#include "mdichild_diskinfo.h"
#include "FoxSDKBurningLib.h"
#include "mainwindow.h"
#include "settingspages.h"
#include "messanger.h"

MdiChildDiskInfo::MdiChildDiskInfo(QWidget* parent, const QString &device)
    :strBurnDrive(device){
      setAttribute(Qt::WA_DeleteOnClose);
      mProjectType = RuleManager::TYPE_PROJECT_DISKINFO;
      isEraseable = false;
      isOpen = false;
      hideEmptyFields = false;
      bCopyVerify = false;
      bImageVerify = false;
      bCopyEject = false;
      bUseErrorCorrection = ConfigurationPage::mSettings.value("ErrorCorrection", true).toBool();
      bBlankBadSectors = false;
      bIsEmptyDisk = false;
      bIsISO = true;
      nReadMethod = 0; //0=ISO;1=RAW;2=RAW+SUB
      nWriteMethod = 0; //0=DAO;1=DAO96
      nHardwareRetry = ConfigurationPage::mSettings.value("HardwareRetryCount", 7).toInt();
      nSoftwareRetry = ConfigurationPage::mSettings.value("HardwareRetryCount", 1).toInt();
      bEraseEject = ConfigurationPage::mSettings.value("EjectAfterErase", true).toBool();
      bEraseFast = true;
      strImagePath = tr("");
      nImageCreateMethode = 0;
      thisSuccessfullCreated = true;

      QString wTitle = tr("Disk Info");
      wTitle += " (";
      wTitle += strBurnDrive;
      wTitle += ")";

      setWindowTitle(wTitle);

      diskInfoTable = new QTableWidget();
      treeWidget = new QTreeWidget();

      splitter = new QSplitter;
      splitter->addWidget(diskInfoTable);
      splitter->addWidget(treeWidget);
      setCentralWidget(splitter);

      //myUpdateMenus = new QAction(this);
      //connect(myUpdateMenus, SIGNAL(triggered()), parent, SLOT(updateMenus()));
      //myRefreshMenus = new QAction(this);
      //connect(myRefreshMenus, SIGNAL(triggered()), parent, SLOT(specialActionDiskInfo()));


      readDiskInfo();
}

void MdiChildDiskInfo::updateDiskInfo()
{
  readDiskInfo();
  myRefreshMenus->trigger(); //We call it here, because at another place the windows is not existsing.

}

void MdiChildDiskInfo::closeEvent(QCloseEvent *event)
{
  event->accept();
}

int32 MdiChildDiskInfo::ExtractTextFromHandle(int32 handle,int32 nCDTCI, QString& rText)
{
    int32 nLen = 0;
    int32 res;

    res = ::GetCDTextDiskTagString(handle, nCDTCI, nullptr, &nLen);
    if(res != BS_SDK_ERROR_NO)
    {
        return res;
    }

    if(!nLen || nLen == 1)
    {
        rText = tr("");
        return BS_SDK_ERROR_NO;
    }

    TCHAR *pBuf = new TCHAR[nLen];
    res = ::GetCDTextDiskTagString(handle, nCDTCI, pBuf, &nLen);
    if(res != BS_SDK_ERROR_NO)
    {
        delete[] pBuf;
        return res;
    }

    pBuf[nLen-1] = _T('\0');
#if defined (WIN32)
    rText = QString::fromUtf16(pBuf);
#else
    rText = QString::fromUtf8(pBuf);
#endif
    delete[] pBuf;

    return BS_SDK_ERROR_NO;
}

int32 MdiChildDiskInfo::ExtractTrackTextFromHandle(int32 handle, int32 nTrack, int32 nCDTCI, QString& rText)
{
    int32 nLen = 0;
    int32 res;

    res = ::GetCDTextTrackTagString(handle, nTrack, nCDTCI, nullptr, &nLen);
    if(showDiskbutlerMessage(res, this)==false) {
        return res;
    }

    if(!nLen || nLen == 1)
    {
        rText = tr("");
        return BS_SDK_ERROR_NO;
    }

    TCHAR *pBuf = new TCHAR[nLen];
    res = ::GetCDTextTrackTagString(handle, nTrack, nCDTCI, pBuf, &nLen);
    if(showDiskbutlerMessage(res, this)==false) {
        delete[] pBuf;
        return res;
    }

    pBuf[nLen-1] = _T('\0');
#if defined (WIN32)
    rText = QString::fromUtf16(pBuf);
#else
    rText = QString::fromUtf8(pBuf);
#endif

    delete[] pBuf;

    return BS_SDK_ERROR_NO;
}

void MdiChildDiskInfo::readDiskInfo()
{
    //QListWidget *listWidget = new QListWidget();
    //for (int i = 0; i < 12; i++) {
    //  QListWidgetItem *item = new QListWidgetItem(tr("feature ")+QString::number(i));
    //  listWidget->addItem(item);
    //}
    diskInfoTable->clear();
    treeWidget->clear();

    diskInfoTable->setRowCount(50);
    diskInfoTable->setColumnCount(2);
    diskInfoTable->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    diskInfoTable->horizontalHeader()->setStretchLastSection(true);
    diskInfoTable->horizontalHeader()->hide();
    diskInfoTable->verticalHeader()->hide();
    //diskInfoTable->setContentsMargins(0, 0, 0, 0);
    diskInfoTable->verticalHeader()->setDefaultSectionSize(21);



    //Group
    QTableWidgetItem* item1 = new QTableWidgetItem(tr("Physical Information"));
    item1->setFlags(Qt::NoItemFlags);
    QFont originalFont = (item1)->font();
    originalFont.setBold(true);
    item1->setFont(originalFont);
    QTableWidgetItem* item2 = new QTableWidgetItem("");
    item2->setFlags(Qt::NoItemFlags);
    diskInfoTable->setItem(0,0,item1);
    diskInfoTable->setItem(0,1,item2);
    diskInfoTable->setSpan(0,0,1,2);



    treeWidget->header() ->close ();

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(treeWidget);
    treeItem->setText(0, tr("Disk"));
    treeItem->setIcon(0,QIcon(":/res/disk_main.png"));
    treeItem->setExpanded(true);

    double nSectorSize = 0.0;
    const double MEGABYTE = 1024.0 * 1024.0;
    pInformations stDiskInfo;
    pUDF stUDFInfo;
    stUDFInfo.isUDF = 0;

    pISO stISOInfo;
    stISOInfo.isISO = 0;

    pAudio stAudioInfo;
    stAudioInfo.isAudio = 0;

    ::SetBurnDevice(strBurnDrive.at(0).toLatin1());


    QString tmp;
    int32 nSize=20;
    SSpeed Speeds[20];
    ::GetPossibleBurnSpeeds(BS_CURRENT_DEVICE, Speeds, &nSize);
    for(int i=0;i<nSize;++i)
    {
        QString tspeed = QString("%1 ,").arg(Speeds[i].nSpeedInKBPerSec);
        tmp += tspeed;
    }
    if(tmp.right(1) == QChar(',')){

        tmp.remove(tmp.size()-1,2);
    }

    stDiskInfo.strMediaSpeeds = tmp;


    BS_BOOL	bReady = BS_FALSE;
    int32 nError = ::IsDeviceReady(BS_CURRENT_DEVICE, &bReady);


    SMediumInfo mi;
    nError = ::GetMediumInformation(BS_CURRENT_DEVICE, &mi);
    if(showDiskbutlerMessage(nError, this)==false) {
        thisSuccessfullCreated=false;
        return;
    }


    isEraseable = false;

    if(mi.wMediumTypeCode == BS_DVD_PLUSRW ||
            mi.wMediumTypeCode == BS_DVD_RW_RO ||
            mi.wMediumTypeCode == BS_BLURAY_RE ||
            mi.wMediumTypeCode == BS_DVD_RAM  ||
            mi.wMediumTypeCode == BS_DVD_RW_SR ||
            mi.wMediumTypeCode == BS_CD_RW ||
            mi.wMediumTypeCode == BS_DVD_RWDL_PLUS){
        isEraseable = true;
    }



    //for (int32 i = mi.nFirstSession; i <= mi.nLastSession; i++)
    //nLastSession = 0 is invalid. But the block before is valid for virtual disc from Image Writer.
    //So we need to stop try to read from lastSession = 0 but avoid a return.
    for (int32 i = mi.nFirstSession; i <= mi.nLastSession && ( mi.nLastSession != 0); i++)
    {
        SSessionInfo si;
        nError = ::GetSessionInformation(BS_CURRENT_DEVICE, i, &si);
        if(showDiskbutlerMessage(nError, this)==false) {
            thisSuccessfullCreated=false;
            return;
        }

        //SessionSize are sectors? What to do with CD and AudioCD?

        QTreeWidgetItem *treechildItem = new QTreeWidgetItem();
        treechildItem->setText(0, tr("unknown"));
        treechildItem->setIcon(0,QIcon(":/res/disk_session.png"));

        treeItem->addChild(treechildItem);

        double nDiskSize = 0.0;

        //Now we need to start the loop

        for (int32 j = si.nFirstTrack; j <= si.nLastTrack && ( si.nLastTrack != 0); ++j)
        {
            STrackInfo ti;
            nError = ::GetTrackInformation(BS_CURRENT_DEVICE, j, &ti);
            if(showDiskbutlerMessage(nError, this)==false) {
                thisSuccessfullCreated=false;
                return;
            }

            QString strInformation;
            switch(ti.nFormat)
                {
                case BS_TF_AUDIO:
                  strInformation = tr("Audio ");
                  nSectorSize = 2352.0;
                  break;
                case BS_TF_DATA_MODE1:
                  nSectorSize = 2048.0;
                  strInformation = tr("Mode 1 ");
                  break;
                case BS_TF_DATA_MODE2:
                  strInformation = tr("Mode 2 ");
                  nSectorSize = 2336.0;
                  break;
                }

            if (ti.nFormat != BS_TF_AUDIO)
            {
                if (ti.nFileSystem & BS_FS_ISO9660)
                {
                    stISOInfo.strISOExtension = tr("No/Unknown");

                    strInformation += tr("ISO 9660");
                    if (ti.nFileSystem & BS_FS_JOLIET){
                        strInformation += tr("/Joliet");
                        stISOInfo.strISOExtension = tr("Joliet");
                    }
                    if (ti.nFileSystem & BS_FS_ROCKRIDGE){
                        strInformation += tr("/Rockridge");
                        stISOInfo.strISOExtension = tr("Rockridge");
                    }


                    //iso
                    HSESSION hSession = nullptr;
                    nError = ::OpenDiskSession(BS_CURRENT_DEVICE, j, &hSession, BS_FS_ISO9660);
                    if(showDiskbutlerMessage(nError, this)==false) {
                        thisSuccessfullCreated=false;
                        return;
                    }

                    if(hSession){
                        stISOInfo.isISO = 1;
                        SISOVolumeInfo pISOVolumeInfo;
                        nError = ::GetISOVolumeInformation(hSession, &pISOVolumeInfo);
                        if(showDiskbutlerMessage(nError, this)==false) {
                            thisSuccessfullCreated=false;
                            return;
                        }

#if defined (WIN32)
    stISOInfo.strVolumeLabel = QString::fromUtf16(pISOVolumeInfo.chVolumeLabel);
#else
    stISOInfo.strVolumeLabel = QString::fromUtf8(pISOVolumeInfo.chVolumeLabel);
#endif
                        switch(pISOVolumeInfo.sInfoEx.ISOLevel){
                         case BS_ISO_LEVEL_1:
                            stISOInfo.strISOLevel = tr("Level 1");
                            break;
                         case BS_ISO_LEVEL_2:
                            stISOInfo.strISOLevel = tr("Level 2");
                            break;
                          case BS_ISO_LEVEL_3:
                            stISOInfo.strISOLevel = tr("Level 3");
                            break;
                          case BS_ISO_LEVEL_ROMEO:
                            stISOInfo.strISOLevel = tr("Romeo");
                            break;
                          default:
                            stISOInfo.strISOLevel = tr("Unknown");


                        }

#if defined (WIN32)
    stISOInfo.strAbstract = QString::fromUtf16(pISOVolumeInfo.sInfoEx.ISOAbstractFileIdentifier);
    stISOInfo.strApplication = QString::fromUtf16(pISOVolumeInfo.sInfoEx.ISOApplicationIdentifier);
    stISOInfo.strBiblio = QString::fromUtf16(pISOVolumeInfo.sInfoEx.ISOBiblioIdentifier);
    stISOInfo.strCopyright = QString::fromUtf16(pISOVolumeInfo.sInfoEx.ISOCopyrightFileIdentifier);
    stISOInfo.strDataPreparer = QString::fromUtf16(pISOVolumeInfo.sInfoEx.ISODataPreparerIdentifier);
    stISOInfo.strPublisher = QString::fromUtf16(pISOVolumeInfo.sInfoEx.ISOPublisherIdentifier);
    stISOInfo.strSet = QString::fromUtf16(pISOVolumeInfo.sInfoEx.ISOSetIdentifier);
    stISOInfo.strSystem = QString::fromUtf16(pISOVolumeInfo.sInfoEx.ISOSystemIdentifier);
#else
    stISOInfo.strAbstract = QString::fromUtf8(pISOVolumeInfo.sInfoEx.ISOAbstractFileIdentifier);
    stISOInfo.strApplication = QString::fromUtf8(pISOVolumeInfo.sInfoEx.ISOApplicationIdentifier);
    stISOInfo.strBiblio = QString::fromUtf8(pISOVolumeInfo.sInfoEx.ISOBiblioIdentifier);
    stISOInfo.strCopyright = QString::fromUtf8(pISOVolumeInfo.sInfoEx.ISOCopyrightFileIdentifier);
    stISOInfo.strDataPreparer = QString::fromUtf8(pISOVolumeInfo.sInfoEx.ISODataPreparerIdentifier);
    stISOInfo.strPublisher = QString::fromUtf8(pISOVolumeInfo.sInfoEx.ISOPublisherIdentifier);
    stISOInfo.strSet = QString::fromUtf8(pISOVolumeInfo.sInfoEx.ISOSetIdentifier);
    stISOInfo.strSystem = QString::fromUtf8(pISOVolumeInfo.sInfoEx.ISOSystemIdentifier);
#endif

                        stISOInfo.strRootAddress = QString::number(pISOVolumeInfo.nRootAddress);

                        stISOInfo.strCreationTime = QString("%1.%2.%3 %4:%5:%6").arg(QString::number(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nDay),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nMonth),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nYear+1900),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nHour),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nMinute),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nSecond));

                        stISOInfo.strModificationTime = QString("%1.%2.%3 %4:%5:%6").arg(QString::number(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nDay),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nMonth),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nYear+1900),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nHour),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nMinute),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nSecond));

                        stISOInfo.strExpirationTime = QString("%1.%2.%3 %4:%5:%6").arg(QString::number(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nDay),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nMonth),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nYear+1900),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nHour),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nMinute),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nSecond));

                        stISOInfo.strEffectiveTime = QString("%1.%2.%3 %4:%5:%6").arg(QString::number(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nDay),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nMonth),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nYear+1900),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nHour),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nMinute),
                                                                          QString::number(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nSecond));

                    }

                }

                if (ti.nFileSystem & BS_FS_UDF)
                {
                    if(strInformation.length()>0)strInformation += tr(" /");
                    strInformation += tr(" UDF");

                    //UDF Version will be fine
                    HSESSION hSession = nullptr;
                    nError = ::OpenDiskSession(BS_CURRENT_DEVICE, j, &hSession, BS_FS_UDF);
                    if(showDiskbutlerMessage(nError, this)==false) {
                        thisSuccessfullCreated=false;
                        return;
                    }

                    if(hSession){
                        stUDFInfo.isUDF = 1;
                        SUDFVolumeInfo pUDFVolumeInfo;
                        ::GetUDFVolumeInformation(hSession, &pUDFVolumeInfo);
                        switch(pUDFVolumeInfo.nVersion){
                          case BS_UDF_VERSION_102:
                            strInformation += tr(" (1.02)");
                            stUDFInfo.strUDFVersion = tr("1.02");
                            break;
                          case BS_UDF_VERSION_150:
                            strInformation += tr(" (1.50)");
                            stUDFInfo.strUDFVersion = tr("1.50");
                            break;
                          case BS_UDF_VERSION_200:
                            strInformation += tr(" (2.00)");
                            stUDFInfo.strUDFVersion = tr("2.00");
                            break;
                          case BS_UDF_VERSION_201:
                            strInformation += tr(" (2.01)");
                            stUDFInfo.strUDFVersion = tr("2.01");
                            break;
                          case BS_UDF_VERSION_250:
                            strInformation += tr(" (2.50)");
                            stUDFInfo.strUDFVersion = tr("2.50");
                            break;
                          case BS_UDF_VERSION_260:
                            strInformation += tr(" (2.60)");
                            stUDFInfo.strUDFVersion = tr("2.60");
                            break;
                          default:
                            stUDFInfo.strUDFVersion = tr("Unknown");

                        }
#if defined (WIN32)
    stUDFInfo.strPerparer = QString::fromUtf16(pUDFVolumeInfo.chPreparer);
    stUDFInfo.strVolumeLabel = QString::fromUtf16(pUDFVolumeInfo.chVolumeLabel);
#else
    stUDFInfo.strPerparer = QString::fromUtf8(pUDFVolumeInfo.chPreparer);
    stUDFInfo.strVolumeLabel = QString::fromUtf8(pUDFVolumeInfo.chVolumeLabel);
#endif

                        stUDFInfo.strUDFFiles = QString::number(pUDFVolumeInfo.nFileCount);
                        stUDFInfo.strUDFFolders = QString::number(pUDFVolumeInfo.nDirCount);
                        stUDFInfo.strRootAddress = QString::number(pUDFVolumeInfo.nRootAddress);
                        switch(pUDFVolumeInfo.nPartitionType){
                          case BS_UDF_PARTITION_PHYSICAL:
                            stUDFInfo.strUDFPartition  = tr("Physical");
                            break;
                          case BS_UDF_PARTITION_VIRTUAL:
                            stUDFInfo.strUDFPartition  = tr("Virtual");
                            break;
                          case BS_UDF_PARTITION_SPARABLE:
                            stUDFInfo.strUDFPartition  = tr("Sparable");
                            break;
                        }


                    }
                }

                if (ti.nFileSystem & BS_FS_BOOTABLE){
                    strInformation += tr("/ Bootable");
                    stDiskInfo.strBootable = tr("Bootable");
                }else{
                    stDiskInfo.strBootable = tr("Not Bootable");
                }
            }






            QString trackInformation;
            if(ti.nSize>0){
              nDiskSize += (ti.nSize*nSectorSize)/MEGABYTE;
              trackInformation = QString("Track %1 LBA:%2 (%3 MB) %4").arg(QString::number(j),QString::number(ti.nStartLBA),QString::number((ti.nSize*nSectorSize)/MEGABYTE),strInformation);
            }else{
              trackInformation = QString("Track %1 LBA:%2 %3").arg(QString::number(j),QString::number(ti.nStartLBA),strInformation);
            }


            QTreeWidgetItem *treeTrackItem = new QTreeWidgetItem(treechildItem);
            treeTrackItem->setText(0, trackInformation);
            treeTrackItem->setIcon(0,QIcon(":/res/disk_data_table.png"));
            treechildItem->setExpanded(true);

            //Now we add the Audio Infos. We have: CDTExt and Indexes
            int32 hCdText;
            int32 res = ::ReadCDText(BS_CURRENT_DEVICE, &hCdText);
            if(res == BS_SDK_ERROR_NO)
            {

                QTreeWidgetItem *treeCDTextMain = new QTreeWidgetItem(treeTrackItem);
                treeCDTextMain->setText(0, tr("CDText Information"));

                QString cdTextString = tr("");
                if(!ExtractTrackTextFromHandle(hCdText, j, BS_CDTCI_PERFORMER, cdTextString)){
                    cdTextString = tr("CDText Performer: ")+cdTextString;
                    QTreeWidgetItem *treeCDTextItem1 = new QTreeWidgetItem(treeCDTextMain);
                    treeCDTextItem1->setText(0, cdTextString);
                }
                if(!ExtractTrackTextFromHandle(hCdText, j, BS_CDTCI_TITLE , cdTextString)){
                    cdTextString = tr("CDText Title: ")+cdTextString;
                    QTreeWidgetItem *treeCDTextItem2 = new QTreeWidgetItem(treeCDTextMain);
                    treeCDTextItem2->setText(0, cdTextString);
                }
                if(!ExtractTrackTextFromHandle(hCdText, j, BS_CDTCI_SONG_WRITER, cdTextString)){
                    cdTextString = tr("CDText Song Writer: ")+cdTextString;
                    QTreeWidgetItem *treeCDTextItem3 = new QTreeWidgetItem(treeCDTextMain);
                    treeCDTextItem3->setText(0, cdTextString);
                }
                if(!ExtractTrackTextFromHandle(hCdText, j, BS_CDTCI_COMPOSER, cdTextString)){
                    cdTextString = tr("CDText Composer: ")+cdTextString;
                    QTreeWidgetItem *treeCDTextItem4 = new QTreeWidgetItem(treeCDTextMain);
                    treeCDTextItem4->setText(0, cdTextString);
                }
                if(!ExtractTrackTextFromHandle(hCdText, j, BS_CDTCI_ARRANGER, cdTextString)){
                  cdTextString = tr("CDText Arranger: ")+cdTextString;
                  QTreeWidgetItem *treeCDTextItem5 = new QTreeWidgetItem(treeCDTextMain);
                  treeCDTextItem5->setText(0, cdTextString);
                }
                if(!ExtractTrackTextFromHandle(hCdText, j, BS_CDTCI_MESSAGE, cdTextString)){
                    cdTextString = tr("CDText Message: ")+cdTextString;
                    QTreeWidgetItem *treeCDTextItem6 = new QTreeWidgetItem(treeCDTextMain);
                    treeCDTextItem6->setText(0, cdTextString);
                }

            }

            /*
            std::vector<int32> Indexes(100);
            int32 nInd  = 100;
            res = ::GetTrackIndexes(BS_CURRENT_DEVICE,j,&(Indexes[0]),&nInd);

            if(res == BS_SDK_ERROR_NO)
            {
                QTreeWidgetItem *treeIndexesMain = new QTreeWidgetItem(treeTrackItem);
                treeIndexesMain->setText(0, tr("Indexes Information"));

                for(int i = 0; (i < nInd) && (i < 10); ++i)
                {


                    QString indexesList = QString("Index %1 : %2").arg(QString::number(i),QString::number(Indexes[i]));
                    QTreeWidgetItem *treeIndexesItem = new QTreeWidgetItem(treeIndexesMain);
                    treeIndexesItem->setText(0, indexesList);

                }
            }
            */





        }
        QString strSession;
        if(si.dSessionSize>0){ //Protect against a crash
          strSession = QString("Session %1 (%2 MB)").arg(QString::number(i),QString::number(nDiskSize));
        }else{
          strSession = QString("Session %1").arg(QString::number(i));
        }
        treechildItem->setText(0,strSession);
    }



    stDiskInfo.strMediaSize = QString("%1 MB").arg(mi.dMediumSize/MEGABYTE);
    stDiskInfo.strMediaUsedSpace = QString("%1 MB").arg(mi.dMediumUsedSize/MEGABYTE);
    stDiskInfo.strMediaFreeSpace = QString("%1 MB").arg(mi.dMediumFreeSize/MEGABYTE);
    bIsEmptyDisk = false;
    isOpen = false;

    switch (mi.nMediumStatus) {
          case BS_MS_EMPTY_DISK:
              bIsEmptyDisk = true;
              stDiskInfo.strMediaStatus = tr("Empty Disk");
          break;
          case BS_MS_INCOMPLETE_DISK:
              isOpen = true;
              stDiskInfo.strMediaStatus = tr("Incomplete Disk");
          break;
          case BS_MS_COMPLETE_DISK:
              stDiskInfo.strMediaStatus = tr("Complete Disk");
              stDiskInfo.strMediaFreeSpace = tr("n/a");
          break;

    }

    switch(mi.nExtendedMediumType){
        case BS_EMT_CD_ROM:
          bIsISO = true; //We can read a CD with ISO and RAW
          bIsBIN = true;
          stDiskInfo.strMediaExType = tr("CD-ROM");
          break;
        case BS_EMT_CD_ROM_XA:
          bIsISO = false;
          bIsBIN = true;
          stDiskInfo.strMediaExType = tr("CD-ROM XA");
          break;
        case BS_EMT_CD_AUDIO:
          bIsISO = false;
          bIsBIN = true;
          stDiskInfo.strMediaExType = tr("Audio CD");
          break;
        case BS_EMT_CD_MIXED_MODE:
          bIsISO = false;
          bIsBIN = true;
          stDiskInfo.strMediaExType = tr("Mixed Mode CD");
          break;
        case BS_EMT_CD_ENHANCED:
          bIsISO = false;
          bIsBIN = true;
          stDiskInfo.strMediaExType = tr("Enhanced CD");
          break;
        case BS_EMT_CD_MULTISESSION:
          bIsISO = true;
          bIsBIN = false;
          stDiskInfo.strMediaExType = tr("Multisession CD");
          break;
        case BS_EMT_DVD:
          bIsISO = true;
          bIsBIN = false;
          stDiskInfo.strMediaExType = tr("DVD");
          break;
        case BS_EMT_BD:
          bIsISO = true;
          bIsBIN = false;
          stDiskInfo.strMediaExType = tr("BD");
          break;
        case BS_EMT_HDDVD:
          bIsISO = true;
          bIsBIN = false;
          stDiskInfo.strMediaExType = tr("HDDVD");
          break;
        default: break;
    }

    if(mi.nExtendedMediumType==BS_EMT_CD_AUDIO){
        stAudioInfo.isAudio = 1;
        int32 hCdText;
        int32 res = ::ReadCDText(BS_CURRENT_DEVICE, &hCdText);
        if(res == BS_SDK_ERROR_NO)
        {

            QString cdTextString = tr("");
            if(!ExtractTextFromHandle(hCdText, BS_CDTCI_ARRANGER, cdTextString)){
              stAudioInfo.strArranger = cdTextString;
              cdTextString = tr("");
            }
            if(!ExtractTextFromHandle(hCdText, BS_CDTCI_COMPOSER, cdTextString)){
              stAudioInfo.strComposer = cdTextString;
              cdTextString = tr("");
            }
            if(!ExtractTextFromHandle(hCdText, BS_CDTCI_MESSAGE, cdTextString)){
              stAudioInfo.strMessage = cdTextString;
              cdTextString = tr("");
            }
            if(!ExtractTextFromHandle(hCdText, BS_CDTCI_PERFORMER, cdTextString)){
              stAudioInfo.strPerformer = cdTextString;
              cdTextString = tr("");
            }
            if(!ExtractTextFromHandle(hCdText, BS_CDTCI_SONG_WRITER, cdTextString)){
              stAudioInfo.strSongWriter = cdTextString;
              cdTextString = tr("");
            }
            if(!ExtractTextFromHandle(hCdText, BS_CDTCI_TITLE , cdTextString)){
              stAudioInfo.strTitle = cdTextString;
              cdTextString = tr("");
            }

        }
    }


    if(mi.fMaxWriteSpeed==-1){
      stDiskInfo.strMediaMaxWSpeed = tr("Unknown");
    }else{
      stDiskInfo.strMediaMaxWSpeed = QString("%1").arg(QString::number(mi.fMaxWriteSpeed));
    }
#if defined (WIN32)
    stDiskInfo.strMediaUPC = QString::fromUtf16(mi.chUPCEANCode);
    stDiskInfo.strMediaType = QString::fromUtf16(mi.chMediumType);
#else
    stDiskInfo.strMediaUPC = QString::fromUtf8(mi.chUPCEANCode);
    stDiskInfo.strMediaType = QString::fromUtf8(mi.chMediumType);
#endif

    stDiskInfo.strBridgeFileSystem = tr("n/a");
    if(stISOInfo.isISO == 1 && stUDFInfo.isUDF == 1){
        stDiskInfo.strBridgeFileSystem = tr("ISO / UDF Bridge");
    }

    int nRowCount = 0;

    if(hideEmptyFields==false || stDiskInfo.strMediaType.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaTypeFix = new QTableWidgetItem(tr("Media Type:"));
        itemMediaTypeFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaType = new QTableWidgetItem(stDiskInfo.strMediaType);
        itemMediaType->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaTypeFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaType);
    }

    if(hideEmptyFields==false || stDiskInfo.strMediaExType.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaExTypeFix = new QTableWidgetItem(tr("Media Type Ex:"));
        itemMediaExTypeFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaExType = new QTableWidgetItem(stDiskInfo.strMediaExType);
        itemMediaExType->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaExTypeFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaExType);
    }

    if(hideEmptyFields==false || stDiskInfo.strMediaID.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaIDFix = new QTableWidgetItem(tr("Media ID:"));
        itemMediaIDFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaID = new QTableWidgetItem(stDiskInfo.strMediaID);
        itemMediaID->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaIDFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaID);
    }

    if(hideEmptyFields==false || stDiskInfo.strBridgeFileSystem.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemBridgeFileSystemFix = new QTableWidgetItem(tr("Bridge:"));
        itemBridgeFileSystemFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemBridgeFileSystem = new QTableWidgetItem(stDiskInfo.strBridgeFileSystem);
        itemBridgeFileSystem->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemBridgeFileSystemFix);
        diskInfoTable->setItem(nRowCount,1,itemBridgeFileSystem);
    }

    if(hideEmptyFields==false || stDiskInfo.strMediaUPC.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaUPCFix = new QTableWidgetItem(tr("UPC/EAN Code:"));
        itemMediaUPCFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaUPC = new QTableWidgetItem(stDiskInfo.strMediaUPC);
        itemMediaUPC->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaUPCFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaUPC);
    }

    if(hideEmptyFields==false || stDiskInfo.strMediaSpeeds.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaSpeedsFix = new QTableWidgetItem(tr("Speeds (kb/s):"));
        itemMediaSpeedsFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaSpeeds = new QTableWidgetItem(stDiskInfo.strMediaSpeeds);
        itemMediaSpeeds->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaSpeedsFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaSpeeds);
    }

    if(hideEmptyFields==false || stDiskInfo.strMediaMaxWSpeed.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaMaxWSpeedFix = new QTableWidgetItem(tr("Max. Write Speed:"));
        itemMediaMaxWSpeedFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaMaxWSpeed = new QTableWidgetItem(stDiskInfo.strMediaMaxWSpeed);
        itemMediaMaxWSpeed->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaMaxWSpeedFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaMaxWSpeed);
    }

    if(hideEmptyFields==false || stDiskInfo.strMediaStatus.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaStatusFix = new QTableWidgetItem(tr("Status:"));
        itemMediaStatusFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaStatus = new QTableWidgetItem(stDiskInfo.strMediaStatus);
        itemMediaStatus->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaStatusFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaStatus);
    }

    if(hideEmptyFields==false || stDiskInfo.strBootable.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaBootableFix = new QTableWidgetItem(tr("Bootable Disk:"));
        itemMediaBootableFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaBootable = new QTableWidgetItem(stDiskInfo.strBootable);
        itemMediaBootable->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaBootableFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaBootable);
    }

    if(hideEmptyFields==false || stDiskInfo.strMediaSize.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaSizeFix = new QTableWidgetItem(tr("Size:"));
        itemMediaSizeFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaSize = new QTableWidgetItem(stDiskInfo.strMediaSize);
        itemMediaSize->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaSizeFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaSize);
    }

    if(hideEmptyFields==false || stDiskInfo.strMediaUsedSpace.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaUsedSpaceFix = new QTableWidgetItem(tr("Used Space:"));
        itemMediaUsedSpaceFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaUsedSpace = new QTableWidgetItem(stDiskInfo.strMediaUsedSpace);
        itemMediaUsedSpace->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaUsedSpaceFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaUsedSpace);
    }

    if(hideEmptyFields==false || stDiskInfo.strMediaFreeSpace.length()>0 ){
        nRowCount++;
        QTableWidgetItem* itemMediaFreeSpaceFix = new QTableWidgetItem(tr("Free Space:"));
        itemMediaFreeSpaceFix->setFlags(Qt::NoItemFlags);
        QTableWidgetItem* itemMediaFreeSpace = new QTableWidgetItem(stDiskInfo.strMediaFreeSpace);
        itemMediaFreeSpace->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,itemMediaFreeSpaceFix);
        diskInfoTable->setItem(nRowCount,1,itemMediaFreeSpace);
    }

    if(stISOInfo.isISO == 1){
        nRowCount++;

        QTableWidgetItem* header2a = new QTableWidgetItem(tr("Contents ISO"));
        header2a->setFlags(Qt::NoItemFlags);
        header2a->setFont(originalFont);
        QTableWidgetItem* header2b = new QTableWidgetItem("");
        header2b->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,header2a);
        diskInfoTable->setItem(nRowCount,1,header2b);
        diskInfoTable->setSpan(nRowCount,0,1,2);

        if(hideEmptyFields==false || stISOInfo.strVolumeLabel.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOVolumeLabelFix = new QTableWidgetItem(tr("Label:"));
            itemISOVolumeLabelFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOVolumeLabel = new QTableWidgetItem(stISOInfo.strVolumeLabel);
            itemISOVolumeLabel->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOVolumeLabelFix);
            diskInfoTable->setItem(nRowCount,1,itemISOVolumeLabel);
        }

        if(hideEmptyFields==false || stISOInfo.strISOLevel.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOLevelFix = new QTableWidgetItem(tr("Level:"));
            itemISOLevelFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOLevel = new QTableWidgetItem(stISOInfo.strISOLevel);
            itemISOLevel->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOLevelFix);
            diskInfoTable->setItem(nRowCount,1,itemISOLevel);
        }

        if(hideEmptyFields==false || stISOInfo.strRootAddress.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISORootAddressFix = new QTableWidgetItem(tr("Root Address:"));
            itemISORootAddressFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISORootAddress = new QTableWidgetItem(stISOInfo.strRootAddress);
            itemISORootAddress->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISORootAddressFix);
            diskInfoTable->setItem(nRowCount,1,itemISORootAddress);
        }

        if(hideEmptyFields==false || stISOInfo.strISOExtension.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOExtensionFix = new QTableWidgetItem(tr("Extension Type:"));
            itemISOExtensionFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOExtension = new QTableWidgetItem(stISOInfo.strISOExtension);
            itemISOExtension->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOExtensionFix);
            diskInfoTable->setItem(nRowCount,1,itemISOExtension);
        }

        if(hideEmptyFields==false || stISOInfo.strCreationTime.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOCreationTimeFix = new QTableWidgetItem(tr("Creation Time:"));
            itemISOCreationTimeFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOCreationTime = new QTableWidgetItem(stISOInfo.strCreationTime);
            itemISOCreationTime->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOCreationTimeFix);
            diskInfoTable->setItem(nRowCount,1,itemISOCreationTime);
        }

        if(hideEmptyFields==false || stISOInfo.strModificationTime.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOModificationTimeFix = new QTableWidgetItem(tr("Creation Time:"));
            itemISOModificationTimeFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOModificationTime = new QTableWidgetItem(stISOInfo.strModificationTime);
            itemISOModificationTime->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOModificationTimeFix);
            diskInfoTable->setItem(nRowCount,1,itemISOModificationTime);
        }

        if(hideEmptyFields==false || stISOInfo.strExpirationTime.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOExpirationTimeFix = new QTableWidgetItem(tr("Expiration Time:"));
            itemISOExpirationTimeFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOExpirationTime = new QTableWidgetItem(stISOInfo.strExpirationTime);
            itemISOExpirationTime->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOExpirationTimeFix);
            diskInfoTable->setItem(nRowCount,1,itemISOExpirationTime);
        }

        if(hideEmptyFields==false || stISOInfo.strEffectiveTime.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOEffectiveTimeFix = new QTableWidgetItem(tr("Effective Time:"));
            itemISOEffectiveTimeFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOEffectiveTime = new QTableWidgetItem(stISOInfo.strEffectiveTime);
            itemISOEffectiveTime->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOEffectiveTimeFix);
            diskInfoTable->setItem(nRowCount,1,itemISOEffectiveTime);
        }

        if(hideEmptyFields==false || stISOInfo.strAbstract.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOAbstractFix = new QTableWidgetItem(tr("Abstract Identifier:"));
            itemISOAbstractFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOAbstract = new QTableWidgetItem(stISOInfo.strAbstract);
            itemISOAbstract->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOAbstractFix);
            diskInfoTable->setItem(nRowCount,1,itemISOAbstract);
        }

        if(hideEmptyFields==false || stISOInfo.strApplication.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOApplicationFix = new QTableWidgetItem(tr("Application Identifier:"));
            itemISOApplicationFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOApplication = new QTableWidgetItem(stISOInfo.strApplication);
            itemISOApplication->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOApplicationFix);
            diskInfoTable->setItem(nRowCount,1,itemISOApplication);
        }

        if(hideEmptyFields==false || stISOInfo.strBiblio.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOBiblioFix = new QTableWidgetItem(tr("Biblio Identifier:"));
            itemISOBiblioFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOBiblio = new QTableWidgetItem(stISOInfo.strBiblio);
            itemISOBiblio->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOBiblioFix);
            diskInfoTable->setItem(nRowCount,1,itemISOBiblio);
        }

        if(hideEmptyFields==false || stISOInfo.strCopyright.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOCopyrightFix = new QTableWidgetItem(tr("Copyright Identifier:"));
            itemISOCopyrightFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOCopyright = new QTableWidgetItem(stISOInfo.strCopyright);
            itemISOCopyright->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOCopyrightFix);
            diskInfoTable->setItem(nRowCount,1,itemISOCopyright);
        }

        if(hideEmptyFields==false || stISOInfo.strDataPreparer.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISODataPreparerFix = new QTableWidgetItem(tr("Data Preparer Identifier:"));
            itemISODataPreparerFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISODataPreparer = new QTableWidgetItem(stISOInfo.strDataPreparer);
            itemISODataPreparer->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISODataPreparerFix);
            diskInfoTable->setItem(nRowCount,1,itemISODataPreparer);
        }

        if(hideEmptyFields==false || stISOInfo.strPublisher.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOPublisherFix = new QTableWidgetItem(tr("Publisher Identifier:"));
            itemISOPublisherFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOPublisher = new QTableWidgetItem(stISOInfo.strPublisher);
            itemISOPublisher->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOPublisherFix);
            diskInfoTable->setItem(nRowCount,1,itemISOPublisher);
        }

        if(hideEmptyFields==false || stISOInfo.strSet.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOSetFix = new QTableWidgetItem(tr("Set Identifier:"));
            itemISOSetFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOSet = new QTableWidgetItem(stISOInfo.strSet);
            itemISOSet->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOSetFix);
            diskInfoTable->setItem(nRowCount,1,itemISOSet);
        }

        if(hideEmptyFields==false || stISOInfo.strSystem.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemISOSystemFix = new QTableWidgetItem(tr("System Identifier:"));
            itemISOSystemFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemISOSystem = new QTableWidgetItem(stISOInfo.strSystem);
            itemISOSystem->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemISOSystemFix);
            diskInfoTable->setItem(nRowCount,1,itemISOSystem);
        }


    }

    if(stUDFInfo.isUDF == 1){
        nRowCount++;
        QTableWidgetItem* header2c = new QTableWidgetItem(tr("Contents UDF"));
        header2c->setFlags(Qt::NoItemFlags);
        header2c->setFont(originalFont);
        QTableWidgetItem* header2d = new QTableWidgetItem("");
        header2d->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,header2c);
        diskInfoTable->setItem(nRowCount,1,header2d);
        diskInfoTable->setSpan(nRowCount,0,1,2);

        if(hideEmptyFields==false || stUDFInfo.strVolumeLabel.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemUDFVolumeLabelFix = new QTableWidgetItem(tr("Label:"));
            itemUDFVolumeLabelFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemUDFVolumeLabel = new QTableWidgetItem(stUDFInfo.strVolumeLabel);
            itemUDFVolumeLabel->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemUDFVolumeLabelFix);
            diskInfoTable->setItem(nRowCount,1,itemUDFVolumeLabel);
        }

        if(hideEmptyFields==false || stUDFInfo.strUDFVersion.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemUDFVersionFix = new QTableWidgetItem(tr("Version:"));
            itemUDFVersionFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemUDFVersion = new QTableWidgetItem(stUDFInfo.strUDFVersion);
            itemUDFVersion->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemUDFVersionFix);
            diskInfoTable->setItem(nRowCount,1,itemUDFVersion);
        }

        if(hideEmptyFields==false || stUDFInfo.strUDFPartition.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemUDFPartitionFix = new QTableWidgetItem(tr("Partition:"));
            itemUDFPartitionFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemUDFPartition = new QTableWidgetItem(stUDFInfo.strUDFPartition);
            itemUDFPartition->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemUDFPartitionFix);
            diskInfoTable->setItem(nRowCount,1,itemUDFPartition);
        }

        if(hideEmptyFields==false || stUDFInfo.strPerparer.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemUDFPerparerFix = new QTableWidgetItem(tr("Preparer:"));
            itemUDFPerparerFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemUDFPerparer = new QTableWidgetItem(stUDFInfo.strPerparer);
            itemUDFPerparer->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemUDFPerparerFix);
            diskInfoTable->setItem(nRowCount,1,itemUDFPerparer);
        }

        if(hideEmptyFields==false || stUDFInfo.strUDFFolders.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemUDFFoldersFix = new QTableWidgetItem(tr("Folder Count:"));
            itemUDFFoldersFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemUDFFolders = new QTableWidgetItem(stUDFInfo.strUDFFolders);
            itemUDFFolders->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemUDFFoldersFix);
            diskInfoTable->setItem(nRowCount,1,itemUDFFolders);
        }

        if(hideEmptyFields==false || stUDFInfo.strUDFFiles.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemUDFFilesFix = new QTableWidgetItem(tr("File Count:"));
            itemUDFFilesFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemUDFFiles = new QTableWidgetItem(stUDFInfo.strUDFFiles);
            itemUDFFiles->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemUDFFilesFix);
            diskInfoTable->setItem(nRowCount,1,itemUDFFiles);
        }

        if(hideEmptyFields==false || stUDFInfo.strRootAddress.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemUDFRootAddressFix = new QTableWidgetItem(tr("Root Address:"));
            itemUDFRootAddressFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemUDFRootAddress = new QTableWidgetItem(stUDFInfo.strRootAddress);
            itemUDFRootAddress->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemUDFRootAddressFix);
            diskInfoTable->setItem(nRowCount,1,itemUDFRootAddress);
        }
    }
    if(stAudioInfo.isAudio == 1){
        nRowCount++;
        QTableWidgetItem* header2e = new QTableWidgetItem(tr("Contents Audio"));
        header2e->setFlags(Qt::NoItemFlags);
        header2e->setFont(originalFont);
        QTableWidgetItem* header2f = new QTableWidgetItem("");
        header2f->setFlags(Qt::NoItemFlags);
        diskInfoTable->setItem(nRowCount,0,header2e);
        diskInfoTable->setItem(nRowCount,1,header2f);
        diskInfoTable->setSpan(nRowCount,0,1,2);

        if(hideEmptyFields==false || stAudioInfo.strTitle.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemAudioTitleFix = new QTableWidgetItem(tr("CDText Title:"));
            itemAudioTitleFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemAudioTitle = new QTableWidgetItem(stAudioInfo.strTitle);
            itemAudioTitle->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemAudioTitleFix);
            diskInfoTable->setItem(nRowCount,1,itemAudioTitle);
        }

        if(hideEmptyFields==false || stAudioInfo.strPerformer.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemAudioPerformerFix = new QTableWidgetItem(tr("CDText Performer:"));
            itemAudioPerformerFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemAudioPerformer = new QTableWidgetItem(stAudioInfo.strPerformer);
            itemAudioPerformer->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemAudioPerformerFix);
            diskInfoTable->setItem(nRowCount,1,itemAudioPerformer);
        }

        if(hideEmptyFields==false || stAudioInfo.strSongWriter.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemAudioSongWriterFix = new QTableWidgetItem(tr("CDText SongWriter:"));
            itemAudioSongWriterFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemAudioSongWriter = new QTableWidgetItem(stAudioInfo.strSongWriter);
            itemAudioSongWriter->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemAudioSongWriterFix);
            diskInfoTable->setItem(nRowCount,1,itemAudioSongWriter);
        }

        if(hideEmptyFields==false || stAudioInfo.strComposer.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemAudioComposerFix = new QTableWidgetItem(tr("CDText Composer:"));
            itemAudioComposerFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemAudioComposer = new QTableWidgetItem(stAudioInfo.strComposer);
            itemAudioComposer->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemAudioComposerFix);
            diskInfoTable->setItem(nRowCount,1,itemAudioComposer);
        }

        if(hideEmptyFields==false || stAudioInfo.strArranger.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemAudioArrangerFix = new QTableWidgetItem(tr("CDText Arranger:"));
            itemAudioArrangerFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemAudioArranger = new QTableWidgetItem(stAudioInfo.strArranger);
            itemAudioArranger->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemAudioArrangerFix);
            diskInfoTable->setItem(nRowCount,1,itemAudioArranger);
        }

        if(hideEmptyFields==false || stAudioInfo.strMessage.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemAudioMessageFix = new QTableWidgetItem(tr("CDText Message:"));
            itemAudioMessageFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemAudioMessage = new QTableWidgetItem(stAudioInfo.strMessage);
            itemAudioMessage->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemAudioMessageFix);
            diskInfoTable->setItem(nRowCount,1,itemAudioMessage);
        }

        if(hideEmptyFields==false || stAudioInfo.strISRC.length()>0 ){
            nRowCount++;
            QTableWidgetItem* itemAudioISRCFix = new QTableWidgetItem(tr("CDText ISRC:"));
            itemAudioISRCFix->setFlags(Qt::NoItemFlags);
            QTableWidgetItem* itemAudioISRC = new QTableWidgetItem(stAudioInfo.strISRC);
            itemAudioISRC->setFlags(Qt::NoItemFlags);
            diskInfoTable->setItem(nRowCount,0,itemAudioISRCFix);
            diskInfoTable->setItem(nRowCount,1,itemAudioISRC);
        }
    }

    diskInfoTable->setRowCount(nRowCount);
    diskInfoTable->resizeColumnsToContents();

    //listWidget->setGeometry(150, 0, 200, 200);
    //listWidget->setContentsMargins(2, 2, 2, 2);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    //myUpdateMenus->trigger();

    QString style(
        "QTableView:disabled {"
            "gridline-color: #32414B;"
                "border: 1px solid #32414B;"
                  "color: #32414B;"
        "}"
    );
    diskInfoTable->setStyleSheet(style);

}
