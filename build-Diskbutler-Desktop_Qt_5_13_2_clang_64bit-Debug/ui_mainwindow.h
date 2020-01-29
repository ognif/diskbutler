/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "ribbon/ribbon.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QStatusBar *statusBar;
    QDockWidget *ribbonDockWidget;
    QWidget *ribbonDockWidgetContents;
    QGridLayout *gridLayout;
    Ribbon *ribbonTabWidget;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1005, 660);
        MainWindow->setStyleSheet(QString::fromUtf8("QTabWidget::pane { border: 0; }"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        ribbonDockWidget = new QDockWidget(MainWindow);
        ribbonDockWidget->setObjectName(QString::fromUtf8("ribbonDockWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ribbonDockWidget->sizePolicy().hasHeightForWidth());
        ribbonDockWidget->setSizePolicy(sizePolicy);
        ribbonDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        ribbonDockWidget->setAllowedAreas(Qt::TopDockWidgetArea);
        ribbonDockWidgetContents = new QWidget();
        ribbonDockWidgetContents->setObjectName(QString::fromUtf8("ribbonDockWidgetContents"));
        sizePolicy.setHeightForWidth(ribbonDockWidgetContents->sizePolicy().hasHeightForWidth());
        ribbonDockWidgetContents->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(ribbonDockWidgetContents);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        ribbonTabWidget = new Ribbon(ribbonDockWidgetContents);
        ribbonTabWidget->setObjectName(QString::fromUtf8("ribbonTabWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(ribbonTabWidget->sizePolicy().hasHeightForWidth());
        ribbonTabWidget->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(ribbonTabWidget, 0, 0, 1, 1);

        ribbonDockWidget->setWidget(ribbonDockWidgetContents);
        MainWindow->addDockWidget(Qt::TopDockWidgetArea, ribbonDockWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Qt-Ribbon-Widget", nullptr));
        ribbonDockWidget->setWindowTitle(QCoreApplication::translate("MainWindow", "Ribbon", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
