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

#include <QtWidgets>
#include "mdichild_diskinfo.h"
#include "FoxSDKBurningLib.h"
#include "mainwindow.h"
#include "settingspages.h"
#include "messanger.h"

void ThreadInfo::run()
{

    myChild->readDiskInfo();
}


MdiChildDiskInfo::MdiChildDiskInfo(QWidget*, const QString &device)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(":/icons/discinfo32.png"));
    mProjectType = RuleManager::TYPE_PROJECT_DISKINFO;
    setBurnDrive(device);
    setEraseable(false);
    setOpenDisk(false);
    setHideEmptyFields(false);
    setCopyVerify(0);
    setImageVerify(0);
    setImageJobVerify(ConfigurationPage::mSettings.value("ImageVerify", 0).toInt());
    setImageJobCreate(ConfigurationPage::mSettings.value("ImageCreate", 1).toInt());
    setCopyEject(false);
    setUseErrorCorrection(ConfigurationPage::mSettings.value("ErrorCorrection", true).toBool());
    setBlankBadSectors(false);
    setEmptyDisk(false);
    setIsIsoDisk(true);
    setCopyReadMethod(0); //0=ISO;1=RAW;2=RAW+SUB
    setCopyWriteMethod(0); //0=DAO;1=DAO96
    setErrorHarwareRetry(ConfigurationPage::mSettings.value("HardwareRetryCount", 10).toInt());
    setErrorSoftwareRetry(ConfigurationPage::mSettings.value("SoftwareRetryCount", 1).toInt());
    setEjectAfterErase(ConfigurationPage::mSettings.value("EjectAfterErase", true).toBool());
    setImageCreateSoftRetry(ConfigurationPage::mSettings.value("SoftwareRetryCount", 10).toInt());
    setImageCreateHardRetry(ConfigurationPage::mSettings.value("HardwareRetryCount", 1).toInt());
    setFastErase(true);
    setImagePath("");
    setImageCreateMethod(0);
    setImageCorrSwitch(2);
    mWorkThread = nullptr;
    hasData=0;

    QString wTitle = tr("Disk Info");
    wTitle += " (";
    wTitle += getBurnDrive();
    wTitle += ")";

    setWindowTitle(wTitle);

    diskInfoTable = new QTableWidget();
    treeWidget = new  QDummyTextTree();

    QString style(
                "QTableView:disabled {"
              "gridline-color: #32414B;"
                  "border: 1px solid #32414B;"
                    "color: #32414B;"
          "}"
      );
    diskInfoTable->setStyleSheet(style);

    splitter = new QSplitter;
    splitter->addWidget(diskInfoTable);
    splitter->addWidget(treeWidget);
    //setCentralWidget(splitter);
    setWidget(splitter);

    diskInfoTable->setRowCount(50);
    diskInfoTable->setColumnCount(2);
    diskInfoTable->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    diskInfoTable->horizontalHeader()->setStretchLastSection(true);
    diskInfoTable->horizontalHeader()->hide();
    diskInfoTable->verticalHeader()->hide();
    diskInfoTable->verticalHeader()->setDefaultSectionSize(21);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QVector<int> >("QList<QTreeWidgetItem*>");
    qRegisterMetaType<QVector<int> >("QVector<pTableItem*>");

    //emit StartWaitingSpinner still not works here.
    QTimer::singleShot( 500, this, SLOT(startUpdateInfo()));

}

void MdiChildDiskInfo::updateDiskInfo()
{
    QTimer::singleShot( 500, this, SLOT(startUpdateInfo()));

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
    if (res != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(res, "GetCDTextTrackTagString");
        return res;
    }


    if(!nLen || nLen == 1)
    {
        rText = tr("");
        return BS_SDK_ERROR_NO;
    }

    TCHAR *pBuf = new TCHAR[nLen];
    res = ::GetCDTextDiskTagString(handle, nCDTCI, pBuf, &nLen);
    if (res != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(res, "GetCDTextDiskTagString");
        delete[] pBuf;
        return res;
    }
    //LOOK HERE. Null termination, problem occour a couple of time from SDK.

    pBuf[nLen-1] = _T('\0');

    rText = convertToQT(pBuf);

    delete[] pBuf;

    return BS_SDK_ERROR_NO;
}

int32 MdiChildDiskInfo::ExtractTrackTextFromHandle(int32 handle, int32 nTrack, int32 nCDTCI, QString& rText)
{
    int32 nLen = 0;
    int32 res;

    res = ::GetCDTextTrackTagString(handle, nTrack, nCDTCI, nullptr, &nLen);
    if (res != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(res, "GetCDTextTrackTagString");
        return res;
    }

    if(!nLen || nLen == 1)
    {
        rText = tr("");
        return BS_SDK_ERROR_NO;
    }

    TCHAR *pBuf = new TCHAR[nLen];
    res = ::GetCDTextTrackTagString(handle, nTrack, nCDTCI, pBuf, &nLen);
    if (res != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(res, "GetCDTextTrackTagString");
        delete[] pBuf;
        return res;
    }

    pBuf[nLen-1] = _T('\0');

    rText = convertToQT(pBuf);

    delete[] pBuf;

    return BS_SDK_ERROR_NO;
}

void MdiChildDiskInfo::qDebugAusgabeSDK(int32 errCode, const QString &customMessage)
{
    TCHAR chError[2048];
    int32 nLength = 2048;

    emit stopSpinner();

    ::GetText(errCode,chError, &nLength);
    QString errDesc;

    errDesc= convertToQT(chError);

    qDebug("SDKError %s: %s", customMessage.toLatin1().constData(), errDesc.toLatin1().constData());
}

void MdiChildDiskInfo::readDiskInfo()
{

    emit startSpinner();

    QList<QTreeWidgetItem *> items;
    QVector<pTableItem *> myTable;

    pTableItem *tableItem0 = new pTableItem();
    tableItem0->columnText = tr("Physical Information");
    tableItem0->itemColumn = 0;
    tableItem0->itemRow = 0;
    tableItem0->rowSpan = true;
    tableItem0->rowBold = true;
    myTable.append(tableItem0);

    pTableItem *tableItem1 = new pTableItem();
    tableItem1->columnText = tr("");
    tableItem1->itemColumn = 1;
    tableItem1->itemRow = 0;
    tableItem1->rowSpan = true;
    tableItem1->rowBold = true;
    myTable.append(tableItem1);

    //QVEctor is faster than QList


    //We create now the items in a QList and then move to the main Thread.
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, tr("Disk"));
    treeItem->setIcon(0,QIcon(":/icons/disk_main16.png"));
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

    int32 nError = ::SetBurnDevice(getBurnDrive().at(0).toLatin1());
    nError = ::SetReadDevice(getBurnDrive().at(0).toLatin1());
    if (nError != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(nError, "SetDevice");
        return;
    }


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
    nError = ::IsDeviceReady(BS_CURRENT_DEVICE, &bReady);
    if (nError != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(nError, "IsDeviceReady");
    }


    SMediumInfo mi;
    nError = ::GetMediumInformation(BS_CURRENT_DEVICE, &mi);
    if (nError != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(nError, "GetMediumInfo");
        return;
    }



    int16 nImageFormats;
    nError =  ::GetPossibleImageFormats(&nImageFormats);
    if (nError != BS_SDK_ERROR_NO){
        qDebugAusgabeSDK(nError, "Possible Imageformats");
    }
    setImageFormats(nImageFormats);

    setEraseable(false);

    if(mi.wMediumTypeCode == BS_DVD_PLUSRW ||
            mi.wMediumTypeCode == BS_DVD_RW_RO ||
            mi.wMediumTypeCode == BS_BLURAY_RE ||
            mi.wMediumTypeCode == BS_DVD_RAM  ||
            mi.wMediumTypeCode == BS_DVD_RW_SR ||
            mi.wMediumTypeCode == BS_CD_RW ||
            mi.wMediumTypeCode == BS_DVD_RWDL_PLUS){
        setEraseable(true);
    }

    //for (int32 i = mi.nFirstSession; i <= mi.nLastSession; i++)
    //nLastSession = 0 is invalid. But the block before is valid for virtual disc from Image Writer.
    //So we need to stop try to read from lastSession = 0 but avoid a return.
    for (int32 i = mi.nFirstSession; i <= mi.nLastSession && ( mi.nLastSession != 0); i++)
    {
        SSessionInfo si;
        nError = ::GetSessionInformation(BS_CURRENT_DEVICE, i, &si);
        if (nError != BS_SDK_ERROR_NO){
            qDebugAusgabeSDK(nError, "GetSessionInfo");
            return;
        }

        //SessionSize are sectors? What to do with CD and AudioCD?



        QTreeWidgetItem *treechildItem = new QTreeWidgetItem();
        treechildItem->setText(0, tr("unknown"));
        treechildItem->setIcon(0,QIcon(":/icons/disk_session16.png"));
        treeItem->addChild(treechildItem);

        double nDiskSize = 0.0;

        //Now we need to start the loop

        for (int32 j = si.nFirstTrack; j <= si.nLastTrack && ( si.nLastTrack != 0); ++j)
        {
            STrackInfo ti;
            nError = ::GetTrackInformation(BS_CURRENT_DEVICE, j, &ti);
            if (nError != BS_SDK_ERROR_NO){
                qDebugAusgabeSDK(nError, "GetTrackInfo");
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
                    if (nError != BS_SDK_ERROR_NO){
                        qDebugAusgabeSDK(nError, "OpenDiskSession");
                        return;
                    }

                    if(hSession){
                        stISOInfo.isISO = 1;
                        SISOVolumeInfo pISOVolumeInfo;
                        nError = ::GetISOVolumeInformation(hSession, &pISOVolumeInfo);
                        if (nError != BS_SDK_ERROR_NO){
                            qDebugAusgabeSDK(nError, "GetISOVolumeInformation");
                            return;
                        }

                        stISOInfo.strVolumeLabel = convertToQT(pISOVolumeInfo.chVolumeLabel);

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


                        stISOInfo.strAbstract = convertToQT(pISOVolumeInfo.sInfoEx.ISOAbstractFileIdentifier);
                        stISOInfo.strApplication = convertToQT(pISOVolumeInfo.sInfoEx.ISOApplicationIdentifier);
                        stISOInfo.strBiblio = convertToQT(pISOVolumeInfo.sInfoEx.ISOBiblioIdentifier);
                        stISOInfo.strCopyright = convertToQT(pISOVolumeInfo.sInfoEx.ISOCopyrightFileIdentifier);
                        stISOInfo.strDataPreparer = convertToQT(pISOVolumeInfo.sInfoEx.ISODataPreparerIdentifier);
                        stISOInfo.strPublisher = convertToQT(pISOVolumeInfo.sInfoEx.ISOPublisherIdentifier);
                        stISOInfo.strSet = convertToQT(pISOVolumeInfo.sInfoEx.ISOSetIdentifier);
                        stISOInfo.strSystem = convertToQT(pISOVolumeInfo.sInfoEx.ISOSystemIdentifier);

                        stISOInfo.strRootAddress = QString::number(pISOVolumeInfo.nRootAddress);

                        stISOInfo.strCreationTime = QString("%1.%2.%3 %4:%5:%6").arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nDay), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nMonth), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nYear+1900), 4, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nHour), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nMinute), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOCreationDateTime.nSecond), 2, 10, QLatin1Char('0'));




                        stISOInfo.strModificationTime = QString("%1.%2.%3 %4:%5:%6").arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nDay), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nMonth), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nYear+1900), 4, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nHour), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nMinute), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOModificationDateTime.nSecond), 2, 10, QLatin1Char('0'));



                        stISOInfo.strExpirationTime = QString("%1.%2.%3 %4:%5:%6").arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nDay), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nMonth), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nYear+1900), 4, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nHour), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nMinute), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOExpirationDateTime.nSecond), 2, 10, QLatin1Char('0'));


                        stISOInfo.strEffectiveTime = QString("%1.%2.%3 %4:%5:%6").arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nDay), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nMonth), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nYear+1900), 4, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nHour), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nMinute), 2, 10, QLatin1Char('0'))
                                .arg(static_cast<int>(pISOVolumeInfo.sInfoEx.ISOEffectiveDateTime.nSecond), 2, 10, QLatin1Char('0'));


                    }

                }

                if (ti.nFileSystem & BS_FS_UDF)
                {
                    if(strInformation.length()>0)strInformation += tr(" /");
                    strInformation += tr(" UDF");

                    //UDF Version will be fine
                    HSESSION hSession = nullptr;
                    nError = ::OpenDiskSession(BS_CURRENT_DEVICE, j, &hSession, BS_FS_UDF);
                    if (nError != BS_SDK_ERROR_NO){
                        qDebugAusgabeSDK(nError, "OpenDiskSession");
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

                        stUDFInfo.strPerparer = convertToQT(pUDFVolumeInfo.chPreparer);
                        stUDFInfo.strVolumeLabel = convertToQT(pUDFVolumeInfo.chVolumeLabel);

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

            //Hier müssen wir noch unterscheiden zwischen AudioTrack, VideoTrack oder Datatrack. Zur Zeit nur Datatrack.

            QTreeWidgetItem *treeTrackItem = new QTreeWidgetItem(treechildItem);
            treeTrackItem->setText(0, trackInformation);
            treeTrackItem->setIcon(0,QIcon(":/icons/disk_data_table16.png"));
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
    setEmptyDisk(false);
    setOpenDisk(false);

    switch (mi.nMediumStatus) {
    case BS_MS_EMPTY_DISK:
        setEmptyDisk(true);
        stDiskInfo.strMediaStatus = tr("Empty Disk");
        break;
    case BS_MS_INCOMPLETE_DISK:
        setOpenDisk(true);
        stDiskInfo.strMediaStatus = tr("Incomplete Disk");
        break;
    case BS_MS_COMPLETE_DISK:
        stDiskInfo.strMediaStatus = tr("Complete Disk");
        stDiskInfo.strMediaFreeSpace = tr("n/a");
        break;

    }

    switch(mi.nExtendedMediumType){
    case BS_EMT_CD_ROM:
        setIsIsoDisk(true); //We can read a CD with ISO and RAW
        setIsBinDisk(true);
        stDiskInfo.strMediaExType = tr("CD-ROM");
        break;
    case BS_EMT_CD_ROM_XA:
        setIsIsoDisk(false);
        setIsBinDisk(true);
        stDiskInfo.strMediaExType = tr("CD-ROM XA");
        break;
    case BS_EMT_CD_AUDIO:
        setIsIsoDisk(false);
        setIsBinDisk(true);
        stDiskInfo.strMediaExType = tr("Audio CD");
        break;
    case BS_EMT_CD_MIXED_MODE:
        setIsIsoDisk(false);
        setIsBinDisk(true);
        stDiskInfo.strMediaExType = tr("Mixed Mode CD");
        break;
    case BS_EMT_CD_ENHANCED:
        setIsIsoDisk(false);
        setIsBinDisk(true);
        stDiskInfo.strMediaExType = tr("Enhanced CD");
        break;
    case BS_EMT_CD_MULTISESSION:
        setIsIsoDisk(true);
        setIsBinDisk(false);
        stDiskInfo.strMediaExType = tr("Multisession CD");
        break;
    case BS_EMT_DVD:
        setIsIsoDisk(true);
        setIsBinDisk(false);
        stDiskInfo.strMediaExType = tr("DVD");
        break;
    case BS_EMT_BD:
        setIsIsoDisk(true);
        setIsBinDisk(false);
        stDiskInfo.strMediaExType = tr("BD");
        break;
    case BS_EMT_HDDVD:
        setIsIsoDisk(true);
        setIsBinDisk(false);
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

    stDiskInfo.strMediaUPC = convertToQT(mi.chUPCEANCode);
    stDiskInfo.strMediaType = convertToQT(mi.chMediumType);

    stDiskInfo.strBridgeFileSystem = tr("n/a");
    if(stISOInfo.isISO == 1 && stUDFInfo.isUDF == 1){
        stDiskInfo.strBridgeFileSystem = tr("ISO / UDF Bridge");
    }

    int nRowCount = 0;

    if(stDiskInfo.strMediaType.length()>0 ){
        nRowCount++;
        pTableItem *tableItem2 = new pTableItem();
        tableItem2->columnText = tr("Media Type:");
        tableItem2->itemColumn = 0;
        tableItem2->itemRow = nRowCount;
        tableItem2->rowSpan = false;
        tableItem2->rowBold = false;
        myTable.append(tableItem2);

        pTableItem *tableItem3 = new pTableItem();
        tableItem3->columnText = stDiskInfo.strMediaType;
        tableItem3->itemColumn = 1;
        tableItem3->itemRow = nRowCount;
        tableItem3->rowSpan = false;
        tableItem3->rowBold = false;
        myTable.append(tableItem3);

    }

    if(stDiskInfo.strMediaExType.length()>0 ){
        nRowCount++;
        pTableItem *tableItem4 = new pTableItem();
        tableItem4->columnText = tr("Media Type Ex:");
        tableItem4->itemColumn = 0;
        tableItem4->itemRow = nRowCount;
        tableItem4->rowSpan = false;
        tableItem4->rowBold = false;
        myTable.append(tableItem4);

        pTableItem *tableItem5 = new pTableItem();
        tableItem5->columnText = stDiskInfo.strMediaExType;
        tableItem5->itemColumn = 1;
        tableItem5->itemRow = nRowCount;
        tableItem5->rowSpan = false;
        tableItem5->rowBold = false;
        myTable.append(tableItem5);
    }

    if(stDiskInfo.strMediaID.length()>0 ){
        nRowCount++;
        pTableItem *tableItem6 = new pTableItem();
        tableItem6->columnText = tr("Media ID:");
        tableItem6->itemColumn = 0;
        tableItem6->itemRow = nRowCount;
        tableItem6->rowSpan = false;
        tableItem6->rowBold = false;
        myTable.append(tableItem6);

        pTableItem *tableItem7 = new pTableItem();
        tableItem7->columnText = stDiskInfo.strMediaID;
        tableItem7->itemColumn = 1;
        tableItem7->itemRow = nRowCount;
        tableItem7->rowSpan = false;
        tableItem7->rowBold = false;
        myTable.append(tableItem7);
    }

    if(stDiskInfo.strBridgeFileSystem.length()>0 ){
        nRowCount++;
        pTableItem *tableItem8 = new pTableItem();
        tableItem8->columnText = tr("Bridge:");
        tableItem8->itemColumn = 0;
        tableItem8->itemRow = nRowCount;
        tableItem8->rowSpan = false;
        tableItem8->rowBold = false;
        myTable.append(tableItem8);

        pTableItem *tableItem9 = new pTableItem();
        tableItem9->columnText = stDiskInfo.strBridgeFileSystem;
        tableItem9->itemColumn = 1;
        tableItem9->itemRow = nRowCount;
        tableItem9->rowSpan = false;
        tableItem9->rowBold = false;
        myTable.append(tableItem9);
    }

    if(stDiskInfo.strMediaUPC.length()>0 ){
        nRowCount++;
        pTableItem *tableItem10 = new pTableItem();
        tableItem10->columnText = tr("UPC/EAN Code:");
        tableItem10->itemColumn = 0;
        tableItem10->itemRow = nRowCount;
        tableItem10->rowSpan = false;
        tableItem10->rowBold = false;
        myTable.append(tableItem10);

        pTableItem *tableItem11 = new pTableItem();
        tableItem11->columnText = stDiskInfo.strMediaUPC;
        tableItem11->itemColumn = 1;
        tableItem11->itemRow = nRowCount;
        tableItem11->rowSpan = false;
        tableItem11->rowBold = false;
        myTable.append(tableItem11);
    }

    if(stDiskInfo.strMediaSpeeds.length()>0 ){
        nRowCount++;
        pTableItem *tableItem12 = new pTableItem();
        tableItem12->columnText = tr("Speeds (kb/s):");
        tableItem12->itemColumn = 0;
        tableItem12->itemRow = nRowCount;
        tableItem12->rowSpan = false;
        tableItem12->rowBold = false;
        myTable.append(tableItem12);

        pTableItem *tableItem13 = new pTableItem();
        tableItem13->columnText = stDiskInfo.strMediaSpeeds;
        tableItem13->itemColumn = 1;
        tableItem13->itemRow = nRowCount;
        tableItem13->rowSpan = false;
        tableItem13->rowBold = false;
        myTable.append(tableItem13);
    }

    if(stDiskInfo.strMediaMaxWSpeed.length()>0 ){
        nRowCount++;
        pTableItem *tableItem14 = new pTableItem();
        tableItem14->columnText = tr("Max. Write Speed:");
        tableItem14->itemColumn = 0;
        tableItem14->itemRow = nRowCount;
        tableItem14->rowSpan = false;
        tableItem14->rowBold = false;
        myTable.append(tableItem14);

        pTableItem *tableItem15 = new pTableItem();
        tableItem15->columnText = stDiskInfo.strMediaMaxWSpeed;
        tableItem15->itemColumn = 1;
        tableItem15->itemRow = nRowCount;
        tableItem15->rowSpan = false;
        tableItem15->rowBold = false;
        myTable.append(tableItem15);
    }

    if(stDiskInfo.strMediaStatus.length()>0 ){
        nRowCount++;
        pTableItem *tableItem16 = new pTableItem();
        tableItem16->columnText = tr("Status:");
        tableItem16->itemColumn = 0;
        tableItem16->itemRow = nRowCount;
        tableItem16->rowSpan = false;
        tableItem16->rowBold = false;
        myTable.append(tableItem16);

        pTableItem *tableItem17 = new pTableItem();
        tableItem17->columnText = stDiskInfo.strMediaStatus;
        tableItem17->itemColumn = 1;
        tableItem17->itemRow = nRowCount;
        tableItem17->rowSpan = false;
        tableItem17->rowBold = false;
        myTable.append(tableItem17);
    }

    if(stDiskInfo.strBootable.length()>0 ){
        nRowCount++;
        pTableItem *tableItem18 = new pTableItem();
        tableItem18->columnText = tr("Bootable Disk:");
        tableItem18->itemColumn = 0;
        tableItem18->itemRow = nRowCount;
        tableItem18->rowSpan = false;
        tableItem18->rowBold = false;
        myTable.append(tableItem18);

        pTableItem *tableItem19 = new pTableItem();
        tableItem19->columnText = stDiskInfo.strBootable;
        tableItem19->itemColumn = 1;
        tableItem19->itemRow = nRowCount;
        tableItem19->rowSpan = false;
        tableItem19->rowBold = false;
        myTable.append(tableItem19);
    }

    if(stDiskInfo.strMediaSize.length()>0 ){
        nRowCount++;
        pTableItem *tableItem20 = new pTableItem();
        tableItem20->columnText = tr("Size:");
        tableItem20->itemColumn = 0;
        tableItem20->itemRow = nRowCount;
        tableItem20->rowSpan = false;
        tableItem20->rowBold = false;
        myTable.append(tableItem20);

        pTableItem *tableItem21 = new pTableItem();
        tableItem21->columnText = stDiskInfo.strMediaSize;
        tableItem21->itemColumn = 1;
        tableItem21->itemRow = nRowCount;
        tableItem21->rowSpan = false;
        tableItem21->rowBold = false;
        myTable.append(tableItem21);
    }

    if(stDiskInfo.strMediaUsedSpace.length()>0 ){
        nRowCount++;
        pTableItem *tableItem22 = new pTableItem();
        tableItem22->columnText = tr("Used Space:");
        tableItem22->itemColumn = 0;
        tableItem22->itemRow = nRowCount;
        tableItem22->rowSpan = false;
        tableItem22->rowBold = false;
        myTable.append(tableItem22);

        pTableItem *tableItem23 = new pTableItem();
        tableItem23->columnText = stDiskInfo.strMediaUsedSpace;
        tableItem23->itemColumn = 1;
        tableItem23->itemRow = nRowCount;
        tableItem23->rowSpan = false;
        tableItem23->rowBold = false;
        myTable.append(tableItem23);
    }

    if(stDiskInfo.strMediaFreeSpace.length()>0 ){
        nRowCount++;
        pTableItem *tableItem24 = new pTableItem();
        tableItem24->columnText = tr("Free Space:");
        tableItem24->itemColumn = 0;
        tableItem24->itemRow = nRowCount;
        tableItem24->rowSpan = false;
        tableItem24->rowBold = false;
        myTable.append(tableItem24);

        pTableItem *tableItem25 = new pTableItem();
        tableItem25->columnText = stDiskInfo.strMediaFreeSpace;
        tableItem25->itemColumn = 1;
        tableItem25->itemRow = nRowCount;
        tableItem25->rowSpan = false;
        tableItem25->rowBold = false;
        myTable.append(tableItem25);
    }

    if(stISOInfo.isISO == 1){
        nRowCount++;

        pTableItem *tableItem26 = new pTableItem();
        tableItem26->columnText = tr("Contents ISO");
        tableItem26->itemColumn = 0;
        tableItem26->itemRow = nRowCount;
        tableItem26->rowSpan = true;
        tableItem26->rowBold = true;
        myTable.append(tableItem26);

        pTableItem *tableItem27 = new pTableItem();
        tableItem27->columnText = "";
        tableItem27->itemColumn = 1;
        tableItem27->itemRow = nRowCount;
        tableItem27->rowSpan = true;
        tableItem27->rowBold = false;
        myTable.append(tableItem27);


        if(stISOInfo.strVolumeLabel.length()>0 ){
            nRowCount++;
            pTableItem *tableItem28 = new pTableItem();
            tableItem28->columnText = tr("Label:");
            tableItem28->itemColumn = 0;
            tableItem28->itemRow = nRowCount;
            tableItem28->rowSpan = false;
            tableItem28->rowBold = false;
            myTable.append(tableItem28);

            pTableItem *tableItem29 = new pTableItem();
            tableItem29->columnText = stISOInfo.strVolumeLabel;
            tableItem29->itemColumn = 1;
            tableItem29->itemRow = nRowCount;
            tableItem29->rowSpan = false;
            tableItem29->rowBold = false;
            myTable.append(tableItem29);
        }

        if(stISOInfo.strISOLevel.length()>0 ){
            nRowCount++;
            pTableItem *tableItem30 = new pTableItem();
            tableItem30->columnText = tr("Level:");
            tableItem30->itemColumn = 0;
            tableItem30->itemRow = nRowCount;
            tableItem30->rowSpan = false;
            tableItem30->rowBold = false;
            myTable.append(tableItem30);

            pTableItem *tableItem31 = new pTableItem();
            tableItem31->columnText = stISOInfo.strISOLevel;
            tableItem31->itemColumn = 1;
            tableItem31->itemRow = nRowCount;
            tableItem31->rowSpan = false;
            tableItem31->rowBold = false;
            myTable.append(tableItem31);
        }

        if(stISOInfo.strRootAddress.length()>0 ){
            nRowCount++;
            pTableItem *tableItem32 = new pTableItem();
            tableItem32->columnText = tr("Root Address:");
            tableItem32->itemColumn = 0;
            tableItem32->itemRow = nRowCount;
            tableItem32->rowSpan = false;
            tableItem32->rowBold = false;
            myTable.append(tableItem32);

            pTableItem *tableItem33 = new pTableItem();
            tableItem33->columnText = stISOInfo.strRootAddress;
            tableItem33->itemColumn = 1;
            tableItem33->itemRow = nRowCount;
            tableItem33->rowSpan = false;
            tableItem33->rowBold = false;
            myTable.append(tableItem33);
        }

        if(stISOInfo.strISOExtension.length()>0 ){
            nRowCount++;
            pTableItem *tableItem34 = new pTableItem();
            tableItem34->columnText = tr("Extension Type:");
            tableItem34->itemColumn = 0;
            tableItem34->itemRow = nRowCount;
            tableItem34->rowSpan = false;
            tableItem34->rowBold = false;
            myTable.append(tableItem34);

            pTableItem *tableItem35 = new pTableItem();
            tableItem35->columnText = stISOInfo.strISOExtension;
            tableItem35->itemColumn = 1;
            tableItem35->itemRow = nRowCount;
            tableItem35->rowSpan = false;
            tableItem35->rowBold = false;
            myTable.append(tableItem35);
        }

        if(stISOInfo.strCreationTime.length()>0 ){
            nRowCount++;
            pTableItem *tableItem36 = new pTableItem();
            tableItem36->columnText = tr("Creation Time:");
            tableItem36->itemColumn = 0;
            tableItem36->itemRow = nRowCount;
            tableItem36->rowSpan = false;
            tableItem36->rowBold = false;
            myTable.append(tableItem36);

            pTableItem *tableItem37 = new pTableItem();
            tableItem37->columnText = stISOInfo.strCreationTime;
            tableItem37->itemColumn = 1;
            tableItem37->itemRow = nRowCount;
            tableItem37->rowSpan = false;
            tableItem37->rowBold = false;
            myTable.append(tableItem37);
        }

        if(stISOInfo.strModificationTime.length()>0 ){
            nRowCount++;
            pTableItem *tableItem38 = new pTableItem();
            tableItem38->columnText = tr("Modification Time:");
            tableItem38->itemColumn = 0;
            tableItem38->itemRow = nRowCount;
            tableItem38->rowSpan = false;
            tableItem38->rowBold = false;
            myTable.append(tableItem38);

            pTableItem *tableItem39 = new pTableItem();
            tableItem39->columnText = stISOInfo.strModificationTime;
            tableItem39->itemColumn = 1;
            tableItem39->itemRow = nRowCount;
            tableItem39->rowSpan = false;
            tableItem39->rowBold = false;
            myTable.append(tableItem39);
        }

        if(stISOInfo.strExpirationTime.length()>0 ){
            nRowCount++;
            pTableItem *tableItem40 = new pTableItem();
            tableItem40->columnText = tr("Expiration Time:");
            tableItem40->itemColumn = 0;
            tableItem40->itemRow = nRowCount;
            tableItem40->rowSpan = false;
            tableItem40->rowBold = false;
            myTable.append(tableItem40);

            pTableItem *tableItem41 = new pTableItem();
            tableItem41->columnText = stISOInfo.strExpirationTime;
            tableItem41->itemColumn = 1;
            tableItem41->itemRow = nRowCount;
            tableItem41->rowSpan = false;
            tableItem41->rowBold = false;
            myTable.append(tableItem41);
        }

        if(stISOInfo.strEffectiveTime.length()>0 ){
            nRowCount++;
            pTableItem *tableItem42 = new pTableItem();
            tableItem42->columnText = tr("Effective Time:");
            tableItem42->itemColumn = 0;
            tableItem42->itemRow = nRowCount;
            tableItem42->rowSpan = false;
            tableItem42->rowBold = false;
            myTable.append(tableItem42);

            pTableItem *tableItem43 = new pTableItem();
            tableItem43->columnText = stISOInfo.strEffectiveTime;
            tableItem43->itemColumn = 1;
            tableItem43->itemRow = nRowCount;
            tableItem43->rowSpan = false;
            tableItem43->rowBold = false;
            myTable.append(tableItem43);
        }

        if(stISOInfo.strAbstract.length()>0 ){
            nRowCount++;
            pTableItem *tableItem44 = new pTableItem();
            tableItem44->columnText = tr("Abstract Identifier:");
            tableItem44->itemColumn = 0;
            tableItem44->itemRow = nRowCount;
            tableItem44->rowSpan = false;
            tableItem44->rowBold = false;
            myTable.append(tableItem44);

            pTableItem *tableItem45 = new pTableItem();
            tableItem45->columnText = stISOInfo.strAbstract;
            tableItem45->itemColumn = 1;
            tableItem45->itemRow = nRowCount;
            tableItem45->rowSpan = false;
            tableItem45->rowBold = false;
            myTable.append(tableItem45);
        }

        if(stISOInfo.strApplication.length()>0 ){
            nRowCount++;
            pTableItem *tableItem46 = new pTableItem();
            tableItem46->columnText = tr("Application Identifier:");
            tableItem46->itemColumn = 0;
            tableItem46->itemRow = nRowCount;
            tableItem46->rowSpan = false;
            tableItem46->rowBold = false;
            myTable.append(tableItem46);

            pTableItem *tableItem47 = new pTableItem();
            tableItem47->columnText = stISOInfo.strApplication;
            tableItem47->itemColumn = 1;
            tableItem47->itemRow = nRowCount;
            tableItem47->rowSpan = false;
            tableItem47->rowBold = false;
            myTable.append(tableItem47);
        }

        if(stISOInfo.strBiblio.length()>0 ){
            nRowCount++;
            pTableItem *tableItem48 = new pTableItem();
            tableItem48->columnText = tr("Biblio Identifier:");
            tableItem48->itemColumn = 0;
            tableItem48->itemRow = nRowCount;
            tableItem48->rowSpan = false;
            tableItem48->rowBold = false;
            myTable.append(tableItem48);

            pTableItem *tableItem49 = new pTableItem();
            tableItem49->columnText = stISOInfo.strBiblio;
            tableItem49->itemColumn = 1;
            tableItem49->itemRow = nRowCount;
            tableItem49->rowSpan = false;
            tableItem49->rowBold = false;
            myTable.append(tableItem49);
        }

        if(stISOInfo.strCopyright.length()>0 ){
            nRowCount++;
            pTableItem *tableItem50 = new pTableItem();
            tableItem50->columnText = tr("Copyright Identifier:");
            tableItem50->itemColumn = 0;
            tableItem50->itemRow = nRowCount;
            tableItem50->rowSpan = false;
            tableItem50->rowBold = false;
            myTable.append(tableItem50);

            pTableItem *tableItem51 = new pTableItem();
            tableItem51->columnText = stISOInfo.strCopyright;
            tableItem51->itemColumn = 1;
            tableItem51->itemRow = nRowCount;
            tableItem51->rowSpan = false;
            tableItem51->rowBold = false;
            myTable.append(tableItem51);
        }

        if(stISOInfo.strDataPreparer.length()>0 ){
            nRowCount++;
            pTableItem *tableItem52 = new pTableItem();
            tableItem52->columnText = tr("Data Preparer Identifier:");
            tableItem52->itemColumn = 0;
            tableItem52->itemRow = nRowCount;
            tableItem52->rowSpan = false;
            tableItem52->rowBold = false;
            myTable.append(tableItem52);

            pTableItem *tableItem53 = new pTableItem();
            tableItem53->columnText = stISOInfo.strDataPreparer;
            tableItem53->itemColumn = 1;
            tableItem53->itemRow = nRowCount;
            tableItem53->rowSpan = false;
            tableItem53->rowBold = false;
            myTable.append(tableItem53);
        }

        if(stISOInfo.strPublisher.length()>0 ){
            nRowCount++;
            pTableItem *tableItem54 = new pTableItem();
            tableItem54->columnText = tr("Publisher Identifier:");
            tableItem54->itemColumn = 0;
            tableItem54->itemRow = nRowCount;
            tableItem54->rowSpan = false;
            tableItem54->rowBold = false;
            myTable.append(tableItem54);

            pTableItem *tableItem55 = new pTableItem();
            tableItem55->columnText = stISOInfo.strPublisher;
            tableItem55->itemColumn = 1;
            tableItem55->itemRow = nRowCount;
            tableItem55->rowSpan = false;
            tableItem55->rowBold = false;
            myTable.append(tableItem55);
        }

        if(stISOInfo.strSet.length()>0 ){
            nRowCount++;
            pTableItem *tableItem56 = new pTableItem();
            tableItem56->columnText = tr("Set Identifier:");
            tableItem56->itemColumn = 0;
            tableItem56->itemRow = nRowCount;
            tableItem56->rowSpan = false;
            tableItem56->rowBold = false;
            myTable.append(tableItem56);

            pTableItem *tableItem57 = new pTableItem();
            tableItem57->columnText = stISOInfo.strSet;
            tableItem57->itemColumn = 1;
            tableItem57->itemRow = nRowCount;
            tableItem57->rowSpan = false;
            tableItem57->rowBold = false;
            myTable.append(tableItem57);
        }

        if(stISOInfo.strSystem.length()>0 ){
            nRowCount++;
            pTableItem *tableItem58 = new pTableItem();
            tableItem58->columnText = tr("System Identifier:");
            tableItem58->itemColumn = 0;
            tableItem58->itemRow = nRowCount;
            tableItem58->rowSpan = false;
            tableItem58->rowBold = false;
            myTable.append(tableItem58);

            pTableItem *tableItem59 = new pTableItem();
            tableItem59->columnText = stISOInfo.strSystem;
            tableItem59->itemColumn = 1;
            tableItem59->itemRow = nRowCount;
            tableItem59->rowSpan = false;
            tableItem59->rowBold = false;
            myTable.append(tableItem59);
        }


    }

    if(stUDFInfo.isUDF == 1){
        nRowCount++;
        pTableItem *tableItem60 = new pTableItem();
        tableItem60->columnText = tr("Contents UDF");
        tableItem60->itemColumn = 0;
        tableItem60->itemRow = nRowCount;
        tableItem60->rowSpan = true;
        tableItem60->rowBold = true;
        myTable.append(tableItem60);

        pTableItem *tableItem61 = new pTableItem();
        tableItem61->columnText = "";
        tableItem61->itemColumn = 1;
        tableItem61->itemRow = nRowCount;
        tableItem61->rowSpan = true;
        tableItem61->rowBold = false;
        myTable.append(tableItem61);

        if(stUDFInfo.strVolumeLabel.length()>0 ){
            nRowCount++;
            pTableItem *tableItem62 = new pTableItem();
            tableItem62->columnText = tr("Label:");
            tableItem62->itemColumn = 0;
            tableItem62->itemRow = nRowCount;
            tableItem62->rowSpan = false;
            tableItem62->rowBold = false;
            myTable.append(tableItem62);

            pTableItem *tableItem63 = new pTableItem();
            tableItem63->columnText = stUDFInfo.strVolumeLabel;
            tableItem63->itemColumn = 1;
            tableItem63->itemRow = nRowCount;
            tableItem63->rowSpan = false;
            tableItem63->rowBold = false;
            myTable.append(tableItem63);
        }

        if(stUDFInfo.strUDFVersion.length()>0 ){
            nRowCount++;
            pTableItem *tableItem64 = new pTableItem();
            tableItem64->columnText = tr("Version:");
            tableItem64->itemColumn = 0;
            tableItem64->itemRow = nRowCount;
            tableItem64->rowSpan = false;
            tableItem64->rowBold = false;
            myTable.append(tableItem64);

            pTableItem *tableItem65 = new pTableItem();
            tableItem65->columnText = stUDFInfo.strUDFVersion;
            tableItem65->itemColumn = 1;
            tableItem65->itemRow = nRowCount;
            tableItem65->rowSpan = false;
            tableItem65->rowBold = false;
            myTable.append(tableItem65);
        }

        if(stUDFInfo.strUDFPartition.length()>0 ){
            nRowCount++;
            pTableItem *tableItem66 = new pTableItem();
            tableItem66->columnText = tr("Partition:");
            tableItem66->itemColumn = 0;
            tableItem66->itemRow = nRowCount;
            tableItem66->rowSpan = false;
            tableItem66->rowBold = false;
            myTable.append(tableItem66);

            pTableItem *tableItem67 = new pTableItem();
            tableItem67->columnText = stUDFInfo.strUDFPartition;
            tableItem67->itemColumn = 1;
            tableItem67->itemRow = nRowCount;
            tableItem67->rowSpan = false;
            tableItem67->rowBold = false;
            myTable.append(tableItem67);
        }

        if(stUDFInfo.strPerparer.length()>0 ){
            nRowCount++;
            pTableItem *tableItem68 = new pTableItem();
            tableItem68->columnText = tr("Preparer:");
            tableItem68->itemColumn = 0;
            tableItem68->itemRow = nRowCount;
            tableItem68->rowSpan = false;
            tableItem68->rowBold = false;
            myTable.append(tableItem68);

            pTableItem *tableItem69 = new pTableItem();
            tableItem69->columnText = stUDFInfo.strPerparer;
            tableItem69->itemColumn = 1;
            tableItem69->itemRow = nRowCount;
            tableItem69->rowSpan = false;
            tableItem69->rowBold = false;
            myTable.append(tableItem69);
        }

        if(stUDFInfo.strUDFFolders.length()>0 ){
            nRowCount++;
            pTableItem *tableItem70 = new pTableItem();
            tableItem70->columnText = tr("Folder Count:");
            tableItem70->itemColumn = 0;
            tableItem70->itemRow = nRowCount;
            tableItem70->rowSpan = false;
            tableItem70->rowBold = false;
            myTable.append(tableItem70);

            pTableItem *tableItem71 = new pTableItem();
            tableItem71->columnText = stUDFInfo.strUDFFolders;
            tableItem71->itemColumn = 1;
            tableItem71->itemRow = nRowCount;
            tableItem71->rowSpan = false;
            tableItem71->rowBold = false;
            myTable.append(tableItem71);
        }

        if(stUDFInfo.strUDFFiles.length()>0 ){
            nRowCount++;
            pTableItem *tableItem72 = new pTableItem();
            tableItem72->columnText = tr("File Count:");
            tableItem72->itemColumn = 0;
            tableItem72->itemRow = nRowCount;
            tableItem72->rowSpan = false;
            tableItem72->rowBold = false;
            myTable.append(tableItem72);

            pTableItem *tableItem73 = new pTableItem();
            tableItem73->columnText = stUDFInfo.strUDFFiles;
            tableItem73->itemColumn = 1;
            tableItem73->itemRow = nRowCount;
            tableItem73->rowSpan = false;
            tableItem73->rowBold = false;
            myTable.append(tableItem73);
        }

        if(stUDFInfo.strRootAddress.length()>0 ){
            nRowCount++;
            pTableItem *tableItem74 = new pTableItem();
            tableItem74->columnText = tr("Root Address:");
            tableItem74->itemColumn = 0;
            tableItem74->itemRow = nRowCount;
            tableItem74->rowSpan = false;
            tableItem74->rowBold = false;
            myTable.append(tableItem74);

            pTableItem *tableItem75 = new pTableItem();
            tableItem75->columnText = stUDFInfo.strRootAddress;
            tableItem75->itemColumn = 1;
            tableItem75->itemRow = nRowCount;
            tableItem75->rowSpan = false;
            tableItem75->rowBold = false;
            myTable.append(tableItem75);
        }
    }
    if(stAudioInfo.isAudio == 1){
        nRowCount++;
        pTableItem *tableItem76 = new pTableItem();
        tableItem76->columnText = tr("Contents Audio");
        tableItem76->itemColumn = 0;
        tableItem76->itemRow = nRowCount;
        tableItem76->rowSpan = true;
        tableItem76->rowBold = true;
        myTable.append(tableItem76);

        pTableItem *tableItem77 = new pTableItem();
        tableItem77->columnText = "";
        tableItem77->itemColumn = 1;
        tableItem77->itemRow = nRowCount;
        tableItem77->rowSpan = true;
        tableItem77->rowBold = false;
        myTable.append(tableItem77);

        if(stAudioInfo.strTitle.length()>0 ){
            nRowCount++;
            pTableItem *tableItem78 = new pTableItem();
            tableItem78->columnText = tr("CDText Title:");
            tableItem78->itemColumn = 0;
            tableItem78->itemRow = nRowCount;
            tableItem78->rowSpan = false;
            tableItem78->rowBold = false;
            myTable.append(tableItem78);

            pTableItem *tableItem79 = new pTableItem();
            tableItem79->columnText = stAudioInfo.strTitle;
            tableItem79->itemColumn = 1;
            tableItem79->itemRow = nRowCount;
            tableItem79->rowSpan = false;
            tableItem79->rowBold = false;
            myTable.append(tableItem79);
        }

        if(stAudioInfo.strPerformer.length()>0 ){
            nRowCount++;
            pTableItem *tableItem80 = new pTableItem();
            tableItem80->columnText = tr("CDText Performer:");
            tableItem80->itemColumn = 0;
            tableItem80->itemRow = nRowCount;
            tableItem80->rowSpan = false;
            tableItem80->rowBold = false;
            myTable.append(tableItem80);

            pTableItem *tableItem81 = new pTableItem();
            tableItem81->columnText = stAudioInfo.strPerformer;
            tableItem81->itemColumn = 1;
            tableItem81->itemRow = nRowCount;
            tableItem81->rowSpan = false;
            tableItem81->rowBold = false;
            myTable.append(tableItem81);
        }

        if(stAudioInfo.strSongWriter.length()>0 ){
            nRowCount++;
            pTableItem *tableItem82 = new pTableItem();
            tableItem82->columnText = tr("CDText SongWriter:");
            tableItem82->itemColumn = 0;
            tableItem82->itemRow = nRowCount;
            tableItem82->rowSpan = false;
            tableItem82->rowBold = false;
            myTable.append(tableItem82);

            pTableItem *tableItem83 = new pTableItem();
            tableItem83->columnText = stAudioInfo.strSongWriter;
            tableItem83->itemColumn = 1;
            tableItem83->itemRow = nRowCount;
            tableItem83->rowSpan = false;
            tableItem83->rowBold = false;
            myTable.append(tableItem83);
        }

        if(stAudioInfo.strComposer.length()>0 ){
            nRowCount++;
            pTableItem *tableItem84 = new pTableItem();
            tableItem84->columnText = tr("CDText Composer:");
            tableItem84->itemColumn = 0;
            tableItem84->itemRow = nRowCount;
            tableItem84->rowSpan = false;
            tableItem84->rowBold = false;
            myTable.append(tableItem84);

            pTableItem *tableItem85 = new pTableItem();
            tableItem85->columnText = stAudioInfo.strComposer;
            tableItem85->itemColumn = 1;
            tableItem85->itemRow = nRowCount;
            tableItem85->rowSpan = false;
            tableItem85->rowBold = false;
            myTable.append(tableItem85);
        }

        if(stAudioInfo.strArranger.length()>0 ){
            nRowCount++;
            pTableItem *tableItem86 = new pTableItem();
            tableItem86->columnText = tr("CDText Arranger:");
            tableItem86->itemColumn = 0;
            tableItem86->itemRow = nRowCount;
            tableItem86->rowSpan = false;
            tableItem86->rowBold = false;
            myTable.append(tableItem86);

            pTableItem *tableItem87 = new pTableItem();
            tableItem87->columnText = stAudioInfo.strArranger;
            tableItem87->itemColumn = 1;
            tableItem87->itemRow = nRowCount;
            tableItem87->rowSpan = false;
            tableItem87->rowBold = false;
            myTable.append(tableItem87);
        }

        if(stAudioInfo.strMessage.length()>0 ){
            nRowCount++;
            pTableItem *tableItem88 = new pTableItem();
            tableItem88->columnText = tr("CDText Message:");
            tableItem88->itemColumn = 0;
            tableItem88->itemRow = nRowCount;
            tableItem88->rowSpan = false;
            tableItem88->rowBold = false;
            myTable.append(tableItem88);

            pTableItem *tableItem89 = new pTableItem();
            tableItem89->columnText = stAudioInfo.strMessage;
            tableItem89->itemColumn = 1;
            tableItem89->itemRow = nRowCount;
            tableItem89->rowSpan = false;
            tableItem89->rowBold = false;
            myTable.append(tableItem89);
        }

        if(stAudioInfo.strISRC.length()>0 ){
            nRowCount++;
            pTableItem *tableItem90 = new pTableItem();
            tableItem90->columnText = tr("CDText ISRC:");
            tableItem90->itemColumn = 0;
            tableItem90->itemRow = nRowCount;
            tableItem90->rowSpan = false;
            tableItem90->rowBold = false;
            myTable.append(tableItem90);

            pTableItem *tableItem91 = new pTableItem();
            tableItem91->columnText = stAudioInfo.strISRC;
            tableItem91->itemColumn = 1;
            tableItem91->itemRow = nRowCount;
            tableItem91->rowSpan = false;
            tableItem91->rowBold = false;
            myTable.append(tableItem91);
        }
    }

    items.append(treeItem);
    emit thread_finished(items, myTable);

}

void MdiChildDiskInfo::startUpdateInfo()
{
    diskInfoTable->clear();
    treeWidget->clear();

    //Move to main thread
    treeWidget->header() ->close ();
    treeWidget->updateData(0);

    if(!mWorkThread){
        mWorkThread = new ThreadInfo(this);
        connect(this, SIGNAL(thread_finished(QList<QTreeWidgetItem *>,QVector<pTableItem *>)), this, SLOT(on_thread_completed(QList<QTreeWidgetItem *>,QVector<pTableItem *>)));
    }

    mWorkThread->start();
}

void MdiChildDiskInfo::on_thread_completed(QList<QTreeWidgetItem *> items, QVector<pTableItem *> myTable)
{

    for (int ridx = 0; ridx < myTable.count(); ridx++ ){
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setText(myTable[ridx]->columnText);
        item->setFlags(Qt::NoItemFlags);
        if(myTable[ridx]->rowBold==true){
            QFont originalFont = (item)->font();
            originalFont.setBold(true);
            item->setFont(originalFont);
        }
        diskInfoTable->setItem(myTable[ridx]->itemRow, myTable[ridx]->itemColumn, item);
        if(myTable[ridx]->rowSpan==true && myTable[ridx]->itemColumn==1){
            diskInfoTable->setSpan(myTable[ridx]->itemRow,0,1,2);
        }
    }

    diskInfoTable->setRowCount(myTable.count()/2); //myTable has one entry for each column, so divided by columncount

    diskInfoTable->resizeColumnsToContents();

    treeWidget->insertTopLevelItems(0, items);
    treeWidget ->expandAll();

    if(treeWidget->treeCount()>1){
        treeWidget->updateData(1);
        hasData = 1;
    }

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    emit stopSpinner();
    //emit subwindowchanged(GetProjectType());
    emit datatrack_changed();
}

void MdiChildDiskInfo::setUIControls(Ribbon *, QWidget* parent)
{
    MainWindow *ribbonOwner = qobject_cast<MainWindow *>(parent);

    ribbonOwner->delEditButton->setEnabled(false);
    ribbonOwner->renameEditButton->setEnabled(false);
    ribbonOwner->viewBrowserButton->setEnabled(false);
    ribbonOwner->viewPropertyGridButton->setEnabled(false);
    ribbonOwner->delAllEditButton->setEnabled(false);
    ribbonOwner->updtEditButton->setEnabled(false);
    ribbonOwner->inverseSelectEditButton->setEnabled(false);
    ribbonOwner->allSelectEditButton->setEnabled(false);
    ribbonOwner->addFolderEditButton->setEnabled(false);
    ribbonOwner->addFileEditButton->setEnabled(false);
    ribbonOwner->dataTrackEditButton->setEnabled(false);
    ribbonOwner->audioTrackEditButton->setEnabled(false);
    ribbonOwner->appSaveButton->setEnabled(false);
    ribbonOwner->appSaveAsButton->setEnabled(false);
    ribbonOwner->createFolderEditButton->setEnabled(false);

    ribbonOwner->imageMediaButton->setEnabled( hasData == 1 ? true : false );
    ribbonOwner->grabAudioMediaButton->setEnabled(false);


}

//SetBurnDevice List ist in den pDevice Projekten dann das ReadDevice.
void MdiChildDiskInfo::setBurnDeviceList(QWidget* parent)
{
    MainWindow *ribbonOwner = qobject_cast<MainWindow *>(parent);

    for(int i = 0; i < ribbonOwner->listReadDevicesWidget->count(); ++i)
    {
        if(ribbonOwner->listReadDevicesWidget->itemText(i) == getBurnDrive()){
            ribbonOwner->listReadDevicesWidget->setCurrentIndex(i);
        }
    }
}
//MixedMode oder AudioCD oder ISO
//emit subwindowchanged(GetProjectType());
void MdiChildDiskInfo::setRibbonTabs(Ribbon *baseRibbon, QWidget* parent)
{

    //Für morgen
    //IM Child wird das letzte aktive Tab gespeichert.
    //Bei DiskInfo wird gespeichert ob 2Image aktiv wwar.
    baseRibbon->blockSignals(true);
    QString lastTab = getlastSelectedTab();

    baseRibbon->hideAll();
    //WEnn last selekted "Create Image" ist, dann
    //müssen wir den auch anezigen,
    if( lastTab == "Create image" && hasDataChild() == 1 ){
        if(!baseRibbon->isTabVisible( lastTab )){
            baseRibbon->showTab( ":/icons/create_image32.png", "Create image" );
        }
    }
    if(baseRibbon->isTabVisible(lastTab)){
        baseRibbon->currentTab(lastTab);
    }else{
        baseRibbon->currentTab("Device");
    }

    if(parent!=nullptr){
        setUIControls(baseRibbon, parent);
        setBurnDeviceList(parent);
    }

    baseRibbon->blockSignals(false);



}
