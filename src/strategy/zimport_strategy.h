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
