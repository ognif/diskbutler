#ifndef QFILESYSITEM_H
#define QFILESYSITEM_H

#include "QDataItem.h"

#include "FoxSDKExport.h"

class QFileSysItem : public QDataItem
{
public:
  enum FSType {
    FS_UNKNOWN,
    FS_ISO9660,
    FS_JOLIET,
    FS_UDF,
    FS_BOOTABLE,
    FS_ROCKRIDGE,
  };
public:
  static QFileSysItem* create(QDataItem *parent, int track);
  ~QFileSysItem() {}
  virtual void ParseGenericInfo() {}
  virtual void CreateAndLoadChildren(HSESSION hSession);
  FSType GetFSType() {return mFSType;}
  QString GetFSTypeStr();
  virtual int32 fsTypeInt() {return BS_FS_UNKNOWN;}


protected:
  QFileSysItem(QDataItem *parent, int track);

protected:
  int mTrack;
  FSType mFSType;
};

#endif // QFILESYSITEM_H
