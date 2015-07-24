import threading
import time
from my_uploader import ImageUploader
import ujson
from my_websocket_client import ControlMessage
from my_action_threads_holder import ActionThreads


class AutoTakeThread(threading.Thread):
    def set_config(self, server_ip, robot_id, interval, rotation):
        self.server_ip = server_ip
        self.robot_id = robot_id
        self.interval = interval
        self.rotation = rotation

    def set_go(self, go):
        self.go = go

    def run(self):
        print "AutoTakeThread started for", self.interval, "seconds,", "at", self.rotation
        while self.go:
            ImageUploader.take_and_upload("auto_take.jpg", "400", "300", str(self.rotation), self.server_ip,
                                          self.robot_id)
            time.sleep(self.interval)
        print "AutoTakeThread ended"


class ActionGreetThread(threading.Thread):
    def set_config(self, width, speed, repeat, randomness, serial, ws, robot_id):
        self.serial = serial
        self.ws = ws
        self.width = width
        self.speed = speed
        self.repeat = repeat
        self.randomness = randomness
        self.robot_id = robot_id
        self.timeout = 5  # wait for this period if serial does not report back for completion of move
        self.waiting_interval = 0.1  # for even quicker responsiveness you can make this value smaller
        self.proceeds = 0
        self.proceed_lock = threading.Lock()

    def wait(self):
        waiting_time = 0
        while self.proceeds <= 0:
            time.sleep(self.waiting_interval)
            waiting_time += self.waiting_interval
            if waiting_time > self.timeout:
                raise Exception("Serial is not reporting back")

        self.proceed_lock.acquire()
        self.proceeds -= 1
        self.proceed_lock.release()

    def do_proceed(self):
        self.proceed_lock.acquire()
        self.proceeds += 1
        self.proceed_lock.release()

    def run(self):
        print "ActionGreetThread started"
        repeat = self.repeat
        # greet:
        # 1. rotate left check_width
        # 2. rotate right check_width * 2
        # 3. rotate left check_width
        # 4. repeat this number of check_repeats

        while repeat > 0:
            try:
                # step 1
                serial_command = "@p" + str(-self.width).zfill(4) + str(self.speed).zfill(4) + "\n"
                print "Driving command (test): " + serial_command
                self.serial.write(serial_command)
                self.wait()

                # step 2
                serial_command = "@p" + str(self.width * 2).zfill(4) + str(self.speed).zfill(4) + "\n"
                print "Driving command (test): " + serial_command
                self.serial.write(serial_command)
                self.wait()

                # step 3
                serial_command = "@p" + str(-self.width).zfill(4) + str(self.speed).zfill(4) + "\n"
                print "Driving command (test): " + serial_command
                self.serial.write(serial_command)
                self.wait()

                repeat -= 1
            except Exception as e:
                print "Exception in ActionGreetThread: ", e
                break

        ActionThreads.action_thread = None
        print "ActionGreetThread ended"

        self.ws.send(
            ujson.dumps(ControlMessage("topic.execute", ujson.dumps([self.robot_id, True]))))


class ActionShooThread(threading.Thread):
    def set_config(self, width, speed, f_distance, f_speed, randomness, serial, ws, robot_id):
        self.serial = serial
        self.ws = ws
        self.width = width
        self.speed = speed
        self.f_distance = f_distance
        self.f_speed = f_speed
        self.randomness = randomness
        self.robot_id = robot_id
        self.timeout = 5  # wait for this period if serial does not report back for completion of move
        self.waiting_interval = 0.1  # for even quicker responsiveness you can make this value smaller
        self.proceeds = 0
        self.proceed_lock = threading.Lock()

    def wait(self):
        waiting_time = 0
        while self.proceeds <= 0:
            time.sleep(self.waiting_interval)
            waiting_time += self.waiting_interval
            if waiting_time > self.timeout:
                raise Exception("Serial is not reporting back")

        self.proceed_lock.acquire()
        self.proceeds -= 1
        self.proceed_lock.release()

    def do_proceed(self):
        self.proceed_lock.acquire()
        self.proceeds += 1
        self.proceed_lock.release()

    def run(self):
        print "ActionShooThread started"
        # shoo:
        # 1. rotate left the check width with a random window of up to 50%
        # //  use this to generate random window :check_width - (randomness *
        # check_width / 2) + (random(randomness * check_width))  // randomness
        # should be decided by 100 before this...
        # 2. rotate right the degree generated in previous step times 2
        # 3. and rotate back to the left the random number
        # 4. rotate 180 with random window
        # 5. move forward flight distance with random window
        # 6. rotate 180 again with random window

        try:
            # step 1
            serial_command = "@p" + str(self.width).zfill(4) + str(self.speed).zfill(4) + "\n"
            print "Driving command (test): " + serial_command
            self.serial.write(serial_command)
            self.wait()

            # step 2
            serial_command = "@p" + str(self.width).zfill(4) + str(self.speed).zfill(4) + "\n"
            print "Driving command (test): " + serial_command
            self.serial.write(serial_command)
            self.wait()

            # step 3
            serial_command = "@p" + str(self.width).zfill(4) + str(self.speed).zfill(4) + "\n"
            print "Driving command (test): " + serial_command
            self.serial.write(serial_command)
            self.wait()

            # step 4
            serial_command = "@p" + str(self.width).zfill(4) + str(self.speed).zfill(4) + "\n"
            print "Driving command (test): " + serial_command
            self.serial.write(serial_command)
            self.wait()

            # step 5
            serial_command = "@p" + str(self.width).zfill(4) + str(self.speed).zfill(4) + "\n"
            print "Driving command (test): " + serial_command
            self.serial.write(serial_command)
            self.wait()

            # step 6
            serial_command = "@p" + str(self.width).zfill(4) + str(self.speed).zfill(4) + "\n"
            print "Driving command (test): " + serial_command
            self.serial.write(serial_command)
            self.wait()

        except Exception as e:
            print "Exception in ActionShooThread: ", e

        ActionThreads.action_thread = None
        print "ActionShooThread ended"

        self.ws.send(
            ujson.dumps(ControlMessage("topic.execute", ujson.dumps([self.robot_id, True]))))
