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

#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSharedMemory shared("61BB200D-3579-453e-9044-#74");
    if(!shared.create(512,QSharedMemory::ReadWrite)==true)
    {
        //OK, we now dissallow the app to start twice but we need the possibility to push up the existing window.
       // exit(0);
    }

    auto teststyle = R"(
                   QGroupBox{
                     font-size: 13px;
                     font-weight: bold;
                     }
                   )";


    a.setStyleSheet(teststyle);
    a.setApplicationVersion(APP_VERSION);

    /*
     * This is the current way from old backseat developer. I do not like this way.
     * Let check how to change language while running app.
     * Hints:
     * We have createed the language "diskbutler_de.ts"
     * In project file we have:
     * TRANSLATIONS = diskbutler_de.ts
     * CODECFORSRC = UTF-8
     * OTHER_FILES += \
     * diskbutler_de.ts
     * To update this file weh ave to go to:
     * Tools/Externals/Linguist/update translation
     * Currently not known what to translate and what not.
     *
     */

    QTranslator pr;

    QSettings settings("IFoerster", "Diskbutler");
    int nLanguage = settings.value("language",0).toInt();

    if(nLanguage!=0){
        if(nLanguage==1031){
            pr.load( "./diskbutler_de.qm" );
        }
        a.installTranslator( &pr);
    }

    if (QFontDatabase::addApplicationFont(":/res/FontAwesome.otf") < 0)
        qWarning() << "FontAwesome cannot be loaded !";

    MainWindow w;
    w.show();
    int result = a.exec();

    shared.detach();
    return result;
}
