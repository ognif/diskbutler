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
