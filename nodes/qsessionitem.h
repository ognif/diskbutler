#ifndef QSESSIONITEM_H
#define QSESSIONITEM_H

#include "QDataItem.h"
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
