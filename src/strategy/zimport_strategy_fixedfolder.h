#ifndef ZIMPORT_STRATEGY_FIXEDFOLDER_H
#define ZIMPORT_STRATEGY_FIXEDFOLDER_H

#include "zimport_strategy.h"
#include <QStringList>

class ZImportStrategyFixedFolder : public ZImportStrategy
{
public:
  ZImportStrategyFixedFolder(const QString &path, const QStringList &fixedfolders
                             , bool exclude_other_all = false, bool exclude_other_folder = false);
  ZImportStrategy::ImportError Check(const QString &root, const QString &path);
  ZImportStrategy::ImportError Check2(const QString &folder_name, const QString &path);
  QStringList GetFixedFolders() {return mFolderNames;}

private:
  QStringList mFolderNames;
  bool mExcludeOtherAll;
  bool mExcludeOtherFolder;
};

#endif // ZIMPORT_STRATEGY_FIXEDFOLDER_H
