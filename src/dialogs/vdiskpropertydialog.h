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

#ifndef VDISKPROPERTYDIALOG_H
#define VDISKPROPERTYDIALOG_H

#include <QDialog>
#include "qdiskitem.h"
#include "ui_vdiskpropertydialog.h"

namespace Ui {
class VDiskPropertyDialog;
}

class VDiskPropertyDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VDiskPropertyDialog(QWidget *parent, QDiskItem *item, bool existAudioTrack, bool existDataTrack);
  ~VDiskPropertyDialog();

  QString getDiskName() {return ui->editName->text();}

  QString getArranger() {return ui->editArranger->text();}
  QString getComposer() {return ui->editComposer->text();}
  QString getSongWriter() {return ui->editSongWriter->text();}
  QString getPerformer() {return ui->editPerformer->text();}
  QString getMessage() {return ui->editMessage->text();}
  QString getTitle() {return ui->editTitle->text();}
  QString getUPCEAN() {return ui->editUPCEAN->text();}
  QStringList *getFilterList();
  bool getByDate() {return ui->checkByDate->isChecked();}
  QString getDateFrom() {return ui->dateFrom->date().toString();}
  QString getDateTo() {return ui->dateTo->date().toString();}
  int getDateUsage() {return ui->dateUsage->checkState();}

  QDateTime getDateTimeCreation() {return ui->dateTimeCreation->dateTime();}
  QDateTime getDateTimeMdification() {return ui->dateTimeMdification->dateTime();}
  QDateTime getDateTimeExpiration() {return ui->dateTimeExpiration->dateTime();}
  QDateTime getDateTimeEffective() {return ui->dateTimeEffective->dateTime();}

private slots:
  void on_buttonAdd_clicked();

  void on_buttonDel_clicked();

  void on_checkByDate_clicked(bool checked);

  void on_buttonImportSystemFilter_clicked();

  void on_buttonResetFilter_clicked();
  void on_dateUsage_stateChanged(int arg1);

private:
  Ui::VDiskPropertyDialog *ui;
  QDiskItem *mDiskItem;
};

#endif // VDISKPROPERTYDIALOG_H
