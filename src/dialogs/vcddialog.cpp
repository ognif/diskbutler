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
