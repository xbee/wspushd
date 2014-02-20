
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


if __name__ == "__main__":
    import sys
    msg = "This is a message from python"
    if len(sys.argv) >= 2:
        msg = sys.argv[1]
        
    conns = []
    for x in range(100000):
        ws = create_connection("ws://localhost:5555/")
        conns.append(ws)
        time.sleep(0.001)
    while True:        
        for x in conns:
            x.send("ping %s" % str(time.time()))
        time.sleep(1)

    