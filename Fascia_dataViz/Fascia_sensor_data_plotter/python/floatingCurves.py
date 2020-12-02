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

    def __init__(self, channelNum, start_i, fft_chan):
        super(floatingCurves, self).__init__()
        self.curveList = list()
        self.plotWidgetList = list()
        
        #Perpare the layout
        self.layout = QtWidgets.QGridLayout()

        self.setLayout(self.layout)
        self.titles = ["packet number", "Valid array", "ADS 1: EMG 1/2 (volts)", "ADS 2: EMG 4/3 (volts)", "ADS 3: EOG 1/2 (volts)",
                       "ADS 4: EMG 7/8 (volts)", "ADS 5: EEG1 (PASSIVE) (volts)", "ADS 6: EEG2 (PASSIVE) (volts)", "ADS 7: EEG (ACTIVE) (volts)", "ADS 8: EEG (ACTIVE) (volts)",
                       "IMU Ax", "IMU Ay","IMU Az", "IMU Gx","IMU Gy", "IMU Gz",#"IMU 7", "IMU 8","IMU 9",
                       "EDA: Rskin (Ohm)","temperature (C)", "PPG raw data", "FFT from ADS "+str(fft_chan)]#,"battery voltage level"]#"heart rate arduino"]
        self.generateGraphsArray(channelNum, start_i)
        self.addText()

    def addText(self):
        PN = QtWidgets.QLabel()
        PN.setText("Packet #: ")
        self.layout.addWidget(PN)
        self.PN = PN
        PDR = QtWidgets.QLabel()
        PDR.setText("P Data Rate: ")
        self.layout.addWidget(PDR) #pg.TextItem("Data Rate: ")
        self.PDR = PDR
        ADR = QtWidgets.QLabel()
        ADR.setText("A Data Rate: ")
        self.layout.addWidget(ADR) #pg.TextItem("Data Rate: ")
        self.ADR = ADR
        HR = QtWidgets.QLabel()
        HR.setText("Heart Rate: ")
        self.layout.addWidget(HR)
        self.HR = HR

    def addCurve(self, newCurve:pg.PlotDataItem, x, y, t):
        self.curveList.append(newCurve)
        plotWidget = pg.PlotWidget(title=t)
        plotWidget.addItem(newCurve)
        self.plotWidgetList.append(plotWidget)
        self.layout.addWidget(plotWidget,x,y)
        #Add the button
        button = QtWidgets.QPushButton("+",plotWidget)
        button.resize(20,20)
        button.clicked.connect(self.make_btn_floatWnd(len(self.curveList)-1))

    def generateGraphsArray(self, channelNum, start_i):
        for i in range(channelNum):
            newCurve = pg.PlotDataItem()
            y = int(i/4)
            x = i%4
            self.addCurve(newCurve ,x,y, self.titles[i+start_i])

    def make_btn_floatWnd(self, index):
        def btn_floatWnd():
            newWnd = floatingCurves_Max(self.curveList[index], self.plotWidgetList[index], self)
            newWnd.show()
        return btn_floatWnd

    def updateCurve(self, index, data: list(), data_x: list() = []):
        if data_x != []:
            self.curveList[index].setData(x=data_x, y=data)
        else:
            self.curveList[index].setData(y=data)

#Used for testing

# win = pg.GraphicsWindow()
# win.addPlot()

