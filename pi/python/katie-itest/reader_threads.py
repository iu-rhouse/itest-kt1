import threading
from datetime import datetime


class SerialReaderThread(threading.Thread):
    def set_serial_device(self, serial_client):
        self.serial_client = serial_client

    def register_handler(self, handler):
        if not hasattr(self, "message_handlers"):
            self.message_handlers = []
        self.message_handlers.append(handler)

    def run(self):
        print "Serial reading started\n"
        while True:
            line = self.serial_client.read_line()
            if line is not None:
                print str(datetime.now()), "     From Serial:\n" + line
                # Pass the message to registered handlers
                for handler in self.message_handlers:
                    # Although each message handler should catch their own exceptions,
                    # but we add a catch here in case some un-caught exceptions would terminate this loop
                    try:
                        handler.process(line)
                    except Exception as e:
                        print "Exception in SerialReaderThread run(): ", e


class SocketReaderThread(threading.Thread):
    def register_handler(self, handler):
        if hasattr(self, "ws_client"):
            self.ws_client.message_handlers.append(handler)
        else:
            raise Exception("Error: you must set ws_client first!")

    def set_ws_client(self, ws_client):
        self.ws_client = ws_client

    def run(self):
        # Start the WebSocket client
        self.ws_client.start()


class TerminalReaderThread(threading.Thread):
    def register_handler(self, handler):
        if not hasattr(self, "message_handlers"):
            self.message_handlers = []
        self.message_handlers.append(handler)

    def run(self):
        print "Terminal reading started\n"
        while True:
            line = raw_input()
            # Pass the message to registered handlers
            for handler in self.message_handlers:
                try:
                    handler.process(line)
                except Exception as e:
                    print "Exception in TerminalReaderThread run(): ", e
