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

private:
  Ui::VDiskPropertyDialog *ui;
  QDiskItem *mDiskItem;
};

#endif // VDISKPROPERTYDIALOG_H
