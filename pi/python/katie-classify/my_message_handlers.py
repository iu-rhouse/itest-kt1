import ujson
from my_uploader import ImageUploader
import time
from my_robot_status import RobotInfo
from my_websocket_client import ControlMessage
from my_other_threads import AutoTakeThread, ActionGreetThread, ActionShooThread
from my_protocol import Protocols
from my_action_threads_holder import ActionThreads
from datetime import datetime
import pickle
import numpy as np
import pandas as pd
from testPresence import Predictor

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

                elif decoded["command"] == Protocols.commands[1]:
                    self.serial_client.write("@r\n")

                elif decoded["command"] == Protocols.commands[2]:
                    RobotInfo.turn_a = not RobotInfo.turn_a
                    self.serial_client.write("@a\n")

                elif decoded["command"] == Protocols.commands[4]:
                    self.serial_client.write("@l\n")

                elif decoded["command"] == Protocols.commands[5]:
                    RobotInfo.model_e = not RobotInfo.model_e

                    if RobotInfo.model_e:
                        if RobotInfo.model_t:
                            self.serial_client.write("@t\n")
                        RobotInfo.model_t = False

                    self.serial_client.write("@t\n")

                elif decoded["command"] == Protocols.commands[6]:
                    RobotInfo.model_t = not RobotInfo.model_t

                    if RobotInfo.model_t:
                        if RobotInfo.model_e:
                            self.serial_client.write("@e\n")
                        RobotInfo.model_e = False

                    self.serial_client.write("@t\n")

                elif decoded["command"] == Protocols.commands[0]:
                    info = ujson.dumps(RobotInfo)
                    self.ws_client.send(ujson.dumps(ControlMessage("topic.robot.status", info)))

                elif decoded["command"] == Protocols.commands[3]:
                    RobotInfo.do_explore = not RobotInfo.do_explore
                    self.serial_client.write("@u\n")

                elif decoded["command"] == Protocols.commands[7]:
                    RobotInfo.do_roomba = not RobotInfo.do_roomba
                    self.serial_client.write("@j\n")

                elif decoded["command"] == Protocols.commands[9]:
                    RobotInfo.rotation = decoded["content"]["rotation"]
                    ImageUploader.take_and_upload("one_photo.jpg", "400", "300", str(RobotInfo.rotation),
                                                  self.server_ip,
                                                  self.robot_id)

                elif decoded["command"] == Protocols.commands[10]:
                    distance = decoded["content"]["driveDistance"]
                    velocity = decoded["content"]["driveVelocity"]
                    degrees = decoded["content"]["driveDegrees"]
                    radius = decoded["content"]["driveRadius"]
                    if distance > 0 and velocity > 0 and degrees == 0 and radius == 0:
                        # go straight
                        serial_command = "@x" + str(distance*10).zfill(4) + str(velocity).zfill(4) + "\n"
                    elif distance == 0 and velocity > 0 and degrees != 0 and radius == 0:
                        # go pivot
                        serial_command = "@p" + str(degrees).zfill(4) + str(velocity).zfill(4) + "\n"
                    elif distance > 0 and velocity > 0 and degrees == 0 and radius != 0:
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

                elif decoded["command"] == Protocols.commands[11]:
                    self.serial_client.write("@w\n")

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

                elif decoded["command"] == Protocols.commands[14]:
                    degree = decoded["content"]["cameraTilt"]
                    # need to write the degree value to serial
                    serial_command = "@m" + str(degree).zfill(4) + "\n"
                    self.serial_client.write(serial_command)


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
    def __init__(self, ws_client, serial_client, robot_id, server_ip):
        self.ws_client = ws_client
        self.serial_client = serial_client
        self.buff = ''

        # timer data
        self.now = time.time()
        # timer 1 = slow sensor data: every 0.5s
        self.last_slow = self.now
        # timer 2 = charge: every 5s
        self.last_charge = self.now

    def process(self, message):
        if RobotInfo.model_e:
            self.now = time.time()
            if ('#ana: ' in message) or ('#cfs: ' in message) or ('#itt: ' in message):
                self.buff += message
                self.buff += str(int(time.time() * 1000)) + '\n'
                self.ws_client.send(self.buff)

            if self.now - self.last_slow > 0.5:
                self.serial_client.write("@l\n")
		self.buff += message + str(int(time.time() * 1000)) + '\n'
                self.last_slow = self.now
                self.ws_client.send(self.buff)

            if self.now - self.last_charge > 5:
                self.serial_client.write("@c\n")
                self.buff += message + str(int(time.time() * 1000)) + '\n'
                self.last_charge = self.now             
                self.ws_client.send(self.buff)

class HandlerForDataFromSerialModelX():
    def __init__ (self, ws_client, serial_client, robot_id, server_ip):
        self.ws_client = ws_client
        self.serial_client = serial_client
        self.robot_id = robot_id
        self.server_ip = server_ip
        self.count = 0
        self.goTime = False
        self.last = 0        

        print 'Creating input array...'
        self.sensorData = np.zeros((100,15), dtype = 'f') 
        self.index = 0

        print 'Opening classifier...'
        f = open('presenceClassifier0.15.0b2.pkl', 'r') 

        print 'Training...'
        self.isInterested = Predictor(f)

       

    # cfs[1] = BUMPS WHEEL DROPS STATE - bumpleft(1), bumpright(2), wheelcaster(14), wheelleft(15), wheelright(16)
    # cfs[2] = WALL SIGNAL STATE - wallir(13)
    # cfs[3] = CLIFF LEFT SIGNAL STATE - lccliffir(5) or lcliffir(6)
    # cfs[4] = CLIFF FRONT LEFT SIGNAL STATE - lccliffir(5) or lcliffir(6)
    # cfs[5] = CLIFF FRONT RIGHT SIGNAL STATE - rccliffir(8) or rcliffir(9)
    # cfs[6] = CLIFF RIGHT SIGNAL STATE - rccliffir(8) or rcliffir(9)
    #
    # ana[1] = piezoret - photo(7)
    # ana[2] = thermret - rearir1(10)
    # ana[3] = lightret - rearer2(11)
    # ana[4] = ultra range - NA
    #
    # itc[1] = irtherm1 - irtherm1(3)
    # itc[2] = irtherm2 - irtherm2(4)

    # ******************************************************

    # sensorData[0] = bumpleft    - #cfs[1] bit 1
    #           [1] = bumpright   - #cfs[1] bit 0
    #           [2] = irtherm1    - #itc[1]
    #           [3] = irtherm2    - #itc[2]
    #           [4] = lccliffir   - #cfs[4]
    #           [5] = lcliffir    - #cfs[3]
    #           [6] = photo       - #ana[1]
    #           [7] = rccliffir   - #cfs[5]
    #           [8] = rcliffir    - #cfs[6]
    #           [9] = rearir1     - #ana[2]
    #          [10] = rearir2     - #ana[3]
    #          [11] = wallir      - #cfs[2] 
    #          [12] = wheelcaster - #cfs[1] bit 4
    #          [13] = wheelleft   - #cfs[1] bit 3
    #          [14] = wheelright  - #cfs[1] bit 2

    def process(self, message):
        if RobotInfo.model_x:
            print 'Streaming...'
            messArray = message.split()
            if '#cfs' in messArray and self.count == 0:            
                self.sensorData[self.index][0]  = (0 if (int(messArray[1])/2)%2 == 0 else 1)  # separate out from bumpstate
                self.sensorData[self.index][1]  = (0 if (int(messArray[1])/1)%2 == 0 else 1)  # ditto
                self.sensorData[self.index][4]  = int(messArray[4])  
                self.sensorData[self.index][5]  = int(messArray[3])
                self.sensorData[self.index][7]  = int(messArray[5]) 
                self.sensorData[self.index][8]  = int(messArray[6])  
                self.sensorData[self.index][11] = int(messArray[2])
                self.sensorData[self.index][12] = (0 if (int(messArray[1])/16)%2 == 0 else 1) # separate
                self.sensorData[self.index][13] = (0 if (int(messArray[1])/8)%2 == 0 else 1) # separate
                self.sensorData[self.index][14] = (0 if (int(messArray[1])/4)%2 == 0 else 1) # separate
            elif '#itc' in messArray and self.count == 1:
                self.sensorData[self.index][2] = float(messArray[1])
                self.sensorData[self.index][3] = float(messArray[2])
            elif '#ana' in messArray and self.count == 2:
                #self.circleArray[0][self.index] = time.time()
                self.sensorData[self.index][6] = int(messArray[1])
                self.sensorData[self.index][9] = int(messArray[2])
                self.sensorData[self.index][10] = int(messArray[3])

            if ('#cfs' in messArray) or ('#ana' in messArray) or ('#itc' in messArray):
                #print messArray
                #print("count = ", self.count, ", index = ", self.index)
                #print self.sensorData[self.index]
                #print self.sensorData
                self.count += 1
            if self.count > 2:
                if self.goTime == True and (time.time() - self.last > 3):
                    print('****************************************')
                    print(self.isInterested.predict(self.sensorData))
                    print('****************************************')
                    self.last = time.time()
                    np.savetxt('sensorData.txt',self.sensorData,fmt='%.1f')
                self.count = 0
                self.index += 1
            if self.index > 99:
                self.index = 0
                self.goTime = True


class HandlerForDataFromSerialModelY():
    def __init__(self, ws_client, serial_client, robot_id, server_ip):
        self.ws_client = ws_client
        self.serial_client = serial_client
        self.irtherm1_array = []
        self.irtherm2_array = []
        self.irtherm1_min = 74
        self.irtherm1_max = 74

    def process(self,message):
        if RobotInfo.model_y:
            if '#itc ' in message:
#                print(self.irtherm1_array)
#                print(message)
                self.irtherm1_array.append(float(message.split()[1]))
#                print(self.irtherm1_array)
                self.irtherm2_array.append(float(message.split()[2]))

                if len(self.irtherm1_array) > 5:
                    self.irtherm1_min = min(self.irtherm1_array[-5:])
                    self.irtherm1_max = max(self.irtherm1_array[-5:])
                else:
                    self.irtherm1_min = min(self.irtherm1_array)
                    self.irtherm1_max = max(self.irtherm1_array)

#                print(self.irtherm1_min)
#                print(self.irtherm1_max)
                
                if self.irtherm1_max - self.irtherm1_min > 10:
                    self.serial_client.write('@b\n')


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
                print "Got model-T message:", self.buff
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
