
import os
os.environ['PYQTGRAPH_QT_LIB'] = 'PyQt5'
from PyQt5 import QtGui, QtCore, QtWidgets
import pyqtgraph as pg
import multiprocessing
import pandas as pd

from BCI_Data_Receiver  import *
import floatingCurves as fc
from CausalButter import *

"""
BUGMAN 12/20/2018
"""




class mainWindow(QtWidgets.QWidget):

    def __init__(self):

        self.n_plots = 20

        super(mainWindow,self).__init__()
        self.title = "EEG Classifier"
        self.initUI()


        PLOTWNDSIZE = 2000
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateGUI)
        #Init Data structures
        self.plotBufs = list()


        #The ip of user's machine
        # self.ip = '18.30.22.83'
        self.ip = '192.168.0.13' # '192.168.0.101'  # '192.168.0.13'
        # self.ip = '172.30.1.251'
        # self.port_number = 35295
        self.port_number = 8899

        self.Data_receiver = BCI_Data_Receiver(self.ip, self.port_number)
        self.Data_receiver.asyncReceiveData(self.dataReadyCallback)

        for i in range(self.n_plots):
            self.plotBufs.append(np.zeros(PLOTWNDSIZE))


        #For data Recording
        self.isRecording = False
        self.recordingBuf = list()


        #For filters
        #Init the filters
        data_rate = 1000
        self.BPfilters = []
        for i in range(0,self.n_plots):
            # 4 order butterworth 10hz to 500hz, 0 is bandpass 1 is bandstop EMG
            self.BPfilters.append(CausalButter(8,5,50,data_rate,0))
            #self.HPfilters.append(CausalButter(4,10,500,1000,0))
            # 4 order butterworth 10hz to 500hz, 0 is bandpass 1 is bandstop EEG1w
            #self.HPfilters.append(CausalButter(4,2,100,1000,0))
            #self.HPfilters.append(CausalButter(4,10,500,1000,0))
            #self.HPfilters.append(CausalButter(4,10,500,1000,0))
            # for EOG FIR
        self.BSfilters = []
        for i in range(0,self.n_plots):
            self.BSfilters.append(CausalButter(8,55,65,data_rate,1))




    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(100,100,1024,768)

        hbox = QtWidgets.QHBoxLayout()
        self.setLayout(hbox)


        #Add the graph arrays
        
        #Perpare the array
        self.dataPlotingWidget = fc.floatingCurves(self.n_plots)

        hbox.addWidget(self.dataPlotingWidget)

        # #Add the button panel
        # self.button_panel = QtWidgets.QWidget(self)
        # self.button_panel_layout = QtWidgets.QVBoxLayout()
        # self.button_panel.setLayout(self.button_panel_layout)
        # hbox.addWidget(self.button_panel)
        # #Add all the buttons
        # self.ICA_btn = QtWidgets.QPushButton("ICA")
        # self.record_btn = QtWidgets.QPushButton("Record")
        # self.filter_btn = QtWidgets.QPushButton("Filters")
        #
        # self.button_panel_layout.addWidget(self.ICA_btn)
        # self.button_panel_layout.addWidget(self.record_btn)
        # self.button_panel_layout.addWidget(self.filter_btn)
        #
        # #Connect all button functions
        # self.linkBtnFunctions()

        self.show()

    def linkBtnFunctions(self):
        self.record_btn.clicked.connect(self.onRecordBtnClicked)        


    def dataReadyCallback(self, newData):
        d = list()
        temp = np.zeros(len(newData))
        temp[0] = newData[0]
        temp[1] = newData[1]
        # print("cnt: ", newData[0])
        # print("status register: ", newData[1])
        #print(newData[10])
        for i in range(self.n_plots):
            #apply filters to newData
            # temp[2+i] = newData[2+i]
            # temp[2+i] = self.BPfilters[i].inputData([newData[2+i]])[0]
            # temp[2+i] = self.BSfilters[i].inputData([temp[2+i]])[0]
            if (i >1 and i<10):
                temp[i] = self.BPfilters[i].inputData([newData[i]])[0]
                temp[i] = self.BSfilters[i].inputData([temp[i]])[0]
            else:
                temp[i] = newData[i]

            # temp[2+i] = self.HPfilters[i].inputData([convert_ADC_volts(newData[2+i])])[0]
            # temp[2+i] = convert_ADC_volts(newData[2+i]);
            # print(i)
            d.append([temp[i]])

            if (i == 18 and temp[i] == 0):
                continue

            #For ploting
            self.plotBufs[i] = self.plotBufs[i][1:]
            self.plotBufs[i] = np.append(self.plotBufs[i],d[i][0])

        if(self.isRecording == True):
            #save new data to the recordingBuf
            self.recordingBuf.append(temp)
            

    def start(self):
        self.timer.start(30)

    def updateGUI(self):
        for i in range(self.n_plots):
            self.dataPlotingWidget.updateCurve(i,self.plotBufs[i])

    def keyPressEvent(self, e):
        """
        press "r" key to record the data
        """
        if e.isAutoRepeat():
            return
        if( e.key() == QtCore.Qt.Key_R and self.isRecording == False):
            self.onRecordBtnClicked()
        return super().keyPressEvent(e)

    def keyReleaseEvent(self, e):
        """
        release "r" key to stop recording data
        """
        if e.isAutoRepeat():
            return
        if( e.key() == QtCore.Qt.Key_R and self.isRecording == True):
            self.onRecordBtnClicked()

        return super().keyReleaseEvent(e)

    def onRecordBtnClicked(self):
        """
        The callback function for the click event of record button
        """
        if(self.isRecording == False):
            self.isRecording = True
            #Start Record
            self.record_btn.setText("stop")
            #Push data to the buffer
            #This step is done in the data ready callback

        else:
            self.record_btn.setText("Record")
            self.isRecording = False
            #Open the save file dialog
            name = QtWidgets.QFileDialog.getSaveFileName(self, "Save File", "0.csv", "CSV(*.csv)")
            if(name[0] != ""):
                df = pd.DataFrame(self.recordingBuf)
                df.to_csv(name[0], encoding='utf-8', sep="\t", index=False)


            #Clear the recording buffer
            self.recordingBuf.clear()



    def onICABtnClicked(self):
        """
        Callback function for the click event of the ICA button
        """
        pass

def convert_ADC_volts(raw_data, gain = 1): #gain was = 24
    # LSB = (2 x VREF)/ Gain / (2^24 - 1)
    vref = 4.5
    fs = 2*vref / gain
    converted_data = fs * raw_data / (2**(24)-1)
    # print(raw_data, " -> ", converted_data)
    return converted_data


if __name__ == "__main__":
    # fs = 4.5/24
    # print(convert_ADC_volts(0x000000) == 0)
    # print(convert_ADC_volts(0x000001) == fs/(2**23 -1))  #+1
    # print(convert_ADC_volts(0x7FFFFF) >= fs) #MAX POS NUM
    # # print(convert_ADC_volts(0xFFFFFF) == -fs/(2**23 -1)) #-1
    # print(convert_ADC_volts(-1) == -fs/(2**23 -1)) #-1
    # # print(convert_ADC_volts(0x800000) <= -fs * 2**23/(2**23 -1)) #MAX NEG NUM
    # print(convert_ADC_volts(-8388608) <= -fs * 2**23/(2**23 -1)) #MAX NEG NUM
    app = QtWidgets.QApplication([])
    ex = mainWindow()
    ex.start()
    sys.exit(app.exec_())

    while (True):
        time.sleep(1)

