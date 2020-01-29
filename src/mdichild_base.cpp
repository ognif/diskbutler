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

#include <QMdiArea>
#include <QtWidgets>
#include "mdichild_base.h"


MdiChildBase::MdiChildBase()
{
}


/*
 * Change events include: QEvent::ToolBarChange, QEvent::ActivationChange, QEvent::EnabledChange,
 * QEvent::FontChange, QEvent::StyleChange, QEvent::PaletteChange, QEvent::WindowTitleChange,
 * QEvent::IconTextChange, QEvent::ModifiedChange, QEvent::MouseTrackingChange, QEvent::ParentChange,
 * QEvent::WindowStateChange, QEvent::LanguageChange, QEvent::LocaleChange, QEvent::LayoutDirectionChange.
 */

//emit subwindowchanged(GetProjectType());


void MdiChildBase::windowStateChanged(Qt::WindowStates oldState, Qt::WindowStates newState)
{
    qDebug("StateChanged");
    if(newState & Qt::WindowActive){
        qDebug("ich bin aktiv");
    }
}

void MdiChildBase::changeEvent(QEvent * e) {

    //QMdiSubWindow::changeEvent(e);
    //Das Problem ist, er geht auf Hidden und der andere auf sichbar Wir müssen also nur activvieren sehen.
    if (e->type() == QEvent::WindowStateChange) {
      if (this->windowState() & Qt::WindowActive) {
        emit subwindowchanged(GetProjectType());
      }
      QMdiSubWindow::changeEvent(e);
    }
    /*
    if(e->type() == QEvent::WindowStateChange && this->isActiveWindow()) {
       // .. this is now the active window
        //qDebug("Iam active now");
        emit subwindowchanged(GetProjectType());
    }
    */

}

