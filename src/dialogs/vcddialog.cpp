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

#include "vcddialog.h"
#include "utils_common.h"

VCDDialog::VCDDialog(QWidget *parent, QAudioTrackItem *audiotrack) :
  QDialog(parent),
  ui(new Ui::VCDDialog)
{
  ui->setupUi(this);
  ui->editArranger->setText(audiotrack->getArranger());
  ui->editComposer->setText(audiotrack->getComposer());
  ui->editSongWriter->setText(audiotrack->getSongWriter());
  ui->editPerformer->setText(audiotrack->getPerformer());
  ui->editMessage->setText(audiotrack->getMessage());
  ui->editTitle->setText(audiotrack->getTitle());
  ui->editUPCEAN->setText(audiotrack->getUPCEAN());
  ui->editPause->setValue(audiotrack->getPause());
  ui->pauseInFrames->setChecked(audiotrack->getPauseInFrames());
  mAudioTrack = audiotrack;
  std::vector<int32> indexes = mAudioTrack->getIndexes();
  for (int i = 0; i < indexes.size(); i++) {
    ui->listIndexes->addItem(MSFInt32ToStr(indexes[i]));
  }
}

VCDDialog::~VCDDialog()
{
  delete ui;
}

void VCDDialog::on_pushButtonAdd_clicked()
{
  int minutes = ui->spinBoxMinutes->value();
  int seconds = ui->spinBoxSeconds->value();
  int frames = ui->spinBoxFrames->value();
  mAudioTrack->addIndex(minutes, seconds, frames);
  ui->listIndexes->addItem(MSFToStr(minutes, seconds, frames));
}

void VCDDialog::on_pushButtonDelete_clicked()
{
  if (ui->listIndexes->selectedItems().size() > 0) {
    QModelIndexList indexes = ui->listIndexes->selectionModel()->selectedIndexes();
    foreach (QModelIndex index, indexes) {
      mAudioTrack->removeIndex(index.row());
    }
    qDeleteAll(ui->listIndexes->selectedItems());
  }
}
