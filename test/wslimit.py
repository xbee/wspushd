
from websocket import create_connection
import base64
import device_pb2

MAXUSERS = 17 * 65536

def encodeMsg(cid, uid, msg):
	dev = device_pb2.Device()
	dev.cid = cid;
	dev.devid = uid
	dev.msg = msg
	xs = dev.SerializeToString()
	es = base64.encodestring(xs)
	return es

if __name__ == "__main__":
	import sys
	msg = "This is a message from python"
	if len(sys.argv) >= 2:
		msg = sys.argv[1]
		
	for x in range(MAXUSERS):		
		ws = create_connection("ws://localhost:5555/")
		es = encodeMsg(2, 5681, msg)
		ws.send(es)
		print "Sent"
		print "Reeiving..."
		result =  ws.recv()
		print "Received '%s'" % result
		ws.close()