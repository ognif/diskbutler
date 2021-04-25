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

#ifndef VPROPERTYDIALOG_H
#define VPROPERTYDIALOG_H

#include <QDialog>
#include "qdataitem.h"
#include "ui_vpropertydialog.h"
#include <QThread>

namespace Ui {
class VPropertyDialog;
}

class ChecksumThread : public QThread
{
  Q_OBJECT
public:
  ChecksumThread(QString &path) {mPath = path;}
signals:
  void calcDone(QByteArray);
protected:
  void run();
  QString mPath;
};

class VPropertyDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VPropertyDialog(QWidget *parent, QDataItem *qItem);
  ~VPropertyDialog();

  QString getName() {return ui->editName->text();}
  QString getComment() {return ui->editComment->text();}
  bool getHidden() {return ui->checkHidden->isChecked();}
  bool getHiddenExt() {return ui->checkHiddenExt->isChecked();}
  int getPriority() {return ui->comboPriority->currentIndex();}
  QDateTime getDateAdded() {return ui->dateTimeAdded->dateTime();}
  QDateTime getDateCreated() {return ui->dateTimeCreated->dateTime();}
  QDateTime getDateModified() {return ui->dateTimeModified->dateTime();}
  bool didSync() {return mDidSync;}

private slots:
  void on_buttonSync_clicked();
  void on_checksum_calc_done(QByteArray checksum);

private:
  Ui::VPropertyDialog *ui;
  QDataItem *mItem;
  bool mDidSync;
  ChecksumThread *mChecksumThread;
};

#endif // VPROPERTYDIALOG_H
