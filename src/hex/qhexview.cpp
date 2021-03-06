/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 2021 Ingo Foerster (pixbytesl@gmail.com).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License 3 as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License 3 for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "qhexview.h"
#include <QScrollBar>
#include <QPainter>
#include <QSize>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QClipboard>
#include <QApplication>

#include <QDebug>

#include <stdexcept>

const int HEXCHARS_IN_LINE = 47;
const int GAP_ADR_HEX = 10;
const int GAP_HEX_ASCII = 16;
const int BYTES_PER_LINE = 16;


QHexView::QHexView(QWidget *parent):
    QAbstractScrollArea(parent),
    m_pdata(NULL)
{
    setFont(QFont("Courier", 9));

    m_charWidth = fontMetrics().horizontalAdvance(QLatin1Char('9'));
    m_charHeight = fontMetrics().height();

    m_posAddr = 0;
    m_posHex = 10 * m_charWidth + GAP_ADR_HEX;
    m_posAscii = m_posHex + HEXCHARS_IN_LINE * m_charWidth + GAP_HEX_ASCII;

    setMinimumWidth(static_cast<int>(m_posAscii + (BYTES_PER_LINE * m_charWidth)));

    setFocusPolicy(Qt::StrongFocus);
}


QHexView::~QHexView()
{
    if(m_pdata)
        delete m_pdata;
}

void QHexView::setData(QHexView::DataStorage *pData)
{
    verticalScrollBar()->setValue(0);
    if(m_pdata)
        delete m_pdata;
    m_pdata = pData;
    m_cursorPos = 0;
    resetSelection(0);
    viewport() -> update();
}


void QHexView::showFromOffset(std::size_t offset)
{
    if(m_pdata && offset < m_pdata->size())
    {
        setCursorPos(static_cast<int>(offset) * 2);

        int cursorY = static_cast<int>(m_cursorPos) / (2 * BYTES_PER_LINE);

        verticalScrollBar() -> setValue(cursorY);
    }
}

void QHexView::clear()
{
    verticalScrollBar()->setValue(0);
}


QSize QHexView::fullSize() const
{
    if(!m_pdata)
        return QSize(0, 0);

    std::size_t width = m_posAscii + (BYTES_PER_LINE * m_charWidth);
    std::size_t height = m_pdata->size() / BYTES_PER_LINE;
    if(m_pdata->size() % BYTES_PER_LINE)
        height++;

    height *= m_charHeight;

    return QSize(static_cast<int>(width), static_cast<int>(height));
}

void QHexView::paintEvent(QPaintEvent *event)
{
    if(!m_pdata){
        QPainter painter(viewport());
        QSize areaSize = viewport()->size();
        QString dataMessage = tr("Empty disc. No data available.");
        painter.fillRect(event->rect(), this->palette().color(QPalette::Base));

        int pX = (areaSize.width()/2)-((static_cast<int>(m_charWidth)*dataMessage.length())/2);
        int pY = areaSize.height()/2;

        painter.setPen(Qt::darkGray);


        painter.drawText(pX, pY, dataMessage);
        return;
    }

    QPainter painter(viewport());

    QSize areaSize = viewport()->size();
    QSize  widgetSize = fullSize();
    verticalScrollBar()->setPageStep(areaSize.height() / static_cast<int>(m_charHeight) );
    verticalScrollBar()->setRange(0, (widgetSize.height() - areaSize.height()) / static_cast<int>(m_charHeight) + 1);

    int firstLineIdx = verticalScrollBar() -> value();

    int lastLineIdx = firstLineIdx + areaSize.height() / static_cast<int>(m_charHeight);
    if(lastLineIdx > m_pdata->size() / BYTES_PER_LINE)
    {
        lastLineIdx = static_cast<int>(m_pdata->size()) / BYTES_PER_LINE;
        if(m_pdata->size() % BYTES_PER_LINE)
            lastLineIdx++;
    }

    painter.fillRect(event->rect(), this->palette().color(QPalette::Base));

    QColor addressAreaColor = QColor(0xd4, 0xd4, 0xd4, 0xff);
    painter.fillRect(QRect(static_cast<int>(m_posAddr), event->rect().top(), static_cast<int>(m_posHex) - GAP_ADR_HEX + 2 , height()), addressAreaColor);

    int linePos = static_cast<int>(m_posAscii) - (GAP_HEX_ASCII / 2);
    painter.setPen(Qt::gray);

    painter.drawLine(linePos, event->rect().top(), linePos, height());

    painter.setPen(Qt::black);

    int yPosStart = static_cast<int>(m_charHeight);

    QBrush def = painter.brush();
    QBrush selected = QBrush(QColor(0x6d, 0x9e, 0xff, 0xff));
    QByteArray data = m_pdata->getData(firstLineIdx * BYTES_PER_LINE, (lastLineIdx - firstLineIdx) * BYTES_PER_LINE);

    for (int lineIdx = firstLineIdx, yPos = yPosStart;  lineIdx < lastLineIdx; lineIdx += 1, yPos += static_cast<int>(m_charHeight))
    {
        QString address = QString("%1").arg(lineIdx * 16, 10, 16, QChar('0'));
        //Looks like the line nUmber
        painter.drawText(static_cast<int>(m_posAddr), yPos, address);

        for(int xPos = static_cast<int>(m_posHex), i=0; i<BYTES_PER_LINE && ((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) < data.size(); i++, xPos += 3 * static_cast<int>(m_charWidth))
        {
            std::size_t pos = (lineIdx * BYTES_PER_LINE + i) * 2;
            if(pos >= m_selectBegin && pos < m_selectEnd)
            {
                painter.setBackground(selected);
                painter.setBackgroundMode(Qt::OpaqueMode);
            }

            QString val = QString::number((data.at((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) & 0xF0) >> 4, 16);
            //This is the first part of the hex?????
            painter.drawText(xPos, yPos, val);


            if((pos+1) >= m_selectBegin && (pos+1) < m_selectEnd)
            {
                painter.setBackground(selected);
                painter.setBackgroundMode(Qt::OpaqueMode);
            }
            else
            {
                painter.setBackground(def);
                painter.setBackgroundMode(Qt::OpaqueMode);
            }

            val = QString::number((data.at((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) & 0xF), 16);
            //This is the second part of the hex?????
            painter.drawText(xPos+static_cast<int>(m_charWidth), yPos, val);

            painter.setBackground(def);
            painter.setBackgroundMode(Qt::OpaqueMode);

        }

        for (int xPosAscii = static_cast<int>(m_posAscii), i=0; ((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) < data.size() && (i < BYTES_PER_LINE); i++, xPosAscii += static_cast<int>(m_charWidth))
        {
            char ch = data[(lineIdx - firstLineIdx) * BYTES_PER_LINE + i];
            if ((ch < 0x20) || (ch > 0x7e))
                ch = '.';

            //qDebug("Sector firstLineIdx %d lineIdx %d i %d ch %s",firstLineIdx,lineIdx,i,QString(ch).toLatin1().constData());

            //This is the part behind the line
            painter.drawText(xPosAscii, yPos, QString(ch));
        }

    }

    if (hasFocus())
    {
        int x = (m_cursorPos % (2 * BYTES_PER_LINE));
        int y = static_cast<int>(m_cursorPos) / (2 * BYTES_PER_LINE);
        y -= firstLineIdx;
        int cursorX = (((x / 2) * 3) + (x % 2)) * static_cast<int>(m_charWidth) + static_cast<int>(m_posHex);
        int cursorY = y * static_cast<int>(m_charHeight) + 4;
        painter.fillRect(cursorX, cursorY, 2, static_cast<int>(m_charHeight) ,this->palette().color(QPalette::WindowText));
    }
}


void QHexView::keyPressEvent(QKeyEvent *event)
{
    bool setVisible = false;

    /*****************************************************************************/
    /* Cursor movements */
    /*****************************************************************************/
    if(event->matches(QKeySequence::MoveToNextChar))
    {
        setCursorPos(static_cast<int>(m_cursorPos) + 1);
        resetSelection(static_cast<int>(m_cursorPos));
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToPreviousChar))
    {
        setCursorPos(static_cast<int>(m_cursorPos) - 1);
        resetSelection(static_cast<int>(m_cursorPos));
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToEndOfLine))
    {
        setCursorPos(static_cast<int>(m_cursorPos) | ((BYTES_PER_LINE * 2) - 1));
        resetSelection(static_cast<int>(m_cursorPos));
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToStartOfLine))
    {
        setCursorPos(static_cast<int>(m_cursorPos) | (m_cursorPos % (BYTES_PER_LINE * 2)));
        resetSelection(static_cast<int>(m_cursorPos));
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToPreviousLine))
    {
        setCursorPos(static_cast<int>(m_cursorPos) - BYTES_PER_LINE * 2);
        resetSelection(static_cast<int>(m_cursorPos));
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToNextLine))
    {
        setCursorPos(static_cast<int>(m_cursorPos) + BYTES_PER_LINE * 2);
        resetSelection(static_cast<int>(m_cursorPos));
        setVisible = true;
    }

    if(event->matches(QKeySequence::MoveToNextPage))
    {
        setCursorPos(static_cast<int>(m_cursorPos) + (viewport()->height() / static_cast<int>(m_charHeight) - 1) * 2 * BYTES_PER_LINE);
        resetSelection(static_cast<int>(m_cursorPos));
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToPreviousPage))
    {
        setCursorPos(static_cast<int>(m_cursorPos) - (viewport()->height() / static_cast<int>(m_charHeight) - 1) * 2 * BYTES_PER_LINE);
        resetSelection(static_cast<int>(m_cursorPos));
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToEndOfDocument))
    {
        if(m_pdata)
            setCursorPos(static_cast<int>(m_pdata->size()) * 2);
        resetSelection(static_cast<int>(m_cursorPos));
        setVisible = true;
    }
    if(event->matches(QKeySequence::MoveToStartOfDocument))
    {
        setCursorPos(0);
        resetSelection(static_cast<int>(m_cursorPos));
        setVisible = true;
    }

    /*****************************************************************************/
    /* Select commands */
    /*****************************************************************************/
    if (event->matches(QKeySequence::SelectAll))
    {
        resetSelection(0);
        if(m_pdata)
            setSelection(2 * static_cast<int>(m_pdata->size()) + 1);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectNextChar))
    {
        int pos = static_cast<int>(m_cursorPos) + 1;
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectPreviousChar))
    {
        int pos = static_cast<int>(m_cursorPos) - 1;
        setSelection(pos);
        setCursorPos(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectEndOfLine))
    {
        int pos =static_cast<int>(m_cursorPos) - (static_cast<int>(m_cursorPos) % (2 * BYTES_PER_LINE)) + (2 * BYTES_PER_LINE);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectStartOfLine))
    {
        int pos = static_cast<int>(m_cursorPos) - (static_cast<int>(m_cursorPos) % (2 * BYTES_PER_LINE));
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectPreviousLine))
    {
        int pos = static_cast<int>(m_cursorPos) - (2 * BYTES_PER_LINE);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectNextLine))
    {
        int pos = static_cast<int>(m_cursorPos) + (2 * BYTES_PER_LINE);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }

    if (event->matches(QKeySequence::SelectNextPage))
    {
        int pos = static_cast<int>(m_cursorPos) + (((viewport()->height() / static_cast<int>(m_charHeight)) - 1) * 2 * BYTES_PER_LINE);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectPreviousPage))
    {
        int pos = static_cast<int>(m_cursorPos) - (((viewport()->height() / static_cast<int>(m_charHeight)) - 1) * 2 * BYTES_PER_LINE);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectEndOfDocument))
    {
        int pos = 0;
        if(m_pdata)
            pos = static_cast<int>(m_pdata->size()) * 2;
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (event->matches(QKeySequence::SelectStartOfDocument))
    {
        int pos = 0;
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }

    if (event->matches(QKeySequence::Copy))
    {
        if(m_pdata)
        {
            QString res;
            int idx = 0;
            int copyOffset = 0;

            QByteArray data = m_pdata->getData(m_selectBegin / 2, (m_selectEnd - m_selectBegin) / 2 + 1);
            if(m_selectBegin % 2)
            {
                res += QString::number((data.at((idx+1) / 2) & 0xF), 16);
                res += " ";
                idx++;
                copyOffset = 1;
            }

            int selectedSize = static_cast<int>(m_selectEnd) - static_cast<int>(m_selectBegin);
            for (;idx < selectedSize; idx+= 2)
            {
                QString val = QString::number((data.at((copyOffset + idx) / 2) & 0xF0) >> 4, 16);
                if(idx + 1 < selectedSize)
                {
                    val += QString::number((data.at((copyOffset + idx) / 2) & 0xF), 16);
                    val += " ";
                }
                res += val;

                if((idx/2) % BYTES_PER_LINE == (BYTES_PER_LINE - 1))
                    res += "\n";
            }
            QClipboard *clipboard = QApplication::clipboard();
            clipboard -> setText(res);
        }
    }

    if(setVisible)
        ensureVisible();
    viewport() -> update();
}

void QHexView::mouseMoveEvent(QMouseEvent * event)
{
    int actPos = static_cast<int>(cursorPos(event->pos()));
    setCursorPos(actPos);
    setSelection(actPos);

    viewport() -> update();
}

void QHexView::mousePressEvent(QMouseEvent * event)
{
    int cPos = static_cast<int>(cursorPos(event->pos()));

    if((QApplication::keyboardModifiers() & Qt::ShiftModifier) && event -> button() == Qt::LeftButton)
        setSelection(cPos);
    else
        resetSelection(cPos);

    setCursorPos(cPos);

    viewport() -> update();
}


std::size_t QHexView::cursorPos(const QPoint &position)
{
    int pos = -1;

    if ((position.x() >= m_posHex) && (position.x() < (m_posHex + HEXCHARS_IN_LINE * m_charWidth)))
    {
        int x = (position.x() - static_cast<int>(m_posHex)) /static_cast<int>(m_charWidth);
        if ((x % 3) == 0)
            x = (x / 3) * 2;
        else
            x = ((x / 3) * 2) + 1;

        int firstLineIdx = verticalScrollBar() -> value();
        int y = (position.y() / static_cast<int>(m_charHeight)) * 2 * BYTES_PER_LINE;
        pos = x + y + firstLineIdx * BYTES_PER_LINE * 2;
    }
    return pos;
}


void QHexView::resetSelection()
{
    m_selectBegin = m_selectInit;
    m_selectEnd = m_selectInit;
}

void QHexView::resetSelection(int pos)
{
    if (pos < 0)
        pos = 0;

    m_selectInit = pos;
    m_selectBegin = pos;
    m_selectEnd = pos;
}

void QHexView::setSelection(int pos)
{
    if (pos < 0)
        pos = 0;

    if (pos >= static_cast<int>(m_selectInit))
    {
        m_selectEnd = pos;
        m_selectBegin = m_selectInit;
    }
    else
    {
        m_selectBegin = pos;
        m_selectEnd = m_selectInit;
    }
}


void QHexView::setCursorPos(int position)
{
    if(position < 0)
        position = 0;

    int maxPos = 0;
    if(m_pdata)
    {
        maxPos = static_cast<int>(m_pdata->size()) * 2;
        if(m_pdata->size() % BYTES_PER_LINE)
            maxPos++;
    }

    if(position > maxPos)
        position = maxPos;

    m_cursorPos = position;
}

void QHexView::ensureVisible()
{
    QSize areaSize = viewport()->size();

    int firstLineIdx = verticalScrollBar() -> value();
    int lastLineIdx = firstLineIdx + areaSize.height() / static_cast<int>(m_charHeight);

    int cursorY = static_cast<int>(m_cursorPos) / (2 * BYTES_PER_LINE);

    if(cursorY < firstLineIdx)
        verticalScrollBar() -> setValue(cursorY);
    else if(cursorY >= lastLineIdx)
        verticalScrollBar() -> setValue(cursorY - areaSize.height() / static_cast<int>(m_charHeight) + 1);
}



QHexView::DataStorageArray::DataStorageArray(const QByteArray &arr)
{
    m_data = arr;
}

QByteArray QHexView::DataStorageArray::getData(std::size_t position, std::size_t length)
{
    return m_data.mid(static_cast<int>(position), static_cast<int>(length));
}


std::size_t QHexView::DataStorageArray::size()
{
    return m_data.count();
}


QHexView::DataStorageFile::DataStorageFile(const QString &fileName): m_file(fileName)
{
    m_file.open(QIODevice::ReadOnly);
    if(!m_file.isOpen())
        throw std::runtime_error(std::string("Failed to open file `") + fileName.toStdString() + "`");
}

QByteArray QHexView::DataStorageFile::getData(std::size_t position, std::size_t length)
{
    m_file.seek(position);
    return m_file.read(length);
}


std::size_t QHexView::DataStorageFile::size()
{
    return m_file.size();
}

void QHexView::saveTofile(QString file)
{

    QString writerBlock = "";

    int yPosStart = static_cast<int>(m_charHeight);
    int firstLineIdx = 0;
    int lastLineIdx = 2048/16;

    QFile outputFile(file);
    outputFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&outputFile);

    QByteArray data = m_pdata->getData(firstLineIdx * BYTES_PER_LINE, (lastLineIdx - firstLineIdx) * BYTES_PER_LINE);

    for (int lineIdx = firstLineIdx, yPos = yPosStart;  lineIdx < lastLineIdx; lineIdx += 1, yPos += static_cast<int>(m_charHeight))
    {
        QString address = QString("%1").arg(lineIdx * 16, 10, 16, QChar('0'));
        //Looks like the line nUmber
        writerBlock += address;
        writerBlock += " : ";


        for(int xPos = static_cast<int>(m_posHex), i=0; i<BYTES_PER_LINE && ((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) < data.size(); i++, xPos += 3 * static_cast<int>(m_charWidth))
        {

            QString val = QString::number((data.at((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) & 0xF0) >> 4, 16);
            writerBlock += val;

            val = QString::number((data.at((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) & 0xF), 16);
            //This is the second part of the hex?????
            writerBlock += val;
            writerBlock += " ";

        }

        writerBlock += " ";

        for (int xPosAscii = static_cast<int>(m_posAscii), i=0; ((lineIdx - firstLineIdx) * BYTES_PER_LINE + i) < data.size() && (i < BYTES_PER_LINE); i++, xPosAscii += static_cast<int>(m_charWidth))
        {
            char ch = data[(lineIdx - firstLineIdx) * BYTES_PER_LINE + i];
            if ((ch < 0x20) || (ch > 0x7e))
                ch = '.';

            //qDebug("Sector firstLineIdx %d lineIdx %d i %d ch %s",firstLineIdx,lineIdx,i,QString(ch).toLatin1().constData());

            //This is the part behind the line
            writerBlock += QString(ch);
        }

        writerBlock += "\n";

    }

    outStream << writerBlock;
    outputFile.close();

}
