#ifndef QMEDIUMITEM_H
#define QMEDIUMITEM_H

#include "QDataItem.h"
#include <QList>

#include "FoxSDKExport.h"

class QMediumItem : public QDataItem
{
public:
  static QMediumItem* create(CommonTreeWidget *view);

  void ParseGenericInfo();
  void CreateAndLoadChildren();

  virtual QString getMediumType() {
#if defined (WIN32)
      return QString::fromUtf16(mInfo.chMediumType);/
#else
      return QString::fromUtf8(mInfo.chMediumType);
#endif
  }
  virtual QString getMediumStatus();
  virtual QString getUPCCode() {
#if defined (WIN32)
      return QString::fromUtf16(mInfo.chUPCEANCode);/
#else
      return QString::fromUtf8(mInfo.chUPCEANCode);
#endif
  }
  virtual int getTrackCount() {return mInfo.nLastTrack;}
  virtual int getSessionCount() {return mInfo.nLastSession;}
  virtual qint64 getMediumSize() {return mInfo.dMediumSize;}
  //We return the new calculated size in property dialog
  virtual qint64 getMediumUsedSize() {return nMediumSize; /*mInfo.dMediumUsedSize;*/}
  virtual qint64 getMediumFreeSize();
  virtual int16 getImageFormats() {return nImageFormats;}
  QString getPerformer() {return strPerformer;}
  QString getTitle() {return strTitle;}
  QString getArranger() {return strArranger;}
  QString getComposer() {return strComposer;}
  QString getMessage() {return strMessage;}
  QString getSongWriter() {return strSongWriter;}
  int16 getExtendedMediumType() {return mInfo.nExtendedMediumType;}
  void setMediumSize(qint64 nSize) {nMediumSize=nSize;}
  void setNewSize() {SetDataSize(nMediumSize,true);}


private:
  QMediumItem(CommonTreeWidget *view);

protected:
  int32 ExtractTextFromHandle(int32 handle,int32 nCDTCI, QString& rText);
  SMediumInfo mInfo;
  int16 nImageFormats;
  QString strPerformer;
  QString strTitle;
  QString strArranger;
  QString strComposer;
  QString strMessage;
  qint64 nMediumSize;
  QString strSongWriter;
};

#endif // QMEDIUMITEM_H
