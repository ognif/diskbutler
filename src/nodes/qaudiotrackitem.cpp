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

#include "qaudiotrackitem.h"
#include "utils/utils_common.h"
#include <vector>

QAudioTrackItem::QAudioTrackItem(QDataItem *parent) :
  QDataItem(parent)
, mArranger("")
, mComposer("")
, mSongWriter("")
, mPerformer("")
, mAudioMessage("")
, mAudioTitle("")
, mUPCEAN("")
, mPause(2)
, bPauseInFrames(false)
{
}

QString QAudioTrackItem::ToXMLElement(int depth)
{
  QString indexes = "";
  for (int i = 0; i < mIndexes.size(); i++) {
    //indexes += QString::number(mIndexes[i]) + ",";
    indexes += MSFInt32ToStr(mIndexes[i]) + ",";
  }

  QString out = "";
  bool folded = !isExpanded();
  out += indent(depth)
      + "<audiotrack name=" + escapedAttribute(text(0))
      + " size=" + escapedAttribute(QString::number(GetDataSize()))
      + " item_count=" + escapedAttribute(QString::number(GetDataItemCount()))
      + " folded=\"" + (folded ? "yes" : "no") + "\""
      + " arranger=" + escapedAttribute(mArranger)
      + " composer=" + escapedAttribute(mComposer)
      + " songwriter=" + escapedAttribute(mSongWriter)
      + " performer=" + escapedAttribute(mPerformer)
      + " message=" + escapedAttribute(mAudioMessage)
      + " title=" + escapedAttribute(mAudioTitle)
      + " upcean=" + escapedAttribute(mUPCEAN)
      + " pause=" + escapedAttribute(QString::number(mPause))
      + " pauseInFrames=" + escapedAttribute(bPauseInFrames?"true":"false")
      + " indexes=" + escapedAttribute(indexes)
      + ">\n";

  for (int i = 0; i < childCount(); ++i)
    out += ((QDataItem*)child(i))->ToXMLElement(depth + 1);

  out += indent(depth) + "</audiotrack>\n";
  return out;
}

void QAudioTrackItem::FromXMLElement(const QString &qName, const QXmlAttributes &attributes)
{
  if (qName == "audiotrack") {
    QString size = attributes.value("size");
    QString count = attributes.value("item_count");
    SetData(QDataItem::AudioTrack, QString(""), attributes.value("name"),
            size.toInt(), count.toInt(), 0);
    setArranger(attributes.value("arranger"));
    setComposer(attributes.value("composer"));
    setSongWriter(attributes.value("songwriter"));
    setPerformer(attributes.value("performer"));
    setMessage(attributes.value("message"));
    setTitle(attributes.value("title"));
    setUPCEAN(attributes.value("upcean"));
    setPause(attributes.value("pause").toInt());
    setPauseInFrames((attributes.value("pauseInFrames")=="true")?true:false);
    bool folded = (attributes.value("folded") != "no");
    setExpanded(!folded);
    QString indexes = attributes.value("indexes");
    QStringList list = indexes.split(",");
    if (!list.isEmpty()) {
      list.removeLast();
    }
    foreach (QString index, list) {
      mIndexes.push_back(MSFStrToInt32(index));
    }
  }
}

void QAudioTrackItem::addIndex(int minutes, int seconds, int frames)
{
  int32 index = MSFToInt32(minutes, seconds, frames);
  mIndexes.push_back(index);
  /*const int32 *p = getPointerIndexes();
  int debug;
  for (int i = 0; i < mIndexes.size(); i++) {
    debug = *(p+i);
  }*/
}

void QAudioTrackItem::removeIndex(int i)
{
  mIndexes.erase(mIndexes.begin() + i);
}

const int32* QAudioTrackItem::getPointerIndexes()
{
  return &mIndexes[0];
}

int32 QAudioTrackItem::getIndexesLength()
{
  return mIndexes.size();
}

void QAudioTrackItem::resetAll()
{
    mIndexes.clear();
    mPause = 2;
    bPauseInFrames = false;
    mArranger = "";
    mComposer = "";
    mSongWriter = "";
    mPerformer = "";
    mAudioMessage = "";
    mAudioTitle = "";
    mUPCEAN = "";

}
