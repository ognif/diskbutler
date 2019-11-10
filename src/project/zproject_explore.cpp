#include <QtWidgets>
#include "zproject_explore.h"
#include "zpropertydialogfoxfile.h"

ZProjectExplore::ZProjectExplore(QWidget *parent)
  : CommonTreeWidget(RuleManager::TYPE_PROJECT_EXPLORE, false, parent)
{
    mExploreExtractAct = new QAction(tr("Extract"), this);
    connect(mExploreExtractAct, SIGNAL(triggered()), this, SLOT(extractContent()));

    mExploreExtractTrack = new QAction(tr("Extract Track"), this);
    connect(mExploreExtractTrack, SIGNAL(triggered()), this, SLOT(extractTrack()));

    mExploreExtractDisk = new QAction(tr("Extract Disk"), this);
    connect(mExploreExtractDisk, SIGNAL(triggered()), this, SLOT(extractDisk()));

    mExploreContentList = new QAction(tr("Content List"), this);
    connect(mExploreContentList, SIGNAL(triggered()), this, SLOT(contenList()));

    mExploreIndexList = new QAction(tr("Table of Contents"), this);
    connect(mExploreIndexList, SIGNAL(triggered()), this, SLOT(trackIndex()));

    mExploreGrabTrack = new QAction(tr("Grab Track"), this);
    connect(mExploreGrabTrack, SIGNAL(triggered()), this, SLOT(grabTrack()));

    mExploreExtractRunAct = new QAction(tr("Extract & Run"), this);

}

void ZProjectExplore::contextMenuEvent(QContextMenuEvent *event)
{
  QModelIndex index = indexAt(event->pos());
  QDataItem *item = (QDataItem*)index.internalPointer();

  int zAudioTrackIndex;
  if(IsDataTrackExist()==true){
      zAudioTrackIndex = 1;
  }else{
      zAudioTrackIndex = 0;
  }

  if (NULL != item) {
    QMenu menu(this);
    if (QDataItem::Disk == item->GetType()) {
        menu.addAction(mPropertyAct);
        menu.addAction(mExploreExtractDisk);
        if(item->childCount()!=0){
            QDataItem *itemChild = (QDataItem *)item->child(0);
            if(itemChild->GetType()==QDataItem::Session){
                if(itemChild->childCount()!=0){
                    QDataItem *itemChildChild = (QDataItem *)itemChild->child(zAudioTrackIndex);
                    if(itemChildChild){ // data disk only has one track so we need to make sure that the item exists.
                        if(itemChildChild->GetType()==QDataItem::AudioTrack){
                            menu.addAction(mExploreIndexList);
                        }
                    }

                }
            }
        }
    }
    if (QDataItem::Session == item->GetType()) {
        menu.addAction(mPropertyAct);
        if(item->childCount()!=0){
            QDataItem *itemChild = (QDataItem *)item->child(zAudioTrackIndex);
            if(itemChild){ // data disk only has one track so we need to make sure that the item exists.
                if(itemChild->GetType()==QDataItem::AudioTrack){
                    menu.addAction(mExploreIndexList);
                }
            }
        }
    }
    if (QDataItem::DataTrack == item->GetType()) {
        menu.addAction(mPropertyAct);
        menu.addAction(mExploreExtractTrack);

    }
    if (QDataItem::AudioTrack == item->GetType()) {
        menu.addAction(mPropertyAct);
        menu.addAction(mExploreExtractTrack);
        menu.addAction(mExploreGrabTrack);

    }
    if (QDataItem::File == item->GetType()) {
      menu.addAction(mPropertyAct);
      menu.addAction(mExploreExtractAct);
      menu.addAction(mExploreExtractRunAct);
      //mPropertyAct->setEnabled(true);
    }
    if (QDataItem::Folder == item->GetType()) {
        menu.addAction(mPropertyAct);
        menu.addAction(mExploreExtractAct);
        menu.addAction(mExploreContentList);
    }
    menu.exec(event->globalPos());
  }
  //CommonTreeWidget::contextMenuEvent(event);

}

void ZProjectExplore::showProperty_Disk() {
  ZPropertyDialogFoxFile dialog(this, mSelectedItem);
  if (dialog.exec() == QDialog::Accepted) {
  }
}

void ZProjectExplore::showProperty_File() {

  ZPropertyDialogFoxFile dialog(this, mSelectedItem);
  if (dialog.exec() == QDialog::Accepted) {
  }
}

void ZProjectExplore::extractTrack()
{
    emit extractItem();
    //QMessageBox::about(this, tr("Information"), tr("Here in Explorer"));

}

void ZProjectExplore::extractContent()
{
    emit extractItem();
    //QMessageBox::about(this, tr("Information"), tr("Here in Explorer"));

}

void ZProjectExplore::extractDisk()
{
    emit extractItem();
    //QMessageBox::about(this, tr("Information"), tr("Here in Explorer"));
}

void ZProjectExplore::contenList()
{
   createContentList();
}

void ZProjectExplore::trackIndex()
{
    createIndex();
}

void ZProjectExplore::grabTrack()
{
    emit grabItem();
}
