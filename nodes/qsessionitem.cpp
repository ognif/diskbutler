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
