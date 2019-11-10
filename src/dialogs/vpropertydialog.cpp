#include "vpropertydialog.h"
#include "ui_vpropertydialog.h"
#include <QFileInfo>
#include <QDateTime>
#include "settingspages.h"
#include "../utils/utils_common.h"

void ChecksumThread::run()
{
  QByteArray sum = CalcChecksum(mPath);
  emit calcDone(sum);
}

VPropertyDialog::VPropertyDialog(QWidget *parent, QDataItem *qItem) :
  QDialog(parent),
  ui(new Ui::VPropertyDialog),
  mDidSync(false)
{
  ui->setupUi(this);

  mItem = qItem;
  // Get read-only info
  ui->editName->setText(qItem->GetName());
  QFont f = ui->labelSource->font();
  QFontMetrics fm = QFontMetrics(f);
  int max_w = ui->labelSource->width();
  QString elided_text = fm.elidedText(qItem->GetFullPath(), Qt::ElideMiddle, max_w);
  ui->labelSource->setText(elided_text); //need change

  f = ui->labelPath->font();
  fm = QFontMetrics(f);
  max_w = ui->labelPath->width();
  elided_text = fm.elidedText(qItem->GetDiskPath(), Qt::ElideMiddle, max_w);
  ui->labelPath->setText(elided_text);

  //ui->editContent->setText(QString::number(qItem->GetDataItemCount()));
  ui->labelContent->setText(tr("%1 files, %2 folders")
                           .arg(qItem->GetDataItemCount()).arg(qItem->GetDataNodeCount()));
  ui->labelSize->setText(humanReadableSize(qItem->GetDataSize())
                        + ", " + humanReadableSector(qItem->GetDataSize()));

  //Get system info
  QFileInfo entry(qItem->GetFullPath());
  if (entry.exists()) {
    // check whether information is synchronized
    QDateTime sysDateCreated = entry.birthTime();
    QDateTime sysDateModified = entry.lastModified();
    qint64 sysSize = entry.size();
    // ignore MSecs, so we /1000
    //if (sysDateCreated != qItem->GetDateCreated()) {
    //qint64 t1 = sysDateCreated.toMSecsSinceEpoch();
    //qint64 t2 = qItem->GetDateCreated().toMSecsSinceEpoch();
    if (sysDateCreated.toMSecsSinceEpoch()/1000 != qItem->GetDateCreated().toMSecsSinceEpoch()/1000) {
      ui->imageWarningDateCreated->setPixmap(QPixmap(":/images/dialog_warning.png"));
    } else {
      ui->imageWarningDateCreated->hide();
    }
    //if (sysDateModified != qItem->GetDateModified()) {
    if (sysDateModified.toMSecsSinceEpoch()/1000 != qItem->GetDateModified().toMSecsSinceEpoch()/1000) {
      ui->imageWarningDateModified->setPixmap(QPixmap(":/images/dialog_warning.png"));
    } else {
      ui->imageWarningDateModified->hide();
    }
    if (QDataItem::File == qItem->GetType() && sysSize != qItem->GetDataSize()) {
      ui->imageWarningSize->setPixmap(QPixmap(":/images/dialog_warning.png"));
    } else {
      ui->imageWarningSize->hide();
    }
  } else {
    //ui->dateTimeCreated->hide();
    //ui->dateTimeModified->hide();
    ui->imageWarningDateCreated->hide();
    ui->imageWarningDateModified->hide();
    ui->imageWarningSize->hide();
    ui->buttonSync->setEnabled(false);
  }
  ui->dateTimeAdded->setDateTime(qItem->GetDateAdded());
  ui->dateTimeCreated->setDateTime(qItem->GetDateCreated());
  ui->dateTimeModified->setDateTime(qItem->GetDateModified());
  ui->labelDateCreatedError->setStyleSheet("QLabel { color : red; }");
  ui->labelDateChangedError->setStyleSheet("QLabel { color : red; }");
  ui->labelDateCreatedError->hide();
  ui->labelDateChangedError->hide();

  if (QDataItem::File == qItem->GetType()
      && ConfigurationPage::mSettings.value("CreateAndShowChecksum", true).toBool()) {
    int64 size = ConfigurationPage::mSettings.value("NoChecksumFileSize", tr("32")).toInt();
    if (qItem->GetDataSize() <= size*1024*1024) {
      QByteArray sum = qItem->GetChecksum();
      if (sum == "") {
        QString strLinuxFullPath = qItem->GetFullPath(); //Fix for Linux because Linux is too strict
        mChecksumThread = new ChecksumThread(strLinuxFullPath);
        connect(mChecksumThread, SIGNAL(calcDone(QByteArray)), this, SLOT(on_checksum_calc_done(QByteArray)));
        mChecksumThread->start();
        sum = tr("calculating...").toStdString().c_str();
      }
      ui->labelChecksum->setText(sum);
    } else {
      ui->labelChecksum->setText(tr("file size > 'No checksum filesize' settings value"));
    }
  } else {
    ui->headChecksum->hide();
    ui->labelChecksum->hide();
  }
  //ui->labelComment->setText(qItem->GetComment());
  ui->editComment->setText(qItem->GetComment());

  // Get writable info
  ui->checkHidden->setChecked(qItem->GetHidden());
  ui->checkHiddenExt->setChecked(qItem->GetHiddenExt());
  ui->comboPriority->setCurrentIndex(qItem->GetPriority());

  ui->checkVideoStream->setEnabled(!entry.suffix().compare("m2ts"));
}

VPropertyDialog::~VPropertyDialog()
{
  delete ui;
}

void VPropertyDialog::on_buttonSync_clicked()
{
  QFileInfo entry(mItem->GetFullPath());
  ui->dateTimeCreated->setDateTime(entry.birthTime());
  ui->dateTimeModified->setDateTime(entry.lastModified());
  ui->imageWarningDateCreated->hide();
  ui->imageWarningDateModified->hide();

  if (QDataItem::File == mItem->GetType()) {
    ui->imageWarningSize->hide();
    ui->labelSize->setText(humanReadableSize(entry.size())
                          + ", " + humanReadableSector(entry.size()));
  }

  mDidSync = true;
}

void VPropertyDialog::on_checksum_calc_done(QByteArray checksum)
{
  ui->labelChecksum->setText(checksum);
  mItem->SetChecksum(checksum);
}
