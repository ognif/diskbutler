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

#ifndef SETTINGSPAGES_H
#define SETTINGSPAGES_H

#include <QtWidgets>
#include <QWidget>
#include <QComboBox>
#include <QListWidget>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QSettings>
#include <QGroupBox>
#include <QRadioButton>
#include "vrulemanager.h"

class ConfigurationPage : public QWidget
{
    Q_OBJECT

public:
    ConfigurationPage(QWidget *parent = 0);
    void saveSettings();

    static QSettings mSettings;
    QComboBox *serverCombo;

    QRadioButton *mAddAudioToDataTrack;
    QRadioButton *mAddAudioToAudioTrack;
    QCheckBox *mAddAudioToDataTrackToo;
    QCheckBox *mCreateDataTrackAuto;
    QCheckBox *mCreateAndShowChecksum;
    QLineEdit *mChecksumFileSizeEdit;
    QCheckBox *mImportAutoCorrect;

    QComboBox *fsCombo;
    QComboBox *mDefaultProjectCombo;
private slots:
    void on_checkAddAudioToDataTrack_clicked(bool checked);
    void on_checkAddAudioToAudioTrack_clicked(bool checked);
    void on_checkCreateAndShowChecksum_clicked(bool checked);
};

class DiskLabelPage : public QWidget
{
    Q_OBJECT

public:

    DiskLabelPage(QWidget *parent = 0);
    void saveSettings();
    void switchGroup(bool isSimple);

    QRadioButton *mLabelSimple;
    QLineEdit *mDiskSimpleName;
    QRadioButton *mLabelCustom;
    QLineEdit *mDiskCustomPattern;
    QLineEdit *mPatternDefaultString;
    QLineEdit *mPatternDefaultNumber;
    QLabel *sPatternNumber;
    QLabel *sPatternString;
private slots:
    void on_mLabelSimple_clicked(bool checked);
    void on_mLabelCustom_clicked(bool checked);
};

class MessagePage : public QWidget
{
    Q_OBJECT


public:
    MessagePage(QWidget *parent = 0);
    void saveSettings();
    QListWidget *listWidget;

    QCheckBox *myMessage1;
    QCheckBox *myMessage2;
    QCheckBox *myMessage3;
};

class ISOExPage : public QWidget
{
  Q_OBJECT

public:
  ISOExPage(QWidget *parent = 0);
  void saveSettings();

public:
  QLineEdit *mEditSystemId;
  QLineEdit *mEditVolumeSet;
  QLineEdit *mEditPublisher;
  QLineEdit *mEditDatapreparer;
  QLineEdit *mEditApp;
  QLineEdit *mEditCorightFile;
  QLineEdit *mEditAbstractFile;
  QLineEdit *mEditBibliographicFile;
};

class FilterPage : public QWidget
{
  Q_OBJECT


public:
  FilterPage(QWidget *parent = 0);
  void saveSettings();

public:
  QListWidget *mListWidget;
  QCheckBox *mCheckByDate;
  QDateEdit *mDateFrom;
  QDateEdit *mDateTo;
  QPushButton *mButtonAdd;
  QPushButton *mButtonRemove;
private slots:
  void on_mButtonAdd_clicked();
  void on_mButtonRemove_clicked();
  void on_checkByDate_clicked(bool checked);
};

class PluginPage : public QWidget
{
  Q_OBJECT


public:
  PluginPage(QWidget *parent = 0);
  void saveSettings();
  bool readPlugins();

public:
  QListWidget *mListWidget;

private:
  void pluginsSub(QString strName, QString strPath, QStringList excludeList);

};

#endif // SETTINGSPAGES_H
