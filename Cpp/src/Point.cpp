//
// Created by aleksei on 05.04.2022.
//

#include "Point.h"
#include <QPainter>
#include <QMouseEvent>

Point::Point(QWidget *parent, unsigned size)
  : QWidget(parent)
  , m_size(size)
{
  setFixedWidth(static_cast<int>(m_size));
  setFixedHeight(static_cast<int>(m_size));
}

void Point::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setPen(QPen(Qt::black, m_size, Qt::SolidLine, Qt::RoundCap));

  painter.drawPoint(m_size / 2, m_size / 2);

}

void Point::moveEvent(QMoveEvent *event)
{
  emit positionChanged();
  QWidget::moveEvent(event);
}
