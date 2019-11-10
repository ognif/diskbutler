#ifndef ZIMPORT_STRATEGY_H
#define ZIMPORT_STRATEGY_H

#include <QString>

class ZImportStrategy
{
public:
  typedef enum {
    TYPE_NONE,
    TYPE_FIXEDFOLDER,
    TYPE_NOSUBFOLDER,
    TYPE_SUFFIXALLOWED
  } StrategyType;
  typedef enum {
    ERROR_NO = 0,
    ERROR_FIXEDFOLDER_MISS = -1,
    ERROR_SUBFOLDER_NOT_ALLOWED = -2,
    ERROR_SUFFIX_NOT_ALLOWED = -3,
    ERROR_FIXEDFOLDER_EXCLUDE_OTHER_FILE = -4,
    ERROR_FIXEDFOLDER_EXCLUDE_OTHER_FOLDER = -5
  } ImportError;

public:
  ZImportStrategy(const QString &path, StrategyType type = TYPE_NONE)
    : mType(type), mPath(path) {}
  virtual ImportError Check(const QString &root, const QString &path) = 0;
  virtual ImportError Check2(const QString &folder_name, const QString &path) = 0;
  QString& GetError() {return mErr;}
  StrategyType GetType() {return mType;}

protected:
  StrategyType mType;
  QString mPath;
  QString mErr;
};

#endif // ZIMPORT_STRATEGY_H
