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

#include "messanger.h"
#include "utils_common.h"
#include <QMessageBox>


bool showDiskbutlerMessage(int32 nError, QWidget* parent){

    if (nError != BS_SDK_ERROR_NO){

        TCHAR chError[2048];
        int32 nLength = 2048;

        ::GetText(nError,chError, &nLength);

        QString errDesc;
#if defined (WIN32)
        errDesc= QString::fromUtf16(chError);
#else
        errDesc= QString::fromUtf8(chError);
#endif

        QMessageBox::information(getMainWindow(),  QWidget::tr("Diskbutler Error"),
                                  errDesc);
        return false;
    }

    return true;
}


