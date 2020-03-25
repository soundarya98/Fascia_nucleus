"""
This class is a widget it allows displaying multiple graphs and float each as a window if needed on real time.
Updating the graph will be done outside of this class.
"""

import sys
from PyQt5 import QtGui, QtCore, QtWidgets
import numpy as np
import pyqtgraph as pg


class floatingCurves_Max(QtWidgets.QMainWindow):
    def __init__(self, curve:pg.PlotDataItem, oldWidget:pg.PlotWidget ,parent=None):
        super(floatingCurves_Max, self).__init__(parent)
        self.oldWidget = oldWidget
        self.curve = curve
        plotWidget = pg.PlotWidget()
        plotWidget.addItem(curve)
        centralWidget = QtWidgets.QWidget(self)
        self.setCentralWidget(plotWidget)

    def closeEvent(self, a0):
        self.oldWidget.addItem(self.curve)
        
        return super().closeEvent(a0)
        

class floatingCurves(QtWidgets.QWidget):

    def __init__(self, channelNum = 0):
        super(floatingCurves, self).__init__()
        self.curveList = list()
        self.plotWidgetList = list()
        
        #Perpare the layout
        self.layout = QtWidgets.QGridLayout()

        self.setLayout(self.layout)

        self.generateGraphsArray(channelNum)
    
    def addCurve(self, newCurve:pg.PlotDataItem, x, y):
        self.curveList.append(newCurve)
        plotWidget = pg.PlotWidget()
        plotWidget.addItem(newCurve)
        self.plotWidgetList.append(plotWidget)
        self.layout.addWidget(plotWidget,x,y)
        #Add the button
        button = QtWidgets.QPushButton("+",plotWidget)
        button.resize(20,20)
        button.clicked.connect(self.make_btn_floatWnd(len(self.curveList)-1))

    def generateGraphsArray(self, channelNum = 8):
        for i in range(channelNum):
            newCurve = pg.PlotDataItem()
            y = int(i/4)
            x = i%4
            self.addCurve(newCurve ,x,y)

    def make_btn_floatWnd(self, index):
        def btn_floatWnd():
            newWnd = floatingCurves_Max(self.curveList[index], self.plotWidgetList[index], self)
            newWnd.show()
        return btn_floatWnd

    def updateCurve(self, index, data: list(), data_x: list() = []):
        if data_x != []:
            self.curveList[index].setData(x=data_x, y=data)
        else:
            self.curveList[index].setData(data)


#Used for testing

# win = pg.GraphicsWindow()
# win.addPlot()

