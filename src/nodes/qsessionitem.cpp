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

#include "qsessionitem.h"
#include "qtrackitem.h"
#include <QDebug>

QSessionItem* QSessionItem::create(QDataItem *parent, int session)
{
  return new QSessionItem(parent, session);
}

QSessionItem::QSessionItem(QDataItem *parent, int session)
  : QDataItem(parent)
  , mSession(session)
{
  SetType(QDataItem::Session);
  ParseGenericInfo();
}

void QSessionItem::ParseGenericInfo()
{
  int res = ::GetSessionInformation(BS_READ_DEVICE, mSession, &mInfo);
  if (res != BS_SDK_ERROR_NO) {
    return;
  }

  SetName(QString("Session %1").arg(mSession));
  SetText0(GetName());
  qlonglong nTemp = (qlonglong)mInfo.dSessionSize*2048;
  //qDebug() << QString::number(nTemp);
  SetDataSize(nTemp,true);
  SetDataLBA(0);
}

void QSessionItem::CreateAndLoadChildren()
{
  setExpanded(true);
  for(int track = mInfo.nFirstTrack; track <= mInfo.nLastTrack; ++track) {


    QTrackItem* trackItem = QTrackItem::create(this, track);
    trackItem->CreateAndLoadChildren();
  }
  if (childCount() > 0) {
    SetDataItemCount(((QDataItem*)child(0))->GetDataItemCount());
  }
}
