/********************************************************************************
** Form generated from reading UI file 'vdiskpropertydialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VDISKPROPERTYDIALOG_H
#define UI_VDISKPROPERTYDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VDiskPropertyDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *tabDisk;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *editName;
    QLabel *label_2;
    QDateTimeEdit *dateTimeCreation;
    QLabel *label_3;
    QDateTimeEdit *dateTimeMdification;
    QLabel *label_4;
    QDateTimeEdit *dateTimeExpiration;
    QLabel *label_5;
    QDateTimeEdit *dateTimeEffective;
    QLabel *label_6;
    QLabel *labelSize;
    QLabel *label_7;
    QLabel *labelFiles;
    QCheckBox *dateUsage;
    QWidget *tabFileExclision;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *buttonImportSystemFilter;
    QPushButton *buttonResetFilter;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_8;
    QHBoxLayout *horizontalLayout_4;
    QListWidget *listWidget;
    QVBoxLayout *verticalLayout_5;
    QPushButton *buttonAdd;
    QPushButton *buttonDel;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout_6;
    QCheckBox *checkByDate;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_15;
    QDateEdit *dateFrom;
    QLabel *label_16;
    QDateEdit *dateTo;
    QFrame *line;
    QWidget *tabCDText;
    QWidget *formLayoutWidget_3;
    QFormLayout *formLayout_3;
    QLabel *label_44;
    QLineEdit *editArranger;
    QLabel *label_45;
    QLineEdit *editComposer;
    QLabel *label_46;
    QLineEdit *editSongWriter;
    QLabel *label_47;
    QLineEdit *editPerformer;
    QLabel *label_48;
    QLineEdit *editMessage;
    QLabel *label_49;
    QLineEdit *editTitle;
    QLabel *label_50;
    QLineEdit *editUPCEAN;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *VDiskPropertyDialog)
    {
        if (VDiskPropertyDialog->objectName().isEmpty())
            VDiskPropertyDialog->setObjectName(QString::fromUtf8("VDiskPropertyDialog"));
        VDiskPropertyDialog->resize(404, 396);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(VDiskPropertyDialog->sizePolicy().hasHeightForWidth());
        VDiskPropertyDialog->setSizePolicy(sizePolicy);
        verticalLayout_2 = new QVBoxLayout(VDiskPropertyDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        tabWidget = new QTabWidget(VDiskPropertyDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        tabWidget->setAutoFillBackground(false);
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setUsesScrollButtons(false);
        tabWidget->setDocumentMode(false);
        tabWidget->setTabsClosable(false);
        tabWidget->setMovable(false);
        tabDisk = new QWidget();
        tabDisk->setObjectName(QString::fromUtf8("tabDisk"));
        formLayoutWidget = new QWidget(tabDisk);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(0, 20, 371, 250));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(formLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        editName = new QLineEdit(formLayoutWidget);
        editName->setObjectName(QString::fromUtf8("editName"));

        formLayout->setWidget(0, QFormLayout::FieldRole, editName);

        label_2 = new QLabel(formLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        dateTimeCreation = new QDateTimeEdit(formLayoutWidget);
        dateTimeCreation->setObjectName(QString::fromUtf8("dateTimeCreation"));
        dateTimeCreation->setEnabled(true);

        formLayout->setWidget(2, QFormLayout::FieldRole, dateTimeCreation);

        label_3 = new QLabel(formLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        dateTimeMdification = new QDateTimeEdit(formLayoutWidget);
        dateTimeMdification->setObjectName(QString::fromUtf8("dateTimeMdification"));
        dateTimeMdification->setEnabled(true);

        formLayout->setWidget(3, QFormLayout::FieldRole, dateTimeMdification);

        label_4 = new QLabel(formLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_4);

        dateTimeExpiration = new QDateTimeEdit(formLayoutWidget);
        dateTimeExpiration->setObjectName(QString::fromUtf8("dateTimeExpiration"));
        dateTimeExpiration->setEnabled(true);

        formLayout->setWidget(4, QFormLayout::FieldRole, dateTimeExpiration);

        label_5 = new QLabel(formLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_5);

        dateTimeEffective = new QDateTimeEdit(formLayoutWidget);
        dateTimeEffective->setObjectName(QString::fromUtf8("dateTimeEffective"));
        dateTimeEffective->setEnabled(true);

        formLayout->setWidget(5, QFormLayout::FieldRole, dateTimeEffective);

        label_6 = new QLabel(formLayoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_6);

        labelSize = new QLabel(formLayoutWidget);
        labelSize->setObjectName(QString::fromUtf8("labelSize"));
        labelSize->setMargin(0);

        formLayout->setWidget(6, QFormLayout::FieldRole, labelSize);

        label_7 = new QLabel(formLayoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_7);

        labelFiles = new QLabel(formLayoutWidget);
        labelFiles->setObjectName(QString::fromUtf8("labelFiles"));

        formLayout->setWidget(7, QFormLayout::FieldRole, labelFiles);

        dateUsage = new QCheckBox(formLayoutWidget);
        dateUsage->setObjectName(QString::fromUtf8("dateUsage"));

        formLayout->setWidget(1, QFormLayout::FieldRole, dateUsage);

        tabWidget->addTab(tabDisk, QString());
        tabFileExclision = new QWidget();
        tabFileExclision->setObjectName(QString::fromUtf8("tabFileExclision"));
        verticalLayoutWidget_2 = new QWidget(tabFileExclision);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(0, 20, 331, 281));
        verticalLayout_3 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        buttonImportSystemFilter = new QPushButton(verticalLayoutWidget_2);
        buttonImportSystemFilter->setObjectName(QString::fromUtf8("buttonImportSystemFilter"));

        horizontalLayout_3->addWidget(buttonImportSystemFilter);

        buttonResetFilter = new QPushButton(verticalLayoutWidget_2);
        buttonResetFilter->setObjectName(QString::fromUtf8("buttonResetFilter"));

        horizontalLayout_3->addWidget(buttonResetFilter);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout_3);

        label_8 = new QLabel(verticalLayoutWidget_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        verticalLayout_3->addWidget(label_8);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        listWidget = new QListWidget(verticalLayoutWidget_2);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        horizontalLayout_4->addWidget(listWidget);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        buttonAdd = new QPushButton(verticalLayoutWidget_2);
        buttonAdd->setObjectName(QString::fromUtf8("buttonAdd"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(buttonAdd->sizePolicy().hasHeightForWidth());
        buttonAdd->setSizePolicy(sizePolicy2);
        buttonAdd->setMaximumSize(QSize(48, 32));

        verticalLayout_5->addWidget(buttonAdd);

        buttonDel = new QPushButton(verticalLayoutWidget_2);
        buttonDel->setObjectName(QString::fromUtf8("buttonDel"));
        sizePolicy2.setHeightForWidth(buttonDel->sizePolicy().hasHeightForWidth());
        buttonDel->setSizePolicy(sizePolicy2);
        buttonDel->setMinimumSize(QSize(0, 0));
        buttonDel->setMaximumSize(QSize(48, 32));

        verticalLayout_5->addWidget(buttonDel);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);


        horizontalLayout_4->addLayout(verticalLayout_5);


        verticalLayout_3->addLayout(horizontalLayout_4);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        checkByDate = new QCheckBox(verticalLayoutWidget_2);
        checkByDate->setObjectName(QString::fromUtf8("checkByDate"));

        verticalLayout_6->addWidget(checkByDate);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_15 = new QLabel(verticalLayoutWidget_2);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        horizontalLayout_5->addWidget(label_15);

        dateFrom = new QDateEdit(verticalLayoutWidget_2);
        dateFrom->setObjectName(QString::fromUtf8("dateFrom"));

        horizontalLayout_5->addWidget(dateFrom);

        label_16 = new QLabel(verticalLayoutWidget_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        horizontalLayout_5->addWidget(label_16);

        dateTo = new QDateEdit(verticalLayoutWidget_2);
        dateTo->setObjectName(QString::fromUtf8("dateTo"));

        horizontalLayout_5->addWidget(dateTo);


        verticalLayout_6->addLayout(horizontalLayout_5);


        verticalLayout_3->addLayout(verticalLayout_6);

        line = new QFrame(verticalLayoutWidget_2);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_3->addWidget(line);

        tabWidget->addTab(tabFileExclision, QString());
        tabCDText = new QWidget();
        tabCDText->setObjectName(QString::fromUtf8("tabCDText"));
        formLayoutWidget_3 = new QWidget(tabCDText);
        formLayoutWidget_3->setObjectName(QString::fromUtf8("formLayoutWidget_3"));
        formLayoutWidget_3->setGeometry(QRect(0, 30, 331, 209));
        formLayout_3 = new QFormLayout(formLayoutWidget_3);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        formLayout_3->setContentsMargins(0, 0, 0, 0);
        label_44 = new QLabel(formLayoutWidget_3);
        label_44->setObjectName(QString::fromUtf8("label_44"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_44);

        editArranger = new QLineEdit(formLayoutWidget_3);
        editArranger->setObjectName(QString::fromUtf8("editArranger"));
        editArranger->setMinimumSize(QSize(0, 21));
        editArranger->setMaximumSize(QSize(16777215, 21));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, editArranger);

        label_45 = new QLabel(formLayoutWidget_3);
        label_45->setObjectName(QString::fromUtf8("label_45"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_45);

        editComposer = new QLineEdit(formLayoutWidget_3);
        editComposer->setObjectName(QString::fromUtf8("editComposer"));
        editComposer->setMinimumSize(QSize(0, 21));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, editComposer);

        label_46 = new QLabel(formLayoutWidget_3);
        label_46->setObjectName(QString::fromUtf8("label_46"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_46);

        editSongWriter = new QLineEdit(formLayoutWidget_3);
        editSongWriter->setObjectName(QString::fromUtf8("editSongWriter"));
        editSongWriter->setMinimumSize(QSize(0, 21));

        formLayout_3->setWidget(2, QFormLayout::FieldRole, editSongWriter);

        label_47 = new QLabel(formLayoutWidget_3);
        label_47->setObjectName(QString::fromUtf8("label_47"));

        formLayout_3->setWidget(3, QFormLayout::LabelRole, label_47);

        editPerformer = new QLineEdit(formLayoutWidget_3);
        editPerformer->setObjectName(QString::fromUtf8("editPerformer"));
        editPerformer->setMinimumSize(QSize(0, 21));

        formLayout_3->setWidget(3, QFormLayout::FieldRole, editPerformer);

        label_48 = new QLabel(formLayoutWidget_3);
        label_48->setObjectName(QString::fromUtf8("label_48"));

        formLayout_3->setWidget(4, QFormLayout::LabelRole, label_48);

        editMessage = new QLineEdit(formLayoutWidget_3);
        editMessage->setObjectName(QString::fromUtf8("editMessage"));
        editMessage->setMinimumSize(QSize(0, 21));

        formLayout_3->setWidget(4, QFormLayout::FieldRole, editMessage);

        label_49 = new QLabel(formLayoutWidget_3);
        label_49->setObjectName(QString::fromUtf8("label_49"));

        formLayout_3->setWidget(5, QFormLayout::LabelRole, label_49);

        editTitle = new QLineEdit(formLayoutWidget_3);
        editTitle->setObjectName(QString::fromUtf8("editTitle"));
        editTitle->setMinimumSize(QSize(0, 21));

        formLayout_3->setWidget(5, QFormLayout::FieldRole, editTitle);

        label_50 = new QLabel(formLayoutWidget_3);
        label_50->setObjectName(QString::fromUtf8("label_50"));

        formLayout_3->setWidget(6, QFormLayout::LabelRole, label_50);

        editUPCEAN = new QLineEdit(formLayoutWidget_3);
        editUPCEAN->setObjectName(QString::fromUtf8("editUPCEAN"));
        editUPCEAN->setMinimumSize(QSize(0, 21));

        formLayout_3->setWidget(6, QFormLayout::FieldRole, editUPCEAN);

        tabWidget->addTab(tabCDText, QString());

        verticalLayout_2->addWidget(tabWidget);

        buttonBox = new QDialogButtonBox(VDiskPropertyDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Help|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(VDiskPropertyDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), VDiskPropertyDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), VDiskPropertyDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(VDiskPropertyDialog);
    } // setupUi

    void retranslateUi(QDialog *VDiskPropertyDialog)
    {
        VDiskPropertyDialog->setWindowTitle(QCoreApplication::translate("VDiskPropertyDialog", "Disk properties", nullptr));
        label->setText(QCoreApplication::translate("VDiskPropertyDialog", "Name:", nullptr));
        label_2->setText(QCoreApplication::translate("VDiskPropertyDialog", "Creation Date & Time:", nullptr));
        label_3->setText(QCoreApplication::translate("VDiskPropertyDialog", "Modification Date & Time:", nullptr));
        label_4->setText(QCoreApplication::translate("VDiskPropertyDialog", "Expiration Date & Time:", nullptr));
        label_5->setText(QCoreApplication::translate("VDiskPropertyDialog", "Effective Date & Time:", nullptr));
        label_6->setText(QCoreApplication::translate("VDiskPropertyDialog", "Project size:", nullptr));
        labelSize->setText(QCoreApplication::translate("VDiskPropertyDialog", "0", nullptr));
        label_7->setText(QCoreApplication::translate("VDiskPropertyDialog", "Amount of files:", nullptr));
        labelFiles->setText(QCoreApplication::translate("VDiskPropertyDialog", "0", nullptr));
        dateUsage->setText(QCoreApplication::translate("VDiskPropertyDialog", "Use dates", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabDisk), QCoreApplication::translate("VDiskPropertyDialog", "Disk", nullptr));
        buttonImportSystemFilter->setText(QCoreApplication::translate("VDiskPropertyDialog", "Import global settings", nullptr));
        buttonResetFilter->setText(QCoreApplication::translate("VDiskPropertyDialog", "Reset", nullptr));
        label_8->setText(QCoreApplication::translate("VDiskPropertyDialog", "By Type", nullptr));
        buttonAdd->setText(QString());
        buttonDel->setText(QString());
        checkByDate->setText(QCoreApplication::translate("VDiskPropertyDialog", "By Date:", nullptr));
        label_15->setText(QCoreApplication::translate("VDiskPropertyDialog", "From:", nullptr));
        label_16->setText(QCoreApplication::translate("VDiskPropertyDialog", "To:", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabFileExclision), QCoreApplication::translate("VDiskPropertyDialog", "File Exclusion", nullptr));
        label_44->setText(QCoreApplication::translate("VDiskPropertyDialog", "Arranger:", nullptr));
        label_45->setText(QCoreApplication::translate("VDiskPropertyDialog", "Composer:", nullptr));
        label_46->setText(QCoreApplication::translate("VDiskPropertyDialog", "Song Writer:", nullptr));
        label_47->setText(QCoreApplication::translate("VDiskPropertyDialog", "Performer:", nullptr));
        label_48->setText(QCoreApplication::translate("VDiskPropertyDialog", "Message:", nullptr));
        label_49->setText(QCoreApplication::translate("VDiskPropertyDialog", "Title:", nullptr));
        label_50->setText(QCoreApplication::translate("VDiskPropertyDialog", "UPC/EAN:", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabCDText), QCoreApplication::translate("VDiskPropertyDialog", "CD Text", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VDiskPropertyDialog: public Ui_VDiskPropertyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VDISKPROPERTYDIALOG_H
