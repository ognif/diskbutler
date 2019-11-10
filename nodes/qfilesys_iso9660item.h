#ifndef QFILESYSISO9660ITEM_H
#define QFILESYSISO9660ITEM_H

#include "qfilesysitem.h"
#include <QList>

#include "FoxSDKExport.h"

class QFileSysISO9660Item : public QFileSysItem
{
public:
  static QFileSysISO9660Item* create(QDataItem *parent, int track, int fsExtension = 0);
  ~QFileSysISO9660Item() {}
  virtual void ParseGenericInfo();
  //virtual void CreateAndLoadChildren();
  virtual bool isISOFileSystem() {return true;}
  virtual int32 fsTypeInt() {return BS_FS_ISO9660;}

  SISOVolumeInfo& GetISOInfo() {return mInfo;}

private:
  QFileSysISO9660Item(QDataItem *parent, int track, int fsExtension);

protected:
  int mFsExtension;
  SISOVolumeInfo mInfo;
};

#endif // QFILESYSISO9660ITEM_H
