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
