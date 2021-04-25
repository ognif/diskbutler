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

#include <QMdiArea>
#include <QtWidgets>
#include "mdichild_base.h"
#include "mainwindow.h"

MdiChildBase::MdiChildBase()
{
}


/*
 * Change events include: QEvent::ToolBarChange, QEvent::ActivationChange, QEvent::EnabledChange,
 * QEvent::FontChange, QEvent::StyleChange, QEvent::PaletteChange, QEvent::WindowTitleChange,
 * QEvent::IconTextChange, QEvent::ModifiedChange, QEvent::MouseTrackingChange, QEvent::ParentChange,
 * QEvent::WindowStateChange, QEvent::LanguageChange, QEvent::LocaleChange, QEvent::LayoutDirectionChange.
 */



void MdiChildBase::nonVirtualItems( QWidget* parent )
{
    MainWindow *ribbonOwner = qobject_cast<MainWindow *>( parent );

    switch( GetProjectType() ){
        case RuleManager::TYPE_PROJECT_DISKINFO:
        case RuleManager::TYPE_PROJECT_HEX:
        case RuleManager::TYPE_PROJECT_SCAN:
        case RuleManager::TYPE_PROJECT_DEVICEINFO:
        ribbonOwner->burnGeneralButton->setEnabled( false );
        break;
    default:
        ribbonOwner->burnGeneralButton->setEnabled( true );
    }

}
