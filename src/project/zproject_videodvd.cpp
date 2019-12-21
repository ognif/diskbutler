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
#include "zproject_videodvd.h"
#include "zimport_strategy_fixedfolder.h"
#include "zimport_strategy_suffixallowed.h"
#include "zimport_strategy_nosubfolder.h"

ZProjectVideoDVD::ZProjectVideoDVD(QWidget *parent)
  : CommonTreeWidget(RuleManager::TYPE_PROJECT_VIDEODVD, false, parent)
{
  mProjectType = RuleManager::TYPE_PROJECT_VIDEODVD;

  getDataTrackItem();

  QStringList fixed_folders;
  fixed_folders.append(tr("VIDEO_TS"));
  fixed_folders.append(tr("AUDIO_TS"));
  ZImportStrategy *strategy = new ZImportStrategyFixedFolder(tr(""), fixed_folders);
  mImportStrategies.append(strategy);

  addFixedFolder("", fixed_folders);
  createFixedFolder();

  QStringList suffix_allowed;
  suffix_allowed.append(tr("ifo"));
  suffix_allowed.append(tr("bup"));
  suffix_allowed.append(tr("vob"));
  strategy = new ZImportStrategySuffixAllowed(tr("/VIDEO_TS"), suffix_allowed);
  mImportStrategies.append(strategy);

  strategy = new ZImportStrategyNoSubfolder(tr("/VIDEO_TS"));
  mImportStrategies.append(strategy);
  strategy = new ZImportStrategyNoSubfolder(tr("/AUDIO_TS"));
  mImportStrategies.append(strategy);
}

void ZProjectVideoDVD::contextMenuEvent(QContextMenuEvent *event)
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
