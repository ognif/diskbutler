#ifndef QTRACKITEM_H
#define QTRACKITEM_H

#include "QDataItem.h"
#include <QList>

#include "FoxSDKExport.h"

class QTrackItem : public QDataItem
{
public:
  static QTrackItem* create(QDataItem *parent, int track);

  void ParseGenericInfo();
  void CreateAndLoadChildren();
  int GetMode();
  STrackInfo& GetTrackInfo() {return mInfo;}

  virtual int getStartLBA() {return mInfo.nStartLBA;}
  virtual int getTrackIndex() {return mInfo.nTrackNumber;}
  virtual int getSessionNumber() {return mInfo.nSessionNumber;}
  //We return the new size
  virtual qint64 getTrackSize(){return trackSize; /*mInfo.nSize;*/}
  virtual QString GetModeStr();
  virtual qint64 getSectorSize() {return sectorSize;}
  QString getPerformer() {return strPerformer;}
  QString getAlbum() {return strAlbum;}
  QString getTitle() {return strTitle;}
  QString getArranger() {return strArranger;}
  QString getComposer() {return strComposer;}
  QString getMessage() {return strMessage;}
  QString getSongWriter() {return strSongWriter;}
  QString getISRC() {return strISRC;}


  void setTitle(QString inTitle) {strTitle = inTitle;}
  void setPerformer(QString inPerformer) {strPerformer = inPerformer;}
  void setAlbum(QString inAlbum) {strAlbum = inAlbum;}
  void setTrackSize(qint64 nSize) {trackSize = nSize;}
  void setNewSize() {SetDataSize(trackSize,true);}
  void setSectorSize(qint64 nSize){sectorSize=nSize;}

private:
  QTrackItem(QDataItem *parent, int track);

protected:
  int32 ExtractTextFromHandle(int32 handle,int32 nTrackNumber,int32 nCDTCI, QString& rText);
  int mTrack;
  QString strPerformer;
  QString strTitle;
  QString strArranger;
  QString strComposer;
  QString strMessage;
  QString strSongWriter;
  QString strISRC;
  QString strAlbum;
  STrackInfo mInfo;
  qint64 trackSize;
  qint64 sectorSize;
};

#endif // QTRACKITEM_H
