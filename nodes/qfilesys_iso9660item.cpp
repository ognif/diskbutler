#include "qfilesys_iso9660item.h"

QFileSysISO9660Item* QFileSysISO9660Item::create(QDataItem *parent, int track, int fsExtension)
{
  return new QFileSysISO9660Item(parent, track, fsExtension);
}

QFileSysISO9660Item::QFileSysISO9660Item(QDataItem *parent, int track, int fsExtension)
  : QFileSysItem(parent, track)
  , mFsExtension(fsExtension)
{
  mFSType = FS_ISO9660;
  HSESSION hSession = 0;
  int res = ::OpenDiskSession(BS_READ_DEVICE, track, &hSession, fsExtension ? fsExtension : BS_FS_ISO9660);

  if (res != BS_SDK_ERROR_NO) {
    // can't read file system, skip this
    return;
  }
  CreateAndLoadChildren(hSession);

  ::GetISOVolumeInformation(hSession, &mInfo);
  ::CloseDiskSession(hSession);

  SetType(QDataItem::Folder);
  ParseGenericInfo();
}

void QFileSysISO9660Item::ParseGenericInfo()
{
  switch (mFsExtension) {
  case BS_FS_JOLIET:
  {
    mFSType = FS_JOLIET;
    SetName(QString("Joliet"));
  }
    break;
  case BS_FS_ROCKRIDGE:
  {
    mFSType = FS_ROCKRIDGE;
    SetName(QString("Rock Ridge"));
  }
    break;
  default :
  {
    SetName(QString("ISO 9660"));
  }
    break;
  }
  SetText0(GetName());
  qlonglong nTemp = (qlonglong)mInfo.nVolumeSize*2048;
  SetDataSize(nTemp,true);
  SetDataLBA(0);

  //info.lba = m_info.nVolumeDescriptorAddress;
}
