#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <wsclient/wsclient.h>
#include "device.pb-c.h"

#define MAXBUFLEN 4096

int onclose(wsclient *c) {
	fprintf(stderr, "onclose called: %d\n", c->sockfd);
	return 0;
}

int onerror(wsclient *c, wsclient_error *err) {
	fprintf(stderr, "onerror: (%d): %s\n", err->code, err->str);
	if(err->extra_code) {
		errno = err->extra_code;
		perror("recv");
	}
	return 0;
}

int onmessage(wsclient *c, wsclient_message *msg) {
	fprintf(stderr, "onmessage: (%llu): %s\n", msg->payload_len, msg->payload);
	return 0;
}

int onopen(wsclient *c) {
	fprintf(stderr, "onopen called: %d\n", c->sockfd);

	void *buf, *b64buf;
	unsigned len;
	Device dev = DEVICE__INIT;
	dev.cid = 1; // 1 means : com.wanda.app.wanhui
	dev.uid = 1568;
	dev.msg = "Hello protobuf";
	len = device__get_packed_size(&dev);
	buf = malloc(len);
	device__pack(&dev, buf);

	// int xlen = len * 2 > MAXBUFLEN ? len*2 : MAXBUFLEN;
	// int outlen;
	// b64buf = malloc(xlen+1);
	// memset(b64buf, 0, xlen+1);
	// outlen = b64_encode_string(buf, len, b64buf, xlen);
	// libwsclient_send(c, b64buf);
	libwsclient_send_binary(c, buf, len);
	return 0;
}

int main(int argc, char **argv) {
	//Initialize new wsclient * using specified URI
	wsclient *client = libwsclient_new("ws://localhost:5555/");
	if(!client) {
		fprintf(stderr, "Unable to initialize new WS client.\n");
		exit(1);
	}
	//set callback functions for this client
	libwsclient_onopen(client, &onopen);
	libwsclient_onmessage(client, &onmessage);
	libwsclient_onerror(client, &onerror);
	libwsclient_onclose(client, &onclose);
	//bind helper UNIX socket to "test.sock"
	//One can then use netcat (nc) to send data to the websocket server end on behalf of the client, like so:
	// $> echo -n "some data that will be echoed by echo.websocket.org" | nc -U test.sock
	libwsclient_helper_socket(client, "test.sock");
	//starts run thread.
	libwsclient_run(client);
	//blocks until run thread for client is done.
	libwsclient_finish(client);
	return 0;
}

