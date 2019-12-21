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

#include <QtWidgets>

#include "zimageloader.h"
#include "qmediumitem.h"
#include "qsessionitem.h"
#include "qtrackitem.h"
#include "qfilesysitem.h"
#include "qfolderitem.h"
#include "qfileitem.h"

ZImageLoader::ZImageLoader(CommonTreeWidget *treeWidget)
  : mTreeWidget(treeWidget)
{
}

bool ZImageLoader::Parse(const QString &imagePath, bool isImage)
{
  //QDataItem *diskItem = (QDataItem *)mTreeWidget->topLevelItem(0);
  mTreeWidget->takeTopLevelItem(0);

  //SDK Fix
  QString imagePath2 = QDir::toNativeSeparators(imagePath);


  int32 res;
  if (isImage) {
#if defined (WIN32)
    res = ::SetImageFilePath((const TCHAR*)imagePath2.toUtf16());
#else
    res = ::SetImageFilePath((const TCHAR*)imagePath2.toUtf8());
#endif
    if(res!=BS_SDK_ERROR_NO)
    {
        return false;
    }
#if defined (WIN32)
    res = ::SetReadDevice(*(const TCHAR*)(QString("#").toUtf16()));
#else
    res = ::SetReadDevice(*(const TCHAR*)(QString("#").toUtf8()));
#endif
    if(res!=BS_SDK_ERROR_NO)
    {
        return false;
    }
  } else {
#if defined (WIN32)
    res = ::SetReadDevice(*(const TCHAR*)(imagePath.toUtf16()));
#else
    res = ::SetReadDevice(*(const TCHAR*)(imagePath.toUtf8()));
#endif
    if(res!=BS_SDK_ERROR_NO)
    {
        return false;
    }
  }

  QMediumItem *mediumItem = QMediumItem::create(mTreeWidget);
  mediumItem->CreateAndLoadChildren();
  bool dataTrackFound = false;
  for(int session = 0; session < mediumItem->childCount(); ++session) {
    QSessionItem *sessionItem = (QSessionItem *)mediumItem->child(session);
    for (int track = 0; track < sessionItem->childCount(); ++track) {
      QTrackItem *trackItem = (QTrackItem *)sessionItem->child(track);
      if (QDataItem::DataTrack == trackItem->GetType()) {
        if (!dataTrackFound) {
          mTreeWidget->SetDataTrackItem(trackItem);
          dataTrackFound = true;
          break;
        }
//        for (int fs = 0; fs < trackItem->childCount(); ++fs) {
//          QFileSysItem *fileSysItem = (QFileSysItem *)trackItem->child(fs);
//          for (int f = 0; f < fileSysItem->childCount(); ++f) {
//            QDataItem *item = (QDataItem *)fileSysItem->child(f);
//            if (QDataItem::Folder == item->GetType()) {
//              item->setIcon(0, mFolderIcon);
//            } else if (QDataItem::File == item->GetType()) {
//              item->setIcon(0, mFileIcon);
//            }
//          }
//        }
      }
    }
  }

  mTreeWidget->SetDataSize(mediumItem->GetDataSize());
  mTreeWidget->SetDataItemCount(mediumItem->GetDataItemCount());
  return (BS_SDK_ERROR_NO == res);
}
