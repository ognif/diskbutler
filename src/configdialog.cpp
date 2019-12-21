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

#include <QtGui>

#include "configdialog.h"
#include "settingspages.h"

ConfigDialog::ConfigDialog()
{
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setFlow(QListView::TopToBottom);
    contentsWidget->setWrapping(false);
    contentsWidget->setIconSize(QSize(32, 32));
    //contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setSpacing(6);


    pagesWidget = new QStackedWidget;

    ConfigurationPage *activeConfig = new ConfigurationPage;
    pagesWidget->addWidget(activeConfig);
    //Connect actions for Apply Button
    connect(activeConfig->serverCombo, SIGNAL(currentIndexChanged(int)),
            this , SLOT(enableApply(int)));

    DiskLabelPage *activeLabels = new DiskLabelPage;
    pagesWidget->addWidget(activeLabels);

    MessagePage *activeMessage = new MessagePage;
    pagesWidget->addWidget(activeMessage);
    //Connect actions for Apply Button
    connect(activeMessage->myMessage1, SIGNAL(stateChanged(int)),
            this , SLOT(enableApply(int)));
    connect(activeMessage->myMessage2, SIGNAL(stateChanged(int)),
            this , SLOT(enableApply(int)));

    ISOExPage *activeISOEx = new ISOExPage;
    pagesWidget->addWidget(activeISOEx);
    FilterPage *activeFilter = new FilterPage;
    pagesWidget->addWidget(activeFilter);

    applyButton = new QPushButton(tr("Apply"));
    okButton = new QPushButton(tr("OK"));
    closeButton = new QPushButton(tr("Close"));

    createIcons();
    contentsWidget->setCurrentRow(0);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(saveAll()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(saveApply()));

    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(closeButton);
    buttonsLayout->addWidget(applyButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    applyButton->setEnabled(false);

    setWindowTitle(tr("Diskbutler Settings"));
}

void ConfigDialog::createIcons()
{
    QListWidgetItem *generalBtn = new QListWidgetItem(contentsWidget);
    generalBtn->setIcon(QIcon(":/icons/diskbutler32.png"));
    generalBtn->setText(tr("General"));
    generalBtn->setTextAlignment(Qt::AlignHCenter);
    generalBtn->setSizeHint(QSize(110,60));
    generalBtn->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *labelsBtn = new QListWidgetItem(contentsWidget);
    labelsBtn->setIcon(QIcon(":/icons/cd_label.png"));
    labelsBtn->setText(tr("Labels"));
    labelsBtn->setTextAlignment(Qt::AlignHCenter);
    labelsBtn->setSizeHint(QSize(110,60));
    labelsBtn->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *messageBtn = new QListWidgetItem(contentsWidget);
    messageBtn->setIcon(QIcon(":/icons/messages.png"));
    messageBtn->setText(tr("Messages"));
    messageBtn->setTextAlignment(Qt::AlignHCenter);
    messageBtn->setSizeHint(QSize(110,60));
    messageBtn->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *isoexBtn = new QListWidgetItem(contentsWidget);
    isoexBtn->setIcon(QIcon(":/icons/extiso.png"));
    isoexBtn->setText(tr("ISO Extended"));
    isoexBtn->setTextAlignment(Qt::AlignHCenter);
    isoexBtn->setSizeHint(QSize(110,60));
    isoexBtn->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *filterBtn = new QListWidgetItem(contentsWidget);
    filterBtn->setIcon(QIcon(":/icons/filter_data.png"));
    filterBtn->setText(tr("File Exclusion"));
    filterBtn->setTextAlignment(Qt::AlignHCenter);
    filterBtn->setSizeHint(QSize(110,60));
    filterBtn->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void ConfigDialog::saveAll()
{
    sendSaveCommand();

    applyButton->setEnabled(false);

    done(1); //Assume 1 for Success
}

void ConfigDialog::saveApply()
{
    sendSaveCommand();
    applyButton->setEnabled(false);
}

void ConfigDialog::sendSaveCommand()
{
    ConfigurationPage *activeConfigPage =  dynamic_cast<ConfigurationPage*>(pagesWidget->widget(0));
    activeConfigPage->saveSettings();

    DiskLabelPage *activeDiskLabelPage =  dynamic_cast<DiskLabelPage*>(pagesWidget->widget(1));
    activeDiskLabelPage->saveSettings();

    MessagePage *activeMessagePage =  dynamic_cast<MessagePage*>(pagesWidget->widget(2));
    activeMessagePage->saveSettings();

    ISOExPage *activeISOExPage =  dynamic_cast<ISOExPage*>(pagesWidget->widget(3));
    activeISOExPage->saveSettings();

    FilterPage *activeFilterPage =  dynamic_cast<FilterPage*>(pagesWidget->widget(4));
    activeFilterPage->saveSettings();
}


void ConfigDialog::enableApply(int index)
{
    applyButton->setEnabled(true);
}
