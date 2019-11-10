#include <QtWidgets>
#include "zproject_bluray.h"
#include "zimport_strategy_fixedfolder.h"
#include "zimport_strategy_suffixallowed.h"
#include "zimport_strategy_nosubfolder.h"

ZProjectBluRay::ZProjectBluRay(QWidget *parent)
  : CommonTreeWidget(RuleManager::TYPE_PROJECT_BLURAYVIDEO, false, parent)
{
  mProjectType = RuleManager::TYPE_PROJECT_BLURAYVIDEO;

  getDataTrackItem();

  QStringList fixed_folders;
  fixed_folders.append(tr("BDMV"));
  fixed_folders.append(tr("CERTIFICATE"));
  ZImportStrategy *strategy = new ZImportStrategyFixedFolder(tr(""), fixed_folders);
  mImportStrategies.append(strategy);

  addFixedFolder("", fixed_folders);

  fixed_folders.clear();
  fixed_folders.append(tr("AUXDATA"));
  fixed_folders.append(tr("BACKUP"));
  fixed_folders.append(tr("BDJO"));
  fixed_folders.append(tr("CLIPINF"));
  fixed_folders.append(tr("JAR"));
  fixed_folders.append(tr("META"));
  fixed_folders.append(tr("PLAYLIST"));
  fixed_folders.append(tr("STREAM"));
  strategy = new ZImportStrategyFixedFolder(tr("/BDMV"), fixed_folders, false, true);
  mImportStrategies.append(strategy);

  addFixedFolder("BDMV", fixed_folders);
  createFixedFolder();

  QStringList suffix_allowed;
  suffix_allowed.append(tr("bdmv"));
  strategy = new ZImportStrategySuffixAllowed(tr("/BDMV"), suffix_allowed);
  mImportStrategies.append(strategy);

  suffix_allowed.clear();
  suffix_allowed.append(tr("bdjo"));
  strategy = new ZImportStrategySuffixAllowed(tr("/BDMV/BDJO"), suffix_allowed);
  mImportStrategies.append(strategy);

  suffix_allowed.clear();
  suffix_allowed.append(tr("clpi"));
  strategy = new ZImportStrategySuffixAllowed(tr("/BDMV/CLIPINF"), suffix_allowed);
  mImportStrategies.append(strategy);

  suffix_allowed.clear();
  suffix_allowed.append(tr("mpls"));
  strategy = new ZImportStrategySuffixAllowed(tr("/BDMV/PLAYLIST"), suffix_allowed);
  mImportStrategies.append(strategy);

  suffix_allowed.clear();
  suffix_allowed.append(tr("m2ts"));
  strategy = new ZImportStrategySuffixAllowed(tr("/BDMV/STREAM"), suffix_allowed);
  mImportStrategies.append(strategy);

  strategy = new ZImportStrategyNoSubfolder(tr("/BDMV/CLIPINF"));
  mImportStrategies.append(strategy);
  strategy = new ZImportStrategyNoSubfolder(tr("/BDMV/PLAYLIST"));
  mImportStrategies.append(strategy);
  strategy = new ZImportStrategyNoSubfolder(tr("/BDMV/STREAM"));
  mImportStrategies.append(strategy);
}

void ZProjectBluRay::contextMenuEvent(QContextMenuEvent *event)
{
//  QModelIndex index = indexAt(event->pos());
//  QDataItem *item = (QDataItem*)index.internalPointer();
//  if (NULL != item) {
//    QMenu menu(this);
//    if (QDataItem::Session == item->GetType()) {
//      menu.addAction(mImportAct);
//      menu.addAction(mResetAct);
//    }
//    if (NULL == mDataTrackItem) {
//      mImportAct->setEnabled(true);
//      mResetAct->setEnabled(false);
//    } else {
//      mImportAct->setEnabled(false);
//      mResetAct->setEnabled(true);
//    }
//    menu.exec(event->globalPos());
//  }
  CommonTreeWidget::contextMenuEvent(event);
}
