#ifndef XBELHANDLER_H
#define XBELHANDLER_H

#include <QIcon>
#include <QXmlDefaultHandler>
#include "CommonTreeWidget.h"
#include "QDataItem.h"

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class XbelHandler : public QXmlDefaultHandler
{
public:
  XbelHandler(CommonTreeWidget *treeWidget);

  bool startElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName, const QXmlAttributes &attributes);
  bool endElement(const QString &namespaceURI, const QString &localName,
                  const QString &qName);
  bool characters(const QString &str);
  bool fatalError(const QXmlParseException &exception);
  QString errorString() const;

private:
  QDataItem *createChildItem(const QString &tagName);

  CommonTreeWidget *treeWidget;
  QDataItem *item;
  QString currentText;
  QString errorStr;
  bool metXbelTag;

  QIcon folderIcon;
};

#endif
