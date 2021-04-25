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

#include "zimport_strategy_fixedfolder.h"
#include <QDir>

ZImportStrategyFixedFolder::ZImportStrategyFixedFolder(
    const QString &path, const QStringList &fixedfolders
    , bool exclude_other_all, bool exclude_other_folder)
  : ZImportStrategy(path, ZImportStrategy::TYPE_FIXEDFOLDER)
  , mFolderNames(fixedfolders)
  , mExcludeOtherAll(exclude_other_all)
  , mExcludeOtherFolder(exclude_other_folder)
{
}

ZImportStrategy::ImportError ZImportStrategyFixedFolder::Check(const QString &root, const QString &path)
{
  if (path != mPath) {
    return ERROR_NO;
  }
  ImportError res = ERROR_NO;
  QDir dir(root+path);
  QFileInfoList entries = dir.entryInfoList();
  mErr = "";
  for (int j = 0; j < mFolderNames.size(); j++) {
    QString fixed_folder = mFolderNames[j];
    bool found = false;
    for (int i = 0; i < entries.size(); i++) {
      QFileInfo entry = entries[i];
      if ("." == entry.fileName() || ".." == entry.fileName())
        continue;
      if (entry.isFile())
        continue;
      if (entry.isDir() && 0 == QString::compare(entry.fileName(), fixed_folder, Qt::CaseInsensitive)) {
        found = true;
        break;
      }
    }
    if (!found) {
      res = ERROR_FIXEDFOLDER_MISS;
      mErr += QString("Error: fixed folder '%1' does not exist\n").arg(fixed_folder);
      //break;
    }
  }
  return res;
}

ZImportStrategy::ImportError ZImportStrategyFixedFolder::Check2(
    const QString &folder_name, const QString &path) {
  if (!mPath.endsWith(folder_name)
      || (!mExcludeOtherAll && !mExcludeOtherFolder)) {
    return ERROR_NO;
  }
  ImportError res = ERROR_NO;
  mErr = "";
  QFileInfo entry(path);
  if (path.indexOf("New Folder") != -1) {
    if (mExcludeOtherAll || mExcludeOtherFolder)
      res = ERROR_FIXEDFOLDER_EXCLUDE_OTHER_FOLDER;
  } else if (entry.isDir()) {
    // no err if "path" is a fixed folder that is allowed in current folder "folder_name"
    bool found = false;
    for (int j = 0; j < mFolderNames.size(); j++) {
      if (0 == QString::compare(entry.fileName(), mFolderNames[j], Qt::CaseInsensitive)) {
        found = true;
        break;
      }
    }
    if (!found && (mExcludeOtherAll || mExcludeOtherFolder))
      res = ERROR_FIXEDFOLDER_EXCLUDE_OTHER_FOLDER;
  } else {
    // if it's file or other
    if (mExcludeOtherAll)
      res = ERROR_FIXEDFOLDER_EXCLUDE_OTHER_FILE;
  }
  if (ERROR_NO != res)
    mErr = QString("Error: Cannot add this item into fixed folder '%1'\n")
        .arg(folder_name);

  return res;
}
