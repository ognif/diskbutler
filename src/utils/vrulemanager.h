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

#ifndef VRULEMANAGER_H
#define VRULEMANAGER_H

#include "QString"

class RuleManager {
public:
  typedef enum {
    TYPE_PROJECT_EXPLORE,
    TYPE_PROJECT_OPEN,
    TYPE_PROJECT_AUDIOCD,
    TYPE_PROJECT_MIXEDMODE,
    TYPE_PROJECT_VIDEODVD,
    TYPE_PROJECT_BLURAYVIDEO,
    TYPE_PROJECT_UDF,
    TYPE_PROJECT_ISO,
    TYPE_PROJECT_ISOUDF,
    TYPE_PROJECT_VIDEOCD,
    TYPE_PROJECT_SUPERVIDEO,
    TYPE_PROJECT_MAX,
    TYPE_PROJECT_DISKINFO,
    TYPE_PROJECT_DEVICEINFO,
    TYPE_PROJECT_HEX,
    TYPE_PROJECT_SCAN
  } ProjectType;

  typedef enum {
    //filesystems
    OPTION_FILESYSTEMS_ISO9660,
    OPTION_FILESYSTEMS_JOLIET,
    OPTION_FILESYSTEMS_UDF102,
    OPTION_FILESYSTEMS_UDF15,
    OPTION_FILESYSTEMS_UDF20,
    OPTION_FILESYSTEMS_UDF201,
    OPTION_FILESYSTEMS_UDF25,
    OPTION_FILESYSTEMS_UDF26,
    //specials
    OPTION_SPECIALS_ISOEXTINFO,
    OPTION_SPECIALS_BootCD_DVD_BD,
    OPTION_SPECIALS_AVCHD,
    //UDF
    OPTION_UDF_PARTITION,
    OPTION_UDF_WRITE_FILE_STREAM,
    //mode
    OPTION_MODE_MODE1,
    OPTION_MODE_MODE2,
    //Filesysteme Extended
    OPTION_FILESYSTEMEXT_ISOLEVEL1,
    OPTION_FILESYSTEMEXT_ISOLEVEL2,
    OPTION_FILESYSTEMEXT_ISOLEVEL3,
    OPTION_FILESYSTEMEXT_ROMEO,
    OPTION_FILESYSTEMEXT_ALLOW_LONGISO_FILENAMES,
    OPTION_FILESYSTEMEXT_ALLOW_LONGJOLIET_FILENAMES,
    OPTION_FILESYSTEMEXT_ALLOW_LOWERCASE_FILENAMES,
    OPTION_FILESYSTEMEXT_ALLOW_MANY_DIRECTORIES,
    OPTION_FILESYSTEMEXT_ROCK_RIDGE,
    //settings
    OPTION_SETTINGS_FINALIZE,
    OPTION_SETTINGS_BURNPROOF,
    OPTION_SETTINGS_VERIFY,
    OPTION_SETTINGS_EJECT,
    OPTION_SETTINGS_SIMULATE,
    OPTION_SETTINGS_MULTISESSION,
    OPTION_SETTINGS_PADDATATRACKS,
    OPTION_SETTINGS_AUTOERASE,
    OPTION_SETTINGS_OPC,
    //Disk Types
    OPTION_DISKTYPES_DVD,
    OPTION_DISKTYPES_BD,
    OPTION_DISKTYPES_CD,
    //Write Method
    OPTION_WRITEMETHOD_TRACKATONCE,
    OPTION_WRITEMETHOD_DISKATONCE,
    OPTION_WRITEMETHOD_DISKATONCE96,
    OPTION_FILESYSTEMS_UDF,
    OPTION_MAX
  } OptionID;

  typedef struct {
    ProjectType type;
    OptionID *ruleArray;
    int arraySize;
  } Rule;

  typedef struct {
    OptionID id1;
    OptionID id2;
  } Rule_Exception;

  static OptionID rule_AudioCD[];
  static OptionID rule_MixMode[];
  static OptionID rule_VideoDVD[];
  static OptionID rule_BlurayVideo[];
  static OptionID rule_UDF[];
  static OptionID rule_ISO[];
  static OptionID rule_ISOUDF[];
  static OptionID rule_VideoCD[];
  static OptionID rule_SuperVideoCD[];
  static Rule rules[];
  static QString rule_String[];
  static Rule_Exception rule_Exception[];

public:
  static bool IsOptionAllowed(ProjectType type, OptionID id);
  static int GetException(OptionID id1, OptionID id2);
  static QString GetOptionsStr(ProjectType type);
  static QString GetProjectTypeStr(ProjectType type);
};

#endif // VRULEMANAGER_H
