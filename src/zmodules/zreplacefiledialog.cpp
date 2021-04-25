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

#include "zreplacefiledialog.h"
#include "ui_zreplacefiledialog.h"
#include <QFont>

ZReplaceFileDialog::ZReplaceFileDialog(QWidget *parent, const QString &flie_name) :
  QDialog(parent),
  ui(new Ui::ZReplaceFileDialog)
{
  ui->setupUi(this);
  ui->label->setText(tr("An item named [%1] already exists in this location."
                        "\nReplace it?").arg(flie_name));
  QFont font = ui->label->font();
  font.setPointSize(10);
  //font.setBold(true);
  ui->label->setFont(font);
}

ZReplaceFileDialog::~ZReplaceFileDialog()
{
  delete ui;
}
