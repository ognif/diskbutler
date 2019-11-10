#ifndef ZIMPORT_STRATEGY_SUFFIXALLOWED_H
#define ZIMPORT_STRATEGY_SUFFIXALLOWED_H

#include "zimport_strategy.h"
#include <QStringList>

class ZImportStrategySuffixAllowed : public ZImportStrategy
{
public:
  ZImportStrategySuffixAllowed(const QString &path, const QStringList &suffix_list);
  ZImportStrategy::ImportError Check(const QString &root, const QString &path);
  ZImportStrategy::ImportError Check2(const QString &folder_name, const QString &path);

private:
  QStringList mSuffixList;
};

#endif // ZIMPORT_STRATEGY_SUFFIXALLOWED_H
