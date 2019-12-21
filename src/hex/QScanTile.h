#ifndef QSCANTILE_H
#define QSCANTILE_H

#pragma once
#include <QPushButton>

const QString unrevealedStyleSheet =
"QScanTile"
"{"
"	border: 1px solid darkgray;"
"	background: qradialgradient(cx : 0.4, cy : -0.1, fx : 0.4, fy : -0.1, radius : 1.35, stop : 0 #fff, stop: 1 #bbb);"
"	border - radius: 1px;"
"}";



class QScanTile : public QPushButton
{
  Q_OBJECT
public:
  QScanTile (QPushButton &basebutton, QWidget * parent = 0) :
    QPushButton (parent),
    m_button (&basebutton)
  {
      setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      setStyleSheet(unrevealedStyleSheet);
      setDown(true);
  }
public:

  virtual QSize sizeHint() const override
    {
        return QSize(12, 12);
    }

private:
  QPushButton *m_button;
};


#endif // QSCANTILE_H
