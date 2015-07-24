import serial
import threading


class SLAMSerialClient():
    def __init__(self, device_name, device_freq):
        self.ser = serial.Serial(device_name, device_freq)
        self.writing_lock = threading.Lock()

    def read_line(self):
        # There should be only ONE thread reading from this serial port
        if self.ser is not None:
            line = self.ser.readline()
            return line

    def write(self, message):
        print "Write to serial...\n"
        # Here we enabled multi-threads writing to the same serial port concurrently
        if self.ser is not None:
            try:
                self.writing_lock.acquire()
                self.ser.write(message)
            finally:
                self.writing_lock.release()

