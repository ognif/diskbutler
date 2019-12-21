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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
QT_END_NAMESPACE

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog();


public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void saveAll();
    void enableApply(int index);
    void saveApply();


private:
    void createIcons();
    void sendSaveCommand();

    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;

    QPushButton *applyButton;
    QPushButton *okButton;
    QPushButton *closeButton;
};
#endif // CONFIGDIALOG_H
