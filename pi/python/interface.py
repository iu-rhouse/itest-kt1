#!/usr/bin/python
'''
Created on Jan 25, 2014

Latest version of Interface,
as of this version full duplex is still not realized,
if using commands which stream data continuously, use noop (or any non-command) to read more data.

@author: zeroshiiro
'''
from __future__ import print_function
import sys
import time
import threading
from translate import Translate

class Interface(threading.Thread):
    '''
    classdocs
    '''
    serClient = None
    translate = None

    sendData = None
   
    def setSerClient(self, ser):
        self.serClient = ser
       
    def run(self):
        print("Interface Thread Started\n")
        self.translate = Translate()
        while(True):
            
            #transfer user input to serial port
            cmd = raw_input(">>:")
            if(cmd == "exit"):
                sys.exit()
            else:
                self.sendData = self.translate.commandForKatie(cmd.lower())


            self.serClient.write(self.sendData)
            #give the arduino time to receive, process and populate data buffer
            print("Sending data: " + self.sendData)
            time.sleep(1)
            
            #get data for millisecs as stated by user command
            #inWaiting() basically checks if there is anymore data in the serial buffer
            #its similar to waiting for EOF for files.
            while(self.serClient.inWaiting()):
                try:
                    data = self.serClient.readline()

                    if("#" not in data): continue
                    print(self.translate.dataFromKatie(data))
                except KeyboardInterrupt:
                    print("Exiting...")
                    break
