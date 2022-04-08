#include "MainWindow.h"
#include <QDragMoveEvent>
#include <QDebug>
#include <QPainter>
#include <QAction>
#include <QFileDialog>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, m_currentPoint(0)
, m_timer(std::make_unique<QTimer>(this))
, m_percents(0)
, m_toolBar(std::make_unique<QToolBar>("Linker ToolBar"))
, m_drawAnimation(false)
, m_drawLines(false)
{
  FillToolBar();

  qsrand(time(0));
  setAcceptDrops(true);


  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(repaint()));
  m_timer->start(20);

  addToolBar(Qt::BottomToolBarArea, m_toolBar.get());
}

void MainWindow::AddPoint(const QPoint& coords)
{
  m_percents = 0;
  m_points.emplace_back(new MovablePoint(this, 10));
  m_points.back()->show();
  size_t idx = m_points.size() - 1;
  m_colors.emplace_back(qrand()%255, qrand()%255, qrand()%255);

  auto& point = m_points.back();

  connect(point.get(), &MovablePoint::moving, this, [idx, this]{ this->m_currentPoint = idx;});
  connect(point.get(), &MovablePoint::positionChanged, this, &MainWindow::CalcBezierCurve);

  point->move(coords.x() - point->width() / 2, coords.y() - point->height() / 2);

  update();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
    AddPoint(QPoint(event->pos().x(), event->pos().y()));
  }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
  auto& point = m_points[m_currentPoint];
  point->move(event->pos().x() - point->width() / 2, event->pos().y() - point->height() / 2);
  update();
  event->accept();
}


void MainWindow::dropEvent(QDropEvent *event)
{
  event->setDropAction(Qt::MoveAction);
  event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
  event->accept();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
  if (m_points.empty())
    return;

  m_percents %= 100;

  Q_UNUSED(event);
  QPainter painter(this);

  painter.drawPixmap(rect(), m_bezierCurvPM);

  std::vector<QPointF> points;
  points.reserve(m_points.size());

  for (auto & point : m_points)
  {
    auto x = point->pos().x() + point->width() / 2;
    auto y = point->pos().y() + point->height() / 2;
    points.emplace_back(x,y);
  }

  double ratio = m_percents / 100.;

  for (unsigned j = 0; j < m_points.size() - 1; j++)
  {
    QLineF line(points[j],points[j + 1]);
    if (m_drawLines || m_drawAnimation)
      painter.drawLine(line);
    points[j] = line.pointAt(ratio);
  }

  if (!m_drawAnimation)
    return;

  for (unsigned i = 2; i < m_points.size(); ++i)
  {
    painter.setPen(QPen(m_colors[i], 2, Qt::SolidLine, Qt::RoundCap));
    for (unsigned j = 0; j < m_points.size() - i; j++)
    {
      QLineF line(points[j],points[j + 1]);
      painter.drawLine(line);
      points[j] = line.pointAt(ratio);
    }
    if (i == m_points.size() - 1)
    {
      painter.setPen(QPen(Qt::blue, 10, Qt::SolidLine, Qt::RoundCap));
      painter.drawPoint(points[0]);
    }
  }
  m_percents++;
}

void MainWindow::CalcBezierCurve()
{
  m_percents = 0;
  m_bezierCurvPM.fill(Qt::transparent);
  std::vector<QPointF> points;
  points.reserve(m_points.size());
  std::optional<QPointF> prevPoint;
  for (unsigned percent = 0; percent <= 100; percent++)
  {
    double ratio = percent / 100.;
    for (auto &point: m_points) {
      auto x = point->pos().x() + point->width() / 2;
      auto y = point->pos().y() + point->height() / 2;
      points.emplace_back(x, y);
    }
    for (unsigned i = 1; i < m_points.size(); ++i) {
      for (unsigned j = 0; j < m_points.size() - i; j++) {
        QLineF line(points[j], points[j + 1]);
        points[j] = line.pointAt(ratio);
      }
      if (i == m_points.size() - 1) {
        QPainter mapPainter(&m_bezierCurvPM);
        mapPainter.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));
        if (prevPoint)
        {
          mapPainter.drawLine(prevPoint.value(), points[0]);
        }
        prevPoint = points[0];
      }
    }
  }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  m_bezierCurvPM = QPixmap(size());
  m_bezierCurvPM.fill(Qt::transparent);
  CalcBezierCurve();

  QWidget::resizeEvent(event);
}

void MainWindow::FillToolBar()
{
  m_toolBar->addAction("Open file",[this]{LoadFromFile(QFileDialog::getOpenFileName(this,
                                                                                    tr("Load Curve"),
                                                                                    "",
                                                                                    tr("Bezier Curve Files (*.bzcv)")));
  } );

  auto * saveAction = new QAction("Save as");
  connect(saveAction, &QAction::triggered, this, [this]{
    QFileDialog dialog(this, "Save Curve", QString(),
                       "Bezier Curve Files (*.bzcv)");
    dialog.setDefaultSuffix(".bzcv");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.exec();
    SaveToFile(dialog.selectedFiles().front());
  });
  m_toolBar->addAction(saveAction);
  m_toolBar->addSeparator();

  auto * animationAction = new QAction("Animation");
  animationAction->setCheckable( true );
  connect(animationAction, &QAction::changed, this, [this]{
                                                                                           m_drawAnimation = !m_drawAnimation;
                                                                                           m_percents = 0;
                                                                                          });
  m_toolBar->addAction(animationAction);
  m_toolBar->addSeparator();

  auto * linesAction = new QAction("Connect dots");
  linesAction->setCheckable( true );
  connect(linesAction, &QAction::changed, this, [this]{ m_drawLines = !m_drawLines;});
  m_toolBar->addAction(linesAction);
  m_toolBar->addSeparator();

  m_toolBar->addAction("Clear", [this]{m_points.clear(); m_colors.clear();});
}

void MainWindow::LoadFromFile(const QString &fileName)
{
  m_points.clear();
  m_colors.clear();

  std::ifstream infile(fileName.toStdString());
  int x, y;
  while (infile >> x >> y)
  {
    AddPoint(QPoint(x, y));
  }
}

void MainWindow::SaveToFile(const QString &fileName)
{
  std::ofstream outFile(fileName.toStdString());
  for(auto& point : m_points)
  {
    outFile << point->x() << ' ' <<  point->y() << std::endl;
  }
}
