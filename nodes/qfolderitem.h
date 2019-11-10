#ifndef QFOLDERITEM_H
#define QFOLDERITEM_H

#include "QDataItem.h"
#include "FoxSDKExport.h"

class QFolderItem : public QDataItem
{
public:
  static QFolderItem* create(QDataItem *parent, HSESSION hSession
                             , const SFileEntry& entry
                             , int &countFile, int &countFolder);

  void ParseGenericInfo();
  void CreateAndLoadChildren(int &countFile, int &countFolder);
  virtual QString GetLBA() {return QString("%1").arg(mInfo.nAddress);}
  virtual QString getPath() {
#if defined (WIN32)
    return QString::fromUtf16((const ushort*)mInfo.lpszFilePath);
#else
    return QString::fromUtf8(mInfo.lpszFilePath);
#endif
  }
  virtual QString createAttributeString();

private:
  QFolderItem(QDataItem *parent, HSESSION hSession
              , const SFileEntry& entry
              , int &countFile, int &countFolder);

protected:
  HSESSION mHSession;
  SFileEntry mInfo;
};

#endif // QFOLDERITEM_H
