from PyQt5.QtWidgets import QWidget
from PyQt5 import QtGui
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtGui import QPainter, QPen


class Point(QWidget):
    position_changed = pyqtSignal()
    def __init__(self, parent: QWidget = 0, size: int = 5):
        super(Point, self).__init__(parent)
        self._size = size
        self.setFixedWidth(self._size)
        self.setFixedHeight(self._size)

    def paintEvent(self, event: QtGui.QPaintEvent) -> None:
        painter = QPainter(self)
        painter.setPen(QPen(Qt.black, self._size, Qt.SolidLine, Qt.RoundCap))
        painter.drawPoint(self._size / 2, self._size / 2)

    def moveEvent(self, event: QtGui.QMoveEvent) -> None:
        self.position_changed.emit()
        super(Point, self).moveEvent(event)


