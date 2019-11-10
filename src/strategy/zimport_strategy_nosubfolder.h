#ifndef ZIMPORT_STRATEGY_NOSUBFOLDER_H
#define ZIMPORT_STRATEGY_NOSUBFOLDER_H

#include "zimport_strategy.h"

class ZImportStrategyNoSubfolder : public ZImportStrategy
{
public:
  ZImportStrategyNoSubfolder(const QString &path);
  ZImportStrategy::ImportError Check(const QString &root, const QString &path);
  ZImportStrategy::ImportError Check2(const QString &folder_name, const QString &path);
};

#endif // ZIMPORT_STRATEGY_NOSUBFOLDER_H
