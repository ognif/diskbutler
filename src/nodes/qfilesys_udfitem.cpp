/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 2021 Ingo Foerster (pixbytesl@gmail.com).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License 3 as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License 3 for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

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
