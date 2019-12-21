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

#include "zpropertydialogfoxfile.h"
#include "ui_zpropertydialogfoxfile.h"
#include "utils_common.h"
#include "qtrackitem.h"
#include "qfilesysitem.h"
#include "qfilesys_udfitem.h"
#include "qfilesys_iso9660item.h"
#include "qmediumitem.h"

ZPropertyDialogFoxFile::ZPropertyDialogFoxFile(QWidget *parent, QDataItem *qItem) :
  QDialog(parent),
  ui(new Ui::ZPropertyDialogFoxFile)
{
  ui->setupUi(this);

  mItem = qItem;

  if(mItem->GetType() != QDataItem::AudioTrack){

      QDataItem *parent_item = dynamic_cast<QDataItem*>(qItem->parent());
      QDataItem *current_item = qItem;

      while (parent_item != NULL && QDataItem::DataTrack != parent_item->GetType()) {
        current_item = parent_item;
        parent_item = dynamic_cast<QDataItem*>(parent_item->parent());
      }


      if(parent_item != NULL){
          if (QDataItem::DataTrack == parent_item->GetType()) {
            QTrackItem *track = dynamic_cast<QTrackItem*>(parent_item);
            STrackInfo info = track->GetTrackInfo();

            //ui->label_tracknumber->setText(QString("%1").arg(info.nTrackNumber));
            if (info.nFormat != BS_TF_AUDIO) {
              // data track
              QFileSysItem *fs = dynamic_cast<QFileSysItem*>(current_item);
              //ui->label_fstype->setText(fs->GetFSTypeStr());
            }
          }
      }

  }


  mInfoModel = new ZInfoTableModel(this);
  ui->infoView->setModel(mInfoModel);
  ui->infoView->setSortingEnabled(false);
  ui->infoView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->infoView->horizontalHeader()->setDefaultSectionSize(120);
  ui->infoView->horizontalHeader()->setStretchLastSection(true);
  ui->infoView->verticalHeader()->hide();
  ui->infoView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->infoView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->infoView->verticalHeader()->setDefaultSectionSize(21);

  if(mItem->GetType()!=QDataItem::File){
      ui->groupBoxTable->setVisible(false);
      ui->tableView->setVisible(false);
      ui->infoView->resize(ui->infoView->width(),341);
      ui->groupBoxGeneral->resize(ui->groupBoxGeneral->width(),371);
  }

  if(mItem->GetType()==QDataItem::File){
     fillFiles();
  }else if(mItem->GetType()==QDataItem::Folder){
      if(mItem->isISOFileSystem()==true){
        fillISO();
      }else if(mItem->isUDFFileSystem()==true){
        fillUDF();
      }else{
        fillFolders();
      }
     return;
  }else if(mItem->GetType()==QDataItem::Disk){
      fillDisk();
      return;
  }else if(mItem->GetType()==QDataItem::Session){
      fillSession();
      return;
  }else if(mItem->GetType()==QDataItem::DataTrack){
      fillTrack();
      return;
  }else if(mItem->GetType()==QDataItem::AudioTrack){
      fillAudioTrack();
      return;
  }

  mTableModel = new ZAllocationTableModel(this);
  ui->tableView->setModel(mTableModel);
  ui->tableView->setSortingEnabled(true);
  ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableView->horizontalHeader()->setDefaultSectionSize(120);
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  ui->tableView->verticalHeader()->hide();
  ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

  for (uint i = 0; i < mItem->GetExtentLocation()->size(); i++) {
    addEntry(QString("%1").arg(mItem->GetExtentLocation()->at(i)), QString("%1").arg(mItem->GetExtentLength()->at(i)));
  }
}

ZPropertyDialogFoxFile::~ZPropertyDialogFoxFile()
{
  delete ui;
}

void ZPropertyDialogFoxFile::addEntryInfo(QString location, QString length)
{
  QList<QPair<QString, QString> >list = mInfoModel->getList();
  QPair<QString, QString> pair(location, length);

  if (!list.contains(pair)) {
    mInfoModel->insertRows(0, 1, QModelIndex());

    QModelIndex index = mInfoModel->index(0, 0, QModelIndex());
    mInfoModel->setData(index, location, Qt::EditRole);
    index = mInfoModel->index(0, 1, QModelIndex());
    mInfoModel->setData(index, length, Qt::EditRole);
  }
}

void ZPropertyDialogFoxFile::addEntry(QString location, QString length)
{
  QList<QPair<QString, QString> >list = mTableModel->getList();
  QPair<QString, QString> pair(location, length);

  if (!list.contains(pair)) {
    mTableModel->insertRows(0, 1, QModelIndex());

    QModelIndex index = mTableModel->index(0, 0, QModelIndex());
    mTableModel->setData(index, location, Qt::EditRole);
    index = mTableModel->index(0, 1, QModelIndex());
    mTableModel->setData(index, length, Qt::EditRole);
  }
}

void ZPropertyDialogFoxFile::fillFiles()
{
    //ui->tableWidget
    addEntryInfo(tr("Modified Date:"), mItem->GetDateModified().toString());
    addEntryInfo(tr("Create Date:"), mItem->GetDateCreated().toString());
    addEntryInfo(tr("Attributes:"), mItem->createAttributeString());    
    addEntryInfo(tr("Path:"), mItem->getPath());
    addEntryInfo(tr("Size(Blocks):"), humanReadableSector(mItem->GetDataSize()));
    addEntryInfo(tr("Size(Bytes):"), QString::number(mItem->GetDataSize()));
    addEntryInfo(tr("Adress(LBA):"), mItem->GetLBA());
    addEntryInfo(tr("File name:"), mItem->GetName());
    addEntryInfo(tr("Type:"), tr("File"));

}

void ZPropertyDialogFoxFile::fillFolders()
{
    //ui->tableWidget
    addEntryInfo(tr("Modified Date:"), mItem->GetDateModified().toString());
    addEntryInfo(tr("Create Date:"), mItem->GetDateCreated().toString());
    addEntryInfo(tr("Attributes:"), mItem->createAttributeString());    
    addEntryInfo(tr("Path:"), mItem->getPath());
    addEntryInfo(tr("Folder count:"), QString::number(mItem->GetDataNodeCount()));
    addEntryInfo(tr("File count:"), QString::number(mItem->GetDataItemCount()));
    addEntryInfo(tr("Size(Blocks):"), humanReadableSector(mItem->GetDataSize()));
    addEntryInfo(tr("Size(Bytes):"), QString::number(mItem->GetDataSize()));
    addEntryInfo(tr("Adress(LBA):"), mItem->GetLBA());
    addEntryInfo(tr("Folder name:"), mItem->GetName());
    addEntryInfo(tr("Type:"), tr("Folder"));
}

void ZPropertyDialogFoxFile::fillDisk()
{
    QMediumItem *medium = dynamic_cast<QMediumItem*>(mItem);
    //ui->tableWidget
    if(medium->getExtendedMediumType()==BS_EMT_CD_AUDIO){
        addEntryInfo(tr("Message:"), medium->getMessage());
        addEntryInfo(tr("Arranger:"), medium->getArranger());
        addEntryInfo(tr("Composer:"), medium->getComposer());
        addEntryInfo(tr("Song Writer:"), medium->getSongWriter());
        addEntryInfo(tr("Artist:"), medium->getPerformer());
        addEntryInfo(tr("Title:"), medium->getTitle());
    }

    addEntryInfo(tr("UPC/EAN:"), mItem->getUPCCode());
    addEntryInfo(tr("Tracks:"), QString::number(mItem->getTrackCount()));
    addEntryInfo(tr("Sessions:"), QString::number(mItem->getSessionCount()));
    addEntryInfo(tr("Free:"), QString::number(mItem->getMediumFreeSize()));
    addEntryInfo(tr("Used:"), QString::number(mItem->getMediumUsedSize()));
    addEntryInfo(tr("Size:"), QString::number(mItem->getMediumSize()));
    addEntryInfo(tr("Size(Blocks):"), QString::number(mItem->getMediumSize()/2048));
    addEntryInfo(tr("Status:"), mItem->getMediumStatus());
    addEntryInfo(tr("Medium Type:"), mItem->getMediumType());
    addEntryInfo(tr("Type:"), tr("Medium"));

}

void ZPropertyDialogFoxFile::fillSession()
{

    addEntryInfo(tr("Last Session:"), QString::number(mItem->getLastSession()));
    addEntryInfo(tr("Last Track:"), QString::number(mItem->getLastTrack()));
    addEntryInfo(tr("First Track:"), QString::number(mItem->getFirstTrack()));
    addEntryInfo(tr("Size(Blocks):"), QString::number(mItem->getSessionSize()));
    addEntryInfo(tr("Size:"), QString::number(mItem->getSessionSize()*2048));
    addEntryInfo(tr("Address:"), QString::number(mItem->getStartLBA()));
    addEntryInfo(tr("Type:"), tr("Session"));
}

void ZPropertyDialogFoxFile::fillTrack()
{
    addEntryInfo(tr("Session:"), QString::number(mItem->getSessionNumber()));
    addEntryInfo(tr("Size:"), QString::number(mItem->getTrackSize()));
    addEntryInfo(tr("Address:"), QString::number(mItem->getStartLBA()));
    addEntryInfo(tr("Mode:"), mItem->GetModeStr());
    addEntryInfo(tr("Index:"), QString::number(mItem->getTrackIndex()));
    addEntryInfo(tr("Type:"), tr("Track"));
}

void ZPropertyDialogFoxFile::fillISO()
{
    QFileSysISO9660Item *ISONode = dynamic_cast<QFileSysISO9660Item*>(mItem);
    SISOVolumeInfo info = ISONode->GetISOInfo();

    QString strEffectiveTime = QString("%1.%2.%3 %4:%5:%6").arg(QString::number(info.sInfoEx.ISOEffectiveDateTime.nDay),
                                                      QString::number(info.sInfoEx.ISOEffectiveDateTime.nMonth),
                                                      QString::number(info.sInfoEx.ISOEffectiveDateTime.nYear+1900),
                                                      QString::number(info.sInfoEx.ISOEffectiveDateTime.nHour),
                                                      QString::number(info.sInfoEx.ISOEffectiveDateTime.nMinute),
                                                      QString::number(info.sInfoEx.ISOEffectiveDateTime.nSecond));

    QString strCreationTime = QString("%1.%2.%3 %4:%5:%6").arg(QString::number(info.sInfoEx.ISOCreationDateTime.nDay),
                                                      QString::number(info.sInfoEx.ISOCreationDateTime.nMonth),
                                                      QString::number(info.sInfoEx.ISOCreationDateTime.nYear+1900),
                                                      QString::number(info.sInfoEx.ISOCreationDateTime.nHour),
                                                      QString::number(info.sInfoEx.ISOCreationDateTime.nMinute),
                                                      QString::number(info.sInfoEx.ISOCreationDateTime.nSecond));

    QString strModificationTime = QString("%1.%2.%3 %4:%5:%6").arg(QString::number(info.sInfoEx.ISOModificationDateTime.nDay),
                                                      QString::number(info.sInfoEx.ISOModificationDateTime.nMonth),
                                                      QString::number(info.sInfoEx.ISOModificationDateTime.nYear+1900),
                                                      QString::number(info.sInfoEx.ISOModificationDateTime.nHour),
                                                      QString::number(info.sInfoEx.ISOModificationDateTime.nMinute),
                                                      QString::number(info.sInfoEx.ISOModificationDateTime.nSecond));

    QString strExpirationTime = QString("%1.%2.%3 %4:%5:%6").arg(QString::number(info.sInfoEx.ISOExpirationDateTime.nDay),
                                                      QString::number(info.sInfoEx.ISOExpirationDateTime.nMonth),
                                                      QString::number(info.sInfoEx.ISOExpirationDateTime.nYear+1900),
                                                      QString::number(info.sInfoEx.ISOExpirationDateTime.nHour),
                                                      QString::number(info.sInfoEx.ISOExpirationDateTime.nMinute),
                                                      QString::number(info.sInfoEx.ISOExpirationDateTime.nSecond));

    addEntryInfo(tr("Effective Time:"), strEffectiveTime);
    addEntryInfo(tr("Expiration Time:"), strExpirationTime);
    addEntryInfo(tr("Modification Time:"), strModificationTime);
    addEntryInfo(tr("Creation Time:"), strCreationTime);

#if defined (WIN32)
    addEntryInfo(tr("System Identifier:"), QString::fromUtf16((const ushort*)info.sInfoEx.ISOSystemIdentifier));
    addEntryInfo(tr("Volume Set Identifier:"), QString::fromUtf16((const ushort*)info.sInfoEx.ISOSetIdentifier));
    addEntryInfo(tr("Publisher Identifier:"), QString::fromUtf16((const ushort*)info.sInfoEx.ISOPublisherIdentifier));
    addEntryInfo(tr("Data Preparer Identifier:"), QString::fromUtf16((const ushort*)info.sInfoEx.ISODataPreparerIdentifier));
    addEntryInfo(tr("Copyright Identifier:"), QString::fromUtf16((const ushort*)info.sInfoEx.ISOCopyrightFileIdentifier));
    addEntryInfo(tr("Biblio Identifier:"), QString::fromUtf16((const ushort*)info.sInfoEx.ISOBiblioIdentifier));
    addEntryInfo(tr("Application Identifier:"), QString::fromUtf16((const ushort*)info.sInfoEx.ISOApplicationIdentifier));
    addEntryInfo(tr("Abstract Identifier:"), QString::fromUtf16((const ushort*)info.sInfoEx.ISOAbstractFileIdentifier));
#else
    addEntryInfo(tr("System Identifier:"), QString::fromUtf8(info.sInfoEx.ISOSystemIdentifier));
    addEntryInfo(tr("Volume Set Identifier:"), QString::fromUtf8(info.sInfoEx.ISOSetIdentifier));
    addEntryInfo(tr("Publisher Identifier:"), QString::fromUtf8(info.sInfoEx.ISOPublisherIdentifier));
    addEntryInfo(tr("Data Preparer Identifier:"), QString::fromUtf8(info.sInfoEx.ISODataPreparerIdentifier));
    addEntryInfo(tr("Copyright Identifier:"), QString::fromUtf8(info.sInfoEx.ISOCopyrightFileIdentifier));
    addEntryInfo(tr("Biblio Identifier:"), QString::fromUtf8(info.sInfoEx.ISOBiblioIdentifier));
    addEntryInfo(tr("Application Identifier:"), QString::fromUtf8(info.sInfoEx.ISOApplicationIdentifier));
    addEntryInfo(tr("Abstract Identifier:"), QString::fromUtf8(info.sInfoEx.ISOAbstractFileIdentifier));
#endif



    addEntryInfo(tr("ISO Level:"), QString::number(info.sInfoEx.ISOLevel));
    addEntryInfo(tr("Path Table Size:"), QString::number(info.nPathTableSize));
    addEntryInfo(tr("Path Table Address:"), QString::number(info.nPathTableAddress));
    addEntryInfo(tr("Root Address:"), QString::number(info.nRootAddress));
    addEntryInfo(tr("Volume Size:"), QString::number(info.nVolumeSize));
    addEntryInfo(tr("Volume Descriptor:"), QString::number(info.nVolumeDescriptorAddress));
    addEntryInfo(tr("Volume Label:"), QString::fromUtf16((const ushort*)info.chVolumeLabel));
    addEntryInfo(tr("Type:"), tr("ISO9660 Filesystem"));
}

void ZPropertyDialogFoxFile::fillUDF()
{
    QFileSysUDFItem *UDFNode = dynamic_cast<QFileSysUDFItem*>(mItem);
    SUDFVolumeInfo info = UDFNode->GetUDFInfo();

    addEntryInfo(tr("VAT Address:"), QString::number(info.nVATAddress));
    addEntryInfo(tr("Sparing Address:"), QString::number(info.nSparingAddress));
    addEntryInfo(tr("Metadata Address:"), QString::number(info.nMetadataAddress));
    addEntryInfo(tr("FSD Address:"), QString::number(info.nFSDAddress));
    addEntryInfo(tr("PVD Address:"), QString::number(info.nPVDAddress));
    addEntryInfo(tr("LVD Address:"), QString::number(info.nLVDAddress));
    addEntryInfo(tr("RVDS Address:"), QString::number(info.nRVDSAddress));
    addEntryInfo(tr("MVDS Address:"), QString::number(info.nMVDSAddress));
    addEntryInfo(tr("Partition Size:"), QString::number(info.nPartitionLength));
    addEntryInfo(tr("Partition Address:"), QString::number(info.nPartitionAddress));
    addEntryInfo(tr("Partition Type:"), mItem->getUDFPartionType());
    addEntryInfo(tr("Root(E)FE:"), QString::number(info.nRootFEAddress));
    addEntryInfo(tr("Root Address:"), QString::number(info.nRootAddress));
    addEntryInfo(tr("FileCount:"), QString::number(info.nFileCount));
    addEntryInfo(tr("Folder Count:"), QString::number(info.nDirCount));
    addEntryInfo(tr("UDF Version:"), mItem->getUDFTypeString());

#if defined (WIN32)
    addEntryInfo(tr("Preparer:"), QString::fromUtf16((const ushort*)info.chPreparer));
    addEntryInfo(tr("Volume Label:"), QString::fromUtf16((const ushort*)info.chVolumeLabel));
#else
    addEntryInfo(tr("Preparer:"), QString::fromUtf8(info.chPreparer));
    addEntryInfo(tr("Volume Label:"), QString::fromUtf8(info.chVolumeLabel));
#endif
    addEntryInfo(tr("Type:"), tr("UDF Filesystem"));

}

void ZPropertyDialogFoxFile::fillAudioTrack()
{
    QTrackItem *track = dynamic_cast<QTrackItem*>(mItem);
    STrackInfo info = track->GetTrackInfo();

    addEntryInfo(tr("Message:"), track->getMessage());
    addEntryInfo(tr("Arranger:"), track->getArranger());
    addEntryInfo(tr("Composer:"), track->getComposer());
    addEntryInfo(tr("Song Writer:"), track->getSongWriter());
    addEntryInfo(tr("Artist:"), track->getPerformer());
    addEntryInfo(tr("Title:"), track->getTitle());
    addEntryInfo(tr("ISRC:"), track->getISRC());
    addEntryInfo(tr("Session:"), QString::number(info.nSessionNumber));
    addEntryInfo(tr("Size:"), QString::number(info.nSize));
    addEntryInfo(tr("Address:"), QString::number(info.nStartLBA));
    addEntryInfo(tr("Index:"), QString::number(info.nTrackNumber));
    addEntryInfo(tr("Type:"), tr("AudioTrack"));
}
