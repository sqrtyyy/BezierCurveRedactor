from point import Point
from PyQt5.QtWidgets import QWidget
from PyQt5 import QtGui
from PyQt5.QtCore import Qt, pyqtSignal, QMimeData
from PyQt5.QtGui import QDrag


class MovablePoint(Point):
    moving = pyqtSignal()

    def __init__(self, parent: QWidget = 0, size: int = 5):
        super(MovablePoint, self).__init__(parent, size)
        self._size = size
        self.setFixedWidth(self._size)
        self.setFixedHeight(self._size)

    def mousePressEvent(self, event: QtGui.QMouseEvent) -> None:
        if event.button() == Qt.LeftButton:
            drag = QDrag(self)
            mime_data = QMimeData()
            drag.setMimeData(mime_data)
            self.moving.emit()
            drag.exec(Qt.MoveAction)
        super(MovablePoint, self).mousePressEvent(event)
