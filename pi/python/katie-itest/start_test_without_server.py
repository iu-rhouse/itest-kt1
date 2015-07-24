from my_reader_threads import SerialReaderThread, SocketReaderThread, TerminalReaderThread
from my_websocket_client import SLAMWebSocketClient
from my_serial_client import SLAMSerialClient
from my_message_handlers import HandlerForMessageFromWebSocket, MessageHandlerCopyToWebSocket, \
    MessageHandlerCopyToSerial, HandlerForDataFromSerialModelT, HandlerForDataFromSerialModelE, \
    HandlerForActionFromSerial
import sys
import socket
from my_robot_status import RobotInfo
import ujson
from my_protocol import Protocols


if __name__ == "__main__":
    id = int(sys.argv[1])
    if id < 0 or id >= 100:
        print "Please provide a small integer as the robot id"
        sys.exit(0)
    RobotInfo.robot_id = id
    print "Start as Robot", RobotInfo.robot_id, "\n"

    #------------------- Read the Protocols --------------------
    f = open("protocol.json", "r")
    decoded = ujson.load(f)
    Protocols.commands = decoded["commands"]
    f.close()
    print "Protocols loaded:\n", str(Protocols.commands)

    #------------------- Instantiate the IO clients --------------------
    # Connect the Serial device
    serial_client = SLAMSerialClient("/dev/ttyACM0", 115200)

    #------------------- Create the Reader Threads and register Message Handlers to them --------------------
    # Create the thread for reading serial
    serial_reader = SerialReaderThread()
    serial_reader.set_serial_device(serial_client)
    # You can add you own message handler here as many as you want
    # The message handler class need to have at least a process(self, message) method
    serial_reader.register_handler(HandlerForDataFromSerialModelT(None))
    serial_reader.register_handler(HandlerForDataFromSerialModelE(None,
                                                                  RobotInfo.robot_id,
                                                                  RobotInfo.server_ip))
    serial_reader.register_handler(HandlerForActionFromSerial(None,
                                                              RobotInfo.robot_id,
                                                              RobotInfo.server_ip))

    # Create the thread for reading terminal
    terminal_reader = TerminalReaderThread()
    terminal_reader.register_handler(MessageHandlerCopyToSerial(serial_client))
    # terminal_reader.register_handler(MessageHandlerCopyToWebSocket(ws_client))

    #------------------- Start the threads --------------------
    # Start the readers
    terminal_reader.start()
    serial_reader.start()

    # The above reader threads are blocking and non-terminated, so the best way to stop this python
    # program is to hit Ctrl+Z and kill this process
    print "Exiting Main Thread\n"