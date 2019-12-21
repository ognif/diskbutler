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

/*
 * Todo: Internal Drag & Drop inside the TreeWidegt
 */

#include "CommonTreeWidget.h"
#include "QDataItem.h"
#include "qdiskitem.h"
#include "qaudiotrackitem.h"
#include <QMessageBox>
#include <QAbstractButton>
#include <QDir>
#include <QUrl>
#include <QDropEvent>
#include <QMenu>
#include <QFileDialog>
#include "settingspages.h"
#include "audiotaginfo.h"

#include "qsessionitem.h"
#include "qtrackitem.h"
#include "qmediumitem.h"
#include "utils_common.h"
#include "vcddialog.h"
#include "vpropertydialog.h"
#include "vdiskpropertydialog.h"
#include "zreplacefiledialog.h"
#include "zplaylistloader.h"
#include <vector>
#include <QDebug>

const QString CommonTreeWidget::KStrISO = ("ISO");
const QString CommonTreeWidget::KStrUDF = ("UDF");

void ThreadAddTree::run() {

    qint64 size = 0;
    int item_count = 0;
    int node_count = 0;
    QDataItem *item;
    bool sort = false;

    if (QDataItem::FixedFolder == mParent->GetType()) {
        sort = true;
    }

    item = mTree->addTree(mParent, mPath, size, item_count, node_count, sort);

    if (item != nullptr && sort)
        mTree->changeFolderPos(mParent, item);

    emit completed(mParent, item);
}

CommonTreeWidget::CommonTreeWidget(RuleManager::ProjectType projectType, bool isXbel, QWidget *parent)
  : QTreeWidget(parent),
    mModified(false),
    mNeedQueryAddAudioTrack(true),
    mDoAddAudioTrack(false),
    mNeedQueryReplaceFile(true),
    mDoReplaceFile(false),
    mSelectedItem(nullptr),
    mDataTrackItem(nullptr),
    mSize(0),
    mItemCount(0),
    mNodeCount(0),
    mAudioCount(0),
    mProjectType(projectType),
    mQueryResetBeforeImport(false)
{
  setAcceptDrops(true);
  setDragEnabled(false);
  setOverwriteFlag(true);

  setDefaultDropAction(Qt::MoveAction);
  setDropIndicatorShown(true);
  //setDragDropMode(QAbstractItemView::InternalMove);

  QDataItem::AddDefaultIcon(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                            style()->standardPixmap(QStyle::SP_DirOpenIcon),
                            QPixmap(":/icons/folder_red_1.png"),
                            QPixmap(":/icons/folder_red_2.png"),
                            style()->standardPixmap(QStyle::SP_FileIcon));

  QDataItem *diskItem;
  diskItem = new QDiskItem(this);
  //diskItem->SetType(QDataItem::Disk);
  QString diskName = buildDiskName();
  diskItem->setText(0, diskName);
  diskItem->SetName(diskName);



  //What are the default values we read in?
  //mSystemId = ConfigurationPage::mSettings.value("systemId","").toString();
  //(QDiskItem *)diskItem->setSystemId("ttest");
  //qobject_cast<QDiskItem *>(diskItem);

  diskItem->setFlags(diskItem->flags() | Qt::ItemIsEditable);
  diskItem->setExpanded(true);
  mHeadItem = diskItem;

  QDataItem *childItem;
  childItem = new QDataItem(diskItem);
  childItem->SetType(QDataItem::Session);
  childItem->setText(0, tr("Session"));
  childItem->SetName(tr("Session"));
  childItem->setExpanded(true);
  mSessionItem = childItem;
  setSelected(childItem);


   //Problem is, that the datatrackitem is created.
  //We can override the function here or delete while open xbel file.
  //Decission: override
  if(isXbel==false){
      if (RuleManager::TYPE_PROJECT_ISO == mProjectType || RuleManager::TYPE_PROJECT_ISOUDF == mProjectType || RuleManager::TYPE_PROJECT_UDF == mProjectType ){
          if(ConfigurationPage::mSettings.value("CreateDataTrackAuto", true).toBool()==true){
              setSelected(getDataTrackItem());
          }
      }
  }


  mInsertItemAct = new QAction(tr("Insert file"), this);
  connect(mInsertItemAct, SIGNAL(triggered()), this, SLOT(slot_insert_item_from_dialog()));
  mInsertNodeAct = new QAction(tr("Insert folder"), this);
  connect(mInsertNodeAct, SIGNAL(triggered()), this, SLOT(slot_insert_node_from_dialog()));
  mDeleteAct = new QAction(tr("Delete"), this);
  connect(mDeleteAct, SIGNAL(triggered()), this, SLOT(DeleteItem()));
  mAddNodeAct = new QAction(tr("Add folder"), this);
  connect(mAddNodeAct, SIGNAL(triggered()), this, SLOT(AddNewFolder()));
  mResetAct = new QAction(tr("Reset"), this);
  connect(mResetAct, SIGNAL(triggered()), this, SLOT(ResetFiles()));
  // For Session
  mInsertDataTrackAct = new QAction(tr("Add data track"), this);
  connect(mInsertDataTrackAct, SIGNAL(triggered()), this, SLOT(AddDataTrack()));
  mInsertAudioTrackAct = new QAction(tr("Create audio track"), this);
  connect(mInsertAudioTrackAct, SIGNAL(triggered()), this, SLOT(AddAudioTrack()));
  mImportPlaylistAct = new QAction(tr("Import playlist"), this);
  connect(mImportPlaylistAct, SIGNAL(triggered()), this, SLOT(ImportPlaylist()));

  // For Data Track
  mDataTrackMode1Act = new QAction(tr("Mode 1"), this);
  connect(mDataTrackMode1Act, SIGNAL(triggered()), this, SLOT(dataTrackMode1()));
  mDataTrackMode2Act = new QAction(tr("Mode 2"), this);
  connect(mDataTrackMode2Act, SIGNAL(triggered()), this, SLOT(dataTrackMode2()));

  // For FSSync\FSUnsync
  mFSSyncAct = new QAction(tr("FSSync"), this);
  connect(mFSSyncAct, SIGNAL(triggered()), this, SLOT(FSSync()));
  mFSUnsyncAct = new QAction(tr("FSUnsync"), this);
  connect(mFSUnsyncAct, SIGNAL(triggered()), this, SLOT(FSUnsync()));
  // For Audio
  mClearAudioAct = new QAction(tr("Clear audio"), this);
  connect(mClearAudioAct, SIGNAL(triggered()), this, SLOT(clearAudio()));
  mInsertAudioAct = new QAction(tr("Insert audio"), this);
  connect(mInsertAudioAct, SIGNAL(triggered()), this, SLOT(insertAudio()));

  mPlayAct = new QAction(tr("Play"), this);
  connect(mPlayAct, SIGNAL(triggered()), this, SLOT(showPlay()));

  mStopAct = new QAction(tr("Stop"), this);
  connect(mStopAct, SIGNAL(triggered()), this, SLOT(showStop()));
  mCDTextAct = new QAction(tr("CDText"), this);
  connect(mCDTextAct, SIGNAL(triggered()), this, SLOT(showCDText()));

  mSignalMapper = new QSignalMapper(this);
  connect(mSignalMapper, SIGNAL(mapped(int)), this, SLOT(MoveAudioTrack(int)));
  mAudioUp = new QAction(tr("Move up"), this);
  connect(mAudioUp, SIGNAL(triggered()), mSignalMapper, SLOT(map()));

  mAudioDown = new QAction(tr("Move down"), this);
  connect(mAudioDown, SIGNAL(triggered()), mSignalMapper, SLOT(map()));
  mSignalMapper->setMapping(mAudioUp, -1);
  mSignalMapper->setMapping(mAudioDown, 1);

  // For Folder and Files
  mRenameAct = new QAction(tr("Rename"), this);
  connect(mRenameAct, SIGNAL(triggered()), this, SLOT(slot_rename_in_place()));
  mPropertyAct = new QAction(tr("Properties"), this);
  connect(mPropertyAct, SIGNAL(triggered()), this, SLOT(showProperty()));



  mSelectAllAct = new QAction(tr("Select all"), this);
  connect(mSelectAllAct, SIGNAL(triggered()), this, SLOT(SelectAll()));
  mReverseSelectionAct = new QAction(tr("Reverse selection"), this);
  connect(mReverseSelectionAct, SIGNAL(triggered()), this, SLOT(ReverseSelection()));

  // For import
  mImportAct = new QAction(tr("Import"), this);
  connect(mImportAct, SIGNAL(triggered()), this, SLOT(slot_import_from_dialog()));

  //connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem *)), this, SLOT(slot_handle_collapse(QTreeWidgetItem *)));
  connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slot_handle_itemChanged(QTreeWidgetItem*,int)));
  //connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(slot_handle_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
  connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slot_handle_itemSelectChanged()));
  connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(slot_handle_itemDoubleClicked(QTreeWidgetItem*,int)));
}

//void CommonTreeWidget::slot_handle_collapse(QTreeWidgetItem *item)
//{
//  if (item == mSessionItem) {
//    expandItem(item);
//  }
//}

void CommonTreeWidget::handleRename(QTreeWidgetItem *item, int column)
{
  QString old_name = ((QDataItem*)item)->GetName();
  QString new_name = item->text(column);
  if (old_name.trimmed() == "" || old_name == new_name) {
    return;
  }
  if (new_name.trimmed() == "") {
    item->setText(column, old_name);
    return;
  }
  QFileInfo info(old_name);
  QString old_suffix = info.suffix();
  info.setFile(new_name);
  QString new_suffix = info.suffix();
  if (old_suffix != "")
    new_name = info.baseName()+"."+old_suffix;
  else
    new_name = info.baseName();
  if (old_suffix != new_suffix) {
    item->setText(column, new_name);
  }
  ((QDataItem*)item)->SetName(new_name);
}

void CommonTreeWidget::handleChangeComment(QTreeWidgetItem *item, int column)
{
    ((QDataItem*)item)->SetComment(item->text(column));
}

void CommonTreeWidget::slot_handle_itemChanged(QTreeWidgetItem *item, int column)
{
    if (0 == column) {
        handleRename(item, column);
    } else if (2 == column) {
        //handleChangeComment(item, column);
    }
}

void CommonTreeWidget::slot_insert_item_from_dialog()
{
    QString path = QFileDialog::getOpenFileName(this);
    if (path != "") {
        InsertItem(path);
        //qDebug("Insert item from Dialog");
        emit datatrackChanged();
    }
}

void CommonTreeWidget::slot_insert_node_from_dialog()
{
    QFileDialog *dialog = new QFileDialog();
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
    QString path = dialog->getExistingDirectory(this);

    if (path != "") {
        InsertNode(path);
    }

    delete dialog;
}

void CommonTreeWidget::slot_import_from_dialog()
{
  QFileDialog *dialog = new QFileDialog();
  dialog->setOption(QFileDialog::ShowDirsOnly, true);
  QString path = dialog->getExistingDirectory(this);
  if (path != "") {
    if (mQueryResetBeforeImport) {
      QMessageBox mb(QMessageBox::Question, tr("Warning"),
                     tr("Structure already filled. Import is only possible on empty ")
                        + tr("structure. Do you want to empty structure now?"),
                     QMessageBox::Yes | QMessageBox::No);
      QMessageBox::StandardButton ret = (QMessageBox::StandardButton)mb.exec();
      if (QMessageBox::Yes == ret) {
        ResetFiles();
        Import(path);
      }
    } else {
      Import(path);
    }
  }
  delete dialog;
}


void CommonTreeWidget::slot_handle_itemSelectChanged()
{
    GetSelectedItem();
    if (mSelectedItem == nullptr)
        return;

    if (QDataItem::AudioTrack == mSelectedItem->GetType() && mAudioCount > 1) {
        emit audiotrackChanged(true);
    } else {
        emit audiotrackChanged(false);
    }
}

void CommonTreeWidget::slot_handle_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
  Qt::ItemFlags tmp = item->flags();
  if(((QDataItem*)item)->GetType()!=QDataItem::Disk){
      if(((QDataItem*)item->parent())->GetType()==QDataItem::AudioTrack  || ((QDataItem*)item)->GetType()==QDataItem::AudioTrack){
          return;
      }
  }
  if (0 == column && (QDataItem::File == ((QDataItem*)item)->GetType()
      || QDataItem::Disk == ((QDataItem*)item)->GetType())) {
    item->setFlags(tmp | Qt::ItemIsEditable);
  } else if (tmp & Qt::ItemIsEditable) {
    item->setFlags(tmp ^ Qt::ItemIsEditable);
  }
}

void CommonTreeWidget::slot_rename_in_place()
{
    GetSelectedItem();
    if (mSelectedItem == nullptr)
        return;

    mSelectedItem->setFlags(mSelectedItem->flags() | Qt::ItemIsEditable);
    editItem(mSelectedItem, 0);
}

void CommonTreeWidget::InsertItemFromRibbon()
{
     GetSelectedItem();
     if (mSelectedItem == nullptr)return;

     if (QDataItem::AudioTrack == mSelectedItem->GetType()) {
         insertAudio();
     }else{
         QString name = QFileDialog::getOpenFileName(this);
         if (name != "") {
            InsertItem(name);
            emit datatrackChanged();
         }
     }
}

QDataItem* CommonTreeWidget::InsertItem(const QString &path)
{

  //Neue Logik einführen:
  //Wenn selected prüfen ob File. Wenn File dann zum nächst hören Item gehen (Parent)

  GetSelectedItem();
  if (mSelectedItem == nullptr /*|| QDataItem::File == mSelectedItem->GetType()*/) {
    QMessageBox::information(this, tr("Information"),
                             tr("You need to select one folder to insert new item or folder!"));
    return nullptr;
  }

  if (getIsFSSyncAvailable() && (QDataItem::Session == mSelectedItem->GetType()
                                 || QDataItem::DataTrack == mSelectedItem->GetType())) {
    QMessageBox::information(this, tr("Information"),
                             tr("In FSSync\\FSUnsync mode, cannot add file to current node!"));
    return nullptr;
  }



  if (QDataItem::Disk == mSelectedItem->GetType()) {
    setSelected(mSessionItem);
  }

  if (isAudioFile(path))
    return InsertAudioItem(path);
  else
    return InsertDataItem(path);
}

QDataItem* CommonTreeWidget::InsertPlaylistItem(const QString &path)
{
  setSelected(mSessionItem);

  if (RuleManager::TYPE_PROJECT_AUDIOCD != mProjectType
       && RuleManager::TYPE_PROJECT_OPEN != mProjectType
       && RuleManager::TYPE_PROJECT_MIXEDMODE != mProjectType) {
    return InsertDataItem(path);
  } else if (isAudioFile(path)) {
    return InsertAudioItem(path);
  }
  return nullptr;
}

QDataItem* CommonTreeWidget::InsertDataItem(const QString &path)
{
  GetSelectedItem();
  QDataItem *childItem = nullptr;


  if (QDataItem::Session == mSelectedItem->GetType()) {
    if (ConfigurationPage::mSettings.value("CreateDataTrackAuto", false).toBool()) {
      mSelectedItem = getDataTrackItem();
    } else if (mDataTrackItem == nullptr) { //general DataTrackItem in tree
      QMessageBox::information(this, tr("Information"),
                               tr("Sorry you need to add a data track first!"));
      return nullptr;
    } else {
      mSelectedItem = mDataTrackItem; // parent set to datatrack
    }
  } else if (QDataItem::DataTrack == mSelectedItem->GetType()) {
    // do nothing here
  } else if (QDataItem::Folder == mSelectedItem->GetType()) {
    // do nothing here
  }else if (QDataItem::File == mSelectedItem->GetType()) { //Temporary added. If File, it will select the parent node and add the file.
      SetSelectedParent();
  } else if (QDataItem::AudioTrack == mSelectedItem->GetType()) {
    QMessageBox::information(this, tr("Information"),
                             tr("Only audio file can be added in audio track!"));
    return nullptr;
  }
  // check whether there is already file with the same name in this level
  int index = findSameNameInLevel(mSelectedItem, path);
  if (index != -1) {
    if ((mNeedQueryReplaceFile && queryReplaceFile(path)) || mDoReplaceFile) {
      // update parents and whole tree
      updateDataSize(mSelectedItem, -1*((QDataItem*)(mSelectedItem->child(index)))->GetDataSize(), -1, 0);
      mSelectedItem->removeChild(mSelectedItem->child(index));
      childItem = InsertItemOperation(mSelectedItem, path);
    } else {
      childItem = nullptr; // not replace
    }
  } else {
    childItem = InsertItemOperation(mSelectedItem, path);
  }
  return childItem;
}

QDataItem* CommonTreeWidget::InsertAudioItem(const QString &path)
{
  GetSelectedItem();
  QDataItem *childItem = 0;

  emit datatrackChanged();

  if (QDataItem::Session == mSelectedItem->GetType()
      || QDataItem::DataTrack == mSelectedItem->GetType()
      || QDataItem::Folder == mSelectedItem->GetType()) {
    if (!queryAddAudioTrack()) {
      // just make sure to get the config for "AudioToAudioTrack"
      return nullptr;
    } else {
      // if user said "Yes", we add audio to audio track first,
      // then add audio to data track, depends on settings
      QDataItem *audio_track = nullptr;

      if (mDoAddAudioTrack) {
        audio_track = addAudioToNewAudioTrack(path);
      }

      if ((RuleManager::TYPE_PROJECT_OPEN == mProjectType
           || RuleManager::TYPE_PROJECT_MIXEDMODE == mProjectType)
          && (ConfigurationPage::mSettings.value("AudioToDataTrack", false).toBool()
              || ConfigurationPage::mSettings.value("AudioToDataTrackToo", false).toBool())) {
        if (mDataTrackItem != nullptr && QDataItem::Session == mSelectedItem->GetType()) {
          mSelectedItem = mDataTrackItem;
        } else if (nullptr == mDataTrackItem) {
          if (ConfigurationPage::mSettings.value("CreateDataTrackAuto", false).toBool()) {
            mSelectedItem = getDataTrackItem();
          } else {
            // do prompting
//            QMessageBox::information(this, tr("Information"),
//                                     tr("Sorry you need to add a data track first!"));
            return audio_track;
          }
        }
      }
      else {
        return audio_track;
      }
    }
  } else if (QDataItem::AudioTrack == mSelectedItem->GetType()) {
    if (!isAudioFile(path)) {
      QMessageBox::information(this, tr("Information"),
                               tr("Only audio file can be added in audio track!"));
      return nullptr;
    } else if (mSelectedItem->childCount() == 0) {
      return addAudioToEmptyAudioTrack(mSelectedItem, path);
    } else {
      QMessageBox mb(tr("Warning"), tr("Replace, or create new audio track?"),
                     QMessageBox::Question, QMessageBox::Yes,
                     QMessageBox::No, QMessageBox::Cancel);
      mb.button(QMessageBox::Yes)->setText("Replace");
      mb.button(QMessageBox::No)->setText("New");
      QMessageBox::StandardButton ret = (QMessageBox::StandardButton)mb.exec();
      if (QMessageBox::Yes == ret) {
        return replaceAudioInAudioTrack(mSelectedItem, path);
      } else if (QMessageBox::No == ret) {
        return addAudioToNewAudioTrack(path);
      } else if (QMessageBox::Cancel == ret) {
        return nullptr;
      }
    }
  }
  childItem = InsertItemOperation(mSelectedItem, path);
  if (nullptr != childItem)
    childItem->setFlags(childItem->flags() & (~Qt::ItemIsEditable));
  return childItem;
}

QDataItem* CommonTreeWidget::InsertItemOperation(QDataItem *parent, const QString &path)
{
  //if (QDataItem::Session == type) return 0;
  //GetSelectedItem();
  QDataItem *childItem = 0;

  childItem = InsertItem(parent, path);
  if (nullptr == childItem)
    return nullptr;
  setSelected(childItem);

  // update parents and whole tree
  updateDataSize(parent, childItem->GetDataSize(), 1, 0);
  if (parent != nullptr)
    parent->setExpanded(true);

  resetQueryAddAudioTrack();
  resetQueryReplaceFile();
  mQueryResetBeforeImport = true;
  mModified = true;
  emit contentsChanged(true);
  return childItem;
}

QDataItem* CommonTreeWidget::InsertItem(QDataItem *parent, const QString &path, bool silent)
{
  if (ZImportStrategy::ERROR_NO != importCheck2(parent->GetName(), path, silent)) {
    return nullptr;
  }
  QDataItem *childItem = 0;
  if (parent) {
    childItem = new QDataItem(parent);
    //parent->setExpanded(true);
  } else {
    childItem = new QDataItem(mSessionItem);
  }
  childItem->setData(0, Qt::UserRole, "");
  childItem->setFlags((childItem->flags() & (~Qt::ItemIsDropEnabled)) | Qt::ItemIsEditable);
  childItem->SetType(QDataItem::File);
  childItem->SetData(path);
  childItem->SetDataItemCount(1);
  //setItemExpanded(childItem, true);
  return childItem;
}

void CommonTreeWidget::InsertNode(const QString &path)
{
  GetSelectedItem();
  QDataItem *parent;



  if (mSelectedItem == nullptr || QDataItem::Disk == mSelectedItem->GetType()) {
    parent = mSessionItem;
  } else if (mSelectedItem->GetType() == QDataItem::File) {
    parent = (QDataItem *)(mSelectedItem->parent());
  } else {
    parent = mSelectedItem;
  }

  if (getIsFSSyncAvailable() && (QDataItem::Session == mSelectedItem->GetType() || QDataItem::DataTrack == mSelectedItem->GetType())) {
      QMessageBox::information(this, tr("Information"),
                               tr("In FSSync\\FSUnsync mode, cannot add folder to current node!"));
      return; // return nullptr;
  }

  // Add Data Track
  if (mSessionItem == parent) {
    if(RuleManager::TYPE_PROJECT_AUDIOCD != mProjectType){
      if (ConfigurationPage::mSettings.value("CreateDataTrackAuto", false).toBool()) {
        parent = getDataTrackItem();
      } else if (mDataTrackItem == nullptr) {
        QMessageBox::information(this, tr("Information"),
                                 tr("Sorry you need to add a data track first!"));
        return; // return nullptr;
      } else {
        parent = mDataTrackItem;
      }
    }
  } else if (QDataItem::AudioTrack == parent->GetType()) {
    QMessageBox::information(this, tr("Information"),
                             tr("Sorry you cannot add a folder to audio track!"));
    return; // return nullptr;
  }

  if (ZImportStrategy::ERROR_NO != importCheck2(parent->GetName(), path, true)) {
    return; // return nullptr;
  }

  mThreadAddTree = new ThreadAddTree(this, parent, path);
  connect(mThreadAddTree, SIGNAL(completed(QDataItem*, QDataItem*)), this, SLOT(on_add_tree_completed(QDataItem*, QDataItem*)));
  mThreadAddTree->start();
  QWidget *w = qobject_cast<QWidget*>(QApplication::activeWindow());
  mBlockSpinner = new QtWaitingSpinner(Qt::ApplicationModal, w, true);
  //mBlockSpinner->setColor(Qt::red);
  mBlockSpinner->start();
}

QDataItem* CommonTreeWidget::InsertNodeToISOUDFRoot(const QString &path)
{
  if (mDataTrackItem->childCount() != 2)
    return nullptr;
  qint64 size = 0;
  int item_count = 0;
  int node_count = 0;
  QDataItem *item;
  QDataItem *itemISO = (QDataItem *)mDataTrackItem->child(0);
  QDataItem *itemUDF = (QDataItem *)mDataTrackItem->child(1);
  item = addTree(itemISO, path, size, item_count, node_count);
  if (item != nullptr) {
    updateDataSize(itemISO, item->GetDataSize(), item->GetDataItemCount(), item->GetDataNodeCount());
  }
  item = addTree(itemUDF, path, size, item_count, node_count);
  if (item != nullptr) {
    updateDataSize(itemUDF, item->GetDataSize(), item->GetDataItemCount(), item->GetDataNodeCount());
    mDataTrackItem->setExpanded(true);
    itemISO->setExpanded(true);
    itemUDF->setExpanded(true);
  }
  return item;
}

void CommonTreeWidget::Import(const QString &path)
{
  qint64 size = 0;
  int item_count = 0;
  int node_count = 0;

  //if (mSessionItem->childCount() > 0 || nullptr != mDataTrackItem)
  //  return;

  // create datatrack first
  QDataItem *parent = getDataTrackItem();
  mRootToBeImport = path;
  mImportErr = "";
  importFromRoot(parent, path, size, item_count, node_count);
  bool auto_correct = false;
  if ("" != mImportErr) {
    QMessageBox mb(QMessageBox::Question, tr("Error"), mImportErr,
                   QMessageBox::Yes | QMessageBox::Cancel);
    mb.button(QMessageBox::Yes)->setText("AutoCorrect");
    QMessageBox::StandardButton ret = (QMessageBox::StandardButton)mb.exec();
    if (QMessageBox::Yes == ret) {
      auto_correct = true;
    }
  }
  //if ("" == mImportErr || ConfigurationPage::mSettings.value("ImportAutoCorrect", true).toBool()) {
  if ("" == mImportErr || auto_correct) {
    // update parents and whole tree
    updateDataSize(parent, size, item_count, node_count);
    parent->setExpanded(true);
    mQueryResetBeforeImport = true;
    mModified = true;
    emit contentsChanged(true);
  } else {
    // failed to import tree, so we remove datatrack
    ResetFiles();
  }
}

QDataItem* CommonTreeWidget::AddNewFolder()
{
  static int sequenceNumber = 0;
  GetSelectedItem();
  QDataItem *parent;
  if (mSelectedItem == nullptr) {
    parent = mSessionItem;
  } else if (mSelectedItem->GetType() == QDataItem::File) {
    parent = (QDataItem *)(mSelectedItem->parent());
  } else {
    parent = mSelectedItem;
  }

  if (getIsFSSyncAvailable() && (QDataItem::Session == mSelectedItem->GetType()
                                 || QDataItem::DataTrack == mSelectedItem->GetType())) {
      QMessageBox::information(this, tr("Information"),
                               tr("In FSSync\\FSUnsync mode, cannot create folder to current node!"));
      return nullptr;
  }

  //Add Data Track
  if (mSessionItem == parent || QDataItem::AudioTrack == parent->GetType()) {
    parent = getDataTrackItem();
  }

  qint64 size = 0;
  int item_count = 0;
  int node_count = 0;
  QString folder_name = tr("New Folder");
  if (sequenceNumber > 0) {
    folder_name += tr("(%1)").arg(sequenceNumber);
  }
  sequenceNumber++;

  if (ZImportStrategy::ERROR_NO != importCheck2(parent->GetName(), folder_name)) {
    return nullptr;
  }

  QDataItem *item = addTree(parent, folder_name, size, item_count, node_count);
  if (nullptr != item) {
    if (QDataItem::FixedFolder == parent->GetType()) {
      changeFolderPos(parent, item);
    }
    // update parents and whole tree
    updateDataSize(parent, 0, 0, 1);

    parent->setExpanded(true);
    item->setExpanded(true);
    item->SetDefaultIcon();
    setSelected(item);
    mQueryResetBeforeImport = true;
    mModified = true;
    emit contentsChanged(true);
  }
  return item;
}

void CommonTreeWidget::DeleteItem()
{
  GetSelectedItem();
  if (mSessionItem == mSelectedItem) {
    QMessageBox::information(this, tr("Information"),
                             tr("Sorry the session node cannot be deleted!"));
  } else if (nullptr == mSelectedItem) {
    QMessageBox::information(this, tr("Information"),
                             tr("Please select one item or folder you want to delete!"));
  } else if (QDataItem::FixedFolder == mSelectedItem->GetType()) {
    QMessageBox::information(this, tr("Information"),
                             tr("Sorry the fixed folder cannot be deleted!"));
  } else {
    QTreeWidgetItem* parentItem = mSelectedItem->parent();
    if (nullptr == parentItem) {
      takeTopLevelItem(indexOfTopLevelItem(mSelectedItem));
    } else {
      if(QDataItem::DataTrack == mSelectedItem->GetType() && mSelectedItem->GetDataSize() > 0) {
        QMessageBox::information(this, tr("Information"),
                                 tr("Sorry the Datatrack is not empty, cannot be deleted!"));
        return;
      }

      if(QDataItem::AudioTrack != mSelectedItem->GetType()) {
        updateDataSize((QDataItem*)parentItem, -1*mSelectedItem->GetDataSize(),
                       -1*mSelectedItem->GetDataItemCount(), -1*mSelectedItem->GetDataNodeCount());
      }
      //parentItem->removeChild(mSelectedItem);
      QDataItem *selected = mSelectedItem;

      if (QDataItem::AudioTrack == mSelectedItem->GetType()) {
        mAudioCount--;
        parentItem->removeChild(selected);
        updateAudioTrackName();
      }else if(QDataItem::AudioTrack == ((QDataItem*)parentItem)->GetType()){
        mAudioCount--;
        parentItem->removeChild(selected);
        QAudioTrackItem *audio_track = (QAudioTrackItem*)mSelectedItem;
        audio_track->resetAll();
        updateAudioTrackName();
      } else if (mSelectedItem == mDataTrackItem) {
        mDataTrackItem = nullptr;
        parentItem->removeChild(selected);
        emit datatrackChanged();
      } else{
         parentItem->removeChild(selected);
         emit datatrackChanged();
      }

      setSelected((QDataItem *)parentItem);
    }
    mModified = true;
    emit contentsChanged(true);
  }
}

QDataItem *CommonTreeWidget::SetSelectedParent()
{
    QList<QTreeWidgetItem*> selected = selectedItems();
    if (selected.size() > 0) {
      mSelectedItem = (QDataItem *)selected[0]->parent();
    } else {
      mSelectedItem = nullptr;
    }
    return mSelectedItem;
}

QDataItem *CommonTreeWidget::GetSelectedItem()
{
    //if selected is more than 0 then just take the first one as reference.
    //are we able to select more than one?
  QList<QTreeWidgetItem*> selected = selectedItems();
  if (selected.size() > 0) {
    mSelectedItem = (QDataItem *)selected[0];
  } else {
    mSelectedItem = nullptr;
  }
  return mSelectedItem;
}

QDataItem *CommonTreeWidget::AddDataTrack()
{
  return getDataTrackItem();
}

QDataItem *CommonTreeWidget::AddAudioTrack()
{
  return addAudioTrack();
}

void CommonTreeWidget::ImportPlaylist()
{
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Import Playlist"),
                                   QDir::currentPath(),
                                   tr("Playlist Files (*.m3u *.pls)"));
  if (fileName.isEmpty())
    return;
  ZPlaylistLoader loader(this);
  loader.Load(fileName);
}

void CommonTreeWidget::SelectAll()
{
  GetSelectedItem();
  if (mSelectedItem == nullptr)
    return;
  if (QDataItem::Folder == mSelectedItem->GetType()
      || QDataItem::VirtualFolder == mSelectedItem->GetType()
      || QDataItem::FixedFolder == mSelectedItem->GetType()) {
    mSelectedItem->setExpanded(true);
    for (int i = 0; i < mSelectedItem->childCount(); i++) {
      QTreeWidgetItem *child = mSelectedItem->child(i);
      child->setExpanded(false);
      child->setSelected(true);
    }
  }
}

void CommonTreeWidget::ReverseSelection()
{
  GetSelectedItem();
  if (mSelectedItem == nullptr)
    return;
  mSelectedItem->setExpanded(false);
  QTreeWidgetItem *parent = mSelectedItem->parent();
  QList<QTreeWidgetItem*> selected = selectedItems();
  for (int i = 0; i < selected.size(); i++) {
    if (selected.at(i)->parent() != parent) {
      selected.at(i)->setSelected(false);
    }
  }
  if (parent != nullptr) {
    for (int i = 0; i < parent->childCount(); i++) {
      parent->child(i)->setSelected(!parent->child(i)->isSelected());
    }
  }
}

void CommonTreeWidget::UpdateProject()
{
  QTreeWidgetItemIterator it(mHeadItem);
  while (*it) {
    QDataItem *item = (QDataItem*)(*it);
    if (QDataItem::File == item->GetType()) {
      QFileInfo fileinfo(item->GetFullPath());
      qint64 size = fileinfo.size();
      item->SetDataSize(size);
      updateDataSize((QDataItem*)(item->parent()), size, 0, 0);
    } else {
      item->SetDataSize(0);
    }
    ++it;
  }
  emit contentsChanged(true);
}

void CommonTreeWidget::MoveAudioTrack(int index_change)
{
  GetSelectedItem();

  if(mSelectedItem == nullptr) return;

  QTreeWidgetItem *checkParent = mSelectedItem->parent();
  if (checkParent != mSessionItem && checkParent->parent()!= mSessionItem) return;
  if (((QDataItem*)checkParent)->GetType()!= QDataItem::AudioTrack && mSelectedItem->GetType() != QDataItem::AudioTrack) return;

/*
  if (mSelectedItem == nullptr || mSelectedItem->GetType() != QDataItem::AudioTrack)
    return;
  QTreeWidgetItem *parent = mSelectedItem->parent();
  if (parent != mSessionItem)
    return;
*/
  QTreeWidgetItem *parent;

  int index;
  if(mSelectedItem->GetType() == QDataItem::AudioTrack){
     parent = mSelectedItem->parent();
     index = parent->indexOfChild(mSelectedItem);
  }else{
     parent = checkParent->parent();
     index = parent->indexOfChild(mSelectedItem->parent());
  }

  //int index = parent->indexOfChild(mSelectedItem);

  if (index_change < 0 && ((IsDataTrackExist() && index == 1) || (!IsDataTrackExist() && index == 0))) {
    return;
  }
  if (index_change > 0 && parent->childCount()-1 == index) {
    return;
  }

  //This is the secret to randomize the order
  QTreeWidgetItem *child = parent->takeChild(index);
  parent->insertChild(index+index_change, child);

  child->setExpanded(true);
  setSelected((QDataItem*)child);
  updateAudioTrackName();
}

void CommonTreeWidget::SetDataTrackItem(QDataItem *datatrack)
{
  if (datatrack->GetType() == QDataItem::DataTrack) {
    mDataTrackItem = datatrack;
    emit datatrackChanged();
  }
}

void CommonTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
  event->acceptProposedAction();
}

void CommonTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
  event->acceptProposedAction();
}

void CommonTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

void CommonTreeWidget::dropEvent(QDropEvent *event)
{
  if (mProjectType == RuleManager::TYPE_PROJECT_EXPLORE)
    return;

  //My Tryout to make internal drag&drop, but cause crashes.
    /*
  QTreeWidgetItem *parent;
  parent = mSelectedItem->parent();
  int index = parent->indexOfChild(mSelectedItem);


  QTreeWidgetItem *child = parent->takeChild(index);


  QModelIndex dropIndex = indexAt(event->pos());

  itemFromIndex(dropIndex)->parent()->insertChild(0, child);

  //parent->insertChild(0, child);
  return;
    */
  //Gehen wir mal davon aus dass internal movement keine mimedata hat, dann könnten wir ja über den index gehen?

  const QMimeData *mimeData = event->mimeData();

      if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if (urlList.size() > 0) {
          QString url = urlList.at(0).path();
          //QString url = urlList.at(0).toString();
    #ifdef Q_OS_WIN
          url = url.remove(0, 1);
    #endif
          QModelIndex dropIndex = indexAt(event->pos());
          QDataItem *dropItem = (QDataItem*)dropIndex.internalPointer();
          setSelected(dropItem);
          //QString temp = mSelectedItem->text(0);
          QFileInfo entry(url);

          if (entry.isDir()) {
            InsertNode(url);
          } else if (entry.isFile()) {
            QDataItem *focusItem = InsertItem(url);
            setSelected(focusItem);
          }
        }
      }
    event->acceptProposedAction();
}

/*
bool CommonTreeWidget::dropMimeData(QTreeWidgetItem *parent, int index,
                          const QMimeData *data, Qt::DropAction action)
{
  QTreeWidget::dropMimeData(parent, index, data, action);
}
*/

void CommonTreeWidget::mousePressEvent(QMouseEvent *event)
{
  bool right_click_in_multiple_items = false;
  if (Qt::RightButton == event->button()) {
    QList<QTreeWidgetItem*> selected = selectedItems();
    if (selected.size() > 1) {
      QModelIndex index = indexAt(event->pos());
      QTreeWidgetItem *item = (QTreeWidgetItem*)index.internalPointer();
      for (int i = 0; i < selected.size(); i++) {
        if (selected.at(i) == item) {
          right_click_in_multiple_items = true;
          break;
        }
      }
    }
  }else{
      //left click
      /*
      QMimeData* mimedata = new QMimeData;
        QList<QUrl> urls;
        urls.append(currentItem() -> text(6));
        mimedata -> setUrls(urls);



        QDrag *drag = new QDrag (this);
        drag -> setMimeData(mimedata);
        drag -> exec();
        */
  }


  if (!right_click_in_multiple_items) {
    QTreeView::mousePressEvent(event);
  }
}

void CommonTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
  //if (mProjectType == RuleManager::TYPE_PROJECT_EXPLORE)
  //  return;

  QList<QTreeWidgetItem*> selected = selectedItems();
  bool isMultipleSelected = selected.size() > 1 ? true : false;

  QModelIndex index = indexAt(event->pos());
  QDataItem *item = (QDataItem*)index.internalPointer();
  if (nullptr != item) {
    //if ((nullptr != item->parent() && QDataItem::AudioTrack == ((QDataItem*)(item->parent()))->GetType())) {
    //  return;
    //}
    QMenu menu(this);

    // restore actions first
    mDeleteAct->setEnabled(true);

    if (QDataItem::AudioTrack == item->GetType()) {
      //|| (nullptr != item->parent() && QDataItem::AudioTrack == ((QDataItem*)(item->parent()))->GetType())) {
      menu.addAction(mInsertAudioAct);
      if (item->childCount() > 0) {
        mInsertAudioAct->setEnabled(false);
      } else {
        mInsertAudioAct->setEnabled(true);
      }
      menu.addAction(mClearAudioAct);
      menu.addAction(mPlayAct);
      menu.addAction(mStopAct);
      menu.addAction(mCDTextAct);
      if (item->childCount() > 0) {
        mClearAudioAct->setEnabled(true);
        mPlayAct->setEnabled(true);
        mStopAct->setEnabled(true);
        mCDTextAct->setEnabled(true);
      } else {
        mClearAudioAct->setEnabled(false);
        mPlayAct->setEnabled(false);
        mStopAct->setEnabled(false);
        mCDTextAct->setEnabled(false);
      }
      menu.addAction(mAudioUp);
      menu.addAction(mAudioDown);
      int index = mSessionItem->indexOfChild(item);
      if ((IsDataTrackExist() && index == 1) || (!IsDataTrackExist() && index == 0)) {
        mAudioUp->setEnabled(false);
      } else {
        mAudioUp->setEnabled(true);
      }
      if (mSessionItem->childCount()-1 == index) {
        mAudioDown->setEnabled(false);
      } else {
        mAudioDown->setEnabled(true);
      }
      menu.addAction(mDeleteAct);
    } else if ((nullptr != item->parent() && QDataItem::AudioTrack == ((QDataItem*)(item->parent()))->GetType())) {
      menu.addAction(mPlayAct);
      menu.addAction(mStopAct);
      menu.addAction(mDeleteAct);
      mPlayAct->setEnabled(true);
      mStopAct->setEnabled(true);
    } else {
      if (QDataItem::Disk == item->GetType()) {
        menu.addAction(mPropertyAct);
      }

      mInsertItemAct->setEnabled(true);
      mInsertNodeAct->setEnabled(true);
      mAddNodeAct->setEnabled(true);

      if (QDataItem::Session == item->GetType() || QDataItem::DataTrack == item->GetType()) {
        if (getIsFSSyncAvailable()) {
          mFSSyncAct->setEnabled(true);
          mInsertItemAct->setEnabled(false);
          mInsertNodeAct->setEnabled(false);
          mAddNodeAct->setEnabled(false);
        } else {
          mFSSyncAct->setEnabled(false);
        }

        if (getIsFSUnsyncAvailable()) {
          mFSUnsyncAct->setEnabled(true);
        } else {
          mFSUnsyncAct->setEnabled(false);
        }
      }
      if ((mProjectType == RuleManager::TYPE_PROJECT_OPEN
          || mProjectType == RuleManager::TYPE_PROJECT_AUDIOCD
          || mProjectType == RuleManager::TYPE_PROJECT_MIXEDMODE)
          && !getIsFSSyncAvailable()) {
        mInsertAudioTrackAct->setEnabled(true);
      } else {
        mInsertAudioTrackAct->setEnabled(false);
      }
      if ((RuleManager::TYPE_PROJECT_VIDEODVD == mProjectType)
          || RuleManager::TYPE_PROJECT_BLURAYVIDEO == mProjectType) {
        mImportAct->setEnabled(true);
        mDataTrackMode2Act->setEnabled(false);
      } else {
        mImportAct->setEnabled(false);
        mDataTrackMode2Act->setEnabled(true);
      }

      if (QDataItem::Session == item->GetType()) {
        menu.addAction(mImportPlaylistAct);
        menu.addAction(mInsertDataTrackAct);
        menu.addAction(mInsertAudioTrackAct);
        menu.addAction(mInsertItemAct);
        menu.addAction(mInsertNodeAct);
        menu.addAction(mAddNodeAct);
        //menu.addAction(mFSSyncAct);
        //menu.addAction(mFSUnsyncAct);
        menu.addAction(mResetAct);

        if (mProjectType == RuleManager::TYPE_PROJECT_AUDIOCD) {
          mInsertDataTrackAct->setEnabled(false);
          mInsertItemAct->setEnabled(false);
          mAddNodeAct->setEnabled(false);
          mInsertNodeAct->setEnabled(false);
        } else {
          if (IsDataTrackExist()) {
            mInsertDataTrackAct->setEnabled(false);
          } else {
            mInsertDataTrackAct->setEnabled(true);
          }
        }

      } else if (QDataItem::DataTrack == item->GetType()) {
        menu.addAction(mDataTrackMode1Act);
        //menu.addAction(mDataTrackMode2Act);
        menu.addAction(mInsertItemAct);
        menu.addAction(mInsertNodeAct);
        menu.addAction(mAddNodeAct);
        menu.addAction(mDeleteAct);
        //menu.addAction(mFSSyncAct);
        //menu.addAction(mFSUnsyncAct);
        menu.addAction(mImportAct);
        menu.addAction(mResetAct);

        mDataTrackMode1Act->setCheckable(true);
        //mDataTrackMode2Act->setCheckable(true);
        if (mDataTrackItem->GetDataTrackMode() == 1) {
          mDataTrackMode1Act->setChecked(true);
          //mDataTrackMode2Act->setChecked(false);
        } else {
          mDataTrackMode1Act->setChecked(false);
          //mDataTrackMode2Act->setChecked(true);
        }

        if (item->childCount() > 0) {
          mDeleteAct->setEnabled(false);
        } else {
          mDeleteAct->setEnabled(true);
        }
      } else if (QDataItem::Folder == item->GetType()
                 || QDataItem::VirtualFolder == item->GetType()
                 || QDataItem::FixedFolder == item->GetType()
                 || QDataItem::File == item->GetType()) {
        menu.addAction(mRenameAct);
        menu.addAction(mInsertItemAct);
        menu.addAction(mInsertNodeAct);
        menu.addAction(mAddNodeAct);
        menu.addAction(mDeleteAct);
        menu.addAction(mSelectAllAct);
        menu.addAction(mReverseSelectionAct);
        menu.addAction(mPropertyAct);
      }

      if (isMultipleSelected) {
        mRenameAct->setEnabled(false);
        mInsertItemAct->setEnabled(false);
        mInsertNodeAct->setEnabled(false);
        mAddNodeAct->setEnabled(false);
        mDeleteAct->setEnabled(true);
        mSelectAllAct->setEnabled(false);
        mReverseSelectionAct->setEnabled(true);
        mPropertyAct->setEnabled(false);
      } else if (QDataItem::Folder == item->GetType()
          || QDataItem::VirtualFolder == item->GetType()) {
        mRenameAct->setEnabled(true);
        mInsertItemAct->setEnabled(true);
        mInsertNodeAct->setEnabled(true);
        mAddNodeAct->setEnabled(true);
        mDeleteAct->setEnabled(true);
        mSelectAllAct->setEnabled(true);
        mReverseSelectionAct->setEnabled(true);
        mPropertyAct->setEnabled(true);
      } else if (QDataItem::FixedFolder == item->GetType()) {
        ZImportStrategy::ImportError err = importCheck2(item->GetName(), tr("New Folder"), true);
        if (ZImportStrategy::ERROR_SUBFOLDER_NOT_ALLOWED == err
            || ZImportStrategy::ERROR_FIXEDFOLDER_EXCLUDE_OTHER_FOLDER == err) {
          mInsertNodeAct->setEnabled(false);
          mAddNodeAct->setEnabled(false);
        } else {
          mInsertNodeAct->setEnabled(true);
          mAddNodeAct->setEnabled(true);
        }
        mRenameAct->setEnabled(false);
        mInsertItemAct->setEnabled(ZImportStrategy::ERROR_FIXEDFOLDER_EXCLUDE_OTHER_FILE != err);
        mDeleteAct->setEnabled(false);
        mSelectAllAct->setEnabled(true);
        mReverseSelectionAct->setEnabled(true);
        mPropertyAct->setEnabled(false);
      } else if (QDataItem::File == item->GetType()) {
        mRenameAct->setEnabled(true);
        mInsertItemAct->setEnabled(true);
        mInsertNodeAct->setEnabled(true);
        mAddNodeAct->setEnabled(true);
        mDeleteAct->setEnabled(true);
        mSelectAllAct->setEnabled(false);
        mReverseSelectionAct->setEnabled(true);
        mPropertyAct->setEnabled(true);
      }
    }
    menu.exec(event->globalPos());
  }
}

QDataItem *CommonTreeWidget::addTree(QDataItem *parent, const QString &path,
                                     qint64 &data_size, int &item_count, int &node_count,
                                     bool sort)
{
  //In AudioCD it doesnt matter if the file is already there.
  //Item müsste hier mParent sein.
   QDir dir(path);
   QFileInfoList entries = dir.entryInfoList();
   qint64 sum = 0;
   int count = 0; // for item
   int count2 = 1; // for node, we have create one node above, so here is 1


   if(RuleManager::TYPE_PROJECT_AUDIOCD == mProjectType){
       for (int i = 0; i < entries.size(); i++) {
         QFileInfo entry = entries[i];
         if ("." == entry.fileName() || ".." == entry.fileName())
           continue;
         if(entry.isDir()) {
           addTree(parent, entry.absoluteFilePath(), sum, count, count2, sort);
         } else if (entry.isFile()) {
           addFileEx(parent, entry, sum, count);
         }
       }
       return nullptr;
   }

  QDataItem *item;
  int index = findSameNameInLevel(parent, path);
  if (index != -1) {
    if ((mNeedQueryReplaceFile && queryReplaceFile(path)) || mDoReplaceFile) {
      item = (QDataItem*)(parent->child(index)); // use old existed item
    } else {
      return nullptr; // not replace
    }
  } else {
    // if not find same name, we create new one
    item = new QDataItem(parent);
    //item->setData(0, Qt::UserRole, "");
    //item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->SetType(QDataItem::Folder);
    item->SetData(path);
    item->SetDataNodeCount(1);
  }


  if (sort) {
    for (int i = 0; i < entries.size(); i++) {
      QFileInfo entry = entries[i];
      if ("." == entry.fileName() || ".." == entry.fileName()
          || entry.isFile())
        continue;
      if(entry.isDir()) {
        addTree(item, entry.absoluteFilePath(), sum, count, count2, sort);
      }
    }
    for (int i = 0; i < entries.size(); i++) {
      QFileInfo entry = entries[i];
      if ("." == entry.fileName() || ".." == entry.fileName()
          || entry.isDir())
        continue;
      if (entry.isFile()) {
        addFileEx(item, entry, sum, count);
      }
    }
  } else {
    for (int i = 0; i < entries.size(); i++) {
      QFileInfo entry = entries[i];
      if ("." == entry.fileName() || ".." == entry.fileName())
        continue;
      if(entry.isDir()) {
        addTree(item, entry.absoluteFilePath(), sum, count, count2, sort);
      } else if (entry.isFile()) {
        addFileEx(item, entry, sum, count);
      }
    }
  }
  item->SetDataSize(sum);
  item->SetDataItemCount(count);
  item->SetDataNodeCount(count2);
  data_size += sum;
  item_count += count;
  node_count += count2;
  return item;
}

QDataItem* CommonTreeWidget::addFile(QDataItem *parent, const QFileInfo &entry
                      , qint64 &data_size, int &item_count)
{
  QDataItem *childItem = nullptr;
  childItem = importItem(parent, entry.absoluteFilePath());
  if (childItem != nullptr) {
    data_size += childItem->GetDataSize();
    item_count += 1;
  }
  return childItem;
}

QDataItem* CommonTreeWidget::addFileEx(QDataItem *parent, const QFileInfo &entry
                      , qint64 &data_size, int &item_count)
{
  bool add_file_to_data_track = false;
  if (isFiltered(entry)) {
    return nullptr;
  }
  if (isAudioFile(entry.filePath())) {
    if(mNeedQueryAddAudioTrack) {
      queryAddAudioTrack();
    }
    if (mDoAddAudioTrack) {
      addAudioToNewAudioTrack(entry.filePath());
    }
    if (ConfigurationPage::mSettings.value("AudioToDataTrack", false).toBool()
        || ConfigurationPage::mSettings.value("AudioToDataTrackToo", false).toBool()) {
      add_file_to_data_track = (RuleManager::TYPE_PROJECT_AUDIOCD == mProjectType)?false:true;
    }

  } else {
    // common data file
    add_file_to_data_track = (RuleManager::TYPE_PROJECT_AUDIOCD == mProjectType)?false:true;

  }
  QDataItem *childItem = nullptr;
  if (add_file_to_data_track) {
    int index = findSameNameInLevel(parent, entry.absoluteFilePath());
    if (index != -1) {
      if ((mNeedQueryReplaceFile && queryReplaceFile(entry.absoluteFilePath())) || mDoReplaceFile) {
        data_size -= ((QDataItem*)(parent->child(index)))->GetDataSize();
        item_count -= 1;
        parent->removeChild(parent->child(index));
        childItem = InsertItem(parent, entry.absoluteFilePath(), true);
      } else {
        childItem = nullptr; // not replace
      }
    } else {
      childItem = InsertItem(parent, entry.absoluteFilePath(), true);
    }
    if (childItem != nullptr) {
      data_size += childItem->GetDataSize();
      item_count += 1;
    }
  }
  return childItem;
}

void CommonTreeWidget::setSelected(QDataItem *item)
{
  if (nullptr == item) return;
  GetSelectedItem();
  if (nullptr != mSelectedItem) {
    mSelectedItem->setSelected(false);
  }
  mSelectedItem = item;
  if (nullptr != mSelectedItem) {
    mSelectedItem->setSelected(true);
  }
}

void CommonTreeWidget::updateDataSize(QDataItem *parent, qint64 size, int item_count, int node_count)
{
  // update parents
  if (nullptr != parent) {
    if (QDataItem::AudioTrack == parent->GetType()) {
      parent->SetDataSize(parent->GetDataSize() + size);
      parent->SetDataItemCount(parent->GetDataItemCount() + item_count);
    } else {
      QTreeWidgetItem *parentTmp = parent;
      while (nullptr != parentTmp) {
        ((QDataItem *)parentTmp)->SetDataSize(
              ((QDataItem *)parentTmp)->GetDataSize() + size);
        ((QDataItem *)parentTmp)->SetDataItemCount(
              ((QDataItem *)parentTmp)->GetDataItemCount() + item_count);
        ((QDataItem *)parentTmp)->SetDataNodeCount(
              ((QDataItem *)parentTmp)->GetDataNodeCount() + node_count);
        parentTmp = ((QDataItem *)parentTmp)->parent();
      }
    }
  }
  if (mHeadItem) {
    mSize = mHeadItem->GetDataSize();
    mItemCount = mHeadItem->GetDataItemCount();
    mNodeCount = mHeadItem->GetDataNodeCount();
  }
}

bool CommonTreeWidget::isAudioFile(const QString &path)
{
    if (path == "")
        return false;

    QFileInfo file(path);
    return isAudioFile(file);
}

bool CommonTreeWidget::isAudioFile(const QFileInfo &file)
{
    if (!file.isFile())
        return false;

    QString suffix = file.suffix();
    if (0 == QString::compare(suffix, "mp3")
        || 0 == QString::compare(suffix, "wav")
        || 0 == QString::compare(suffix, "aac")
        || 0 == QString::compare(suffix, "ogg")
        || 0 == QString::compare(suffix, "m4a")
        || 0 == QString::compare(suffix, "mp2")
        || 0 == QString::compare(suffix, "mp1")
        || 0 == QString::compare(suffix, "aiff")) {
            return true;
    }

    return false;
}

QDataItem* CommonTreeWidget::getDataTrackItem() {
  if (mDataTrackItem == nullptr) {
    //mDataTrackItem = new QDataItem(mSessionItem);
    mDataTrackItem = new QDataItem();
//    if (RuleManager::TYPE_PROJECT_ISOUDF == mProjectType) {
//       QDataItem *itemISO = addFSVirtualFolder(KStrISO);
//       QDataItem *itemUDF = addFSVirtualFolder(KStrUDF);
//       mDataTrackItem->addChild(itemISO);
//       mDataTrackItem->addChild(itemUDF);
//    }
    mSessionItem->insertChild(0, mDataTrackItem);
    //if (mDataTrackItem->parent() == mSessionItem) {
    //  QMessageBox::QMessageBox::about(this, tr("Information"), tr("Ok"));
    //}
    mDataTrackItem->setData(0, Qt::UserRole, "");
    mDataTrackItem->SetType(QDataItem::DataTrack);
    mDataTrackItem->SetDataWithName(tr("DataTrack(Mode1)"));
    mDataTrackItem->setExpanded(true);
    emit datatrackChanged();
  }
  return mDataTrackItem;
}

QDataItem* CommonTreeWidget::addAudioTrack() {
  QDataItem *audiotrack = nullptr;

  if (getIsFSSyncAvailable()) {
    QMessageBox::about(this, tr("Information"),
                       tr("In FSUnsync mode, cannot add audio track"));
    return nullptr;
  }
  if (RuleManager::TYPE_PROJECT_VIDEODVD == mProjectType
      || RuleManager::TYPE_PROJECT_BLURAYVIDEO == mProjectType) {
    return nullptr;
  }
  if (mAudioCount >= 99) {
    QMessageBox::about(this, tr("Information"),
                       tr("The number of Audiotrack has reached the limit!"));
    return nullptr;
  }
  mAudioCount++;
  if (mAudioCount == 2) {
    GetSelectedItem();
    if (nullptr != mSelectedItem && QDataItem::AudioTrack == mSelectedItem->GetType()) {
      emit audiotrackChanged(true);
    }
  }
  audiotrack= new QAudioTrackItem(mSessionItem);
  audiotrack->SetType(QDataItem::AudioTrack);
  audiotrack->setText(0, tr("AudioTrack[%1]").arg(mAudioCount));
  audiotrack->setExpanded(true);

  return audiotrack;
}

QDataItem* CommonTreeWidget::addAudioToNewAudioTrack(const QString &path)
{
    if (!isAudioFile(path) || mAudioCount >= 99)
        return nullptr;

    // Add new audio track
    QDataItem *audio_track = addAudioTrack();
    // Add audio
    if (nullptr != audio_track)
        return addAudioToEmptyAudioTrack(audio_track, path);
    else
        return nullptr;
}

QDataItem* CommonTreeWidget::replaceAudioInAudioTrack(QDataItem *audio_track, const QString &path)
{
    if (audio_track->childCount() != 1)
        return nullptr;
    audio_track->removeChild(audio_track->child(0));

    // Add new audio
    return addAudioToEmptyAudioTrack(audio_track, path);
}

QDataItem* CommonTreeWidget::addAudioToEmptyAudioTrack(QDataItem *audio_track, const QString &path)
{
    return nullptr;
    /*
  if (audio_track->childCount() != 0)
    return nullptr;
  QFileInfo file_info(path);
  QDataItem *audioItem;
  audioItem = new QDataItem(audio_track);
  audioItem->setFlags((audioItem->flags() & (~Qt::ItemIsDropEnabled)));
  audioItem->SetType(QDataItem::File);



  //CDTEXT
  if(BASS_GetDevice()==-1){
      if (!BASS_Init(-1, 44100, 0, NULL, NULL)){
          QMessageBox::about(this, tr("Information"), tr("Error Init Bass"));
      }
      BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 100 * 100);
  }

  BASS_StreamFree(str);
  str= BASS_StreamCreateFile(FALSE,path.toLatin1().data(),0,0,0);

  //Now we read the tags with the bass.dll and tag.dll

  const char* fmt = "[%TITL|%ARTI|%ALBM|%GNRE|%YEAR|%CMNT|%TRCK|%COMP|%COPY|%SUBT|%AART]";
  const char* tags = TAGS_Read( str, fmt );

  QString tmpComment = "";

  if( *tags  )
  {
      QString tagsString (tags);
      tagsString.remove('[');
      tagsString.remove(']');
      QStringList tagsList = tagsString.split("|");

      QAudioTrackItem *audioTrack = (QAudioTrackItem*)audio_track;
      audioTrack->setPerformer(tagsList.at(1)); //ARTIST
      audioTrack->setTitle(tagsList.at(0)); //TITLE
      audioTrack->setMessage(tagsList.at(5)); //COMMENT
      audioTrack->setComposer(tagsList.at(7)); //COMPOSER

      if(addTRackCDTextToDisc==true){
        QDiskItem *diskItem = (QDiskItem*)mHeadItem;
        diskItem->setTitle(tagsList.at(2)); //ALBUM NAME
        diskItem->setPerformer(tagsList.at(10)); //ALBUM ARTIST
      }

      tmpComment += tagsList.at(1);
      if(tmpComment.length()>0) tmpComment += " - ";
      tmpComment += tagsList.at(0);

  }

  double realAudioLength = 0;

  int audioLength = BASS_ChannelGetLength(str,BASS_POS_BYTE);
  if(audioLength!=-1){
    realAudioLength = BASS_ChannelBytes2Seconds(str, audioLength);
    //QMessageBox::about(this, tr("Information"), QDateTime::fromTime_t(audioLength).toUTC().toString("hh:mm:ss"));
  }

  audioItem->SetDataAudio(file_info.absoluteFilePath(),realAudioLength,tmpComment);

  //A Track on an audio CD has 44100 and 16 bits so we need to calculate the seconds with 176400 to get the size on CD

  audio_track->SetDataSize(realAudioLength*176400);
  audio_track->SetDataTime(realAudioLength);

  BASS_StreamFree(str);

  return audio_track;
  */
}

void CommonTreeWidget::updateAudioTrackName()
{
  int start = IsDataTrackExist() ? 1 : 0;
  //qDebug() << QString::number(start);
  for (int i = start; i < mSessionItem->childCount(); i++) {
    if(start==1){
      mSessionItem->child(i)->setText(0, tr("AudioTrack[%1]").arg(i));
    }else{
      mSessionItem->child(i)->setText(0, tr("AudioTrack[%1]").arg(i+1));
    }
  }
}

void CommonTreeWidget::clearAudio()
{
  GetSelectedItem();
  if (mSelectedItem == nullptr || QDataItem::AudioTrack != mSelectedItem->GetType()) {
    QMessageBox::warning(this, tr("Error"), tr("Cannot clear audio!"));
    return;
  }

  mSelectedItem->removeChild(mSelectedItem->child(0));
  QAudioTrackItem *audio_track = (QAudioTrackItem*)mSelectedItem;
  audio_track->resetAll();

  emit contentsChanged(true);
}

void CommonTreeWidget::insertAudio()
{
  QString path = QFileDialog::getOpenFileName(this, QString(), QString(),
                                              tr("Audios (*.aac *.mp3 *.ogg *.wav *.m4a *.mp2 *.mp1 *.aiff)"));
  if (path != "") {
    InsertItem(path);
    //emit datatrackChanged();
  }
}

void CommonTreeWidget::showCDText() {

  QDataItem *selected = GetSelectedItem();

  if(selected == nullptr) return;
  QTreeWidgetItem *checkParent = selected->parent();
  if (checkParent != mSessionItem && checkParent->parent()!= mSessionItem) return;
  if (((QDataItem*)checkParent)->GetType()!= QDataItem::AudioTrack && selected->GetType() != QDataItem::AudioTrack) return;

  QAudioTrackItem *audio_track;
  if(selected->GetType() == QDataItem::AudioTrack){
    audio_track = (QAudioTrackItem*)selected;
  }else{
    audio_track = (QAudioTrackItem*)checkParent;
  }

  VCDDialog dialog(this, audio_track);
  //dialog.show();
  if (dialog.exec() == QDialog::Accepted) {
    audio_track->setArranger(dialog.getArranger());
    audio_track->setComposer(dialog.getComposer());
    audio_track->setSongWriter(dialog.getSongWriter());
    audio_track->setPerformer(dialog.getPerformer());
    audio_track->setMessage(dialog.getMessage());
    audio_track->setTitle(dialog.getTitle());
    audio_track->setUPCEAN(dialog.getUPCEAN());
    audio_track->setPause(dialog.getPause());
    audio_track->setPauseInFrames(dialog.getPauseInFrames());
  }
}

void CommonTreeWidget::showProperty() {
  if (QDataItem::Disk == mSelectedItem->GetType()) {
    showProperty_Disk();
  } else {
    showProperty_File();
  }
}

void CommonTreeWidget::showProperty_File() {
  VPropertyDialog dialog(this, mSelectedItem);
  if (dialog.exec() == QDialog::Accepted) {
    QString newName = dialog.getName();
    if (mSelectedItem->GetName() != newName && newName != "") {
      QFileInfo info(mSelectedItem->GetName());
      QString old_suffix = info.suffix();
      info.setFile(newName);
      QString new_suffix = info.suffix();
      if (old_suffix != "")
        newName = info.baseName()+"."+old_suffix;
      else
        newName = info.baseName();
      mSelectedItem->SetName(newName);
      mSelectedItem->setText(0, dialog.getName());
    }

    mSelectedItem->SetHidden(dialog.getHidden());
    mSelectedItem->SetHiddenExt(dialog.getHiddenExt());
    mSelectedItem->SetPriority(dialog.getPriority());
    mSelectedItem->SetDateAdded(dialog.getDateAdded());
    mSelectedItem->SetDateCreated(dialog.getDateCreated());
    mSelectedItem->SetDateModified(dialog.getDateModified());

    if (QDataItem::File == mSelectedItem->GetType() && dialog.didSync()) {
      QFileInfo entry(mSelectedItem->GetFullPath());
      // update parent
      qint64 old_size = mSelectedItem->GetDataSize();
      updateDataSize((QDataItem*)mSelectedItem->parent(), entry.size()-old_size, 0, 0);
      // update self
      mSelectedItem->SetDataSize(entry.size());
    }
    mSelectedItem->SetComment(dialog.getComment());
    mSelectedItem->setText(2, dialog.getComment());
  }
}

void CommonTreeWidget::showProperty_Disk() {
  QDataItem *selected = GetSelectedItem();
  if (selected->GetType() != QDataItem::Disk) {
    return;
  }
  QDiskItem *item = (QDiskItem*)selected;

  VDiskPropertyDialog dialog(this, (QDiskItem*)mHeadItem, mAudioCount, mDataTrackItem != nullptr);
  if (dialog.exec() == QDialog::Accepted) {
    item->SetName(dialog.getDiskName());
    if (mAudioCount){
      item->setArranger(dialog.getArranger());
      item->setComposer(dialog.getComposer());
      item->setSongWriter(dialog.getSongWriter());
      item->setPerformer(dialog.getPerformer());
      item->setMessage(dialog.getMessage());
      item->setTitle(dialog.getTitle());
      item->setUPCEAN(dialog.getUPCEAN());
    }

    if(mDataTrackItem==nullptr){ //This is the basic information of the disk, no matter if used or not
        item->SetText0(dialog.getDiskName());
        item->setDiskDateExpiration(dialog.getDateTimeExpiration());
        item->setDiskDateEffective(dialog.getDateTimeEffective());
        item->setDiskDateCreation(dialog.getDateTimeCreation());
        item->setDateMdification(dialog.getDateTimeMdification());
    }

    if (mDataTrackItem!=nullptr) {
      item->setDiskDateExpiration(dialog.getDateTimeExpiration());
      item->setDiskDateEffective(dialog.getDateTimeEffective());
      item->setDiskDateCreation(dialog.getDateTimeCreation());
      item->setDateMdification(dialog.getDateTimeMdification());


    }

    item->setFilterList(dialog.getFilterList());
    item->setByDate(dialog.getByDate());
    item->setDateFrom(dialog.getDateFrom());
    item->setDateTo(dialog.getDateTo());
  }
}

void CommonTreeWidget::showPlay() {
    GetSelectedItem();
    if (mSelectedItem == nullptr)
      return;

    QString mFile = mSelectedItem->GetFullPath();
    if(mFile==""){
        if(mSelectedItem->childCount()>0){
            QDataItem *virtualItem = (QDataItem *)mSelectedItem->child(0);
            mFile = virtualItem->GetFullPath();
        }
    }

    if(BASS_GetDevice()==-1){
        if (!BASS_Init(-1, 44100, 0, NULL, NULL)){
            QMessageBox::about(this, tr("Information"), tr("Error Init Bass"));
        }
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 100 * 100);
    }

    BASS_StreamFree(str);

    //QString mFile = mSelectedItem->GetFullPath();
    str= BASS_StreamCreateFile(FALSE,mFile.toLatin1().data(),0,0,0);

    if (!BASS_ChannelPlay(str,TRUE)){
      BASS_StreamFree(str);
      QMessageBox::about(this, tr("Information"), tr("Error Play with Bass"));
    }


}

void CommonTreeWidget::showStop() {

    BASS_ChannelStop(str);
    BASS_StreamFree(str);
}

void CommonTreeWidget::dataTrackMode1() {
  if (mDataTrackItem != nullptr) {
    mDataTrackItem->SetDataTrackMode(1);
  }
}

void CommonTreeWidget::dataTrackMode2() {
  if (mDataTrackItem != nullptr) {
    mDataTrackItem->SetDataTrackMode(2);
  }
}

void CommonTreeWidget::on_add_tree_completed(QDataItem *parent, QDataItem *item) {
  if (item != nullptr) {
    // update parents and whole tree
    updateDataSize(parent, item->GetDataSize(), item->GetDataItemCount(), item->GetDataNodeCount());
    parent->setExpanded(true);
    item->setExpanded(true);
  }
  resetQueryAddAudioTrack();
  resetQueryReplaceFile();
  mQueryResetBeforeImport = true;
  mModified = true;
  setSelected(item);
  mBlockSpinner->stop();
  mBlockSpinner->deleteLater();
  //We need to Fire it here because the Spinner is topwindow. Maybe remove the spinner?
  emit contentsChanged(true);

}

bool CommonTreeWidget::queryAddAudioTrack() {
//  QMessageBox mb(tr("Information"), tr("Create AudioTrack for audio files?"),
//                 QMessageBox::Question, QMessageBox::Yes|QMessageBox::Default,
//                 QMessageBox::No|QMessageBox::Escape, QMessageBox::NoButton);
//  mNeedQueryAddAudioTrack = false;
//  if (mb.exec() == QMessageBox::Yes) {
//    mDoAddAudioTrack = true;
//    return true;
//  } else {
//    mDoAddAudioTrack = false;
//    return false;
//  }

  mNeedQueryAddAudioTrack = false;
  if (ConfigurationPage::mSettings.value("AudioToAudioTrack", false).toBool()) {
    mDoAddAudioTrack = true;
  } else {
    mDoAddAudioTrack = false;
  }
  return true;
}

void CommonTreeWidget::resetQueryAddAudioTrack() {
    mNeedQueryAddAudioTrack = true;
    mDoAddAudioTrack = false;
}

int CommonTreeWidget::findSameNameInLevel(QDataItem *parent, const QString &path, Qt::CaseSensitivity cs)
{
  QFileInfo fileinfo(path);
  QString filename = fileinfo.fileName();
  for (int i = 0; i < parent->childCount(); i++) {
    //if (filename == parent->child(i)->text(0)) {
    if (0 == QString::compare(filename, parent->child(i)->text(0), cs)) {
      return i;
    }
  }
  return -1;
}

bool CommonTreeWidget::queryReplaceFile(const QString &path)
{
  QFileInfo fileinfo(path);
  QString filename = fileinfo.fileName();
  ZReplaceFileDialog dialog(this, filename);
  if (dialog.exec() == QDialog::Accepted) {
    mDoReplaceFile = true;
  } else {
    mDoReplaceFile = false;
  }
  if (dialog.DoReplaceActionForAll()) {
    mNeedQueryReplaceFile = false;
  } else {
    mNeedQueryReplaceFile = true;
  }
  return mDoReplaceFile;
}

void CommonTreeWidget::resetQueryReplaceFile()
{
    mNeedQueryReplaceFile = true;
    mDoReplaceFile = false;
}

bool CommonTreeWidget::isFiltered(QFileInfo info)
{
  QDiskItem *diskItem = (QDiskItem*)mHeadItem;

  QDate create = info.birthTime().date();
  QDate from = QDate::fromString(diskItem->getDateFrom());
  QDate to = QDate::fromString(diskItem->getDateTo());

  if (diskItem->getByDate()) {
    if (create.operator >=(from) && create.operator <=(to)) {
      return true;
    }
  } else {
    QStringList *list = diskItem->getFilterList();
    for (int i=0; i<list->count(); i++) {
      if (0 == info.suffix().compare(list->at(i)))
        return true;
    }
  }

  return false;
}

bool CommonTreeWidget::getIsFSSyncAvailable()
{
  if (mProjectType != RuleManager::TYPE_PROJECT_OPEN && mProjectType != RuleManager::TYPE_PROJECT_ISOUDF)
    return false;

  if (mDataTrackItem == nullptr || 0 != mAudioCount)
    return false;

  if (mDataTrackItem->childCount() == 0)
    return false;

  for (int i=0; i<mDataTrackItem->childCount(); i++) {
    QDataItem *child = (QDataItem *)mDataTrackItem->child(i);
    if (child->GetType() != QDataItem::VirtualFolder)
      return false;
  }
  return true;
}

bool CommonTreeWidget::getIsFSUnsyncAvailable()
{
  if (mProjectType != RuleManager::TYPE_PROJECT_OPEN && mProjectType != RuleManager::TYPE_PROJECT_ISOUDF)
    return false;

  if (mDataTrackItem == nullptr || 0 != mAudioCount)
    return false;

  if (mDataTrackItem->childCount() == 2) {
    QDataItem *child = (QDataItem *)mDataTrackItem->child(0);
    if (child->GetType() == QDataItem::VirtualFolder) {
      return false;
    }
  }

  return true;
}

void CommonTreeWidget::changeFolderPos(QDataItem *parent, QDataItem *child_folder)
{
  int index = -1;
  for (int i = 0; i < parent->childCount(); i++) {
    QDataItem *child = (QDataItem *)parent->child(i);
    if (QDataItem::FixedFolder != child->GetType()
        && QDataItem::Folder != child->GetType()) {
      index = i;
      break;
    }
  }
  if (-1 != index) {
    parent->insertChild(index, parent->takeChild(parent->indexOfChild(child_folder)));
  }
}
int CommonTreeWidget::importFromRoot(QDataItem *parent, const QString &root,
                                     qint64 &data_size, int &item_count, int &node_count)
{
  //if (importCheck(root) == -1)
  //  return -1;
  importCheck(root);

  QDir dir(root);
  QFileInfoList entries = dir.entryInfoList();
  for (int i = 0; i < entries.size(); i++) {
    QFileInfo entry = entries[i];
    if ("." == entry.fileName() || ".." == entry.fileName())
      continue;
    if (entry.isDir()) {
      importTree(parent, entry.absoluteFilePath(), data_size, item_count, node_count, true);
    } else if (entry.isFile()) {
      QDataItem *childItem = nullptr;
      childItem = importItem(parent, entry.absoluteFilePath());
      if (childItem != nullptr) {
        data_size += childItem->GetDataSize();
        item_count += 1;
      }
    }
  }
  return 0;
}

QDataItem *CommonTreeWidget::importTree(QDataItem *parent, const QString &path,
                                        qint64 &data_size, int &item_count, int &node_count,
                                        bool sort)
{
  if (ZImportStrategy::ERROR_SUBFOLDER_NOT_ALLOWED == importCheck(path))
    return nullptr;

  QDataItem *item;

  int index = findSameNameInLevel(parent, path, Qt::CaseInsensitive);
  if (index != -1) {
    item = (QDataItem*)(parent->child(index));
  } else {
    // if not find same name, we create new one
    item = new QDataItem(parent);
    //item->setData(0, Qt::UserRole, "");
    //item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->SetType(QDataItem::Folder);
    item->SetData(path);
    item->SetDataNodeCount(1);
  }

  QDir dir(path);
  QFileInfoList entries = dir.entryInfoList();
  qint64 sum = 0;
  int count = 0; // for item
  int count2 = 1; // for node, we have create one node above, so here is 1
  if (sort) {
    for (int i = 0; i < entries.size(); i++) {
      QFileInfo entry = entries[i];
      if ("." == entry.fileName() || ".." == entry.fileName()
          || entry.isFile())
        continue;
      if (entry.isDir()) {
        importTree(item, entry.absoluteFilePath(), sum, count, count2, sort);
      }
    }
    for (int i = 0; i < entries.size(); i++) {
      QFileInfo entry = entries[i];
      if ("." == entry.fileName() || ".." == entry.fileName()
          || entry.isDir())
        continue;
      if (entry.isFile()) {
        addFile(item, entry, sum, count);
      }
    }
  } else {
    for (int i = 0; i < entries.size(); i++) {
      QFileInfo entry = entries[i];
      if ("." == entry.fileName() || ".." == entry.fileName())
        continue;
      if (entry.isDir()) {
        importTree(item, entry.absoluteFilePath(), sum, count, count2, sort);
      } else if (entry.isFile()) {
        addFile(item, entry, sum, count);
      }
    }
  }
  item->SetDataSize(sum);
  item->SetDataItemCount(count);
  item->SetDataNodeCount(count2);
  data_size += sum;
  item_count += count;
  node_count += count2;
  return item;
}

QDataItem* CommonTreeWidget::importItem(QDataItem *parent, const QString &path)
{
  if (importCheck(path) != ZImportStrategy::ERROR_NO)
    return nullptr;
  return InsertItem(parent, path, true);
}

ZImportStrategy::ImportError CommonTreeWidget::importCheck(const QString &path)
{
  ZImportStrategy::ImportError res = ZImportStrategy::ERROR_NO;
  QString new_path = path;
  new_path.remove(mRootToBeImport);
  for (int i = 0; i < mImportStrategies.size(); i++) {
    ZImportStrategy::ImportError temp_res = mImportStrategies[i]->Check(mRootToBeImport, new_path);
    if (temp_res != ZImportStrategy::ERROR_NO) {
      res = temp_res;
      mImportErr += mImportStrategies[i]->GetError();
    }
  }
  return res;
}

ZImportStrategy::ImportError CommonTreeWidget::importCheck2(
    const QString &folder_name, const QString &path, bool silent)
{
  ZImportStrategy::ImportError res = ZImportStrategy::ERROR_NO;
  for (int i = 0; i < mImportStrategies.size(); i++) {
    res = mImportStrategies[i]->Check2(folder_name, path);
    if (res != ZImportStrategy::ERROR_NO) {
      if (!silent)
        QMessageBox::information(this, tr("Error"), mImportStrategies[i]->GetError());
      break;
    }
  }
  return res;
}

void CommonTreeWidget::addFixedFolder(const QString &parent, const QStringList &folders)
{
  mFixedFolderMap.insert(parent, folders);
}

void CommonTreeWidget::createFixedFolder()
{
    QList<QString> keys = mFixedFolderMap.keys();
    for (int i = 0; i < keys.size(); i++) {
        QString parent_name = keys[i];
        QDataItem *parent;
        if ("" == parent_name) {
            parent = mDataTrackItem;
        } else {
            parent = findItemByName(mDataTrackItem, parent_name);
        }
        if (parent != nullptr) {
            for (int j = 0; j < mFixedFolderMap.value(parent_name).size(); j++) {
                QDataItem *folder = new QDataItem(parent);
                folder->SetType(QDataItem::FixedFolder);
                folder->SetDataWithName(mFixedFolderMap.value(parent_name)[j]);
                folder->setFlags(folder->flags() & (~Qt::ItemIsEditable));
            }
        }
    }
}

QDataItem* CommonTreeWidget::findItemByName(QDataItem *parent, const QString &name)
{
    QDataItem *res = nullptr;

    if (parent->GetName() != name) {
        for (int i = 0; i < parent->childCount(); i++) {
            res = findItemByName((QDataItem*)parent->child(i), name);
            if (res != nullptr)
                 break;
        }
    } else {
        res = parent;
    }

    return res;
}

void CommonTreeWidget::doFSSync(const QString &remove, const QString &keep)
{
  QDataItem *child_to_remove = nullptr, *child_to_keep = nullptr;
  QList<QTreeWidgetItem*> listVirtual;
  for (int i=0; i<mDataTrackItem->childCount(); i++) {
    QDataItem *child = (QDataItem *)mDataTrackItem->child(i);
    if (child->GetType() != QDataItem::VirtualFolder)
      continue;
    if (0 == child->GetName().compare(remove)) {
      child_to_remove = child;
    } else if (0 == child->GetName().compare(keep)) {
      child_to_keep = child;
    }
  }
  if (child_to_remove != nullptr && child_to_keep != nullptr) {
    removeChildWithUpdatedInfo(mDataTrackItem, child_to_remove);
    qint64 size = mDataTrackItem->GetDataSize();
    int item_count = mDataTrackItem->GetDataItemCount();
    int node_count = mDataTrackItem->GetDataNodeCount();
    listVirtual = takeChildrenWithUpdatedInfo(child_to_keep);
    removeChildWithUpdatedInfo(mDataTrackItem, child_to_keep);
    addChildrenWithUpdatedInfo(mDataTrackItem, listVirtual, size, item_count, node_count-1);
  }
}

void CommonTreeWidget::FSSync()
{
  int syncType = ConfigurationPage::mSettings.value("FSsyncType", 3).toInt();
  //QList<QTreeWidgetItem*> listVirtual;

  if (1 == syncType) {
    doFSSync(KStrISO, KStrUDF);
  } else if (2 == syncType || 3 == syncType) {
    doFSSync(KStrUDF, KStrISO);
  }

  emit contentsChanged(true);
}

void CommonTreeWidget::FSUnsync()
{
  QList<QTreeWidgetItem*> list;
//  QList<QTreeWidgetItem*> listCopy;
  if (!getIsFSUnsyncAvailable())
    return;

  if (mDataTrackItem->childCount() == 1
      && ((QDataItem *)mDataTrackItem->child(0))->GetType() == QDataItem::VirtualFolder) {
    QDataItem *child = (QDataItem *)mDataTrackItem->child(0);
    QDataItem *cloneChild =(QDataItem*)(child->clone());

    if (0 == child->GetName().compare(KStrISO)) {
      cloneChild->SetDataWithName(KStrUDF);
    } else  if (0 == child->GetName().compare(KStrUDF)) {
      cloneChild->SetDataWithName(KStrISO);
    }
    addChildWithUpdatedInfo(mDataTrackItem, cloneChild);
  } else {
    QDataItem *isoItem = addFSVirtualFolder(KStrISO);
    QDataItem *udfItem;//= addFSLiveFolder(KStrUDF);
    qint64 size = mDataTrackItem->GetDataSize();
    int item_count = mDataTrackItem->GetDataItemCount();
    int node_count = mDataTrackItem->GetDataNodeCount();
    list = takeChildrenWithUpdatedInfo(mDataTrackItem);
    addChildrenWithUpdatedInfo(isoItem, list, size, item_count, node_count);
    udfItem = (QDataItem*)(isoItem->clone());
    udfItem->SetDataWithName(KStrUDF);
    addChildWithUpdatedInfo(mDataTrackItem, isoItem);
    addChildWithUpdatedInfo(mDataTrackItem, udfItem);
  }

  emit contentsChanged(true);
}

QDataItem* CommonTreeWidget::addFSVirtualFolder(const QString &name)
{
  QDataItem *item = new QDataItem();
  item->SetType(QDataItem::VirtualFolder);
  item->SetDataWithName(name);
  item->SetDataNodeCount(1);

  mDataTrackItem->setExpanded(true);
  return item;
}

QList<QTreeWidgetItem*> CommonTreeWidget::takeChildrenWithUpdatedInfo(QDataItem *parent)
{
    QList<QTreeWidgetItem*> children;
    children = parent->takeChildren();
    updateDataSize(parent, -parent->GetDataSize(), -parent->GetDataItemCount(),
                 -parent->GetDataNodeCount());

    return children;
}

void CommonTreeWidget::removeChildWithUpdatedInfo(QDataItem *parent, QDataItem *child)
{
  parent->removeChild(child);
  updateDataSize(parent, -child->GetDataSize(), -child->GetDataItemCount(),
                 -child->GetDataNodeCount());
}

void CommonTreeWidget::addChildrenWithUpdatedInfo(QDataItem *parent,
    const QList<QTreeWidgetItem*> &children,
    qint64 data_size, int item_count, int node_count)
{
    parent->addChildren(children);
    updateDataSize(parent, data_size, item_count, node_count);
}

void CommonTreeWidget::addChildWithUpdatedInfo(QDataItem *parent, QDataItem *child)
{
    parent->addChild(child);
    updateDataSize(parent, child->GetDataSize(), child->GetDataItemCount(),
                 child->GetDataNodeCount());
}

void CommonTreeWidget::ResetFiles()
{
    GetSelectedItem();

    if (RuleManager::TYPE_PROJECT_OPEN == mProjectType) {
        if (mSelectedItem == nullptr || mSelectedItem->GetType() == QDataItem::Disk
          || (mSelectedItem->GetType() != QDataItem::DataTrack
              && mSelectedItem->GetType() != QDataItem::VirtualFolder))
        mSelectedItem = mSessionItem;
    }else{
        mSelectedItem = mDataTrackItem;
        mSelectedItem->setSelected(true);
    }


    if(mSelectedItem->GetType() == QDataItem::AudioTrack) mAudioCount = 0;

    if (getIsFSSyncAvailable() && mSelectedItem == mDataTrackItem) {
        for (int i=0; i<mSelectedItem->childCount(); i++) {
            QDataItem *virtualItem = (QDataItem *)mSelectedItem->child(i);
            takeChildrenWithUpdatedInfo(virtualItem);
        }
    } else {
        if (RuleManager::TYPE_PROJECT_OPEN != mProjectType) {
            takeChildrenWithUpdatedInfo(mDataTrackItem);
            createFixedFolder();
            mQueryResetBeforeImport = false;
        } else {
            takeChildrenWithUpdatedInfo(mSelectedItem);
            if (mSelectedItem == mSessionItem) {
                mDataTrackItem = nullptr;
                mAudioCount = 0;
            }
        }
    }

    mModified = true;
    resetQueryAddAudioTrack();
    resetQueryReplaceFile();

    emit contentsChanged(true);
}

int CommonTreeWidget::getTotalTimeExplorer()
{
    double totalTime = 0.0;

    QTreeWidgetItemIterator it(topLevelItem(0));

    while (*it) {
        if (((QDataItem*)(*it))->GetType()==QDataItem::AudioTrack){

            QTrackItem *track = dynamic_cast<QTrackItem*>(*it);
            STrackInfo info = track->GetTrackInfo();

            double nSeconds = (info.nSize*2352)/(double)176400.00;

            totalTime = totalTime + nSeconds;

        }
        ++it;
    }

    return totalTime;
}

int CommonTreeWidget::getTotalTime()
{
    int totalTime = 0;
    int start = IsDataTrackExist() ? 1 : 0;

    //qDebug() << QString::number(mSessionItem->childCount());
    for (int i = start; i < mSessionItem->childCount(); i++) {
      QDataItem *vItem = (QDataItem *)mSessionItem->child(i);
      if(vItem->GetType() == QDataItem::AudioTrack){
          QDataItem *subItem = (QDataItem *)vItem->child(0);
          if(subItem){
            //qDebug() << QString::number(subItem->getItemPlayTime());
            totalTime = totalTime + subItem->getItemPlayTime();
          }
      }
    }
    //Ask when more than 74 Mins

    return totalTime;
}

int CommonTreeWidget::audioTrackCount()
{
    int nCounter = 0;
    int start = IsDataTrackExist() ? 1 : 0;
    for (int i = start; i < mSessionItem->childCount(); i++) {
        QDataItem *vItem = (QDataItem *)mSessionItem->child(i);
        if(vItem->GetType() == QDataItem::AudioTrack){
            nCounter++;
        }
    }
    //qDebug() << QString::number(nCounter);
    return nCounter;
}

void CommonTreeWidget::randomizeAudioTracks()
{
    int maxCount = audioTrackCount();
    int start = IsDataTrackExist() ? 1 : 0;
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    for (int i = start; i < start+maxCount; i++) {
        int randomValue = qrand() % ((maxCount) - start) + start;
        //qDebug() << QString::number(i) + " // " + QString::number(randomValue);
        QTreeWidgetItem *child = mSessionItem->takeChild(i);
        mSessionItem->insertChild(randomValue, child);
        child->setExpanded(true);
    }
    //qDebug() << "Ende";

    updateAudioTrackName();

    //Submit dataTrackChanged to get the function "UpdateMenus" on the main screen.
    emit datatrackChanged();
}

QString CommonTreeWidget::buildDiskName()
{
    QString strPattern;

    if(ConfigurationPage::mSettings.value("LabelIsSimple", true).toBool()==true){ //IsSimple
        strPattern =  ConfigurationPage::mSettings.value("DefaultDiskName", tr("New")).toString();
    }else{
        int nCounter = ConfigurationPage::mSettings.value("CustomDiskNameNumber", tr("001")).toInt();
        nCounter++;
        //SaveBack
        ConfigurationPage::mSettings.setValue("CustomDiskNameNumber", QString::number(nCounter));

        strPattern = ConfigurationPage::mSettings.value("CustomDiskNamePattern", tr("%S-%D-%C")).toString();

        QString strName = ConfigurationPage::mSettings.value("CustomDiskNameString", tr("New")).toString();
        QString strDate = QDate::currentDate().toString("yyyy-MM-dd");



        if(strPattern.contains("%S", Qt::CaseInsensitive)==true){
            strPattern.replace("%S",strName);
        }
        if(strPattern.contains("%D", Qt::CaseInsensitive)==true){
            strPattern.replace("%D",strDate);
        }
        if(strPattern.contains("%C", Qt::CaseInsensitive)==true){
            strPattern.replace("%C",QString::number(nCounter));
        }

    }

    return strPattern;
}

void CommonTreeWidget::createIndex()
{
    GetSelectedItem();

    if(mSelectedItem == nullptr) return;

    QString fileName =  QFileDialog::getSaveFileName(
                    this,
                    tr("Save Track List"),
                    QDir::currentPath(),
                    tr("Text file (*.txt)") );

    if (fileName.isEmpty())
      return;

    QFile outputFile(fileName);
    outputFile.open(QIODevice::WriteOnly|QFile::Text);

    if(!outputFile.isOpen()){
        return;
    }

    QTextStream out(&outputFile);

    //First we need to clear the vector
    QTreeWidgetItemIterator it(mSelectedItem);
    while (*it) {

        if(((QDataItem*)(*it))->GetType()==QDataItem::Disk){
            //First we read out Artist and Album Name from disk
            //out << strLine << endl;
            QMediumItem *medium = dynamic_cast<QMediumItem*>((QDataItem*)(*it));

            QString strLine = QString("Album: %1 - %2").arg(medium->getPerformer(),medium->getTitle());
            out << strLine << endl;

        }
        if(((QDataItem*)(*it))->GetType()==QDataItem::AudioTrack){
            //Then we read out the Details
            QTrackItem *track = dynamic_cast<QTrackItem*>((QDataItem*)(*it));
            STrackInfo info = track->GetTrackInfo();

            int nSeconds = (info.nSize*2352)/176400;


            QString strLine = QString("%1 - %2 - %3").arg(QString::number(info.nTrackNumber),track->getTitle(),QDateTime::fromTime_t(nSeconds).toUTC().toString("hh:mm:ss"));
            out << strLine << endl;

        }

        ++it;
    }

    outputFile.close();
}

void CommonTreeWidget::createContentList()
{
    GetSelectedItem();

    if(mSelectedItem == nullptr) return;

    QString fileName =  QFileDialog::getSaveFileName(
                    this,
                    tr("Save Content List"),
                    QDir::currentPath(),
                    tr("Text file (*.txt)") );

    if (fileName.isEmpty())
      return;

    QFile outputFile(fileName);
    outputFile.open(QIODevice::WriteOnly|QFile::Text);

    if(!outputFile.isOpen()){
        return;
    }

    QTextStream out(&outputFile);

    //First we need to clear the vector
    QTreeWidgetItemIterator it(mSelectedItem);
    while (*it) {

        if(dynamic_cast<QDataItem*>(*it)->GetType()==QDataItem::File){
            //Lba?
        }
        if(dynamic_cast<QDataItem*>(*it)->GetType()==QDataItem::Folder){
            //Lba
        }
        QString myItemLBA = dynamic_cast<QDataItem*>(*it)->GetLBA();
        QString myItemPath = dynamic_cast<QDataItem*>(*it)->GetFullPath();
        QString myItemName = dynamic_cast<QDataItem*>(*it)->GetName();


        //QString strLine = QString("%1, %2\n").arg(myItemLBA,myItemPath);
        QString strLine = QString("%1, %2, %3").arg(myItemLBA,humanReadableSize(((QDataItem*)(*it))->GetDataSize()),myItemPath);
        //endl schreibt wohl den Linefeed
        out << strLine << endl;
        //out.flush();
        //convertToFoxValue(((QDataItem*)(*it))->GetDiskPath());
        //convertToFoxValue(((QDataItem*)(*it))->GetName());
        //convertToFoxValue(((QDataItem*)(*it))->GetDiskPath()
        ++it;
    }

    outputFile.close();
}

void CommonTreeWidget::setRootItem()
{

    mSelectedItem = (QDataItem*)topLevelItem(0);
    if (nullptr != mSelectedItem) {
      mSelectedItem->setSelected(true);
    }

}

void CommonTreeWidget::getTags(int32 sourceTags)
{
    audioTagInfo tagInfo;
    if(tagInfo.inititalize(sourceTags)==false) return;
    audioTags tagValues;

    QTreeWidgetItemIterator it(topLevelItem(0));


    while (*it) {
        if (((QDataItem*)(*it))->GetType()==QDataItem::AudioTrack){

            QTrackItem *track = dynamic_cast<QTrackItem*>(*it);
            STrackInfo info = track->GetTrackInfo();

            tagValues = tagInfo.getData(info.nTrackNumber);

            track->setPerformer(tagValues.m_Artist);
            track->setAlbum(tagValues.m_Album);
            track->setTitle(tagValues.m_Title);

            QString strInfo = QString("%1 - %2").arg(tagValues.m_Artist,tagValues.m_Title);
            track->SetAudioTrackInfo(strInfo);

        }
        ++it;
    }


}

void CommonTreeWidget::calculateRealSizes()
{
    QTreeWidgetItemIterator it(topLevelItem(0));

    int16 eMedium;
    int64 totalSize = 0;
    int64 sessionSize = 0;
    QMediumItem *medium = nullptr;
    QSessionItem *sessionItem = nullptr;

    while (*it) {

        if(((QDataItem*)(*it))->GetType()==QDataItem::Disk){
            medium = dynamic_cast<QMediumItem*>((QDataItem*)(*it));
            eMedium = medium->getExtendedMediumType();
        }
        if(((QDataItem*)(*it))->GetType()==QDataItem::Session){
            if(sessionSize>0){
                sessionItem->setSessionSize(sessionSize);
                sessionSize = 0;
            }
            sessionItem = dynamic_cast<QSessionItem*>((QDataItem*)(*it));

        }
        if(((QDataItem*)(*it))->GetType()==QDataItem::DataTrack){
            QTrackItem *track = dynamic_cast<QTrackItem*>(*it);
            STrackInfo info = track->GetTrackInfo();

            qint64 trackSize;
            if(eMedium==BS_EMT_CD_ROM || eMedium==BS_EMT_CD_MULTISESSION || eMedium==BS_EMT_DVD || eMedium==BS_EMT_BD ){
                //I set nTrack from SDK because the sector size is 2048
                trackSize = info.nSize*2048;
                totalSize = totalSize + trackSize;
                sessionSize = sessionSize + trackSize;
                track->setSectorSize(2048);
            }
            if(eMedium==BS_EMT_CD_ROM_XA){
                //nTrack 1=2048
                //nTrack > 1 = 2324
                if(info.nTrackNumber==1){
                    trackSize = info.nSize*2048;
                    totalSize = totalSize + trackSize;
                    sessionSize = sessionSize + trackSize;
                    track->setSectorSize(2048);
                }else{
                    trackSize = info.nSize*2324;
                    totalSize = totalSize + trackSize;
                    sessionSize = sessionSize + trackSize;
                    track->setSectorSize(2324);
                }
            }
            if(eMedium==BS_EMT_CD_MIXED_MODE){
                //nTrack 1= 2048
                //Else = AudioTrack
                if(info.nTrackNumber==1){
                    trackSize = info.nSize*2048;
                    totalSize = totalSize + trackSize;
                    sessionSize = sessionSize + trackSize;
                    track->setSectorSize(2048);
                }
            }
            track->setTrackSize(trackSize);
        }
        if(((QDataItem*)(*it))->GetType()==QDataItem::AudioTrack){
            QTrackItem *track = dynamic_cast<QTrackItem*>(*it);
            STrackInfo info = track->GetTrackInfo();
            qint64 trackSize;
            //we calculate with 2352
            trackSize = info.nSize*2352;
            totalSize = totalSize + trackSize;
            sessionSize = sessionSize + trackSize;
            track->setTrackSize(trackSize);
            track->setSectorSize(2352);

        }


        ++it;
    }

    if(sessionItem) sessionItem->setSessionSize(sessionSize);
    if(medium) medium->setMediumSize(totalSize);

    setSizes();
}

void CommonTreeWidget::setSizes()
{
    QTreeWidgetItemIterator it(topLevelItem(0));
    while (*it) {

        if(((QDataItem*)(*it))->GetType()==QDataItem::Disk){
            QMediumItem *medium = dynamic_cast<QMediumItem*>((QDataItem*)(*it));
            medium->setNewSize();
        }
        if(((QDataItem*)(*it))->GetType()==QDataItem::Session){
            QSessionItem *sessionItem = dynamic_cast<QSessionItem*>((QDataItem*)(*it));
            sessionItem->setNewSize();
        }
        if(((QDataItem*)(*it))->GetType()==QDataItem::DataTrack){
            QTrackItem *track = dynamic_cast<QTrackItem*>((QDataItem*)(*it));
            track->setNewSize();
        }
        if(((QDataItem*)(*it))->GetType()==QDataItem::AudioTrack){
            QTrackItem *track = dynamic_cast<QTrackItem*>((QDataItem*)(*it));
            track->setNewSize();
        }

        ++it;
    }
}
