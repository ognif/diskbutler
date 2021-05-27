/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbon.h"
#include "ribbontabcontent.h"

#include <QApplication>
#include <QStyleOption>
#include <QPainter>

Ribbon::Ribbon(QWidget *parent)
    : QTabWidget(parent)
{
    // Determine default colors
    QColor bg = qApp->palette().color(QPalette::Window);
    QColor mid = qApp->palette().color(QPalette::Mid);

    // Note: the order in which the background/palette/stylesheet functions are
    // called does matter. Should be same as in Qt designer.
    setAutoFillBackground(true);

    // Set stylesheet
    QString styleSheetText = QString(
                "QTabWidget::pane {"
    "  border-top: 1px solid rgb(%4, %5, %6);"
    "  position: absolute;"
    "  top: -1px;"
    "}"
    ""
    "QTabBar::tab {"
    "  padding-top: 5px;"
    "  padding-bottom: 5px;"
    "  padding-left: 10px;"
    "  padding-right: 10px;"
    "  margin-top: 1px;"
    "}"
    ""
    "QTabBar::tab::!selected {"
    "  border-bottom: 1px solid rgb(%4, %5, %6);"
    "  background-color:#ffffff;"
    "}"
    ""
    "QTabBar::tab:selected {"
    "  background-color: rgb(%1, %2, %3);"
    "  border-top: 1px solid rgb(%4, %5, %6);"
    "  border-right: 1px solid rgb(%4, %5, %6);"
    "  border-left: 1px solid rgb(%4, %5, %6);"
    "  border-bottom: 1px solid rgb(%1, %2, %3);"
    "}"
    ""
    "QTabBar::tab:hover"
    "{"
    "  background-color: rgb(205, 232, 255);"
    "}"
    ""
    "QTabBar::tab:selected:hover {"
    "  background-color: rgb(%1, %2, %3);"
    "}"
    ).arg(bg.red()).arg(bg.green()).arg(bg.blue())
            .arg(mid.red()).arg(mid.green()).arg(mid.blue());

    setStyleSheet(styleSheetText);

    // Set background color
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void Ribbon::addTab(const QString &tabName)
{
    // Note: superclass QTabWidget also has a function addTab()
    RibbonTabContent *ribbonTabContent = new RibbonTabContent;
    QTabWidget::addTab(ribbonTabContent, tabName);
}

void Ribbon::addTab(const QIcon &tabIcon, const QString &tabName)
{
    // Note: superclass QTabWidget also has a function addTab()
    RibbonTabContent *ribbonTabContent = new RibbonTabContent;
    QTabWidget::addTab(ribbonTabContent, tabIcon, tabName);
}

void Ribbon::showTab(const QString &tabIcon, const QString &tabName)
{
    auto tab = tabManager.take(tabName.toLower());
    if(tab){

        QTabWidget::insertTab(QTabWidget::count()-1, tab, QIcon(tabIcon), tabName);
        tabManager.remove(tabName.toLower());
    }
}

int Ribbon::getIndexByName(const QString &tabName)
{
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            // QWidget *tab = QTabWidget::widget(i);
            return i;
        }

    }
    return -1;
}

void Ribbon::currentTab(const QString &tabName)
{
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            // QWidget *tab = QTabWidget::widget(i);
            QTabWidget::setCurrentIndex(i);
        }

    }
}

void Ribbon::hideTab(const QString &tabName)
{
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            QWidget *tab = QTabWidget::widget(i);
            tabManager.insert(tabName.toLower(),tab);
            QTabWidget::removeTab(i);
        }

    }
}

void Ribbon::removeTab(const QString &tabName)
{
    // Find ribbon tab
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            // Remove tab
            QWidget *tab = QTabWidget::widget(i);
            QTabWidget::removeTab(i);
            delete tab;
            break;
        }
    }
}

void Ribbon::addGroup(const QString &tabName, const QString &groupName)
{
    // Find ribbon tab
    QWidget *tab = nullptr;
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            tab = QTabWidget::widget(i);
            break;
        }
    }

    if (tab != nullptr)
    {
        // Tab found
        // Add ribbon group
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        ribbonTabContent->addGroup(groupName);
    }
    else
    {
        // Tab not found
        // Create tab
        addTab(tabName);

        // Add ribbon group
        addGroup(tabName, groupName);
    }
}

void Ribbon::addGButton(const QString &tabName, const QString &groupName, QToolButton *button, QGridLayout *gLayout)
{
    // Find ribbon tab
    QWidget *tab = nullptr;
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            tab = QTabWidget::widget(i);
            break;
        }
    }

    if (tab != nullptr)
    {
        // Tab found
        // Add ribbon button
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        ribbonTabContent->addButton(groupName, button, gLayout);
    }
    else
    {
        // Tab not found.
        // Create tab
        addTab(tabName);

        // Add ribbon button
        addGButton(tabName, groupName, button, gLayout);
    }
}

void Ribbon::addVButton(const QString &tabName, const QString &groupName, QToolButton *button, QVBoxLayout *vLayout)
{
    // Find ribbon tab
    QWidget *tab = nullptr;
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            tab = QTabWidget::widget(i);
            break;
        }
    }

    if (tab != nullptr)
    {
        // Tab found
        // Add ribbon button
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        ribbonTabContent->addButton(groupName, button, vLayout);
    }
    else
    {
        // Tab not found.
        // Create tab
        addTab(tabName);

        // Add ribbon button
        addVButton(tabName, groupName, button, vLayout);
    }
}
void Ribbon::addWidgetGroup(const QString &tabName, const QString &groupName, QWidget *newItem)
{
    // Find ribbon tab
    QWidget *tab = nullptr;
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            tab = QTabWidget::widget(i);
            break;
        }
    }

    if (tab != nullptr)
    {
        // Tab found
        // Add ribbon button
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        ribbonTabContent->addWidget(groupName, newItem);
    }
    else
    {
        // Tab not found.
        // Create tab
        addTab(tabName);

        // Add ribbon button
        addWidgetGroup(tabName, groupName, newItem);
    }
}

void Ribbon::setTabIndex(int index)
{
    QTabWidget::setCurrentIndex(index);
}

void Ribbon::addComboBox(const QString &tabName, const QString &groupName, QComboBox *combobox, QVBoxLayout *vLayout)
{
    // Find ribbon tab
    QWidget *tab = nullptr;
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            tab = QTabWidget::widget(i);
            break;
        }
    }

    if (tab != nullptr)
    {
        // Tab found
        // Add ribbon button
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        ribbonTabContent->addCombobox(groupName, combobox, vLayout);
    }
    else
    {
        // Tab not found.
        // Create tab
        addTab(tabName);

        // Add ribbon button
        addComboBox(tabName, groupName, combobox, vLayout);
    }
}

void Ribbon::addGridLayout(const QString &tabName, const QString &groupName, QGridLayout *gLayout){

    QWidget *tab = nullptr;
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            tab = QTabWidget::widget(i);
            break;
        }
    }

    if (tab != nullptr)
    {
        // Tab found
        // Add ribbon button
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        ribbonTabContent->addGLayout(groupName, gLayout);
    }
}

void Ribbon::addVerticalLayout(const QString &tabName, const QString &groupName, QVBoxLayout *vLayout){

    QWidget *tab = nullptr;
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            tab = QTabWidget::widget(i);
            break;
        }
    }

    if (tab != nullptr)
    {
        // Tab found
        // Add ribbon button
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        ribbonTabContent->addVLayout(groupName, vLayout);
    }
}

void Ribbon::removeButton(const QString &tabName, const QString &groupName, QToolButton *button)
{
    // Find ribbon tab
    QWidget *tab = nullptr;
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            tab = QTabWidget::widget(i);
            break;
        }
    }

    if (tab != nullptr)
    {
        // Tab found
        // Remove ribbon button
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        ribbonTabContent->removeButton(groupName, button);

        if (ribbonTabContent->groupCount() == 0)
        {
            removeTab(tabName);
        }
    }
}

void Ribbon::disableButtonGroup(const QString &tabName, const QString &groupName, bool buttonSwitch)
{
    // Find ribbon tab
    QWidget *tab = nullptr;
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            tab = QTabWidget::widget(i);
            break;
        }
    }

    if (tab != nullptr)
    {
        // Tab found
        // Remove ribbon button
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        ribbonTabContent->disableGroup(groupName, buttonSwitch);

    }
}

bool Ribbon::isTabVisible(const QString &tabName)
{
    for (int i = 0; i < count(); i++)
    {
        if (tabText(i).toLower() == tabName.toLower())
        {
            return true;
        }
    }

    return false;
}

void Ribbon::hideAll()
{
    hideTab(tr("File System"));
    hideTab(tr("ISO Extended"));
    hideTab(tr("Boot Disc"));
    hideTab(tr("Audio"));
    hideTab(tr("Scan Editor"));
    hideTab(tr("Hex Editor"));
    hideTab(tr("Create image"));

}


