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

#include "qmediumitem.h"
#include "qsessionitem.h"
#include "utils_common.h"

QMediumItem* QMediumItem::create(CommonTreeWidget *view)
{
    return new QMediumItem(view);
}

QMediumItem::QMediumItem(CommonTreeWidget *view)
    : QDataItem(view)
{
    SetType(QDataItem::Disk);
    ParseGenericInfo();
}

void QMediumItem::ParseGenericInfo()
{

    //Was passiert bei Einem Fehler?

    int32 res = ::GetMediumInformation(BS_READ_DEVICE, &mInfo);
    if (res != BS_SDK_ERROR_NO) {
        if (BS_SCSI_ERROR_DISK_31 == res) {
            SetName(QString(QObject::tr("No medium")));
            SetText0(GetName());
        }else{
            SetName(QString(QObject::tr("Unknown disk image or disk type.")));
            SetText0(GetName());
        }
        return;
    }

    SetName(convertToQT(mInfo.chMediumType));


    if (mInfo.nMediumStatus == BS_MS_EMPTY_DISK) {
        SetName(QString("Empty %1").arg(QString::fromUtf16((const ushort*)mInfo.chMediumType)));
    } else {
        qlonglong nTemp;
        if(mInfo.nExtendedMediumType == BS_EMT_CD_AUDIO){
            nTemp = ((qlonglong)mInfo.dMediumUsedSize/2048)*2352;
        }else{
            nTemp = (qlonglong)mInfo.dMediumUsedSize;
        }

        //qlonglong nTemp = (qlonglong)mInfo.dMediumSize;
        SetDataSize(nTemp,true);
        SetDataLBA(0);
    }
    SetText0(GetName());

    //mInfo.nExtendedMediumType

    int32 hCdText;
    int32 nError = ::ReadCDText(BS_READ_DEVICE, &hCdText);

    if(nError == BS_SDK_ERROR_NO){

        ExtractTextFromHandle(hCdText, BS_CDTCI_PERFORMER, strPerformer);
        ExtractTextFromHandle(hCdText, BS_CDTCI_TITLE, strTitle);
        ExtractTextFromHandle(hCdText, BS_CDTCI_ARRANGER, strArranger);
        ExtractTextFromHandle(hCdText, BS_CDTCI_COMPOSER, strComposer);
        ExtractTextFromHandle(hCdText, BS_CDTCI_MESSAGE, strMessage);
        ExtractTextFromHandle(hCdText, BS_CDTCI_SONG_WRITER, strSongWriter);

        ::CloseCDTextHandle(hCdText);

    }


    ::GetPossibleImageFormats(&nImageFormats);

}

int32 QMediumItem::ExtractTextFromHandle(int32 handle,int32 nCDTCI, QString& rText)
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
        rText = "";
        return BS_SDK_ERROR_NO;
    }

    TCHAR *pBuf = new TCHAR[nLen];
    res = ::GetCDTextDiskTagString(handle, nCDTCI, pBuf, &nLen);
    if(res != BS_SDK_ERROR_NO)
    {
        delete[] pBuf;
        return res;
    }

    pBuf[nLen-1] = '\0';

    rText = convertToQT(pBuf);

    delete[] pBuf;

    return BS_SDK_ERROR_NO;
}

void QMediumItem::CreateAndLoadChildren()
{
    setExpanded(true);
    for(int session = 1; session <= mInfo.nLastSession; ++session) {
        QSessionItem* sessionItem = QSessionItem::create(this, session);
        sessionItem->CreateAndLoadChildren();
    }
    if (childCount() > 0) {
        SetDataItemCount(((QDataItem*)child(0))->GetDataItemCount());
    }
}

QString QMediumItem::getMediumStatus()
{
    QString strStatus;
    switch (mInfo.nMediumStatus) {
    case BS_MS_EMPTY_DISK:
        strStatus = QObject::tr("Empty Disk");
        break;
    case BS_MS_INCOMPLETE_DISK:
        strStatus = QObject::tr("Incomplete Disk");
        break;
    case BS_MS_COMPLETE_DISK:
        strStatus = QObject::tr("Complete Disk");
        break;
    }
    return strStatus;
}

qint64 QMediumItem::getMediumFreeSize()
{
    if(mInfo.nMediumStatus == BS_MS_COMPLETE_DISK){
        return 0;
    }
    return mInfo.dMediumFreeSize;
}
