#ifndef QFILEITEM_H
#define QFILEITEM_H

#include "QDataItem.h"
#include "FoxSDKExport.h"

class QFileItem : public QDataItem
{
public:
  static QFileItem* create(QDataItem *parent, const HSESSION hSession, const SFileEntry& entry);

  void ParseGenericInfo();
  virtual QString GetLBA() {return QString("%1").arg(mInfo.nAddress);}
  virtual std::vector<uint32>* GetExtentLocation() {return &mLocationVector;}
  virtual std::vector<uint32>* GetExtentLength() {return &mLengthVector;}
  virtual QString createAttributeString();
  virtual QString getPath() {
#if defined (WIN32)
    return QString::fromUtf16((const ushort*)mInfo.lpszFilePath);
#else
    return QString::fromUtf8(mInfo.lpszFilePath);
#endif

  }


private:
  QFileItem(QDataItem *parent, const HSESSION hSession, const SFileEntry& entry);
  void ParseAllocationTable(const HSESSION hSession);


protected:
  SFileEntry mInfo;
  std::vector<uint32> mLocationVector;
  std::vector<uint32> mLengthVector;

};

#endif // QFILEITEM_H
