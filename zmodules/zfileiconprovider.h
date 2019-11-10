#ifndef ZFILEICONPROVIDER_H
#define ZFILEICONPROVIDER_H

#include <QFileIconProvider>

class ZFileIconProvider : public QFileIconProvider
{
public:
  explicit ZFileIconProvider();
  virtual QIcon icon(const QFileInfo &info) const;

signals:

public slots:
};

#endif // ZFILEICONPROVIDER_H
