#include "qtrackitem.h"
#include "qfilesysitem.h"
#include "qfilesys_iso9660item.h"
#include "qfilesys_udfitem.h"

QTrackItem* QTrackItem::create(QDataItem *parent, int track)
{
  return new QTrackItem(parent, track);
}

QTrackItem::QTrackItem(QDataItem *parent, int track)
  : QDataItem(parent)
  , mTrack(track)
{
  ParseGenericInfo();
}

void QTrackItem::ParseGenericInfo()
{
  int res = ::GetTrackInformation(BS_READ_DEVICE, mTrack, &mInfo);
  if (BS_SDK_ERROR_NO != res) {
    return;
  }
  qlonglong nTemp;
  if (BS_TF_AUDIO == mInfo.nFormat){
    SetType(QDataItem::AudioTrack);

    SetDataLBA(mInfo.nStartLBA);
    //To be correct we need to check the media type. If CD-ROM in all cases it is 2352
    nTemp = (qlonglong)mInfo.nSize*2352;

    int32 hCdText;
    int32 nError = ::ReadCDText(BS_READ_DEVICE, &hCdText);

    QString strInfo = QString("AudioTrack %1").arg(QString::number(mTrack));

    if(nError == BS_SDK_ERROR_NO){

        int32 res1 = ExtractTextFromHandle(hCdText, mTrack, BS_CDTCI_PERFORMER, strPerformer);
        int32 res2 = ExtractTextFromHandle(hCdText, mTrack, BS_CDTCI_TITLE, strTitle);
        ExtractTextFromHandle(hCdText, mTrack, BS_CDTCI_ARRANGER, strArranger);
        ExtractTextFromHandle(hCdText, mTrack, BS_CDTCI_COMPOSER, strComposer);
        ExtractTextFromHandle(hCdText, mTrack, BS_CDTCI_MESSAGE, strMessage);
        ExtractTextFromHandle(hCdText, mTrack, BS_CDTCI_SONG_WRITER, strSongWriter);

        ::CloseCDTextHandle(hCdText);

        if(res1 == BS_SDK_ERROR_NO && res2 == BS_SDK_ERROR_NO){
            strInfo = QString("%1 - %2").arg(strPerformer,strTitle);
        }
    }

    //TCHAR bufISRC[13];
    //nError = ::GetTrackISRC(BS_READ_DEVICE, mTrack, bufISRC);
   // if(nError == BS_SDK_ERROR_NO){
    //    bufISRC[12] = TCHAR(0);
    //    strISRC = QString::fromWCharArray(bufISRC);
    //}


    SetAudioTrackInfo(strInfo);

  } else {
    SetType(QDataItem::DataTrack);
    SetDataTrackMode(GetMode());
    SetDataLBA(0);
    //Mode 2 is 2336
    nTemp = (qlonglong)mInfo.nSize*2048;
  }

  SetDataSize(nTemp,true);

}

int32 QTrackItem::ExtractTextFromHandle(int32 handle,int32 nTrackNumber,int32 nCDTCI, QString& rText)
{
    int32 nLen = 0;
    int32 res;

    res = ::GetCDTextTrackTagString(handle, nTrackNumber, nCDTCI, 0, &nLen);
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
    res = ::GetCDTextTrackTagString(handle, nTrackNumber, nCDTCI, pBuf, &nLen);
    if(res != BS_SDK_ERROR_NO)
    {
        delete pBuf;
        return res;
    }

    pBuf[nLen-1] = '\0';
#if defined (WIN32)
      rText = QString::fromUtf16(pBuf);//QString::fromWCharArray(pBuf);
#else
      rText = QString::fromUtf8(pBuf);//QString::fromWCharArray(pBuf);
#endif

    delete pBuf;

    return BS_SDK_ERROR_NO;
}

void QTrackItem::CreateAndLoadChildren()
{
  setExpanded(true);
  if (mInfo.nFormat != BS_TF_AUDIO) {
    // data track, create file system nodes
    if (mInfo.nFileSystem & BS_FS_ISO9660) {
      QFileSysISO9660Item::create(this, mTrack);
    }
    if (mInfo.nFileSystem & BS_FS_JOLIET) {
      QFileSysISO9660Item::create(this, mTrack, BS_FS_JOLIET);
    }
    if (mInfo.nFileSystem & BS_FS_ROCKRIDGE) {
      QFileSysISO9660Item::create(this, mTrack, BS_FS_ROCKRIDGE);
    }
    if (mInfo.nFileSystem & BS_FS_UDF) {
      QFileSysUDFItem::create(this, mTrack);
    }
  }
  if (childCount() > 0) {
    SetDataItemCount(((QDataItem*)child(0))->GetDataItemCount());
  }
}

int QTrackItem::GetMode()
{
  if (BS_TF_DATA_MODE1 == mInfo.nFormat) {
    return 1;
  } else if (BS_TF_DATA_MODE2 == mInfo.nFormat) {
    return 2;
  } else {
    return 3; // Unknown
  }

}

QString QTrackItem::GetModeStr()
{
  if (BS_TF_DATA_MODE1 == mInfo.nFormat) {
    return "Mode1";
  } else if (BS_TF_DATA_MODE2 == mInfo.nFormat) {
    return "Mode2";
  }

  return "Mode2";

}
