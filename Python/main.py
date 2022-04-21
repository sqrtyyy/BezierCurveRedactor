from PyQt5.QtWidgets import  QApplication
import sys

from MainWindow import BezierCurveWindow

if __name__=="__main__":
    app = QApplication(sys.argv)
    window = BezierCurveWindow()
    window.resize(800, 600)
    window.setWindowTitle(QApplication.translate("bezier", "Bezier Curve redactor"))
    window.show()
    QApplication.exec()