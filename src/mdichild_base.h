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

#ifndef MDICHILD_BASE_H
#define MDICHILD_BASE_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QPointer>
#include "vrulemanager.h"
#include "utils_common.h"
#include "ribbon.h"

class MdiChildBase : public QMdiSubWindow
{
    Q_OBJECT

public:
    MdiChildBase();
    virtual ~MdiChildBase() {}
    virtual QString updateStatus() {return "";}
    virtual QString getListViewRootPath() const {return "";}
    virtual RuleManager::ProjectType GetProjectType() {return RuleManager::TYPE_PROJECT_OPEN;}
    virtual QString getBurnDrive() {return "";}
    virtual void setBurnDrive(QString) {};
    virtual QString getlastSelectedTab(){ return "";}
    virtual void setlastSelectedTab( QString) {};
    virtual void setUIControls(Ribbon *, QWidget*) {};
    virtual int hasDataChild() {return 0;};
    virtual void setBurnDeviceList(QWidget*) {};

protected:
    QSize minimumSizeHint() const {
        return QSize(555, 370);
    }
private:
    void nonVirtualItems(QWidget* parent);
};

#endif
