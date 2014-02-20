
from websocket import create_connection
import base64
import time
import device_pb2

CMD_GETUID      = 0
CMD_GETUID_RESP = 1
CMD_LOGIN       = 2 
CMD_LOGIN_RESP  = 3
CMD_NEAR        = 4
CMD_NEAR_RESP   = 5

mac2uid = {'1c:3e:84:3a:ac:a9': 1234}

def encodeMsg(cid, uid, cmdid, msg):
    dev = device_pb2.Device()
    dev.cid = cid;
    dev.uid = uid
    dev.msg = msg
    dev.cmdid = cmdid
    xs = dev.SerializeToString()
    # es = base64.encodestring(xs)
    return xs

def decodeMsg(msg):
    # ds = base64.decodestring(msg)
    dev = device_pb2.Device()
    dev.ParseFromString(msg)
    return dev


if __name__ == "__main__":
    import sys
    msg = "This is a message from python"
    if len(sys.argv) >= 2:
        msg = sys.argv[1]
        
    ws = create_connection("ws://localhost:5555/")

    # cmdid=0 : getuid mac
    es = encodeMsg(0, 0, CMD_GETUID, "1c:3e:84:3a:ac:a9")
    ws.send_binary(es)
    # print "Sent %s" % es
    print "Reeiving..."
    result =  ws.recv()
    # print "Received: %s" % result
    xd = decodeMsg(result)
    print "Received uid: '%d'" % xd.uid
    ws.close()

    uid = xd.uid

    # cmdid=2 : login 
    ws = create_connection("ws://localhost:5555/")
    es = encodeMsg(0, uid, CMD_LOGIN, "login")
    ws.send_binary(es)
    # print "Sent %s" % es
    print "Receiving..."
    result =  ws.recv()
    if result is not None:        
        # print "Received: %s" % result
        xd = decodeMsg(result)
        print "Received msg: '%s'" % xd.msg  

    # cmdid=4 : near
    es = encodeMsg(0, uid, CMD_NEAR, "B0702881-A295-A8AB-F734-031A98A512D")
    ws.send_binary(es)
    # print "Sent %s" % es
    print "Receiving..."
    result =  ws.recv()
    if result is not None:        
        # print "Received: %s" % result
        xd = decodeMsg(result)
        print "Received msg: '%s'" % xd.msg  
    time.sleep(60)
    # ws.run_forever()
    ws.close()