'''
Created on Feb 2, 2014

@author: zeroshiiro
'''
from __future__ import print_function
import threading
import sys
import time
from translate import Translate

class SerialInterface(threading.Thread):
    '''
    classdocs
    '''
    serClient = None
    wsClient = None

    def __init__(self):
        '''
        Constructor
        '''
    
    def setSerialDevice(self, ser):
        self.serClient = ser
        
    def setWebSocket(self, ws):
        self.wsClient = ws
        
    def run(self):
        print("Serial Interface Started")
        while(True):
            try:
                data = self.serClient.readline()
                if("#" not in data): continue
#                Human readable format translation here
#                Print results
            except KeyboardInterrupt as e:
                print(e)