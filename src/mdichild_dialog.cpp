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

//#include <QtGui>
#include <QtWidgets>

#include "mdichild_dialog.h"
#include <QDomDocument>

#include "xbelgenerator.h"
#include "xbelhandler.h"
#include "zfileiconprovider.h"
#include "utils_common.h"

#include "FoxSDKBurningLib.h"
#include "zimageloader.h"
#include "zproject_videodvd.h"
#include "zproject_bluray.h"
#include "zproject_explore.h"
#include <iostream>
#include <cstdlib>
#include "settingspages.h"
#include "qmediumitem.h"

using namespace std;


MdiChildDialog::MdiChildDialog()
{
    setAttribute(Qt::WA_DeleteOnClose);
    createFileExplorers();
    isUntitled = true;
}



MdiChildDialog::MdiChildDialog(RuleManager::ProjectType projectType)
{
    setAttribute(Qt::WA_DeleteOnClose);
    createFileExplorers();
    isUntitled = true;

    createTreeWidget(projectType);

    switch(projectType){
        case RuleManager::TYPE_PROJECT_ISO:
            setWindowIcon(QIcon(":/icons/datacd32.png"));
        break;
        case RuleManager::TYPE_PROJECT_ISOUDF:
            setWindowIcon(QIcon(":/icons/dataudfiso32.png"));
        break;
        case RuleManager::TYPE_PROJECT_UDF:
            setWindowIcon(QIcon(":/icons/dataudf32.png"));
        break;
        case RuleManager::TYPE_PROJECT_AUDIOCD:
            setWindowIcon(QIcon(":/icons/audiocd32.png"));
        break;
        case RuleManager::TYPE_PROJECT_MIXEDMODE:
            setWindowIcon(QIcon(":/icons/mixedmode32.png"));
        break;
        case RuleManager::TYPE_PROJECT_SUPERVIDEO:
            setWindowIcon(QIcon(":/icons/svideocd32.png"));
        break;
        case RuleManager::TYPE_PROJECT_VIDEODVD:
            setWindowIcon(QIcon(":/icons/videodvd32.png"));
        break;
        case RuleManager::TYPE_PROJECT_BLURAYVIDEO:
            setWindowIcon(QIcon(":/icons/bluray32.png"));
        break;
        case RuleManager::RuleManager::TYPE_PROJECT_EXPLORE:
            fileExplorerSplitter->hide();
        break;
        default:
            setWindowIcon(QIcon(":/icons/datacd32.png"));
    break;
    }

}

void MdiChildDialog::createTreeWidget(RuleManager::ProjectType projectType, bool isXbel)
{
    mProjectType = projectType;

    //QMessageBox::information(NULL,"Information","Open");

    QStringList labels;
    if (RuleManager::TYPE_PROJECT_VIDEODVD == mProjectType) {
      treeWidget = new ZProjectVideoDVD();
    } else if (RuleManager::TYPE_PROJECT_BLURAYVIDEO == mProjectType) {
      treeWidget = new ZProjectBluRay();
    } else if (RuleManager::TYPE_PROJECT_EXPLORE == mProjectType) {
      treeWidget = new ZProjectExplore();
    } else {
      treeWidget = new CommonTreeWidget(mProjectType,isXbel);
      //treeWidget->SetProjectType(mProjectType);
    }



    if (RuleManager::TYPE_PROJECT_AUDIOCD == mProjectType || RuleManager::TYPE_PROJECT_MIXEDMODE == mProjectType){
        labels << tr("Disk Node") << tr("Size") << tr("Length") << tr("Description");
        treeWidget->setHeaderLabels(labels);
        treeWidget->header()->setStretchLastSection(true);
        treeWidget->header()->setDefaultSectionSize(350);
        treeWidget->header()->setSectionResizeMode(0, QHeaderView::Interactive);
        treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        treeWidget->header()->setSectionResizeMode(3, QHeaderView::Stretch);
    }else if(RuleManager::TYPE_PROJECT_EXPLORE == mProjectType){
        labels << tr("Disk Node") << tr("LBA") << tr("Size") << tr("Description");
        treeWidget->setHeaderLabels(labels);
        treeWidget->header()->setStretchLastSection(true);
        treeWidget->header()->setDefaultSectionSize(350);
        treeWidget->header()->setSectionResizeMode(0, QHeaderView::Interactive);
        treeWidget->header()->setSectionResizeMode(1, QHeaderView::Interactive);
        treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        treeWidget->header()->setSectionResizeMode(3, QHeaderView::Stretch);
    }else{
        labels << tr("Disk Node") << tr("Size") << tr("Description");
        treeWidget->setHeaderLabels(labels);
        treeWidget->header()->setStretchLastSection(true);
        treeWidget->header()->setDefaultSectionSize(350);
        treeWidget->header()->setSectionResizeMode(0, QHeaderView::Interactive);
        treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        treeWidget->header()->setSectionResizeMode(2, QHeaderView::Stretch);
    }



    // Get default icons
    QStyle *style = treeWidget->style();
    folderOffPixmap = style->standardPixmap(QStyle::SP_DirClosedIcon);
    folderOnPixmap = style->standardPixmap(QStyle::SP_DirOpenIcon);
    filePixmap = style->standardPixmap(QStyle::SP_FileIcon);

    QSplitter *vSplitter = new QSplitter;
    vSplitter->setOrientation(Qt::Vertical);
    fileExplorerSplitter = new QSplitter;
    vSplitter->addWidget(fileExplorerSplitter);
    vSplitter->addWidget(treeWidget);
    fileExplorerSplitter->addWidget(treeFileExplorer);
    fileExplorerSplitter->addWidget(listFileExplorer);
    //setCentralWidget(vSplitter);
    setWidget(vSplitter);
    //connect(treeWidget, SIGNAL(itemSelectionChanged()),
    //        this, SLOT(on_project_selection_changed()));
    connect(treeWidget, SIGNAL(contentsChanged(bool)),
            this, SLOT(documentWasModified(bool)));
    connect(treeWidget, SIGNAL(datatrackChanged()),
            this, SLOT(on_datatrack_changed()));
    connect(treeWidget, SIGNAL(audiotrackChanged(bool)),
            this, SLOT(on_audiotrack_changed(bool)));
    connect(treeWidget, SIGNAL(extractItem()),
            this, SLOT(on_extract_item()));
    connect(treeWidget, SIGNAL(grabItem()),
            this, SLOT(on_grab_item()));

}

void MdiChildDialog::newFile()
{
  static int sequenceNumber = 1;

  isUntitled = true;
  curFile = tr("Project%1.xbel").arg(sequenceNumber++);
  setWindowTitle(curFile + "[*]");

  // initialize fake data
  //  QDataItem *folder1 = treeWidget->InsertItem(0, QDataItem::Folder, "Folder1");
  //  folder1->AddFolderIcon(folderOnPixmap, folderOffPixmap);
  //  QDataItem *file = treeWidget->InsertItem(folder1, QDataItem::File, "file 01");
  //  file->AddFileIcon(filePixmap);
  //  file = treeWidget->InsertItem(folder1, QDataItem::File, "file 02");
  //  file->AddFileIcon(filePixmap);

  //  QDataItem *folder2 = treeWidget->InsertItem(0, QDataItem::Folder, "Folder2");
  //  folder2->AddFolderIcon(folderOnPixmap, folderOffPixmap);
  //  file = treeWidget->InsertItem(folder2, QDataItem::File, "file 03");
  //  // file->AddFileIcon(...); // maybe you want to set other file icon here?
  //  file->AddFileIcon(filePixmap);
  //  file = treeWidget->InsertItem(folder2, QDataItem::File, "file 04");
  //  file->AddFileIcon(filePixmap);

}

bool MdiChildDialog::loadFile(const QString &fileName)
{
  QFile domFile(fileName);
  if (!domFile.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, tr(""),
                         tr("Cannot read file %1:\n%2.")
                         .arg(fileName)
                         .arg(domFile.errorString()));
    return false;
  }

  RuleManager::ProjectType project_type;
  setWindowIcon(QIcon(":/icons/diskbutler32.png"));

  QDomDocument doc;
  doc.setContent(&domFile);

  int nDiskType = -1;

  QDomNodeList diskList = doc.elementsByTagName("disk");

  for (int i = 0; i < diskList.count(); i++)
  {
      if (i < diskList.count())
      {
          QDomElement node = diskList.at(i).toElement();
          if (node.hasAttribute("projecttype"))
          {
              nDiskType = node.attribute("projecttype").toInt();
              break;
          }
      }
  }


  domFile.close();

  if(nDiskType==-1)return false;
  switch(nDiskType){
      case 0:
          project_type = RuleManager::TYPE_PROJECT_EXPLORE;
          break;
      case 1:
          project_type = RuleManager::TYPE_PROJECT_OPEN;
          break;
      case 2:
          project_type = RuleManager::TYPE_PROJECT_AUDIOCD;
          break;
      case 3:
          project_type = RuleManager::TYPE_PROJECT_MIXEDMODE;
          break;
      case 4:
          project_type = RuleManager::TYPE_PROJECT_VIDEODVD;
          break;
      case 5:
          project_type = RuleManager::TYPE_PROJECT_BLURAYVIDEO;
          break;
      case 6:
          project_type = RuleManager::TYPE_PROJECT_UDF;
          break;
      case 7:
          project_type = RuleManager::TYPE_PROJECT_ISO;
          break;
      case 8:
          project_type = RuleManager::TYPE_PROJECT_ISOUDF;
          break;
      case 9:
          project_type = RuleManager::TYPE_PROJECT_VIDEOCD;
          break;
      case 10:
          project_type = RuleManager::TYPE_PROJECT_SUPERVIDEO;
          break;
      case 11:
          project_type = RuleManager::TYPE_PROJECT_MAX;
          break;
  }

  //Now we call the ProjectType and creation of the environment, that was before declared inside the constructor
  //We need to use a loadfile handler, without datatrack.
   createTreeWidget(project_type,true);


   QFile file(fileName);
   if (!file.open(QFile::ReadOnly | QFile::Text)) {
     QMessageBox::warning(this, tr(""),
                          tr("Cannot read file %1:\n%2.")
                          .arg(fileName)
                          .arg(file.errorString()));
     return false;
   }

  XbelHandler handler(treeWidget);
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  QXmlInputSource xmlInputSource(&file);
  bool res = reader.parse(xmlInputSource);

  QApplication::restoreOverrideCursor();

  if (res) {
    setCurrentFile(fileName);
    return true;
  } else {
    return false;
  }
}

bool MdiChildDialog::loadImage(const QString &fileName)
{
  QFile file(fileName);
  if (!file.exists()) {
    QMessageBox::warning(this, tr(""),
                         tr("Cannot read file %1:\n%2.")
                         .arg(fileName)
                         .arg(file.errorString()));
    return false;
  }
  ZImageLoader loader(treeWidget);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  bool res = loader.Parse(fileName);
  QApplication::restoreOverrideCursor();

  if (res) {
    setCurrentFile(fileName);
    treeWidget->calculateRealSizes();
    return true;
  } else {
    return false;
  }
}

bool MdiChildDialog::loadDisk(const QString &fileName)
{
  ZImageLoader loader(treeWidget);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  bool res = loader.Parse(fileName, false);
  QApplication::restoreOverrideCursor();

  if (res) {
    setCurrentFile(fileName, false);
    treeWidget->calculateRealSizes();
    return true;
  } else {
    return false;
  }
}

void MdiChildDialog::checkAndTags(int32 sourceTags)
{
    QTreeWidgetItem *rootItem = treeWidget->topLevelItem(0);
    QMediumItem *medium = dynamic_cast<QMediumItem*>(rootItem);

    if(medium->getExtendedMediumType()==BS_EMT_CD_AUDIO){
        treeWidget->getTags(sourceTags);
    }
}

bool MdiChildDialog::save()
{
  if (isUntitled) {
    return saveAs();
  } else {
    return saveFile(curFile);
  }
}

bool MdiChildDialog::saveAs()
{
  QString fileName =
      QFileDialog::getSaveFileName(this, tr("Save Project File"),
                                   curFile,
                                   tr("XBEL Files (*.xbel *.xml)"));
  if (fileName.isEmpty())
    return false;

  return saveFile(fileName);
}

bool MdiChildDialog::saveFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, tr(""),
                         tr("Cannot write file %1:\n%2.")
                         .arg(fileName)
                         .arg(file.errorString()));
    return false;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  XbelGenerator generator(treeWidget);
  bool res = generator.write(&file);
  QApplication::restoreOverrideCursor();

  if (res) {
    setCurrentFile(fileName);
    return true;
  } else {
    return false;
  }
}

bool MdiChildDialog::exportIndexFile()
{
  QString fileName =
      QFileDialog::getSaveFileName(this, tr("Export Index File"),
                                   curFile,
                                   tr("XBEL Files (*.xbel *.xml)"));
  if (fileName.isEmpty())
    return false;

  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, tr(""),
                         tr("Cannot write file %1:\n%2.")
                         .arg(fileName)
                         .arg(file.errorString()));
    return false;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  XbelGenerator generator(treeWidget);
  bool res = generator.write(&file, true);
  QApplication::restoreOverrideCursor();

  return res;
}

QString MdiChildDialog::userFriendlyCurrentFile()
{
  return strippedName(curFile);
}

void MdiChildDialog::closeEvent(QCloseEvent *event)
{
  treeWidget->stopFromExternal();

  if (maybeSave()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MdiChildDialog::createFileExplorers()
{
  treeFileExplorer = new QTreeView(this);
  treeModel = new QFileSystemModel(this);
  treeModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
  QModelIndex rootIndex = treeModel->setRootPath("C:/");
  treeFileExplorer->setModel(treeModel);
  treeFileExplorer->setExpanded(rootIndex, true);
  treeFileExplorer->setDragEnabled(true);
  treeFileExplorer->setDragDropMode(QAbstractItemView::DragOnly);
  QHeaderView *header = treeFileExplorer->header();
  for (int i = 1; i < header->count(); i++) {
    header->hideSection(i);
  }
  treeFileExplorer->setGeometry(0, 0, 320, 240);

  listFileExplorer = new QTreeView(this);
  listModel = new QFileSystemModel(this);
  listModel->setFilter(QDir::NoDot | QDir::Files | QDir::Dirs);
  listModel->setIconProvider(new ZFileIconProvider());
  //QDir::Filters f = listModel->filter(); // 0x1033
  QString root_path = "C:";
  rootIndex = listModel->setRootPath(root_path);
  emit list_view_root_changed(root_path);
  listFileExplorer->setModel(listModel);
  listFileExplorer->setRootIndex(rootIndex);
  listFileExplorer->setItemsExpandable(false);
  listFileExplorer->setDragEnabled(true);
  listFileExplorer->setDragDropMode(QAbstractItemView::DragOnly);
  listFileExplorer->setGeometry(340, 0, 480, 240);

  connect(treeFileExplorer, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(on_treeView_clicked(const QModelIndex &)));
  connect(listFileExplorer, SIGNAL(doubleClicked(const QModelIndex &)),
          this, SLOT(on_listView_doubleClicked(const QModelIndex &)));
}

bool MdiChildDialog::hasSelectedTreeItem()
{
  return treeWidget->GetSelectedItem() != NULL;
}

QDataItem *MdiChildDialog::GetSelectedTreeItem()
{
  return treeWidget->GetSelectedItem();
}

void MdiChildDialog::insertItem()
{
  //  QDataItem *child = treeWidget->InsertItem(QDataItem::File, name);
  //  if (child != NULL)
  //    child->AddFileIcon(filePixmap);
  treeWidget->InsertItemFromRibbon();
}

void MdiChildDialog::insertNode(const QString &name)
{
  //setCursor(Qt::WaitCursor);
  //QApplication::processEvents();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  treeWidget->InsertNode(name);
  QApplication::restoreOverrideCursor();
  //setCursor(Qt::ArrowCursor);
}

//void MdiChildDialog::addFolder(const QString &name)
//{
//  //setCursor(Qt::WaitCursor);
//  //QApplication::processEvents();
//  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
//  treeWidget->AddFolder(name);
//  QApplication::restoreOverrideCursor();
//  //setCursor(Qt::ArrowCursor);
//}

void MdiChildDialog::addNewFolder()
{
  treeWidget->AddNewFolder();
}

void MdiChildDialog::renameItem()
{
  treeWidget->slot_rename_in_place();
}

void MdiChildDialog::deleteItem()
{
  treeWidget->DeleteItem();
}

bool MdiChildDialog::isDataTrackExist()
{
  return treeWidget->IsDataTrackExist();
}

void MdiChildDialog::addDataTrack()
{
  treeWidget->AddDataTrack();
}

void MdiChildDialog::addAudioTrack()
{
  treeWidget->AddAudioTrack();
}

void MdiChildDialog::selectAll()
{
  treeWidget->SelectAll();
}

void MdiChildDialog::reverseSelection()
{
  treeWidget->ReverseSelection();
}

void MdiChildDialog::updateProject()
{
  treeWidget->UpdateProject();
}

void MdiChildDialog::moveAudioTrack(int index_change)
{
  treeWidget->MoveAudioTrack(index_change);
}

void MdiChildDialog::toggleFileExplorerTree()
{
  //if (NULL != dockTree) {
  //  dockTree->toggleViewAction()->trigger();
  //}
}

void MdiChildDialog::toggleFileExplorerList()
{
  //if (NULL != dockList) {
  //  dockList->toggleViewAction()->trigger();
  //}
}

void MdiChildDialog::toggleFileExplorer()
{
    if (fileExplorerSplitter->isHidden()) {
        fileExplorerSplitter->show();
    } else {
        fileExplorerSplitter->hide();
    }
}

void MdiChildDialog::documentWasModified(bool modified)
{
    setWindowModified(modified);
    emit status_changed("");
    emit datatrack_changed();
}

void MdiChildDialog::on_treeView_clicked(const QModelIndex &index)
{
    QString sPath = treeModel->fileInfo(index).absoluteFilePath();
    listFileExplorer->setRootIndex(listModel->setRootPath(sPath));
    emit list_view_root_changed(sPath);
}

void MdiChildDialog::on_listView_doubleClicked(const QModelIndex &index)
{
    QString sPath = listModel->fileInfo(index).absoluteFilePath();
    QModelIndex new_index = listModel->index(sPath);
    listFileExplorer->setRootIndex(new_index);
    listModel->setRootPath(sPath);
    emit list_view_root_changed(sPath);
    QModelIndex treeIndex = treeModel->index(sPath);
    treeFileExplorer->setExpanded(treeIndex, true);
    QModelIndex parent = treeIndex.parent();
    while (parent.isValid()) {
        treeFileExplorer->setExpanded(parent, true);
        parent = parent.parent();
    }
}

void MdiChildDialog::on_extract_item()
{
    emit extract_item();
}

void MdiChildDialog::on_grab_item()
{
    emit grab_item();
}

void MdiChildDialog::on_datatrack_changed()
{

    emit status_changed("");
    emit datatrack_changed();

}

void MdiChildDialog::on_audiotrack_changed(bool bEnable)
{
    emit audiotrack_changed(bEnable);
}

void MdiChildDialog::on_project_selection_changed()
{
    QString status = updateStatus();
    emit status_changed(status);
}

QString MdiChildDialog::updateStatus() {
  //QDataItem *item = treeWidget->GetSelectedItem();
  //if (item == NULL) {
  //  return;
  //}

  //if (QDataItem::Session == item->GetType()) {
  //  statusBar()->clearMessage();
  //  return;
  //}
  //QString fileNum = tr("1");
  //if (QDataItem::Folder == item->GetType()) {
  //  fileNum = QString::number(item->childCount());
  //}

  //QString unitStr;
  //qint64 dstSize;
  //calcDataSize(item->GetDataSize(), dstSize, unitStr);
  //QString statusStr = tr("Items: ") + fileNum
  //    + tr("\t/\tTotal size: ") + QString::number(dstSize)
  //    + tr(" ") + unitStr;
  //statusBar()->showMessage(statusStr);

    QString size_str;
    QString project_str = RuleManager::GetProjectTypeStr(mProjectType);

    QString statusStr;

    if (RuleManager::TYPE_PROJECT_AUDIOCD == mProjectType) {

        statusStr = buildAudioCD(project_str);

    }else if(RuleManager::TYPE_PROJECT_MIXEDMODE == mProjectType || RuleManager::TYPE_PROJECT_OPEN == mProjectType){
        qint64 nTotalTimeSeconds = treeWidget->getTotalTime();
        qint64 nDataSize = treeWidget->GetDataSize();
        qint64 nTotalDataSize = nDataSize + nTotalTimeSeconds*176400;
        size_str = humanReadableSize(nTotalDataSize,nullptr);

        statusStr = tr("Project type: ") + project_str + tr(", ") + tr("Total size: ") + size_str
            + tr("\t/\t") + QString::number(treeWidget->GetDataItemCount()) + tr(" items")
            + tr("\t/\t") + QDateTime::fromSecsSinceEpoch(nTotalTimeSeconds).toUTC().toString("hh:mm:ss");
    }else if(RuleManager::TYPE_PROJECT_EXPLORE == mProjectType){
        statusStr = buildExploreStatusBar(project_str);
    }else{
        //Explore: We have also an audio cd here. Disktype
        statusStr = buildCommonCD(project_str);
    }

    return statusStr;
}

QString MdiChildDialog::buildExploreStatusBar(QString inProjectType)
{
    QTreeWidgetItem *rootItem = treeWidget->topLevelItem(0);
    QMediumItem *medium = dynamic_cast<QMediumItem*>(rootItem);
    QString statusStr;

    if(medium->getExtendedMediumType()==BS_EMT_CD_AUDIO){
        statusStr = buildAudioCD(tr("Audio Disk"));
    }else{
        statusStr = (tr("Data /Video Disk"));
    }

    return statusStr;
}

QString MdiChildDialog::buildCommonCD(QString inProjectType)
{
    QString size_str = humanReadableSize(treeWidget->GetDataSize(),nullptr);
    QString statusStr = tr("Project type: ") + inProjectType + tr(", ") + tr("Total size: ") + size_str
          + tr("\t/\t") + QString::number(treeWidget->GetDataItemCount()) + tr(" items");

    return statusStr;
}

QString MdiChildDialog::buildAudioCD(QString inProjectType)
{
    qint64 nTotalTimeSeconds;

    if(RuleManager::TYPE_PROJECT_EXPLORE == mProjectType){
        nTotalTimeSeconds = treeWidget->getTotalTimeExplorer();
    }else{
        nTotalTimeSeconds = treeWidget->getTotalTime();
    }

    qint64 nDataSize = 0;
    qint64 nTotalDataSize = nDataSize + nTotalTimeSeconds*176400;
    QString size_str = humanReadableSize(nTotalDataSize,nullptr);

    QString statusStr = tr("Project type: ") + inProjectType + tr(", ") + tr("Total size: ") + size_str
          + tr("\t/\t") + QDateTime::fromSecsSinceEpoch(nTotalTimeSeconds).toUTC().toString("hh:mm:ss");

    return statusStr;

}


bool MdiChildDialog::gotoListViewParent()
{
  QString path = listModel->rootPath();
  int index = path.lastIndexOf('/', -2);
  if (-1 == index)
    return false;
  QString new_path = path.left(index);
  listFileExplorer->setRootIndex(listModel->setRootPath(new_path));
  emit list_view_root_changed(new_path);
  // also update tree view
  QModelIndex old_tree_index = treeModel->index(path);
  //QTreeWidgetItem* item = (QTreeWidgetItem*)old_tree_index.internalPointer();
  //treeFileExplorer->setItemSelected(item, true);
  //treeFileExplorer->indexAbove(old_tree_index);
  QModelIndex new_tree_index = old_tree_index.parent();
  treeFileExplorer->collapse(old_tree_index);
  treeFileExplorer->expand(new_tree_index);
  //QRect rect = treeFileExplorer->visualRect(new_tree_index);
  //treeFileExplorer->setSelection(QRect, ...); // protected
  return true;
}

QString MdiChildDialog::getListViewRootPath() const
{
    return listModel->rootPath();
}

bool MdiChildDialog::maybeSave()
{
  if (treeWidget->IsModified()) {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(
          this, tr("MDI"), tr("'%1' has been modified.\nDo you want to save your changes?")
          .arg(userFriendlyCurrentFile()),
          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
      return save();
    else if (ret == QMessageBox::Cancel)
      return false;
  }
  return true;
}

void MdiChildDialog::setCurrentFile(const QString &fileName, bool isFile)
{
    isImageFile = isFile;
    if (isImageFile) {
        curFile = QFileInfo(fileName).canonicalFilePath();
        setWindowTitle(userFriendlyCurrentFile() + "[*]");
    } else {
        curFile = fileName;
        setWindowTitle(curFile + "[*]");
    }
    isUntitled = false;
    treeWidget->resetModified();
    setWindowModified(false);
}

QString MdiChildDialog::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MdiChildDialog::triggerFSSync()
{
    if (treeWidget != nullptr)
        treeWidget->FSSync();
}

void MdiChildDialog::triggerFSUnsync()
{
    if (treeWidget != nullptr)
        treeWidget->FSUnsync();
}

void MdiChildDialog::triggerReset()
{
    if (treeWidget != nullptr)
        treeWidget->ResetFiles();
}

void MdiChildDialog::setUIControls(Ribbon *baseRibbon, QWidget* parent)
{
    //Mal schauen ob der Cast arbeitet.
    MainWindow *ribbonOwner = qobject_cast<MainWindow *>(parent);

    ribbonOwner->simulateBurnGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_SIMULATE));
    ribbonOwner->burnProofGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_BURNPROOF));
    ribbonOwner->ejectAfterGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_EJECT));
    ribbonOwner->finishDiscGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_FINALIZE));
    ribbonOwner->verifyAfterGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_VERIFY));
    ribbonOwner->autoEraseGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_AUTOERASE));
    ribbonOwner->padDataGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_PADDATATRACKS));
    ribbonOwner->avchdGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SPECIALS_AVCHD));
    ribbonOwner->activeOPCGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_OPC));


    ribbonOwner->imageMediaButton->setEnabled( false );
    ribbonOwner->appSaveButton->setEnabled(true);
    ribbonOwner->appSaveAsButton->setEnabled(true);

    ribbonOwner->burnGeneralButton->setEnabled(true);

    ribbonOwner->grabAudioMediaButton->setEnabled(false);

    ribbonOwner->dataTrackEditButton->setEnabled(
        !isDataTrackExist()
        && GetProjectType() != RuleManager::TYPE_PROJECT_VIDEODVD
        && GetProjectType() != RuleManager::TYPE_PROJECT_BLURAYVIDEO
        );
    ribbonOwner->audioTrackEditButton->setEnabled(
        (GetProjectType() == RuleManager::TYPE_PROJECT_OPEN
            || GetProjectType() == RuleManager::TYPE_PROJECT_MIXEDMODE
            || GetProjectType() == RuleManager::TYPE_PROJECT_AUDIOCD));

    ribbonOwner->addFileEditButton->setEnabled(
        GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
        );

    ribbonOwner->addFolderEditButton->setEnabled(
                GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
        );

    ribbonOwner->addFolderEditButton->setEnabled(GetProjectType() != RuleManager::TYPE_PROJECT_AUDIOCD);

    bool hasSelection = hasSelectedTreeItem();

    ribbonOwner->renameEditButton->setEnabled(hasSelection
                          && GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
        );

    ribbonOwner->allSelectEditButton->setEnabled(
                GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
            );

    ribbonOwner->inverseSelectEditButton->setEnabled(
                GetProjectType() != RuleManager::TYPE_PROJECT_EXPLORE
            );

    ribbonOwner->delAllEditButton->setEnabled(true);

    ribbonOwner->updtEditButton->setEnabled(true);

    QDataItem *tempItem = nullptr;


    if(GetProjectType() != RuleManager::TYPE_PROJECT_DISKINFO
            && GetProjectType() != RuleManager::TYPE_PROJECT_DEVICEINFO)
    {

        tempItem = GetSelectedTreeItem();
        if(tempItem){
            if(tempItem->GetType()==QDataItem::Disk || tempItem->GetType()==QDataItem::Session || tempItem->GetType()==QDataItem::DataTrack){
                ribbonOwner->delEditButton->setEnabled(false);
                ribbonOwner->renameEditButton->setEnabled(false);
            }else{
                ribbonOwner->delEditButton->setEnabled(hasSelection);
                ribbonOwner->renameEditButton->setEnabled(hasSelection);
            }
        }
    }

    ribbonOwner->viewBrowserButton->setEnabled(true);
    if(GetProjectType() == RuleManager::TYPE_PROJECT_EXPLORE){
        ribbonOwner->viewBrowserButton->setEnabled(false);
    }

    if(GetProjectType() == RuleManager::TYPE_PROJECT_AUDIOCD || GetProjectType() == RuleManager::TYPE_PROJECT_MIXEDMODE || GetProjectType() == RuleManager::TYPE_PROJECT_OPEN)
    {
        int nCounter = (isDataTrackExist()==true)?1:0;
        int nChildCount = getTreeWidget()->getSessionItem()->childCount();

        tempItem = GetSelectedTreeItem();

        int index; // = activeMdiChild()->getTreeWidget()->getSessionItem()->indexOfChild(tempItem);

        if(tempItem){
            if(nChildCount > nCounter){
                if(tempItem->GetType()==QDataItem::Disk || tempItem->GetType()==QDataItem::Session || tempItem->GetType()==QDataItem::DataTrack){
                    //groupAudioOrder->setEnabled(false);
                }else{
                    //groupAudioOrder->setEnabled(true);
                }
            }else{
                //groupAudioOrder->setEnabled(false);
            }
            //Why not show CDText dialog also if the audio item is selectd? It is also a track? Isn't it.
            //(QDataItem*)(tempItem->parent())->GetType()
            if(tempItem->GetType()==QDataItem::Disk || isAudioTrack()==true){
                //actionCDText->setEnabled(true);
            }else{
                //actionCDText->setEnabled(false);
            }
            if(isAudioTrack()==true){
                QTreeWidgetItem* parentItem = tempItem->parent();
                if(parentItem){
                    if(((QDataItem*)parentItem)->GetType()==QDataItem::AudioTrack){
                        //actionAudioIndexies->setEnabled(true);
                    }
                }

                if(tempItem->GetType()==QDataItem::AudioTrack){
                    if(tempItem->childCount()==1){
                        //actionCDText->setEnabled(true);
                        //actionAudioIndexies->setEnabled(true);
                    }else{
                        //actionCDText->setEnabled(false);
                        //actionAudioIndexies->setEnabled(false);
                    }
                }

                if(tempItem->GetType()==QDataItem::AudioTrack){
                    index = getTreeWidget()->getSessionItem()->indexOfChild(tempItem);
                }else{
                    QTreeWidgetItem* parentItem = tempItem->parent();
                    index = getTreeWidget()->getSessionItem()->indexOfChild(parentItem);
                }

                if ((isDataTrackExist() && index == 1) || (!isDataTrackExist() && index == 0)) {
                  //actionAudioUp->setEnabled(false);
                } else {
                  //actionAudioUp->setEnabled(true);
                }
                if (nChildCount-1 == index) {
                  //actionAudioDown->setEnabled(false);
                } else {
                  //actionAudioDown->setEnabled(true);
                }
            }else{
                //actionAudioIndexies->setEnabled(false);
            }
        }else{
            //actionCDText->setEnabled(false);
            //actionAudioIndexies->setEnabled(false);
            //actionAudioDown->setEnabled(false);
            //actionAudioUp->setEnabled(false);
        }

    }

    QDiskItem *diskItem = (QDiskItem *)getTreeWidget()->topLevelItem(0);
    //Wir fangen an die Sachen zu lesen.
    if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SPECIALS_BootCD_DVD_BD)){
        //Has Boot
        if(diskItem->getDoBootDisk()==true){
            baseRibbon->disableButtonGroup(tr("Boot Disc"), tr("El-Torito"), true);
            ribbonOwner->delBootImageButton->setEnabled(true);
        }else{
            baseRibbon->disableButtonGroup(tr("Boot Disc"), tr("El-Torito"), false);
            ribbonOwner->delBootImageButton->setEnabled(false);
        }

        ribbonOwner->imagePathBootEdit->setText(diskItem->getBootDiskFile());
        ribbonOwner->emulationTypeBootCombo->setCurrentIndex(diskItem->getBootEmulationType());
        ribbonOwner->platformBootCombo->setCurrentIndex(diskItem->getBootPlatformID());
        ribbonOwner->developerIDBootEdit->setText(diskItem->getBootDeveloperID());
        ribbonOwner->sectorsBootEdit->setText(diskItem->getBootSectors());
        ribbonOwner->bootISOLevelCombo->setCurrentIndex(diskItem->getISOFsType());
        ribbonOwner->loadSegmentBootEdit->setText(diskItem->getBootLoadSegment());
    }


    if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SPECIALS_ISOEXTINFO)){
        ribbonOwner->edIsoExSystemIdValue->setText(diskItem->getSystemId());
        ribbonOwner->edIsoExVolumeSetValue->setText(diskItem->getVolumeSet());
        ribbonOwner->edIsoExPublisherValue->setText(diskItem->getPublisher());
        ribbonOwner->edIsoExDataPreparerValue->setText(diskItem->getDatapreparer());
        ribbonOwner->edIsoExApplicationValue->setText(diskItem->getApplication());
        ribbonOwner->edIsoExCopyrightFileValue->setText(diskItem->getCoprightFile());
        ribbonOwner->edIsoExAbstractFileValue->setText(diskItem->getAbstractFile());
        ribbonOwner->edIsoExBibliographicFileValue->setText(diskItem->getBibliographicFile());
    }


    if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMS_UDF))
    {
        baseRibbon->disableButtonGroup(tr("File System"), tr("UDF"), true);
        ribbonOwner->udfVersionFSCombo->clear();
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMS_UDF102)) ribbonOwner->udfVersionFSCombo->addItem(tr("UDF 1.02"));
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMS_UDF15)) ribbonOwner->udfVersionFSCombo->addItem(tr("UDF 1.5"));
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMS_UDF20)) ribbonOwner->udfVersionFSCombo->addItem(tr("UDF 2.0"));
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMS_UDF201)) ribbonOwner->udfVersionFSCombo->addItem(tr("UDF 2.01"));
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMS_UDF25)) ribbonOwner->udfVersionFSCombo->addItem(tr("UDF 2.5"));
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMS_UDF26)) ribbonOwner->udfVersionFSCombo->addItem(tr("UDF 2.6"));

        ribbonOwner->udfPartitionFSCombo->clear();
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_UDF_PARTITION))
        {
            ribbonOwner->udfPartitionFSCombo->setEnabled(true);
            ribbonOwner->udfPartitionFSCombo->addItem(tr("Physical"));
            ribbonOwner->udfPartitionFSCombo->addItem(tr("Virtual"));
            ribbonOwner->udfPartitionFSCombo->addItem(tr("Sparable"));
        }else{
            ribbonOwner->udfPartitionFSCombo->setEnabled(false);
        }

        ribbonOwner->udfFileStreamFSCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_UDF_WRITE_FILE_STREAM));

    }else{
        baseRibbon->disableButtonGroup(tr("File System"), tr("UDF"), false);
    }


    if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMS_ISO9660))
    {
        baseRibbon->disableButtonGroup(tr("File System"), tr("ISO9660"), true);

        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SPECIALS_BootCD_DVD_BD)){
            ribbonOwner->bootISOLevelCombo->clear();
            if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL1)) ribbonOwner->bootISOLevelCombo->addItem(tr("ISO Level 1"),BS_ISO_LEVEL_1);
            if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL2)) ribbonOwner->bootISOLevelCombo->addItem(tr("ISO Level 2"),BS_ISO_LEVEL_2);
            if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL3)) ribbonOwner->bootISOLevelCombo->addItem(tr("ISO Level 3"),BS_ISO_LEVEL_3);
            if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ROMEO)) ribbonOwner->bootISOLevelCombo->addItem(tr("Romeo"),BS_ISO_LEVEL_ROMEO);

        }

        ribbonOwner->isoLevelFSCombo->clear();
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL1)) ribbonOwner->isoLevelFSCombo->addItem(tr("ISO Level 1"),BS_ISO_LEVEL_1);
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL2)) ribbonOwner->isoLevelFSCombo->addItem(tr("ISO Level 2"),BS_ISO_LEVEL_2);
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ISOLEVEL3)) ribbonOwner->isoLevelFSCombo->addItem(tr("ISO Level 3"),BS_ISO_LEVEL_3);
        if(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ROMEO)) ribbonOwner->isoLevelFSCombo->addItem(tr("Romeo"),BS_ISO_LEVEL_ROMEO);

        ribbonOwner->isoLongFileNamesFSCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ALLOW_LONGISO_FILENAMES));
        ribbonOwner->isoManyDirectoriesFSCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ALLOW_MANY_DIRECTORIES));
        ribbonOwner->useJolietFSCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMS_JOLIET));
        ribbonOwner->useRockRidgeFSCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_FILESYSTEMEXT_ROCK_RIDGE));
    } else{

        baseRibbon->disableButtonGroup(tr("File System"), tr("ISO9660"), false);
    }

    ribbonOwner->isoLevelFSCombo->setCurrentIndex(diskItem->getISOFsType());
    ribbonOwner->bootISOLevelCombo->setCurrentIndex(diskItem->getISOFsType());

    ribbonOwner->useJolietFSCheck->setChecked(diskItem->getUseJoliet());
    ribbonOwner->useRockRidgeFSCheck->setChecked(diskItem->getUseRockRidge());
    ribbonOwner->isoManyDirectoriesFSCheck->setChecked(diskItem->getAllowManyDirectories());
    ribbonOwner->isoLongFileNamesFSCheck->setChecked(diskItem->getAllowLongFileNames());
    ribbonOwner->jolietLongNamesFSCheck->setChecked(diskItem->getAllowLongJolietFileNames());
    ribbonOwner->udfVersionFSCombo->setCurrentIndex(diskItem->getUDFType());
    ribbonOwner->udfPartitionFSCombo->setCurrentIndex(diskItem->getUDFPartition());
    ribbonOwner->udfFileStreamFSCheck->setChecked(diskItem->getUDFWriteStream());
    ribbonOwner->isoExtent1FSCheck->setChecked(diskItem->getNotWriteISO1Extension());

    ribbonOwner->simulateBurnGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_SIMULATE));
    ribbonOwner->simulateBurnGeneralCheck->setChecked(diskItem->getFeatureSimulate());
    ribbonOwner->activeOPCGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_OPC));
    ribbonOwner->activeOPCGeneralCheck->setChecked(diskItem->getFeatureOPC());
    ribbonOwner->finishDiscGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_FINALIZE));
    ribbonOwner->finishDiscGeneralCheck->setChecked(diskItem->getFeatureFinishDisk());
    ribbonOwner->padDataGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_PADDATATRACKS));
    ribbonOwner->padDataGeneralCheck->setChecked(diskItem->getFeaturePadDataTrack());
    ribbonOwner->avchdGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SPECIALS_AVCHD));
    ribbonOwner->avchdGeneralCheck->setChecked(diskItem->getFeatureAVCHD());
    ribbonOwner->verifyAfterGeneralCheck->setEnabled(RuleManager::IsOptionAllowed(mProjectType, RuleManager::OPTION_SETTINGS_VERIFY));
    ribbonOwner->verifyAfterGeneralCheck->setChecked(diskItem->getFeatureVerify());

    //Always allowed
    ribbonOwner->ejectAfterGeneralCheck->setChecked(diskItem->getFeatureEject());
    ribbonOwner->burnProofGeneralCheck->setChecked(diskItem->getFeatureBurnProof());
    ribbonOwner->autoEraseGeneralCheck->setChecked(diskItem->getFeatureAutoErase());

    ribbonOwner->createFolderEditButton->setEnabled(true);
    if(GetProjectType() == RuleManager::TYPE_PROJECT_AUDIOCD){
        ribbonOwner->addFolderEditButton->setEnabled(false);
        ribbonOwner->createFolderEditButton->setEnabled(false);
        ribbonOwner->dataTrackEditButton->setEnabled(false);
        ribbonOwner->renameEditButton->setEnabled(false);
    }

    tempItem = GetSelectedTreeItem();
    if(tempItem){
        if(tempItem->GetType()==QDataItem::Disk || tempItem->GetType()==QDataItem::Session || tempItem->GetType()==QDataItem::AudioTrack){
            //Kein Folder, Kein File, Kein Track
            ribbonOwner->createFolderEditButton->setEnabled(false);
            ribbonOwner->addFolderEditButton->setEnabled(false);
            ribbonOwner->addFileEditButton->setEnabled(false);

            if(tempItem->GetType()!=QDataItem::AudioTrack){
                ribbonOwner->createFolderEditButton->setEnabled(false);
                ribbonOwner->delEditButton->setEnabled(false);
            }

        }
    }

}

bool MdiChildDialog::isAudioTrack()
{

    QDataItem *tempItem = GetSelectedTreeItem();

    if(tempItem){

        if(tempItem->GetType()==QDataItem::Session || tempItem->GetType()==QDataItem::Disk || tempItem->GetType()==QDataItem::DataTrack)
        return false;

        QTreeWidgetItem* parentItem = tempItem->parent();
        if(tempItem->GetType()==QDataItem::AudioTrack || ((QDataItem*)parentItem)->GetType()==QDataItem::AudioTrack)
        return true;

    }

    return false;
}

//SetBurnDevice List ist in den pDevice Projekten dann das ReadDevice.
void MdiChildDialog::setBurnDeviceList(QWidget* parent)
{
    MainWindow *ribbonOwner = qobject_cast<MainWindow *>(parent);
    QDiskItem *diskItem = nullptr;
    diskItem = static_cast<QDiskItem *>(getTreeWidget()->topLevelItem(0));

    for(int i = 0; i < ribbonOwner->listBurnDevicesWidget->count(); ++i)
    {
        if(ribbonOwner->listBurnDevicesWidget->itemText(i) == diskItem->getBurnDevice()){
            ribbonOwner->listBurnDevicesWidget->setCurrentIndex(i);
        }
    }
}

void MdiChildDialog::setRibbonTabs(Ribbon *baseRibbon, QWidget* parent)
{
    baseRibbon->blockSignals(true);
    QString lastTab = getlastSelectedTab();
    switch(GetProjectType()){
    case RuleManager::TYPE_PROJECT_ISO:
        qDebug() << "ISO";
        baseRibbon->hideAll();
        baseRibbon->showTab(":/icons/filesystem32.png", tr("File System"));
        baseRibbon->showTab(":/icons/extiso32.png", tr("ISO Extended"));
        baseRibbon->showTab(":/icons/boot32.png", tr("Boot Disc"));
        if(baseRibbon->isTabVisible(lastTab)){
            baseRibbon->currentTab(lastTab);
        }else{
            baseRibbon->currentTab("General");
        }
        break;
    case RuleManager::TYPE_PROJECT_UDF:
        baseRibbon->hideAll();
        baseRibbon->showTab(":/icons/filesystem32.png", tr("File System"));
        if(baseRibbon->isTabVisible(lastTab)){
            baseRibbon->currentTab(lastTab);
        }else{
            baseRibbon->currentTab("General");
        }
        break;
    case RuleManager::TYPE_PROJECT_ISOUDF:
        baseRibbon->hideAll();
        baseRibbon->showTab(":/icons/filesystem32.png", tr("File System"));
        baseRibbon->showTab(":/icons/extiso32.png", tr("ISO Extended"));
        baseRibbon->showTab(":/icons/boot32.png", tr("Boot Disc"));
        if(baseRibbon->isTabVisible(lastTab)){
            baseRibbon->currentTab(lastTab);
        }else{
            baseRibbon->currentTab("General");
        }
        break;
    }

    if(parent!=nullptr){
        setUIControls(baseRibbon, parent);
        setBurnDeviceList(parent);
    }

    baseRibbon->blockSignals(false);
}
