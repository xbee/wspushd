
import websocket
import thread
import time

MAXUSERS = 17 * 65536

def encodeMsg(cid, uid, cmdid, msg):
    dev = device_pb2.Device()
    dev.cid = cid;
    dev.uid = uid
    dev.msg = msg
    dev.cmdid = cmdid
    xs = dev.SerializeToString()
    es = base64.encodestring(xs)
    return es

def decodeMsg(msg):
    ds = base64.decodestring(msg)
    dev = device_pb2.Device()
    dev.ParseFromString(ds)
    return dev

def on_message(ws, message):
    print message

def on_error(ws, error):
    print error

def on_close(ws):
    print "### closed ###"

def on_open(ws):
    def run(*args):
        while True:
          for i in range(MAXUSERS):            
            ws.send("Hello %d" % i)
          time.sleep(1)
        # ws.close()
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