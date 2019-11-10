#ifndef VPROPERTYDIALOG_H
#define VPROPERTYDIALOG_H

#include <QDialog>
#include "QDataItem.h"
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
