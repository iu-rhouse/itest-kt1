import subprocess
import threading


class CameraUtil():
    lock = threading.Lock()

    @staticmethod
    def take_photo(name, width, height, rotation):
        CameraUtil.lock.acquire()
        cmd = "raspistill -t 100 -o " + name + " -w " + width + " -h " + height + " -rot " + rotation
        pid = subprocess.call(cmd, shell=True)
        print "Finished photo taken for:", name
        CameraUtil.lock.release()