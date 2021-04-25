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

#ifndef VCDDIALOG_H
#define VCDDIALOG_H

#include <QDialog>
#include "ui_vcddialog.h"
#include "qaudiotrackitem.h"

namespace Ui {
class VCDDialog;
}

class VCDDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VCDDialog(QWidget *parent, QAudioTrackItem *audiotrack);
  ~VCDDialog();

  QString getArranger() {return ui->editArranger->text();}
  QString getComposer() {return ui->editComposer->text();}
  QString getSongWriter() {return ui->editSongWriter->text();}
  QString getPerformer() {return ui->editPerformer->text();}
  QString getMessage() {return ui->editMessage->text();}
  QString getTitle() {return ui->editTitle->text();}
  QString getUPCEAN() {return ui->editUPCEAN->text();}
  int getPause() {return ui->editPause->value();}
  bool getPauseInFrames() {return ui->pauseInFrames->isChecked();}
private slots:
  void on_pushButtonAdd_clicked();
  void on_pushButtonDelete_clicked();

private:
  Ui::VCDDialog *ui;
  QAudioTrackItem *mAudioTrack;
};

#endif // VCDDIALOG_H
