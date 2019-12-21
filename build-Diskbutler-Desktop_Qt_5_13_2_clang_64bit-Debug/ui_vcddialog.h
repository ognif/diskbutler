/********************************************************************************
** Form generated from reading UI file 'vcddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VCDDIALOG_H
#define UI_VCDDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VCDDialog
{
public:
    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QWidget *tabCDText;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLineEdit *editArranger;
    QLineEdit *editComposer;
    QLineEdit *editSongWriter;
    QLineEdit *editPerformer;
    QLineEdit *editMessage;
    QLineEdit *editTitle;
    QLineEdit *editUPCEAN;
    QWidget *layoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QSpinBox *editPause;
    QCheckBox *pauseInFrames;
    QWidget *tabIndexes;
    QListWidget *listIndexes;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_19;
    QSpinBox *spinBoxMinutes;
    QSpinBox *spinBoxSeconds;
    QSpinBox *spinBoxFrames;
    QPushButton *pushButtonAdd;
    QPushButton *pushButtonDelete;

    void setupUi(QDialog *VCDDialog)
    {
        if (VCDDialog->objectName().isEmpty())
            VCDDialog->setObjectName(QString::fromUtf8("VCDDialog"));
        VCDDialog->resize(428, 357);
        buttonBox = new QDialogButtonBox(VCDDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(70, 320, 340, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        tabWidget = new QTabWidget(VCDDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(20, 20, 390, 290));
        tabCDText = new QWidget();
        tabCDText->setObjectName(QString::fromUtf8("tabCDText"));
        layoutWidget = new QWidget(tabCDText);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(84, 1, 290, 181));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        editArranger = new QLineEdit(layoutWidget);
        editArranger->setObjectName(QString::fromUtf8("editArranger"));

        verticalLayout->addWidget(editArranger);

        editComposer = new QLineEdit(layoutWidget);
        editComposer->setObjectName(QString::fromUtf8("editComposer"));

        verticalLayout->addWidget(editComposer);

        editSongWriter = new QLineEdit(layoutWidget);
        editSongWriter->setObjectName(QString::fromUtf8("editSongWriter"));

        verticalLayout->addWidget(editSongWriter);

        editPerformer = new QLineEdit(layoutWidget);
        editPerformer->setObjectName(QString::fromUtf8("editPerformer"));

        verticalLayout->addWidget(editPerformer);

        editMessage = new QLineEdit(layoutWidget);
        editMessage->setObjectName(QString::fromUtf8("editMessage"));

        verticalLayout->addWidget(editMessage);

        editTitle = new QLineEdit(layoutWidget);
        editTitle->setObjectName(QString::fromUtf8("editTitle"));

        verticalLayout->addWidget(editTitle);

        editUPCEAN = new QLineEdit(layoutWidget);
        editUPCEAN->setObjectName(QString::fromUtf8("editUPCEAN"));

        verticalLayout->addWidget(editUPCEAN);

        layoutWidget_2 = new QWidget(tabCDText);
        layoutWidget_2->setObjectName(QString::fromUtf8("layoutWidget_2"));
        layoutWidget_2->setGeometry(QRect(14, 4, 71, 201));
        verticalLayout_2 = new QVBoxLayout(layoutWidget_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget_2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_2->addWidget(label);

        label_2 = new QLabel(layoutWidget_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_2->addWidget(label_2);

        label_3 = new QLabel(layoutWidget_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_2->addWidget(label_3);

        label_4 = new QLabel(layoutWidget_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_2->addWidget(label_4);

        label_5 = new QLabel(layoutWidget_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_2->addWidget(label_5);

        label_6 = new QLabel(layoutWidget_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        verticalLayout_2->addWidget(label_6);

        label_7 = new QLabel(layoutWidget_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_2->addWidget(label_7);

        label_8 = new QLabel(layoutWidget_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        verticalLayout_2->addWidget(label_8);

        editPause = new QSpinBox(tabCDText);
        editPause->setObjectName(QString::fromUtf8("editPause"));
        editPause->setGeometry(QRect(86, 184, 61, 21));
        pauseInFrames = new QCheckBox(tabCDText);
        pauseInFrames->setObjectName(QString::fromUtf8("pauseInFrames"));
        pauseInFrames->setGeometry(QRect(90, 210, 121, 17));
        tabWidget->addTab(tabCDText, QString());
        tabIndexes = new QWidget();
        tabIndexes->setObjectName(QString::fromUtf8("tabIndexes"));
        listIndexes = new QListWidget(tabIndexes);
        listIndexes->setObjectName(QString::fromUtf8("listIndexes"));
        listIndexes->setGeometry(QRect(10, 10, 151, 240));
        label_17 = new QLabel(tabIndexes);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(170, 10, 21, 21));
        label_18 = new QLabel(tabIndexes);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(240, 10, 21, 21));
        label_19 = new QLabel(tabIndexes);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setGeometry(QRect(310, 10, 21, 21));
        spinBoxMinutes = new QSpinBox(tabIndexes);
        spinBoxMinutes->setObjectName(QString::fromUtf8("spinBoxMinutes"));
        spinBoxMinutes->setGeometry(QRect(190, 10, 42, 22));
        spinBoxSeconds = new QSpinBox(tabIndexes);
        spinBoxSeconds->setObjectName(QString::fromUtf8("spinBoxSeconds"));
        spinBoxSeconds->setGeometry(QRect(260, 10, 42, 22));
        spinBoxFrames = new QSpinBox(tabIndexes);
        spinBoxFrames->setObjectName(QString::fromUtf8("spinBoxFrames"));
        spinBoxFrames->setGeometry(QRect(330, 10, 42, 22));
        pushButtonAdd = new QPushButton(tabIndexes);
        pushButtonAdd->setObjectName(QString::fromUtf8("pushButtonAdd"));
        pushButtonAdd->setGeometry(QRect(230, 50, 60, 23));
        pushButtonDelete = new QPushButton(tabIndexes);
        pushButtonDelete->setObjectName(QString::fromUtf8("pushButtonDelete"));
        pushButtonDelete->setGeometry(QRect(300, 50, 70, 23));
        tabWidget->addTab(tabIndexes, QString());

        retranslateUi(VCDDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), VCDDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), VCDDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(VCDDialog);
    } // setupUi

    void retranslateUi(QDialog *VCDDialog)
    {
        VCDDialog->setWindowTitle(QCoreApplication::translate("VCDDialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("VCDDialog", "Arranger", nullptr));
        label_2->setText(QCoreApplication::translate("VCDDialog", "Composer", nullptr));
        label_3->setText(QCoreApplication::translate("VCDDialog", "Song Writer", nullptr));
        label_4->setText(QCoreApplication::translate("VCDDialog", "Performer", nullptr));
        label_5->setText(QCoreApplication::translate("VCDDialog", "Message", nullptr));
        label_6->setText(QCoreApplication::translate("VCDDialog", "Title", nullptr));
        label_7->setText(QCoreApplication::translate("VCDDialog", "UPC/EAN", nullptr));
        label_8->setText(QCoreApplication::translate("VCDDialog", "Pause", nullptr));
        pauseInFrames->setText(QCoreApplication::translate("VCDDialog", "Pause in frames", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabCDText), QCoreApplication::translate("VCDDialog", "CD Text", nullptr));
#if QT_CONFIG(whatsthis)
        tabIndexes->setWhatsThis(QString());
#endif // QT_CONFIG(whatsthis)
        label_17->setText(QCoreApplication::translate("VCDDialog", "M:", nullptr));
        label_18->setText(QCoreApplication::translate("VCDDialog", "S:", nullptr));
        label_19->setText(QCoreApplication::translate("VCDDialog", "F:", nullptr));
        pushButtonAdd->setText(QCoreApplication::translate("VCDDialog", "Add", nullptr));
        pushButtonDelete->setText(QCoreApplication::translate("VCDDialog", "Delete", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabIndexes), QCoreApplication::translate("VCDDialog", "Indexes", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VCDDialog: public Ui_VCDDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VCDDIALOG_H
