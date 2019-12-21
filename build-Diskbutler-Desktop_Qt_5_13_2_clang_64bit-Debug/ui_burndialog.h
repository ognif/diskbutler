/********************************************************************************
** Form generated from reading UI file 'burndialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BURNDIALOG_H
#define UI_BURNDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_burnDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QListWidget *listWidget;
    QFrame *line_2;
    QGridLayout *gridLayout_2;
    QLabel *label_4;
    QLabel *label;
    QLabel *label_6;
    QLabel *currentSpeed;
    QLabel *currentTask;
    QSpacerItem *horizontalSpacer;
    QLabel *totalTime;
    QFrame *line;
    QLabel *label_2;
    QProgressBar *bufferValue;
    QLabel *label_3;
    QProgressBar *totalProgress;
    QGridLayout *gridLayout;
    QPushButton *saveLog;
    QPushButton *cancel;
    QPushButton *testmenu;
    QComboBox *logLevel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *burnDialog)
    {
        if (burnDialog->objectName().isEmpty())
            burnDialog->setObjectName(QString::fromUtf8("burnDialog"));
        burnDialog->resize(688, 558);
        verticalLayout_2 = new QVBoxLayout(burnDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        listWidget = new QListWidget(burnDialog);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setAutoScroll(false);
        listWidget->setLayoutMode(QListView::SinglePass);
        listWidget->setBatchSize(4);

        verticalLayout->addWidget(listWidget);

        line_2 = new QFrame(burnDialog);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_4 = new QLabel(burnDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);

        label = new QLabel(burnDialog);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        label_6 = new QLabel(burnDialog);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_6, 0, 3, 1, 1);

        currentSpeed = new QLabel(burnDialog);
        currentSpeed->setObjectName(QString::fromUtf8("currentSpeed"));

        gridLayout_2->addWidget(currentSpeed, 1, 1, 1, 1);

        currentTask = new QLabel(burnDialog);
        currentTask->setObjectName(QString::fromUtf8("currentTask"));

        gridLayout_2->addWidget(currentTask, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(200, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 2, 1, 1);

        totalTime = new QLabel(burnDialog);
        totalTime->setObjectName(QString::fromUtf8("totalTime"));
        totalTime->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(totalTime, 0, 4, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        line = new QFrame(burnDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        label_2 = new QLabel(burnDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        bufferValue = new QProgressBar(burnDialog);
        bufferValue->setObjectName(QString::fromUtf8("bufferValue"));
        bufferValue->setValue(24);
        bufferValue->setTextVisible(false);

        verticalLayout->addWidget(bufferValue);

        label_3 = new QLabel(burnDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        totalProgress = new QProgressBar(burnDialog);
        totalProgress->setObjectName(QString::fromUtf8("totalProgress"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(totalProgress->sizePolicy().hasHeightForWidth());
        totalProgress->setSizePolicy(sizePolicy);
        totalProgress->setValue(24);
        totalProgress->setTextVisible(false);

        verticalLayout->addWidget(totalProgress);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        saveLog = new QPushButton(burnDialog);
        saveLog->setObjectName(QString::fromUtf8("saveLog"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(saveLog->sizePolicy().hasHeightForWidth());
        saveLog->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(saveLog, 0, 1, 1, 1);

        cancel = new QPushButton(burnDialog);
        cancel->setObjectName(QString::fromUtf8("cancel"));
        sizePolicy1.setHeightForWidth(cancel->sizePolicy().hasHeightForWidth());
        cancel->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(cancel, 0, 0, 1, 1);

        testmenu = new QPushButton(burnDialog);
        testmenu->setObjectName(QString::fromUtf8("testmenu"));
        sizePolicy1.setHeightForWidth(testmenu->sizePolicy().hasHeightForWidth());
        testmenu->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(testmenu, 0, 4, 1, 1);

        logLevel = new QComboBox(burnDialog);
        logLevel->setObjectName(QString::fromUtf8("logLevel"));
        sizePolicy1.setHeightForWidth(logLevel->sizePolicy().hasHeightForWidth());
        logLevel->setSizePolicy(sizePolicy1);
        logLevel->setMinimumSize(QSize(200, 0));

        gridLayout->addWidget(logLevel, 0, 3, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(burnDialog);

        QMetaObject::connectSlotsByName(burnDialog);
    } // setupUi

    void retranslateUi(QDialog *burnDialog)
    {
        burnDialog->setWindowTitle(QCoreApplication::translate("burnDialog", "Dialog", nullptr));
        label_4->setText(QCoreApplication::translate("burnDialog", "Write:", nullptr));
        label->setText(QCoreApplication::translate("burnDialog", "Current Speed:", nullptr));
        label_6->setText(QCoreApplication::translate("burnDialog", "Total Time:", nullptr));
        currentSpeed->setText(QCoreApplication::translate("burnDialog", "- speedd label - ", nullptr));
        currentTask->setText(QCoreApplication::translate("burnDialog", "- current write task -", nullptr));
        totalTime->setText(QCoreApplication::translate("burnDialog", "- time -", nullptr));
        label_2->setText(QCoreApplication::translate("burnDialog", "Current Buffer", nullptr));
        label_3->setText(QCoreApplication::translate("burnDialog", "Total Prorgess", nullptr));
        saveLog->setText(QCoreApplication::translate("burnDialog", "Save Log", nullptr));
        cancel->setText(QCoreApplication::translate("burnDialog", "Cancel", nullptr));
        testmenu->setText(QCoreApplication::translate("burnDialog", "New burn", nullptr));
    } // retranslateUi

};

namespace Ui {
    class burnDialog: public Ui_burnDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BURNDIALOG_H
