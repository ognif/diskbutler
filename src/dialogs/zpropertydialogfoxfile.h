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

#ifndef ZPROPERTYDIALOGFOXFILE_H
#define ZPROPERTYDIALOGFOXFILE_H

#include <QDialog>
#include "nodes/QDataItem.h"
#include "zallocation_table_model.h"
#include "zinfotablemodel.h"

namespace Ui {
class ZPropertyDialogFoxFile;
}

class ZPropertyDialogFoxFile : public QDialog
{
  Q_OBJECT

public:
  explicit ZPropertyDialogFoxFile(QWidget *parent, QDataItem *qItem);
  ~ZPropertyDialogFoxFile();

private:
  void addEntry(QString location, QString length);
  void addEntryInfo(QString location, QString length);

private:
  Ui::ZPropertyDialogFoxFile *ui;
  QDataItem *mItem;
  ZAllocationTableModel *mTableModel;
  ZInfoTableModel *mInfoModel;
  void fillFiles();
  void fillFolders();
  void fillDisk();
  void fillSession();
  void fillTrack();
  void fillISO();
  void fillUDF();
  void fillAudioTrack();
};

#endif // ZPROPERTYDIALOGFOXFILE_H
