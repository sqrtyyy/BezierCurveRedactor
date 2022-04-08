#ifndef BEZIER_MAINWINDOW_H
#define BEZIER_MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QToolBar>

#include <memory>

#include "MovablePoint.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget* parent = nullptr);
  void AddPoint(const QPoint& coords);

public:
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  void CalcBezierCurve();
  void FillToolBar();
  void LoadFromFile(const QString& fileName);
  void SaveToFile(const QString& fileName);

private:
  using PointPtr = std::unique_ptr<MovablePoint>;

  std::vector<PointPtr> m_points;
  size_t m_currentPoint;

  std::vector<QColor> m_colors;
  QPixmap m_bezierCurvPM;
  std::unique_ptr<QTimer> m_timer;
  unsigned m_percents;

  std::unique_ptr<QToolBar> m_toolBar;

  bool m_drawAnimation;
  bool m_drawLines;
};


#endif //BEZIER_MAINWINDOW_H
