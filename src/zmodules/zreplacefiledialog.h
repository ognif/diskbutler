#ifndef ZREPLACEFILEDIALOG_H
#define ZREPLACEFILEDIALOG_H

#include <QDialog>
#include "ui_zreplacefiledialog.h"

namespace Ui {
class ZReplaceFileDialog;
}

class ZReplaceFileDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ZReplaceFileDialog(QWidget *parent, const QString &flie_name);
  ~ZReplaceFileDialog();
  bool DoReplaceActionForAll() {return ui->checkBox->isChecked();}

private:
  Ui::ZReplaceFileDialog *ui;
};

#endif // ZREPLACEFILEDIALOG_H
