/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbonbuttongroup.h"
#include "ui_ribbonbuttongroup.h"

#include <QToolButton>


RibbonButtonGroup::RibbonButtonGroup(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::RibbonButtonGroup)
  , m_title("")
{
  ui->setupUi(this);
}

RibbonButtonGroup::~RibbonButtonGroup()
{
  delete ui;
}

void RibbonButtonGroup::setTitle(const QString &title)
{
  m_title = title;
  ui->label->setText(m_title);
}

QString RibbonButtonGroup::title() const
{
  return m_title;
}

int RibbonButtonGroup::buttonCount() const
{
  return ui->horizontalLayout->count();
}

void RibbonButtonGroup::addButton(QToolButton *button)
{
  button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  button->setMinimumSize(64, 80);
  //button->setStyleSheet("QToolButton { margin-top: 10px; } "); //NO access to the Iocn
  button->setAutoRaise(true);
  button->setIconSize(QSize(32,32));

  button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  ui->horizontalLayout->addWidget(button);
}

void RibbonButtonGroup::removeButton(QToolButton *button)
{

  ui->horizontalLayout->removeWidget(button);
}

void RibbonButtonGroup::addVertical(QVBoxLayout *vLayout){

    ui->horizontalLayout->addLayout(vLayout);
}

void RibbonButtonGroup::addGrid(QGridLayout *gLayout){

    ui->horizontalLayout->addLayout(gLayout);
}

void RibbonButtonGroup::addVButton(QToolButton *button, QVBoxLayout *vLayout)
{

  button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  button->setMinimumSize(16, 16);
  button->setAutoRaise(true); //Should decide between flat and 3D?
  button->setIconSize(QSize(16,16));
  button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  vLayout->addWidget(button);
}

void RibbonButtonGroup::addGButton(QToolButton *button, QGridLayout *gLayout)
{

  button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  button->setMinimumSize(16, 16);
  button->setAutoRaise(true); //Should decide between flat and 3D?
  button->setIconSize(QSize(16,16));
  button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  qDebug() << gLayout->rowCount();
  gLayout->addWidget(button,gLayout->rowCount(),0);
}

void RibbonButtonGroup::addVComboBox(QComboBox *comboBox, QVBoxLayout *vLayout)
{
  //comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  vLayout->addWidget(comboBox);
}

void RibbonButtonGroup::addNewWidget(QWidget *newItem)
{
  //comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  ui->horizontalLayout->addWidget(newItem);
}
