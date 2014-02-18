import websocket
import thread
import time

def encodeMsg(cid, uid, msg):
    dev = device_pb2.Device()
    dev.cid = cid;
    dev.devid = uid
    dev.msg = msg
    xs = dev.SerializeToString()
    es = base64.encodestring(xs)
    return es

def on_message(ws, message):
    print message

def on_error(ws, error):
    print error

def on_close(ws):
    print "### closed ###"

def on_open(ws):
    def run(*args):
        for i in range(3):
            time.sleep(1)
            ws.send("Hello %d" % i)
        time.sleep(1)
        ws.close()
        print "thread terminating..."
    thread.start_new_thread(run, ())


if __name__ == "__main__":
    websocket.enableTrace(True)
    ws = websocket.WebSocketApp("ws://localhost:5555/",
                                on_message = on_message,
                                on_error = on_error,
                                on_close = on_close)
    ws.on_open = on_open

    ws.run_forever()