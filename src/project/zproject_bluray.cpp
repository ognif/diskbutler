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
