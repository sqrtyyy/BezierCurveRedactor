//
// Created by aleksei on 05.04.2022.
//

#include "MovablePoint.h"
#include <QPainter>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QDebug>

MovablePoint::MovablePoint(QWidget *parent, unsigned size)
: Point(parent, size)
, m_size(size)
{
  setFixedWidth(static_cast<int>(m_size));
  setFixedHeight(static_cast<int>(m_size));
}

void MovablePoint::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    QDrag drag(this);
    QMimeData *mimeData = new QMimeData();
    drag.setMimeData(mimeData);
    emit moving();
    drag.exec(Qt::MoveAction);
  }
  Point::mousePressEvent(event);
}