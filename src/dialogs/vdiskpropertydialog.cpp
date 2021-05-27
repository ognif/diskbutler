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

#include "vdiskpropertydialog.h"
#include "utils_common.h"
#include "settingspages.h"

VDiskPropertyDialog::VDiskPropertyDialog(QWidget *parent, QDiskItem *item, bool existAudioTrack, bool) :
    QDialog(parent),
    ui(new Ui::VDiskPropertyDialog)
{
    ui->setupUi(this);
    setWindowTitle("Disk Properties");

    QFont font;
    font.setFamily("Font Awesome 5 Free Solid");
    font.setPixelSize(16);

    mDiskItem = item;

    ui->buttonAdd->setFont(font);
    ui->buttonDel->setFont(font);
    ui->buttonAdd->setText(QChar(0xf055));
    ui->buttonDel->setText(QChar(0xf056));

    ui->editName->setText(item->GetName());
    ui->labelSize->setText(humanReadableSize(item->GetDataSize(),nullptr));
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


    ui->dateUsage->setCheckState(item->getIsoExUseDates()==true?Qt::Checked:Qt::Unchecked);
    ui->dateTimeCreation->setEnabled(item->getIsoExUseDates());
    ui->dateTimeMdification->setEnabled(item->getIsoExUseDates());
    ui->dateTimeExpiration->setEnabled(item->getIsoExUseDates());
    ui->dateTimeEffective->setEnabled(item->getIsoExUseDates());

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
        QListWidgetItem *item = new QListWidgetItem(ConfigurationPage::mSettings.value("filteritem" + QString::number(i), "").toString());
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

void VDiskPropertyDialog::on_dateUsage_stateChanged(int arg1)
{
    ui->dateTimeCreation->setEnabled(arg1==2?true:false);
    ui->dateTimeMdification->setEnabled(arg1==2?true:false);
    ui->dateTimeExpiration->setEnabled(arg1==2?true:false);
    ui->dateTimeEffective->setEnabled(arg1==2?true:false);
}

void VDiskPropertyDialog::on_checkByDate_stateChanged(int arg1)
{

}
