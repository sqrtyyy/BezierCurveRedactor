#ifndef BEZIER_POINT_H
#define BEZIER_POINT_H

#include <QPoint>
#include <QWidget>

class Point : public QWidget
{
Q_OBJECT
public:
  explicit Point(QWidget* parent = nullptr, unsigned size = 5);

  void moveEvent(QMoveEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

signals:
  void positionChanged();
public:
  ~Point() override = default;

private:
  unsigned m_size;
};


#endif //BEZIER_POINT_H
