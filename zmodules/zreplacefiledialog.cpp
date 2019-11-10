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
