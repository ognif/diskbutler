/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 20019 Ingo Foerster (pixbytesl@gmail.com).
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

#include "qfilesysitem.h"
#include "qfolderitem.h"
#include "qfileitem.h"

QFileSysItem* QFileSysItem::create(QDataItem *parent, int track)
{
  return new QFileSysItem(parent, track);
}

QFileSysItem::QFileSysItem(QDataItem *parent, int track)
  : QDataItem(parent)
  , mTrack(track)
  , mFSType(FS_UNKNOWN)
{
}

void QFileSysItem::CreateAndLoadChildren(HSESSION hSession)
{
  //setExpanded(true);
  //m_root = new DirectoryNode(hSession, "", SFileEntry(), this);
  //QFolderItem::create(this, hSession, "", SFileEntry());
  HDIR hDir = 0;
  int res = ::OpenDirectory(hSession, (const TCHAR*)QString("").utf16(), &hDir);
  if (res != BS_SDK_ERROR_NO) {
    return;
  }

  int countFile = 0;
  int countFolder = 0;
  for (int i = 0; ; ++i) {
    SFileEntry entry;
    res = ::ReadDirectory(hDir, i, &entry);
    if (res != BS_SDK_ERROR_NO)
      break;

    if (entry.nAttrib & BS_FA_DIRECTORY) {
      int tempCountFile = 0, tempCountFolder = 0;
      QFolderItem::create(this, hSession, entry, tempCountFile, tempCountFolder);
      countFile += tempCountFile;
      countFolder += tempCountFolder;
    } else {
      QFileItem::create(this, hSession, entry);
      countFile += 1;
    }
  }
  SetDataItemCount(countFile);
  SetDataItemCount(countFolder);
  ::CloseDirectory(hDir);
}

QString QFileSysItem::GetFSTypeStr()
{
  switch (mFSType) {
  //case FS_UNKNOWN:
  //  return QString("UNKNOWN");
  case FS_ISO9660:
    return QString("ISO9660");
  case FS_JOLIET:
    return QString("JOLIET");
  case FS_UDF:
    return QString("UDF");
  case FS_BOOTABLE:
    return QString("BOOTABLE");
  case FS_ROCKRIDGE:
    return QString("ROCK RIDGE");
  default:
    return QString("UNKNOWN");
  }
}
