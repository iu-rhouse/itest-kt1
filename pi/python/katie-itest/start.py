from reader_threads import SerialReaderThread, SocketReaderThread, TerminalReaderThread
from websocket_client import SLAMWebSocketClient
from serial_client import SLAMSerialClient
from message_handlers import HandlerForMessageFromWebSocket, MessageHandlerCopyToWebSocket, \
    MessageHandlerCopyToSerial, HandlerForDataFromSerialModelT, HandlerForDataFromSerialModelE, \
    HandlerForActionFromSerial
import sys
import socket
from robot_status import RobotInfo
import ujson
from protocol import Protocols


if __name__ == "__main__":
    id = int(sys.argv[1])
    if id < 0 or id >= 100:
        print "Please provide a small integer as the robot id"
        sys.exit(0)
    RobotInfo.robot_id = id
    print "Start as Robot", RobotInfo.robot_id, "\n"

    #------------------- Read the Protocols --------------------
    f = open("/home/pi/katie-drive/protocol.json", "r")
    decoded = ujson.load(f)
    Protocols.commands = decoded["commands"]
    f.close()
    print "Protocols loaded:\n", str(Protocols.commands)

    #------------------- Instantiate the IO clients --------------------
    # Connect the Serial device
    serial_client = SLAMSerialClient("/dev/ttyACM0", 115200)

    # Connect the remote WebSocket server
    ws_client = SLAMWebSocketClient(RobotInfo.server_ip,
                                    [RobotInfo.robot_id,
                                     socket.gethostbyname(socket.gethostname())])  # To-Do: change for Linux

    #------------------- Create the Reader Threads and register Message Handlers to them --------------------
    # Create the thread for reading serial
    serial_reader = SerialReaderThread()
    serial_reader.set_serial_device(serial_client)
    # You can add you own message handler here as many as you want
    # The message handler class need to have at least a process(self, message) method
    serial_reader.register_handler(HandlerForDataFromSerialModelT(ws_client))
    serial_reader.register_handler(HandlerForDataFromSerialModelE(ws_client,
                                                                  RobotInfo.robot_id,
                                                                  RobotInfo.server_ip))
    serial_reader.register_handler(HandlerForActionFromSerial(ws_client,
                                                              RobotInfo.robot_id,
                                                              RobotInfo.server_ip))

    # Create the thread for reading/starting WebSocket client
    socket_reader = SocketReaderThread()
    # You must set ws_client first before register message handlers to this reader
    socket_reader.set_ws_client(ws_client)
    # You can add you own message handler here as many as you want
    # The message handler class need to have at least a process(self, message) method
    socket_reader.register_handler(HandlerForMessageFromWebSocket(ws_client,
                                                                  serial_client,
                                                                  RobotInfo.robot_id,
                                                                  RobotInfo.server_ip))

    # Create the thread for reading terminal
    terminal_reader = TerminalReaderThread()
    terminal_reader.register_handler(MessageHandlerCopyToSerial(serial_client))
    # terminal_reader.register_handler(MessageHandlerCopyToWebSocket(ws_client))

    #------------------- Start the threads --------------------
    # Start the readers
    socket_reader.start()  # Better start this WebSocket reader first before Serial reader
    terminal_reader.start()
    serial_reader.start()

    # The above reader threads are blocking and non-terminated, so the best way to stop this python
    # program is to hit Ctrl+Z and kill this process
    print "Exiting Main Thread\n"
