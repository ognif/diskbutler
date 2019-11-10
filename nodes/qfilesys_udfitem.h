#ifndef QFILESYSUDFITEM_H
#define QFILESYSUDFITEM_H

#include "qfilesysitem.h"
#include <QList>

#include "FoxSDKExport.h"

class QFileSysUDFItem : public QFileSysItem
{
public:
  static QFileSysUDFItem* create(QDataItem *parent, int track);
  ~QFileSysUDFItem() {}
  virtual void ParseGenericInfo();
  //virtual void CreateAndLoadChildren();
  virtual QString getUDFTypeString();
  virtual QString getUDFPartionType();
  virtual bool isUDFFileSystem() {return true;}
  virtual int32 fsTypeInt() {return BS_FS_UDF;}


  SUDFVolumeInfo& GetUDFInfo() {return mInfo;}


private:
  QFileSysUDFItem(QDataItem *parent, int track);

protected:
  SUDFVolumeInfo mInfo;
};

#endif // QFILESYSUDFITEM_H
