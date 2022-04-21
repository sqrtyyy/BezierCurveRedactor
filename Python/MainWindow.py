import os
import re
from random import randint

from PyQt5.QtWidgets import QMainWindow, QToolBar, QFileDialog, QAction
from PyQt5.QtCore import QTimer, Qt, QPoint, QPointF, QLineF
from moveble_point import MovablePoint
from PyQt5.QtGui import QColor, QPainter, QPen, QPixmap
from PyQt5 import QtGui


class BezierCurveWindow(QMainWindow):
    __current_point: int
    __timer: QTimer
    __percents: int
    __toolbar: QToolBar
    __draw_animation: bool
    __draw_lines: bool

    def __init__(self, parent=None):
        super().__init__(parent)
        self.__current_point = 0
        self.__timer = QTimer(self)
        self.__percents = 0
        self.__points = []
        self.__colors = []
        self.__bezier_curv_PM = QPixmap(self.size())
        self.__toolbar = QToolBar("Linker ToolBar")
        self.__draw_animation = False
        self.__draw_lines = False

        self.setAcceptDrops(True)
        self.fill_tool_bar()
        self.__timer.timeout.connect(self.repaint)
        self.__timer.start(20)
        self.addToolBar(Qt.BottomToolBarArea, self.__toolbar)

    def on_animation(self):
        self.__draw_animation = not self.__draw_animation
        self.__percents = 0

    def on_draw_lines(self):
        self.__draw_lines = not self.__draw_lines

    def on_clear(self):
        for point in self.__points:
            point.deleteLater()
        self.__points.clear()
        self.__colors.clear()
        self.repaint()

    def fill_tool_bar(self):
        self.__toolbar.addAction("Open file", lambda: self.load_from_file(QFileDialog.getOpenFileName(self, "Load "
                                                                                                            "Curve",
                                                                                                      "",
                                                                                                      "Bezier Curve "
                                                                                                      "Files ("
                                                                                                      "*.bzcv)")[0]))
        self.__toolbar.addSeparator()

        self.__toolbar.addAction("Save as", lambda: self.save(QFileDialog.getSaveFileName(self, "Save Curve",
                                                                                                "",
                                                                                                "Bezier Curve "
                                                                                                "Files ("
                                                                                                "*.bzcv)")[0]))
        self.__toolbar.addSeparator()

        self.__animation_action = QAction("Animation")
        self.__animation_action.setCheckable(True)
        self.__animation_action.changed.connect(self.on_animation)
        self.__toolbar.addAction(self.__animation_action)
        self.__toolbar.addSeparator()

        self.__draw_lines_action = QAction("Connect dots")
        self.__draw_lines_action.setCheckable(True)
        self.__draw_lines_action.changed.connect(self.on_draw_lines)
        self.__toolbar.addAction(self.__draw_lines_action)
        self.__toolbar.addSeparator()

        self.__clear_action = QAction("Clear")
        self.__clear_action.triggered.connect(self.on_clear)
        self.__toolbar.addAction(self.__clear_action)
        self.__toolbar.addSeparator()

    def add_point(self, coords: QPoint) -> None:
        self.__percents = 0
        self.__points.append(MovablePoint(self, 10))
        self.__colors.append(QColor(randint(0, 255), randint(0, 255), randint(0, 255)))
        idx = len(self.__points) - 1
        point: MovablePoint = self.__points[-1]
        point.show()
        point.moving.connect(lambda: self.set_current_point(idx))
        point.position_changed.connect(self.calc_bezier_curve)
        point.move(coords.x() - point.width() / 2, coords.y() - point.height() / 2)
        self.update()

    def set_current_point(self, idx: int) -> None:
        self.__current_point = idx

    def mouseDoubleClickEvent(self, event: QtGui.QMouseEvent) -> None:
        if event.button() == Qt.LeftButton:
            self.add_point(QPoint(event.pos().x(), event.pos().y()))

    def dragMoveEvent(self, event: QtGui.QDragMoveEvent) -> None:
        point: MovablePoint = self.__points[self.__current_point]
        point.move(event.pos().x() - point.width() / 2, event.pos().y() - point.height() / 2)
        self.update()
        event.accept()

    def dropEvent(self, event: QtGui.QDropEvent) -> None:
        event.setDropAction(Qt.MoveAction)
        event.accept()

    def dragEnterEvent(self, event: QtGui.QDragEnterEvent) -> None:
        event.accept()

    def dragLeaveEvent(self, event: QtGui.QDragLeaveEvent) -> None:
        event.accept()

    def paintEvent(self, event: QtGui.QPaintEvent) -> None:
        if len(self.__points) == 0:
            return
        self.__percents %= 100
        painter = QPainter(self)
        painter.drawPixmap(self.rect(), self.__bezier_curv_PM)
        points = []
        for point in self.__points:
            x = point.pos().x() + point.width() / 2
            y = point.pos().y() + point.height() / 2
            points.append(QPointF(x, y))
        ratio = self.__percents / 100
        for i in range(len(self.__points) - 1):
            line = QLineF(points[i], points[i + 1])
            if self.__draw_lines or self.__draw_animation:
                painter.drawLine(line)
            points[i] = line.pointAt(ratio)
        if not self.__draw_animation:
            return

        for i in range(2, len(self.__points)):
            painter.setPen(QPen(self.__colors[i], 2, Qt.SolidLine, Qt.RoundCap))
            for j in range(len(self.__points) - i):
                line = QLineF(points[j], points[j + 1])
                painter.drawLine(line)
                points[j] = line.pointAt(ratio)
            if i == len(self.__points) - 1:
                painter.setPen(QPen(Qt.blue, 10, Qt.SolidLine, Qt.RoundCap))
                painter.drawPoint(points[0])
        self.__percents += 1

    def resizeEvent(self, event: QtGui.QResizeEvent) -> None:
        self.calc_bezier_curve()
        super(BezierCurveWindow, self).resizeEvent(event)

    def calc_bezier_curve(self) -> None:
        self.__bezier_curv_PM = QPixmap(self.size())
        self.__bezier_curv_PM.fill(Qt.transparent)
        map_painter = QPainter(self.__bezier_curv_PM)
        map_painter.setPen(QPen(Qt.red, 5, Qt.SolidLine, Qt.RoundCap))

        self.__percents = 0
        points = []
        prev_point = None
        for percent in range(0, 101):
            ratio = percent / 100
            for point in self.__points:
                x = point.pos().x() + point.width() / 2
                y = point.pos().y() + point.height() / 2
                points.append(QPointF(x, y))
            for i in range(1, len(self.__points)):
                for j in range(len(self.__points) - i):
                    line = QLineF(points[j], points[j + 1])
                    points[j] = line.pointAt(ratio)
                if i == len(self.__points) - 1:
                    if prev_point:
                        map_painter.drawLine(prev_point, points[0])
                    prev_point = points[0]

    def load_from_file(self, file_name: str):
        if len(file_name) == 0:
            return
        self.on_clear()
        with open(file_name, "r") as f:
            for line in f:
                coord = [int(x) for x in line.split()]
                self.add_point(QPoint(coord[0], coord[1]))

    def save(self, file_name: str):
        if len(file_name) == 0:
            return
        _, extension = os.path.splitext(file_name)
        if len(extension) == 0:
            file_name += ".bzcv"

        with open(file_name, "w") as f:
            for point in self.__points:
                f.write(f"{point.x()} {point.y()}\n")