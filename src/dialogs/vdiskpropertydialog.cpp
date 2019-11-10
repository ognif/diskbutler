#include "vdiskpropertydialog.h"
#include "utils_common.h"
#include "settingspages.h"

VDiskPropertyDialog::VDiskPropertyDialog(QWidget *parent, QDiskItem *item, bool existAudioTrack, bool existDataTrack) :
  QDialog(parent),
  ui(new Ui::VDiskPropertyDialog)
{
  ui->setupUi(this);
  setWindowTitle("Disk Properties");

  QFont font;
  font.setFamily("Font Awesome 5 Free");
  font.setPixelSize(16);

  mDiskItem = item;

  ui->buttonAdd->setFont(font);
  ui->buttonDel->setFont(font);
  ui->buttonAdd->setText("\uf055");
  ui->buttonDel->setText("\uf056");

  ui->editName->setText(item->GetName());
  ui->labelSize->setText(humanReadableSize(item->GetDataSize()));
  ui->labelFiles->setText(QString::number(item->GetDataItemCount()));

  ui->dateTimeCreation->setDateTime(item->getDiskDateCreation());
  ui->dateTimeMdification->setDateTime(item->getDiskDateMdification());
  ui->dateTimeExpiration->setDateTime(item->getDiskDateExpiration());
  ui->dateTimeEffective->setDateTime(item->getDiskDateEffective());

  ui->editArranger->setText(item->getArranger());
  ui->editComposer->setText(item->getComposer());
  ui->editSongWriter->setText(item->getSongWriter());
  ui->editPerformer->setText(item->getPerformer());
  ui->editMessage->setText(item->getMessage());
  ui->editTitle->setText(item->getTitle());
  ui->editUPCEAN->setText(item->getUPCEAN());

  on_buttonResetFilter_clicked();

  if (!existAudioTrack) {
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabCDText));
  }
}

VDiskPropertyDialog::~VDiskPropertyDialog()
{
  delete ui;
}

void VDiskPropertyDialog::on_buttonAdd_clicked()
{
  //QListWidgetItem *item = new QListWidgetItem((ui->listWidget);
  QListWidgetItem *item = new QListWidgetItem("");
  ui->listWidget->insertItem(0,item);
  item->setFlags(item->flags() | Qt::ItemIsEditable);
  ui->listWidget->setCurrentItem(item);
  ui->listWidget->editItem(item);
}

void VDiskPropertyDialog::on_buttonDel_clicked()
{
  ui->listWidget->takeItem(ui->listWidget->currentRow());
}

QStringList *VDiskPropertyDialog::getFilterList()
{
  QStringList *list = new QStringList();
  for (int i=0; i<ui->listWidget->count(); i++) {
    list->insert(i, ui->listWidget->item(i)->text());
  }
  return list;
}

void VDiskPropertyDialog::on_checkByDate_clicked(bool checked)
{
  ui->dateFrom->setEnabled(checked);
  ui->dateTo->setEnabled(checked);

  ui->listWidget->setEnabled(!checked);
  ui->buttonAdd->setEnabled(!checked);
  ui->buttonDel->setEnabled(!checked);
}

void VDiskPropertyDialog::on_buttonImportSystemFilter_clicked()
{
  ui->checkByDate->setChecked(ConfigurationPage::mSettings.value("bydate", false).toBool());
  ui->dateFrom->setDate(ConfigurationPage::mSettings.value("datefrom").toDate());
  ui->dateTo->setDate(ConfigurationPage::mSettings.value("dateto").toDate());

  int filterCount = ConfigurationPage::mSettings.value("filtercount", 0).toInt();
  ui->listWidget->clear();
  for (int i=0; i<filterCount; i++) {
    QListWidgetItem *item = new QListWidgetItem(ConfigurationPage::mSettings.value("filteritem"+i, "").toString());
    ui->listWidget->addItem(item);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
  }

  on_checkByDate_clicked(ui->checkByDate->isChecked());
}



void VDiskPropertyDialog::on_buttonResetFilter_clicked()
{
  ui->checkByDate->setChecked(mDiskItem->getByDate());
  ui->dateFrom->setDate(QDate::fromString(mDiskItem->getDateFrom()));
  ui->dateTo->setDate(QDate::fromString(mDiskItem->getDateTo()));

  QStringList *list = mDiskItem->getFilterList();
  ui->listWidget->clear();
  for (int i=0; i<list->count(); i++) {
    QListWidgetItem *listItem = new QListWidgetItem(ui->listWidget);
    listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
    listItem->setText(list->at(i));
  }

  on_checkByDate_clicked(ui->checkByDate->isChecked());
}
