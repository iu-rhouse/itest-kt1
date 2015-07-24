import websocket
import ujson

websocket.enableTrace(False)


class SLAMWebSocketClient():
    def __init__(self, server_ip, robot_info):
        self.message_handlers = []
        self.robot_info = robot_info
        self.wb_app = websocket.WebSocketApp("ws://" + server_ip + ":8080/SLAM/datalog",
                                             on_open=self.on_open,
                                             on_message=self.on_message,
                                             on_error=self.on_error,
                                             on_close=self.on_close)

    def register_handler(self, handler):
        self.message_handlers.append(handler)

    def start(self):
        self.wb_app.run_forever()

    def send(self, msg):
        self.wb_app.send(msg)

    def on_message(self, ws, message):
        try:
            # Pass the message to registered handlers
            for handler in self.message_handlers:
                handler.process(message)
        except Exception as e:
            print "Exception in SLAMWebSocketClient on_message(): ", e

    def on_error(self, ws, error):
        self.wb_app = None
        print error

    def on_close(self, ws):
        self.wb_app = None
        print "WebSocket closed\n"

    def on_open(self, ws):
        print "WebSocket reading started\n"
        self.wb_app.send(ujson.dumps(ControlMessage("topic.new", self.robot_info)))
        print "Robot id sent to server\n"


class ControlMessage():
    def __init__(self, topic, content):
        self.topic = topic
        self.content = content

