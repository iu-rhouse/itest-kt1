var WebSocketServer = require("ws").Server;
var readline = require("readline");

var wss = new WebSocketServer({
    port: 8080});

wss.on("connection", function (ws) {
    console.log("new WebSocket client");

    ws.on("message", function (message) {
        console.log("Server receive: %s", message);
    });

    ws.on("close", function (reason) {
        console.log("client disconnected due to: " + reason);
    });
});

function broadcast(data) {
    console.log("broadcast: " + data);
    for (var i in wss.clients) {
        wss.clients[i].send(data);
    }
};

var rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});

rl.on("line", function (cmd) {
    broadcast(cmd);
});

console.log("WebSocket Server is running at: 8080");
