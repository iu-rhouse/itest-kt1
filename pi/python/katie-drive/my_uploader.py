from poster.encode import multipart_encode
from poster.streaminghttp import register_openers
import urllib2
from my_camera import CameraUtil
import thread
import threading


def t_a_u(name, width, height, rotation, address, robot_id):
    CameraUtil.take_photo(name, width, height, rotation)
    ImageUploader.upload("http://" + address + ":8080/SLAM/uploadPhoto", name,
                         robot_id)


class ImageUploader:
    lock = threading.Lock()

    @staticmethod
    def upload(address, file_name, robot_id):
        ImageUploader.lock.acquire()
        register_openers()
        datagen, headers = multipart_encode({"image": open(file_name, "rb"), "robotId": robot_id})
        request = urllib2.Request(address, datagen, headers)
        urllib2.urlopen(request).read()
        print "Image uploading finished"
        ImageUploader.lock.release()

    @staticmethod
    def take_and_upload(name, width, height, rotation, address, robot_id):
        thread.start_new_thread(t_a_u, (name, width, height, rotation, address, robot_id))



