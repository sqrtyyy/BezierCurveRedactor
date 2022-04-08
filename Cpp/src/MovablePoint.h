#ifndef BEZIER_MOVEBLEPOINT_H
#define BEZIER_MOVEBLEPOINT_H

#include <QPoint>
#include "Point.h"

class MovablePoint : public Point
{
  Q_OBJECT
public:
  explicit MovablePoint(QWidget* parent = nullptr, unsigned size = 5);
  void mousePressEvent(QMouseEvent *event) override;

signals:
  void moving();

public:
  ~MovablePoint() override = default;

private:
  unsigned m_size;
};


#endif //BEZIER_MOVEBLEPOINT_H
