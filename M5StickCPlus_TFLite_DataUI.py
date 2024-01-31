from socket import *
import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import QCoreApplication
import datetime
import time
import csv
import pprint

class Example(QMainWindow):
    SrcIP = "192.168.1.104" #M5StickC Address
    outputName = "sample"
    labelName = "flat"
    loop = 10
    n = 0

    #初期化
    def __init__(self):
        super().__init__()
        self.initUI()
        self.dt_now = datetime.datetime.now()

    #UI create
    def setIP(self, msg):
        SrcIP = msg                             # 受信元IP
        SrcPort = 55998                                 # 受信元ポート番号
        self.SrcAddr = (SrcIP, SrcPort)                 # アドレスをtupleに格納

        self.BUFSIZE = 1024                             # バッファサイズ指定
        self.udpServSock = socket(AF_INET, SOCK_DGRAM)  # ソケット作成
        self.udpServSock.bind(self.SrcAddr)             # 受信元アドレスでバインド

    #UI create
    def initUI(self):      

        btn1 = QPushButton("Record", self)
        btn1.move(30, 30)

        btn2 = QPushButton("Quit", self)
        btn2.move(150, 30)

        btn3 = QPushButton('Set IP', self)
        btn3.move(30, 80)        
        self.inputURL = QLineEdit(self)
        self.inputURL.move(150, 80)
        self.inputURL.setText("192.168.1.100")

        btn4 = QPushButton('Set name', self)
        btn4.move(30, 130)
        self.inputFileName = QLineEdit(self)
        self.inputFileName.move(150, 130)
        self.inputFileName.setText("sample")

        btn5 = QPushButton('Set Label', self)
        btn5.move(30, 180)
        self.inputLabelName = QLineEdit(self)
        self.inputLabelName.move(150, 180)
        self.inputLabelName.setText("flat")

        btn6 = QPushButton('Set Length', self)
        btn6.move(30, 230)
        self.inputLength = QLineEdit(self)
        self.inputLength.move(150, 230)
        self.inputLength.setText("100")

        # クリックされたらbuttonClickedの呼び出し
        btn1.clicked.connect(self.recv)            
        btn2.clicked.connect(QCoreApplication.instance().quit)
        btn3.clicked.connect(self.Output)
        btn4.clicked.connect(self.Setfilename)
        btn5.clicked.connect(self.Setlabel)
        btn6.clicked.connect(self.Setlength)

        self.statusBar()

        self.setGeometry(300, 300, 290, 300)
        self.setWindowTitle('Getting accel data')
        self.show()

    #UDP receive
    def recv(self):
        print('\n----------\nIP Adress: ', self.SrcIP)
        print('loop count: ', self.loop)
        print('Output filename: ', self.outputName)
        print('label name:', self.labelName)
        i = 0

        #with open(self.outputName + '_' + self.dt_now.isoformat().replace(':','') + '.txt', 'a', newline='') as f:
        with open(self.labelName + '.csv', 'a', newline='') as f:

            f.write('\n\n\n -,-,-\n')
            time.sleep(2)

            self.setIP(self.SrcIP)
            while i < self.loop: # 受信待ち
                i = i + 1
                data, addr = self.udpServSock.recvfrom(self.BUFSIZE) # 受信
                l = data.decode().split(',')
                # 受信データと送信アドレス表示
                # print(i, "\t", float(l[self.n]), float(l[self.n+1]), float(l[self.n+2]),float(l[self.n+3]), float(l[self.n+4]),float(l[self.n+5]),addr) 
                print(i, "\t", float(l[self.n]), float(l[self.n+1]), float(l[self.n+2]),addr) 

                # s = 'aX, aY, aZ, gX, gY, gZ = ' + l[self.n] + ', ' + l[self.n+1] + ', ' + l[self.n+2]+ ', ' + l[self.n+3]+ ', ' + l[self.n+4]+ ', ' + l[self.n+5]
                # self.statusBar().showMessage(s)

                # f.write(str(float(l[self.n])) + ',' + str(float(l[self.n+1])) + ',' + str(float(l[self.n+2])) +',' + str(float(l[self.n+3])) + ',' + str(float(l[self.n+4])) +',' + str(float(l[self.n+5])) +'\r\n')
                f.write(str(float(l[self.n])) + ',' + str(float(l[self.n+1])) + ',' + str(float(l[self.n+2]))  +'\r\n')

            i = 0
            self.udpServSock.close()

    #ULR set
    def Output(self):
        self.SrcIP = self.inputURL.text()
        print('Change IP Adress:', self.SrcIP)

    #ULR set
    def Setlength(self):
        self.loop = int(self.inputLength.text())
        print('Change loop count:', self.loop)

    #Label set
    def Setlabel(self):
        self.labelName = self.inputLabelName.text()
        print('Change label name:', self.labelName)

    #filename set
    def Setfilename(self):
        self.outputName = self.inputFileName.text()
        print('Change output filename:', self.outputName)

        #時間の取得
        self.dt_now = datetime.datetime.now()

    def buttonClicked(self):

        # ステータスバーへメッセージの表示
        sender = self.sender()
        self.statusBar().showMessage(sender.text() + ' was pressed')


if __name__ == '__main__':

    app = QApplication(sys.argv)
    ex = Example()
    sys.exit(app.exec_())
