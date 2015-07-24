'''
Created on Jan 25, 2014

@author: zeroshiiro

for OS X

'''
import serial
from interface import Interface
                
        
if __name__ == "__main__":
    
    #Connect with serial device, change as per device
    ser = serial.Serial("/dev/tty.usbmodem1411", 115200, timeout = 1)
    
    #Connect with Serial Interface
    ifObj = Interface()
    ifObj.setSerClient(ser)
    
    #Start thread
    ifObj.start()
    
    # The above reader threads are blocking and non-terminated, so the best way to stop this python
    # program is to hit Ctrl+Z and kill this process
    print("Exiting main thread\n")
