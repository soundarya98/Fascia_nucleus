
import os
os.environ['PYQTGRAPH_QT_LIB'] = 'PyQt5'
from PyQt5 import QtGui, QtCore, QtWidgets
import pyqtgraph as pg
import multiprocessing
import pandas as pd

from BCI_Data_Receiver  import *
import floatingCurves as fc
from CausalButter import *

import heartpy as hp
import math
import threading


i_CNT = 0
i_VAL = i_CNT + 1
i_ADS = i_VAL + 1
i_IMU = i_ADS + 8
i_EDA = i_IMU + 6
i_TEM = i_EDA + 1
i_PPG = i_TEM + 1
i_TIM = i_PPG + 1

class mainWindow(QtWidgets.QWidget):

    def __init__(self):

        #Init Data structures
        super(mainWindow,self).__init__()
        self.plotBufs = list()
        PLOTWNDSIZE = 2000
        self.PLOTWNDSIZE = PLOTWNDSIZE

        # for plotting
        self.start_idx = 2 #TODO: make this 0 if you want to graph all the packet data
        self.n_plots = 19
        self.fft_idx = self.n_plots-self.start_idx

        # for FFT
        self.graph_fft = 1 #TODO: change this to 1 if you dont want FFT graph
        self.FFT_CHANNEL = 1 #TODO: make sure this is the channel you want the FFT for (0 indexed)
        self.fft_lock = threading.Lock();
        # self.fft_thread = threading.Thread(target = self.fft_calc, args = (self.FFT_CHANNEL,) );
        self.moreData = False;
        # self.fft_thread.start()

        # for initial plotting
        for i in range(self.n_plots-self.start_idx + self.graph_fft):
            self.plotBufs.append(np.zeros(PLOTWNDSIZE))

        #For data Recording
        self.isRecording = False
        self.recordingBuf = list()

        #For filters
        #Init/store all the required filters
        data_rate = 1000
        self.data_rate = data_rate
        num_ADS_plots = 8
        # Do the bandpass filters
        self.BPfilters = []
        # for i in range(0,num_ADS_plots):
        self.BPfilters.append(CausalButter(4, 10, 500, data_rate, 0))  # EMG 1/2
        self.BPfilters.append(CausalButter(4, 10, 500, data_rate, 0))  # EMG 4/3
        self.BPfilters.append(CausalButter(4, 10, 500, data_rate, 0))  # EOG 1/2
        self.BPfilters.append(CausalButter(4, 10, 500, data_rate, 0))  # EMG 5/6
        self.BPfilters.append(CausalButter(4, 10, 500, data_rate, 0))  # EMG 7/8
        self.BPfilters.append(CausalButter(8,  5,  50, data_rate, 0))  # EEG 1
        self.BPfilters.append(CausalButter(8,  5,  50, data_rate, 0))  # EEG 2
        self.BPfilters.append(CausalButter(8,  5,  50, data_rate, 0))  # EEG 3
        self.BPfilters.append(CausalButter(8,  5,  50, data_rate, 0))  # EEG 4
        # and  the bandstop filters
        self.BSfilters = []
        for i in range(0,num_ADS_plots):
            self.BSfilters.append(CausalButter(8, 55, 65, data_rate, 1))

        # for heart rate measuring algorithm
        self.heart_sig_arr  = []
        self.heartbeat_ts   = []
        self.heartrate_avg  = []

        # initialize the UI
        self.title = "EEG Classifier"
        self.initUI()

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateGUI)

        # The ip of user's machine
        self.ip = '10.0.0.77'#'192.168.0.13' # '192.168.0.101'  # '192.168.0.13'
        self.port_number = 8899

        self.Data_receiver = BCI_Data_Receiver(self.ip, self.port_number, self.dataPlottingWidget)
        self.Data_receiver.asyncReceiveData(self.dataReadyCallback)



    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(100,100,1024,768)

        hbox = QtWidgets.QHBoxLayout()
        self.setLayout(hbox)


        #Add the graph arrays
        
        #Perpare the array
        self.dataPlottingWidget = fc.floatingCurves(self.n_plots-self.start_idx+self.graph_fft, self.start_idx, self.FFT_CHANNEL)

        hbox.addWidget(self.dataPlottingWidget)

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
        invalid_arr = newData[1];
        # t = "Packet #: " + str(newData[i_CNT])
        # self.dataPlottingWidget.PN.setText(t)

        # # for recording purposes
        # for i in range(0, self.start_idx):
        #     temp[i] = newData[i]

        if self.graph_fft:
            self.fft_thread = threading.Thread(target = self.fft_calc, args = (self.FFT_CHANNEL,) );
            self.fft_thread.start()


        for i in range(self.start_idx, self.n_plots):
            #apply filters to newData
            # temp[2+i] = newData[2+i]
            # temp[2+i] = self.BPfilters[i].inputData([newData[2+i]])[0]
            # temp[2+i] = self.BSfilters[i].inputData([temp[2+i]])[0]
            if (i >1 and i<10):
                temp[i] = self.BPfilters[i-i_ADS].inputData([newData[i]])[0]
                temp[i] = self.BSfilters[i-i_ADS].inputData([temp[i]])[0]
            else:
                temp[i] = newData[i]

            # temp[2+i] = self.HPfilters[i].inputData([convert_ADC_volts(newData[2+i])])[0]
            # temp[2+i] = convert_ADC_volts(newData[2+i]);
            # print(i)
            d.append([temp[i]])

            if ((invalid_arr>>i) & 1):
                # print("invalid data at "+str(i))
                continue

            #For plotting
            idx = i - self.start_idx
            self.plotBufs[idx] = self.plotBufs[idx][1:]
            self.plotBufs[idx] = np.append(self.plotBufs[idx],d[idx][0])

        # calculate heart rate
        # me: if it drops 400 counts in 5 samples -> heart beat
        # if not ((invalid_arr >> 18) & 1):
        #     try:
        #         working_data, measures = hp.process(self.plotBufs[18], self.data_rate/10);
        #         heart_rate = measures['bpm']
        #         if not math.isnan(heart_rate):
        #             print("heart rate: ", int(heart_rate)," bpm")
        #         else:
        #             print("hp nan")
        #     except hp.exceptions.BadSignalWarning:
        #         print("hp exception")
        #         pass

        # below would probably be good if the data is invalid due to lack of connection or incorrect data, instead of
        # the sampling rate issue where i am only sampling the PPG data once every 10 packets.
        # if ((invalid_arr >> i_PPG) & 1):
        #     self.heart_sig_arr = []
        #     self.heartbeat_ts = []
        # else:
        if not ((invalid_arr >> i_PPG) & 1):
            ppg_sig = newData[i_PPG]
            l = len(self.heart_sig_arr);
            for i in range(min(10, l)):#len(self.heart_sig_arr[])):
                if self.heart_sig_arr[l-1-i] - ppg_sig >= 100 and self.heart_sig_arr[l-1-i] - ppg_sig < 700 and l>20:
                    # print (ppg_sig,"-",self.heart_sig_arr[i])
                    print("heart beat!",newData[i_TIM])#newData[i_CNT])
                    self.heartbeat_ts.append(newData[i_TIM])#CNT])
                    self.plotBufs[i_PPG - self.start_idx][-1] *=-1
                    # calculate heart rate
                    if len(self.heartbeat_ts) > 1:
                        # delta_ts = time in ms difference between the current and most recent heart beat
                        delta_ts = self.heartbeat_ts[-1] - self.heartbeat_ts[len(self.heartbeat_ts)-2]
                        delta_sec = delta_ts / 1000 #* 1./(self.Data_receiver.current_data_rate)#/10) #not /10 because using the count of packets which is at regular data rate
                        bpm = 1/(delta_sec/60)#len(self.heartbeat_ts)/(delta_sec/60)
                        print("local heart rate: "+str(int(bpm)))
                        self.heartrate_avg.append(bpm)
                        bpm = np.average(self.heartrate_avg)
                        print("heart rate:",int(bpm), "bpm, ",len(self.heartbeat_ts)," / ",delta_ts)
                        t = "Heart Rate: " + str(int(bpm)) + " BPM"
                        self.dataPlottingWidget.HR.setText(t)
                    self.heart_sig_arr = []
                    break
            self.heart_sig_arr.append(ppg_sig)
            # trim arrays to max lengths
            if len(self.heart_sig_arr) > 25:
                self.heart_sig_arr = self.heart_sig_arr[1:]
            if len(self.heartbeat_ts) > 100:
                self.heartbeat_ts = self.heartbeat_ts[1:]
            if len(self.heartrate_avg) > 100:
                self.heartrate_avg = self.heartrate_avg[1:]
        elif ((invalid_arr >> i_TIM) & 1):
            print("no contact with ppg sensor")
            self.heart_sig_arr = []
            self.heartbeat_ts = []
            self.heartrate_avg = []
        # else:
        #     print("invalid heart and temp data")
        # if(self.isRecording == True):
        #     #save new data to the recordingBuf
        #     self.recordingBuf.append(temp)
        self.moreData = True

            
    def fft_calc(self, channel):
        # while True:
        #     if self.moreData:
        #         self.moreData = False;
        #FFT stuff here
        # FFT_CHANNEL = 1;
        # self.fft_lock.acquire()
        channel_idx = i_ADS + channel - self.start_idx
        # print(channel_idx)
        bins = np.fft.rfft(self.plotBufs[channel_idx])
        size = len(self.plotBufs[channel_idx])
        # self.fft_lock.release()
        bins = np.abs(bins)#[np.abs(v) for v in bins]
        bins[0] = 0 # first element is DC element
        timestep = 1/self.data_rate
        freq = np.fft.rfftfreq(self.PLOTWNDSIZE, timestep);
        self.plotBufs[self.fft_idx] = bins,freq;
        # print("fft freq", freq)
        # print("fft bins", bins)
        # sys.stdout.flush()
        # print(len(bins), len(freq))

    def start(self):
        self.timer.start(1)

    def updateGUI(self):
        if self.moreData:
            self.moreData = False
            for i in range(self.n_plots-self.start_idx):
                # print(i,type(self.plotBufs[i]),self.plotBufs[i])
                self.dataPlottingWidget.updateCurve(i,self.plotBufs[i])
            if self.graph_fft:
                self.dataPlottingWidget.updateCurve(self.fft_idx,self.plotBufs[self.fft_idx][0],self.plotBufs[self.fft_idx][1])

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

    # while (True):
    #     time.sleep(1)