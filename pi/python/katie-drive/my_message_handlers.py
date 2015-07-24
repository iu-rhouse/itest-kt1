import ujson
from my_uploader import ImageUploader
import time
from my_robot_status import RobotInfo
from my_websocket_client import ControlMessage
from my_other_threads import AutoTakeThread, ActionGreetThread, ActionShooThread
from my_protocol import Protocols
from my_action_threads_holder import ActionThreads
from datetime import datetime


class HandlerForMessageFromWebSocket():
    def __init__(self, ws_client, serial_client, robot_id, server_ip):
        self.ws_client = ws_client
        self.serial_client = serial_client
        self.robot_id = robot_id
        self.server_ip = server_ip
        self.auto_take = None

    def process(self, message):
        print str(datetime.now()), "     From WebSocket:\n" + message
        try:
            decoded = ujson.loads(message)

            if "command" in decoded:

                # TURN ON AUTO TAKE OF PHOTOS
                if decoded["command"] == Protocols.commands[8]:
                    RobotInfo.auto_take = not RobotInfo.auto_take
                    if RobotInfo.auto_take:
                        self.auto_take = AutoTakeThread()
                        RobotInfo.interval = decoded["content"]["interval"]
                        RobotInfo.rotation = decoded["content"]["rotation"]
                        self.auto_take.set_config(self.server_ip, self.robot_id, RobotInfo.interval,
                                                  RobotInfo.rotation)
                        self.auto_take.set_go(True)
                        self.auto_take.start()
                    else:
                        self.auto_take.set_go(False)
                        self.auto_take = None

                #  RESET ARDUINO       
                elif decoded["command"] == Protocols.commands[1]:
                    self.serial_client.write("@r\n")

                #  TOGGLE QUICK SCRIPT TURN (THIS SHOULD BE CHANGED)
                elif decoded["command"] == Protocols.commands[2]:
                    RobotInfo.turn_a = not RobotInfo.turn_a
                    self.serial_client.write("@a\n")

                #  360 DEGREE TURN
                elif decoded["command"] == Protocols.commands[4]:
                    self.serial_client.write("@h\n")

                # TURN ON MODEL E DATA COLLECTION
                elif decoded["command"] == Protocols.commands[5]:
                    RobotInfo.model_e = not RobotInfo.model_e

                    if RobotInfo.model_e:
                        if RobotInfo.model_t:
                            self.serial_client.write("@t\n")
                        RobotInfo.model_t = False

                    self.serial_client.write("@e\n")

                # TURN ON MODEL T DATA COLLECTION
                elif decoded["command"] == Protocols.commands[6]:
                    RobotInfo.model_t = not RobotInfo.model_t

                    if RobotInfo.model_t:
                        if RobotInfo.model_e:
                            self.serial_client.write("@e\n")
                        RobotInfo.model_e = False

                    self.serial_client.write("@t\n")

                # REPORT STATUS (CHECK ON WHAT THIS DOES)
                elif decoded["command"] == Protocols.commands[0]:
                    info = ujson.dumps(RobotInfo)
                    self.ws_client.send(ujson.dumps(ControlMessage("topic.robot.status", info)))

                # RUN EXPLORATION SCRIPT (OUTMODED)
                elif decoded["command"] == Protocols.commands[3]:
                    RobotInfo.do_explore = not RobotInfo.do_explore
                    self.serial_client.write("@u\n")

                # RUN ROOMBA COVER SCRIPT
                elif decoded["command"] == Protocols.commands[7]:
                    RobotInfo.do_roomba = not RobotInfo.do_roomba
                    self.serial_client.write("@j\n")

                # TAKE ONE PHOTO
                elif decoded["command"] == Protocols.commands[9]:
                    RobotInfo.rotation = decoded["content"]["rotation"]
                    ImageUploader.take_and_upload("one_photo.jpg", "400", "300", str(RobotInfo.rotation),
                                                  self.server_ip,
                                                  self.robot_id)

                # DRIVE 
                elif decoded["command"] == Protocols.commands[10]:
                    distance = decoded["content"]["driveDistance"]
                    velocity = decoded["content"]["driveVelocity"]
                    degrees = decoded["content"]["driveDegrees"]
                    radius = decoded["content"]["driveRadius"]
                    if distance != 0 and degrees == 0 and radius == 0: #and velocity > 0 
                        # go straight
                        serial_command = "@x" + str(distance).zfill(4) + str(velocity).zfill(4) + "\n"
                    elif distance == 0 and degrees != 0 and radius == 0:  #and velocity > 0 
                        # go pivot
                        serial_command = "@p" + str(degrees).zfill(4) + str(velocity).zfill(4) + "\n"
                    elif distance != 0 and degrees == 0 and radius != 0: #and velocity > 0 
                        # go around
                        serial_command = "@v" + str(distance).zfill(4) + str(radius).zfill(4) + str(velocity).zfill(
                            4) + "\n"
                    else:
                        print "Invalid driving command"
                        self.ws_client.send(
                            ujson.dumps(ControlMessage("topic.execute", ujson.dumps([self.robot_id, False]))))
                        return

                    print "Driving command (test): " + serial_command
                    self.serial_client.write(serial_command)

                    #test
                    self.ws_client.send(
                        ujson.dumps(
                            ControlMessage("topic.current.heading", ujson.dumps([self.robot_id, serial_command]))))
                    #test
                    self.ws_client.send(
                        ujson.dumps(
                            ControlMessage("topic.rear.distance", ujson.dumps([self.robot_id, 0]))))

                # STOP
                elif decoded["command"] == Protocols.commands[11]:
                    self.serial_client.write("@w\n")

                # SHOO BEHAVIOR
                elif decoded["command"] == Protocols.commands[12]:
                    # shoo
                    width = decoded["content"]["checkWidth"]
                    speed = decoded["content"]["checkSpeed"]
                    flight_distance = decoded["content"]["flightDistance"]
                    flight_speed = decoded["content"]["flightSpeed"]
                    randomness = decoded["content"]["randomness"]

                    if ActionThreads.action_thread is not None:
                        raise Exception("Previous ActionThread is not finished yet")

                    ActionThreads.action_thread = ActionShooThread()
                    ActionThreads.action_thread.set_config(width, speed, flight_distance, flight_speed, randomness,
                                                           self.serial_client, self.ws_client, self.robot_id)
                    ActionThreads.action_thread.start()

                # GREET BEHAVIOR
                elif decoded["command"] == Protocols.commands[13]:
                    # greet
                    width = decoded["content"]["checkWidth"]
                    speed = decoded["content"]["checkSpeed"]
                    repeat = decoded["content"]["checkRepeat"]
                    randomness = decoded["content"]["randomness"]

                    if ActionThreads.action_thread is not None:
                        raise Exception("Previous ActionThread is not finished yet")

                    ActionThreads.action_thread = ActionGreetThread()
                    ActionThreads.action_thread.set_config(width, speed, repeat, randomness,
                                                           self.serial_client, self.ws_client, self.robot_id)
                    ActionThreads.action_thread.start()

                # CAMERA TILT
                elif decoded["command"] == Protocols.commands[14]:
                    degree = decoded["content"]["cameraTilt"]
                    # need to write the degree value to serial
                    serial_command = "@m" + str(degree).zfill(4) + "\n"
                    self.serial_client.write(serial_command)

                # BEEP
                elif decoded["command"] == Protocols.commands[15]:
                    self.serial_client.write("@b\n")

                # SCAN (NEED TO BUILD IT ON ARDUINO)
                elif decoded["command"] == Protocols.commands[16]:
                    # to-do: "scan" command
                    self.serial_client.write("\n")

                else:
                    raise Exception("Unknown command")

                print "Current Robot Status:", ujson.dumps(RobotInfo), "\n"
                self.ws_client.send(
                    ujson.dumps(ControlMessage("topic.execute", ujson.dumps([self.robot_id, True]))))

        except Exception as e:
            print "Exception in HandlerForMessageFromWebSocket: ", e
            self.ws_client.send(
                ujson.dumps(ControlMessage("topic.execute", ujson.dumps([self.robot_id, False]))))


class HandlerForDataFromSerialModelE():
    def __init__(self, ws_client, robot_id, server_ip):
        self.ws_client = ws_client
        self.robot_id = robot_id
        self.server_ip = server_ip

    def process(self, message):
        if RobotInfo.model_e:
            if "i" in message or "#" in message:
                return

            #print "Got model-E message:", message
            if self.ws_client is not None:
                self.ws_client.send(message)


class HandlerForActionFromSerial():
    def __init__(self, ws_client, robot_id, server_ip):
        self.ws_client = ws_client
        self.robot_id = robot_id
        self.server_ip = server_ip

    def process(self, message):
        if "timed_move_completed" in message:
            if ActionThreads.action_thread is not None:
                ActionThreads.action_thread.do_proceed()

        elif "#stt" in message:
            ImageUploader.take_and_upload("stt.jpg", "400", "300", str(RobotInfo.rotation), self.server_ip,
                                          self.robot_id)
            if self.ws_client is not None:
                self.ws_client.send(
                    ujson.dumps(ControlMessage("topic.perception", ujson.dumps([self.robot_id, "kick"]))))


class HandlerForDataFromSerialModelT():
    def __init__(self, ws_client):
        self.ws_client = ws_client
        self.buff = ''
        self.count = 0

    def process(self, message):
        if RobotInfo.model_t:

            # Matt's previous method
            if self.count == 0 and '#ana ' in message:
                self.buff = message.split('#ana ')[1][:-2]
                self.count += 1
            elif self.count == 1 and '#cfs ' in message:
                self.buff = self.buff + message.split('#cfs ')[1][:-2]
                self.count += 1
            elif self.count == 2 and '#itc ' in message:
                self.buff = self.buff + message.split('#itc ')[1][:-2]
                self.buff = self.buff + " " + str(int(time.time() * 1000))
                self.count = 0
                #print "Got model-T message:", self.buff
                if self.ws_client is not None:
                    self.ws_client.send(self.buff)


class MessageHandlerCopyToSerial():
    def __init__(self, serial_client):
        self.serial_client = serial_client

    def process(self, message):
        if not message.startswith("ws:"):
            self.serial_client.write(message)


class MessageHandlerCopyToWebSocket():
    def __init__(self, ws_client):
        self.ws_client = ws_client

    def process(self, message):
        if message.startswith("ws:"):
            ws_message = message.split("ws:")[1]
            self.ws_client.send(ws_message)