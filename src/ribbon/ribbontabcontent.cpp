/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbontabcontent.h"
#include "ui_ribbontabcontent.h"
#include "ribbonbuttongroup.h"

RibbonTabContent::RibbonTabContent(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::RibbonTabContent)
{
  ui->setupUi(this);
}

RibbonTabContent::~RibbonTabContent()
{
  delete ui;
}

void RibbonTabContent::addGroup(const QString &groupName)
{
  RibbonButtonGroup *ribbonButtonGroup = new RibbonButtonGroup;
  ribbonButtonGroup->setTitle(groupName);

  ui->ribbonHorizontalLayout->addWidget(ribbonButtonGroup);
}

void RibbonTabContent::disableGroup(const QString &groupName, bool groupSwitch){
    // Find ribbon group
    for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
    {
      RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ui->ribbonHorizontalLayout->itemAt(i)->widget());
      if (group->title().toLower() == groupName.toLower())
      {
        group->setEnabled(groupSwitch);
        break;
      }
    }
}

void RibbonTabContent::removeGroup(const QString &groupName)
{
  // Find ribbon group
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ui->ribbonHorizontalLayout->itemAt(i)->widget());
    if (group->title().toLower() == groupName.toLower())
    {
      ui->ribbonHorizontalLayout->removeWidget(group); /// \todo :( No effect
      delete group;
      break;
    }
  }

}

int RibbonTabContent::groupCount() const
{
  return ui->ribbonHorizontalLayout->count();
}

void RibbonTabContent::addVLayout(const QString &groupName, QVBoxLayout *vLayout){

    RibbonButtonGroup *ribbonButtonGroup = nullptr;
    for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
    {
      RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ui->ribbonHorizontalLayout->itemAt(i)->widget());
      if (group->title().toLower() == groupName.toLower())
      {
        ribbonButtonGroup = group;
        break;
      }
    }

    if (ribbonButtonGroup != nullptr)
    {

        ribbonButtonGroup->addVertical(vLayout);

    }
    else
    {
      // Group not found
      // Add ribbon group
      addGroup(groupName);

      // Add ribbon button
      addVLayout(groupName, vLayout);
    }

}

void RibbonTabContent::addGLayout(const QString &groupName, QGridLayout *gLayout){

    RibbonButtonGroup *ribbonButtonGroup = nullptr;
    for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
    {
      RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ui->ribbonHorizontalLayout->itemAt(i)->widget());
      if (group->title().toLower() == groupName.toLower())
      {
        ribbonButtonGroup = group;
        break;
      }
    }

    if (ribbonButtonGroup != nullptr)
    {

        ribbonButtonGroup->addGrid(gLayout);

    }
    else
    {
      // Group not found
      // Add ribbon group
      addGroup(groupName);

      // Add ribbon button
      addGLayout(groupName, gLayout);
    }

}

void RibbonTabContent::addWidget(const QString &groupName, QWidget *newItem)
{
    // Find ribbon group
    RibbonButtonGroup *ribbonButtonGroup = nullptr;
    for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
    {
      RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ui->ribbonHorizontalLayout->itemAt(i)->widget());
      if (group->title().toLower() == groupName.toLower())
      {
        ribbonButtonGroup = group;
        break;
      }
    }

    if (ribbonButtonGroup != nullptr)
    {
      // Group found
      // Add ribbon button
        ribbonButtonGroup->addNewWidget(newItem);
    }
    else
    {
      // Group not found
      // Add ribbon group
      addGroup(groupName);

      // Add ribbon button
      addWidget(groupName, newItem);
    }
}

void RibbonTabContent::addCombobox(const QString &groupName, QComboBox *combobbox, QVBoxLayout *vLayout)
{
  // Find ribbon group
  RibbonButtonGroup *ribbonButtonGroup = nullptr;
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ui->ribbonHorizontalLayout->itemAt(i)->widget());
    if (group->title().toLower() == groupName.toLower())
    {
      ribbonButtonGroup = group;
      break;
    }
  }

  if (ribbonButtonGroup != nullptr)
  {
    // Group found
    // Add ribbon button
      ribbonButtonGroup->addVComboBox(combobbox, vLayout);
  }
  else
  {
    // Group not found
    // Add ribbon group
    addGroup(groupName);

    // Add ribbon button
    addCombobox(groupName, combobbox, vLayout);
  }
}

void RibbonTabContent::addButton(const QString &groupName, QToolButton *button, QGridLayout *gLayout)
{
  // Find ribbon group
  RibbonButtonGroup *ribbonButtonGroup = nullptr;
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ui->ribbonHorizontalLayout->itemAt(i)->widget());
    if (group->title().toLower() == groupName.toLower())
    {
      ribbonButtonGroup = group;
      break;
    }
  }

  if (ribbonButtonGroup != nullptr)
  {
    // Group found
    // Add ribbon button
      if(gLayout!=nullptr){
          ribbonButtonGroup->addGButton(button,gLayout);
      }else{
          ribbonButtonGroup->addButton(button);
      }
  }
  else
  {
    // Group not found
    // Add ribbon group
    addGroup(groupName);

    // Add ribbon button
    addButton(groupName, button, gLayout);
  }
}

void RibbonTabContent::addButton(const QString &groupName, QToolButton *button, QVBoxLayout *vLayout)
{
  // Find ribbon group
  RibbonButtonGroup *ribbonButtonGroup = nullptr;
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ui->ribbonHorizontalLayout->itemAt(i)->widget());
    if (group->title().toLower() == groupName.toLower())
    {
      ribbonButtonGroup = group;
      break;
    }
  }

  if (ribbonButtonGroup != nullptr)
  {
    // Group found
    // Add ribbon button
      if(vLayout!=nullptr){
          ribbonButtonGroup->addVButton(button,vLayout);
      }else{
          ribbonButtonGroup->addButton(button);
      }
  }
  else
  {
    // Group not found
    // Add ribbon group
    addGroup(groupName);

    // Add ribbon button
    addButton(groupName, button, vLayout);
  }
}

void RibbonTabContent::removeButton(const QString &groupName, QToolButton *button)
{
  // Find ribbon group
  RibbonButtonGroup *ribbonButtonGroup = nullptr;
  for (int i = 0; i < ui->ribbonHorizontalLayout->count(); i++)
  {
    RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ui->ribbonHorizontalLayout->itemAt(i)->widget());
    if (group->title().toLower() == groupName.toLower())
    {
      ribbonButtonGroup = group;
      break;
    }
  }

  if (ribbonButtonGroup != nullptr)
  {
    // Group found
    // Remove ribbon button
    ribbonButtonGroup->removeButton(button);

    if (ribbonButtonGroup->buttonCount() == 0)
    {
      // Empty button group
      // Remove button group
      removeGroup(groupName);
    }
  }
}
