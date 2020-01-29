#ifndef QDUMMYTEXTTREE_H
#define QDUMMYTEXTTREE_H

#pragma once
#include <QtWidgets>
#include <QBrush>
#include <QPen>

class QDummyTextTree : public QTreeWidget
{
    Q_OBJECT
public:
    QDummyTextTree(QWidget* parent = nullptr);
    void updateData(int inData);
    int treeCount(QTreeWidgetItem *parent = 0);
protected:
    void paintEvent(QPaintEvent *event);
private:
    std::size_t m_charWidth;
    QFont orgFont;
    int mData;


};

#endif // QDUMMYTEXTTREE_H
