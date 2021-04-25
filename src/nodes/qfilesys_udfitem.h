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
