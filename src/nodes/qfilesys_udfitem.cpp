#include "qfilesys_udfitem.h"

QFileSysUDFItem* QFileSysUDFItem::create(QDataItem *parent, int track)
{
  return new QFileSysUDFItem(parent, track);
}

QFileSysUDFItem::QFileSysUDFItem(QDataItem *parent, int track)
  : QFileSysItem(parent, track)
{
  mFSType = FS_UDF;
  HSESSION hSession = 0;
  int res = ::OpenDiskSession(BS_READ_DEVICE, track, &hSession, BS_FS_UDF);

  if (res != BS_SDK_ERROR_NO) {
    // can't read file system, skip this
    return;
  }
  CreateAndLoadChildren(hSession);

  ::GetUDFVolumeInformation(hSession, &mInfo);
  ::CloseDiskSession(hSession);

  SetType(QDataItem::Folder);
  ParseGenericInfo();
}

void QFileSysUDFItem::ParseGenericInfo()
{
  SetName(QString("UDF"));
  SetText0(GetName());
  qlonglong nTemp = (qlonglong)mInfo.nPartitionLength*2048;
  SetDataSize(nTemp,true);
  SetDataLBA(0);
  //info.lba = m_info.nPartitionAddress;

}

QString QFileSysUDFItem::getUDFTypeString()
{
    QString strType;
    switch(mInfo.nVersion){
        case BS_UDF_VERSION_102:
          strType = QObject::tr("(1.02)");
          break;
        case BS_UDF_VERSION_150:
          strType =  QObject::tr("1.50");
          break;
        case BS_UDF_VERSION_200:
          strType =  QObject::tr("2.00");
          break;
        case BS_UDF_VERSION_201:
          strType =  QObject::tr("2.01");
          break;
        case BS_UDF_VERSION_250:
          strType =  QObject::tr("2.50");
          break;
        case BS_UDF_VERSION_260:
          strType =  QObject::tr("2.60");
          break;
        default:
          strType =  QObject::tr("Unknown");
    }

    return strType;

}

QString QFileSysUDFItem::getUDFPartionType()
{
    QString strType;
    switch(mInfo.nPartitionType){
        case BS_UDF_PARTITION_PHYSICAL:
          strType = QObject::tr("Physical");
          break;
        case BS_UDF_PARTITION_VIRTUAL:
          strType = QObject::tr("Virtual");
          break;
        case BS_UDF_PARTITION_SPARABLE:
          strType = QObject::tr("Sparable");
          break;
    }

    return strType;

}
