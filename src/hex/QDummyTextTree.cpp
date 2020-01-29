#include "QDummyTextTree.h"

QDummyTextTree::QDummyTextTree(QWidget* parent)
{
    orgFont = this->font();
    mData = 0;
}

void QDummyTextTree::updateData(int inData)
{
    mData = inData;
    viewport()->update();
}


void QDummyTextTree::paintEvent(QPaintEvent *event)
{
    if(mData==0){

        viewport()->setFont(QFont("Courier", 14));
        m_charWidth = fontMetrics().width(QLatin1Char('9'));

        QPainter painter(viewport());
        QSize areaSize = viewport()->size();

        QString dataMessage = tr("Empty disc. No data available.");
        painter.fillRect(event->rect(), this->palette().color(QPalette::Base));

        int pX = (areaSize.width()/2)-((m_charWidth*dataMessage.length())/2);
        int pY = areaSize.height()/2;

        painter.setPen(Qt::darkGray);

        painter.drawText(pX, pY, dataMessage);

        return;
    }

    setFont(orgFont);
    QTreeWidget::paintEvent(event); // draw original content
    //QPainter painter(viewport());

}

int QDummyTextTree::treeCount(QTreeWidgetItem *parent)
{
    int count = 0;
    if (parent == 0) {
        int topCount = this->topLevelItemCount();
        for (int i = 0; i < topCount; i++) {
            QTreeWidgetItem *item = this->topLevelItem(i);
            if (item->isExpanded()) {
                count += treeCount(item);
            }
        }
        count += topCount;
    } else {
        int childCount = parent->childCount();
        for (int i = 0; i < childCount; i++) {
            QTreeWidgetItem *item = parent->child(i);
            if (item->isExpanded()) {
                count += treeCount(item);
            }
        }
        count += childCount;
    }
    return count;
}
