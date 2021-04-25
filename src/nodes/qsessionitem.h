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

#ifndef QSESSIONITEM_H
#define QSESSIONITEM_H

#include "qdataitem.h"
#include <QList>

#include "FoxSDKExport.h"

class QSessionItem : public QDataItem
{
public:
  static QSessionItem* create(QDataItem *parent, int session);
  void ParseGenericInfo();
  void CreateAndLoadChildren();
  virtual int getFirstTrack() {return mInfo.nFirstTrack;}
  virtual int getLastTrack() {return mInfo.nLastTrack;}
  virtual int getLastSession() {return mInfo.bLastSession;}
  //We return the new calculates session size not the one reported by SDK
  virtual qint64 getSessionSize() {return nSessionSize; /*mInfo.dSessionSize;*/}
  virtual int getStartLBA() {return mInfo.dStartLBA;}
  void setSessionSize(qint64 nSize) {nSessionSize = nSize;}
  void setNewSize() {SetDataSize(nSessionSize,true);}

private:
  QSessionItem(QDataItem *parent, int session);

protected:
  int mSession;
  SSessionInfo mInfo;
  qint64 nSessionSize;

};

#endif // QSESSIONITEM_H
