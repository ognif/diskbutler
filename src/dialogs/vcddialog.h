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
