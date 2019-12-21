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

#ifndef QAUDIOTRACKITEM_H
#define QAUDIOTRACKITEM_H

#include "QDataItem.h"

class QAudioTrackItem : public QDataItem
{
  //Q_OBJECT
public:
  explicit QAudioTrackItem(QDataItem *parent = 0);
  virtual QString ToXMLElement(int depth);
  virtual void FromXMLElement(const QString &qName, const QXmlAttributes &attributes);

  QString getArranger() {return mArranger;}
  QString getComposer() {return mComposer;}
  QString getSongWriter() {return mSongWriter;}
  QString getPerformer() {return mPerformer;}
  QString getMessage() {return mAudioMessage;}
  QString getTitle() {return mAudioTitle;}
  QString getUPCEAN() {return mUPCEAN;}
  int getPause() {return mPause;}
  bool getPauseInFrames() {return bPauseInFrames;}

  void setArranger(QString str) {mArranger = str;}
  void setComposer(QString str) {mComposer = str;}
  void setSongWriter(QString str) {mSongWriter = str;}
  void setPerformer(QString str) {mPerformer = str;}
  void setMessage(QString str) {mAudioMessage = str;}
  void setTitle(QString str) {mAudioTitle = str;}
  void setUPCEAN(QString str) {mUPCEAN = str;}
  void setPause(int value) {mPause = value;}
  void setPauseInFrames(bool value) {bPauseInFrames = value;}
  void resetAll();

  std::vector<int32> getIndexes() {return mIndexes;}
  void addIndex(int minutes, int seconds, int frames);
  void removeIndex(int i);
  const int32* getPointerIndexes();
  int32 getIndexesLength();
signals:

public slots:

private:
  QString mArranger;
  QString mComposer;
  QString mSongWriter;
  QString mPerformer;
  QString mAudioMessage;
  QString mAudioTitle;
  QString mUPCEAN;
  int mPause;
  bool bPauseInFrames;
  //const int32* mIndexes;
  //int32 mIndexesLength;
  std::vector<int32> mIndexes;
};

#endif // QAUDIOTRACKITEM_H
