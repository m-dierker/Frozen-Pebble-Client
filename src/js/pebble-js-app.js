function ajax(url, method, data, callback) {
    var req = new XMLHttpRequest();
    req.open(method, url, true);
    req.onload = function(e) {
        if (req.readyState == 4 && req.status == 200) {
            if (req.status == 200) {
                callback(JSON.parse(req.responseText));
            } else {
                console.log("HTTP Error: " + JSON.stringify(req));
            }
        }
    };
    req.send(data);
}

function getReq(url, callback) {
    return ajax(url, 'GET', null, callback);
}

function patchReq(url, data, callback) {
    return ajax(url, 'PATCH', JSON.stringify(data), callback);
}

function firePatch(path, data, callback) {
    return patchReq('https://frozenpebble.firebaseio.com' + path, data, callback);
}

function sendFirebaseCommand(cmd) {
    firePatch('/users/mdierker.json', {'cmd':cmd}, function() {
        console.log("Command request made!");
    });
}

/**
 * Handle a command with a given ID. The ID should be specified by the Pebble,
 * and is defined by adding a case here.
 * @param  {int} command_id The ID of the command to execute
 */
function handleCommand(command_id, payload) {
    switch (command_id) {
        case 1:
            sendFirebaseCommand('playpause');
            break;
        case 2:
            sendFirebaseCommand('next_track');
            break;
        case 3:
            sendFirebaseCommand('prev_track');
            break;
        case 4:
            sendFirebaseCommand('rock_out_to_frozen');
            break;
    }
}

function wsLog(log_item, called_from) {
    console.log("WS Log from " + called_from + ": " + log_item);
}

var pendingWsMessageQueue = [];
function sendWsMessage(msg) {
    if (!websocket) {
        setupWebsocket();

        // We have to queue this message up to send because the socket isn't open yet
        pendingWsMessageQueue.push(msg);
    } else {
        websocket.send(msg);
    }
}

function shortenProperty(obj, property, num_chars) {
    if (property in obj && obj[property].length > num_chars) {
        obj[property] = obj[property].substring(0, num_chars);
    }
}

function updateTrack(track) {
    shortenProperty(track, 'name', 40);
    shortenProperty(track, 'artist', 20);

    var trackDataToSend = {
        0: 1, // Track Update
        1: track['name'],
        2: track['artist'],
        3: track['duration']
    };
    console.log("Data to send: " + JSON.stringify(trackDataToSend));
    Pebble.sendAppMessage(trackDataToSend);
}

function updatePlayPauseStatus(isPlaying) {
    var data = {
        0: 2, // Play Pause Update
        1: isPlaying
    };

    Pebble.sendAppMessage(data);
}

function onWsMsg(evt) {
    var data = evt.data;
    var msg = JSON.parse(data);
    wsLog(JSON.stringify(msg), 'onMessage');

    switch(msg.cmd) {
        case 'track_info':
            updateTrack(msg.track);
            break;
        case 'playpause_info':
            updatePlayPauseStatus(msg.desktop_status);
            break;
    }
}

function onWsOpen(evt) {
    wsLog(evt, 'onOpen');

    while (pendingWsMessageQueue.length > 0) {
        websocket.send(pendingWsMessageQueue.pop());
    }
}

function onWsClose(evt) {
    wsLog(evt, 'onClose');
    websocket = null;
}

function onWsError(evt) {
    wsLog(evt, 'onError');
    websocket = null;
}

var websocket;
var websocketKeepAliveInterval;
function setupWebsocket() {
    var wsUrl = "ws://158.130.162.246:58484/";
    websocket = new WebSocket(wsUrl);
    websocket.onopen = onWsOpen;
    websocket.onclose = onWsClose;
    websocket.onmessage = onWsMsg;
    websocket.onerror = onWsError;

    if (websocketKeepAliveInterval) {
        clearInterval(websocketKeepAliveInterval);
    }

    websocketKeepAliveInterval = setInterval(keepWebsocketAlive, 5000);
}

function keepWebsocketAlive() {
    if (!websocket) {
        console.log("Reviving Websocket");
        setupWebsocket();
    }
}


Pebble.addEventListener("appmessage", function(e) {
    console.log("--> " + JSON.stringify(e));
    if (0 in e.payload) {
        handleCommand(e.payload[0], e.payload);
    }
});

Pebble.addEventListener("ready", function(e) {
    console.log("Frozen Pebble launched");
    setupWebsocket();
});
