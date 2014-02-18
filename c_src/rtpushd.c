
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


#include <stdlib.h>
#include <websock/websock.h>

#include "erl_interface.h"
#include "ei.h"
#include "device.pb-c.h"

#define DEF_PORT "5000"

#define MAXBUFLEN 4096
#define BUFSIZE 1024
#define MAXUSERS (17*65536) // C1024K

// List of current websocket requests by uid:
struct libwebsock_client_state * clients[MAXUSERS+1];
// Memory to store uids passed to the cleanup callback:
int slots[MAXUSERS+1];


char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}

/*

Define message callback with same prototype as below.
Accepts pointer to libwebsock_client_state structure and pointer to libwebsock_message structure.
libwebsock_client_state has int sockfd, struct sockaddr_storage *sa
libwebsock_message has unsigned int opcode, unsigned long long payload_len, and char *payload

You probably shouldn't modify any of the data contained in the structures passed to the callback.  It will probably cause bad things to happen.
You can, of course, make copies of the data contained therein.

Here is the default receive_callback as an example:
(libwebsock_send_text accepts libwebsock_client_state * and character array)

int some_callback_name(libwebsock_client_state *state, libwebsock_message *msg) {
  libwebsock_send_text(state, msg->payload);
  return 0;
}

This callback just provides echoing messages back to the websocket client.

You would register this callback via:

ctx->onopen = some_callback_name;


*/


//basic onmessage callback, prints some information about this particular message
//then echos back to the client.
int
onmessage(libwebsock_client_state *state, libwebsock_message *msg)
{
    fprintf(stderr, "Received message from client: %d\n", state->sockfd);
    fprintf(stderr, "Uri: %s\n", state->uri);
    fprintf(stderr, "Message opcode: %d\n", msg->opcode);
    fprintf(stderr, "Payload Length: %llu\n", msg->payload_len);
    fprintf(stderr, "Payload: %s\n", msg->payload);

    char buf[MAXBUFLEN] = {0};
    int xlen = b64_decode_string(trim(msg->payload), buf, sizeof buf);

    printf("base64 decoded len: %d\n", xlen);
    // printMemBlock(buf, xlen);

    Device *dev = NULL;
    dev = device__unpack(NULL, xlen, buf);
    if (NULL == dev) {
        dev = device__unpack(NULL, xlen-1, buf);
        if (NULL == dev) {
            dev = device__unpack(NULL, xlen+1, buf);
            if (NULL == dev) {
                fprintf(stderr, "error unpacking incoming message\n");
                exit(1);    
            }
            
        }
        
    }
    printf("cid: %zu, devid: %zu \n", dev->cid, dev->devid);
    printf("message: %s\n", dev->msg);
    
    // get uid msg: com.wanda.app.wanhui/getuid/1c:3e:84:3a:ac:a9
    // msg: com.wanda.app.wanhui/1246/login

    // parse it 
    // printf("%s\n", headers);

    // strchr()
    // for (tok = strtok(msg->payload, "/"); tok != NULL; tok = strtok(NULL, "/")) {

    //     if (strstr(tok, "Sec-WebSocket-Key: ") != NULL) {
    //       key = (char *) lws_malloc(strlen(tok));
    //       strncpy(key, tok + strlen("Sec-WebSocket-Key: "), strlen(tok));
    //       break;
    //     }
    // }

    // libwebsock_send_text(state, dev->msg);
    return 0;
}

int
onopen(libwebsock_client_state *state)
{
    fprintf(stderr, "onopen: %d\n", state->sockfd);
    libwebsock_send_text(state, "Welcome");
    // add this client to client_table

    return 0;
}

int
onclose(libwebsock_client_state *state)
{
    fprintf(stderr, "onclose: %d\n", state->sockfd);
    return 0;
}

// int my_listen(int port)
// {
//     int listen_fd;
//     struct sockaddr_in addr;
//     int on = 1;

//     if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//         return (-1);

//     setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

//     memset((void *) &addr, 0, (size_t) sizeof(addr));
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(port);
//     addr.sin_addr.s_addr = htonl(INADDR_ANY);

//     if (bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
//         return (-1);

//     listen(listen_fd, 5);
//     return listen_fd;
// }

// int main(int argc, char **argv)
// {
//     struct in_addr addr;                     /* 32-bit IP number of host */
//     int port;                                /* Listen port number */
//     int listen;                              /* Listen socket */
//     int fd;                                  /* fd to Erlang node */
//     ErlConnect conn;                         /* Connection data */

//     int loop = 1;                            /* Loop flag */
//     int got;                                 /* Result of receive */
//     unsigned char buf[BUFSIZE];              /* Buffer for incoming message */
//     ErlMessage emsg;                         /* Incoming message */

//     ETERM *fromp, *tuplep, *fnp, *argp, *resp;
//     int res;

//     port = atoi(argv[1]);

//     erl_init(NULL, 0);

//     addr.s_addr = inet_addr("134.138.177.89");
//     if (erl_connect_xinit("idril", "cnode", "cnode@idril.du.uab.ericsson.se",
//                           &addr, "secretcookie", 0) == -1)
//         erl_err_quit("erl_connect_xinit");

//     /* Make a listen socket */
//     if ((listen = my_listen(port)) <= 0)
//         erl_err_quit("my_listen");

//     if (erl_publish(port) == -1)
//         erl_err_quit("erl_publish");

//     if ((fd = erl_accept(listen, &conn)) == ERL_ERROR)
//         erl_err_quit("erl_accept");
//     fprintf(stderr, "Connected to %s\n\r", conn.nodename);

//     while (loop)
//     {

//         got = erl_receive_msg(fd, buf, BUFSIZE, &emsg);
//         if (got == ERL_TICK)
//         {
//             /* ignore */
//         }
//         else if (got == ERL_ERROR)
//         {
//             loop = 0;
//         }
//         else
//         {

//             if (emsg.type == ERL_REG_SEND)
//             {
//                 fromp = erl_element(2, emsg.msg);
//                 tuplep = erl_element(3, emsg.msg);
//                 fnp = erl_element(1, tuplep);
//                 argp = erl_element(2, tuplep);

//                 if (strncmp(ERL_ATOM_PTR(fnp), "foo", 3) == 0)
//                 {
//                     res = foo(ERL_INT_VALUE(argp));
//                 }
//                 else if (strncmp(ERL_ATOM_PTR(fnp), "bar", 3) == 0)
//                 {
//                     res = bar(ERL_INT_VALUE(argp));
//                 }

//                 resp = erl_format("{cnode, ~i}", res);
//                 erl_send(fd, fromp, resp);

//                 erl_free_term(emsg.from); erl_free_term(emsg.msg);
//                 erl_free_term(fromp); erl_free_term(tuplep);
//                 erl_free_term(fnp); erl_free_term(argp);
//                 erl_free_term(resp);
//             }
//         }
//     }
// }

int
main(int argc, char *argv[])
{
    char *port = DEF_PORT;
    libwebsock_context *ctx = NULL;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port to listen on>\n\nNote: You must be root to bind to port below 1024\n", argv[0]);
        exit(0);
    }
    else
    {
        port = argv[1];
    }

    ctx = libwebsock_init();
    if (ctx == NULL)
    {
        fprintf(stderr, "Error during libwebsock_init.\n");
        exit(1);
    }

    libwebsock_bind(ctx, "0.0.0.0", argv[1]);
    fprintf(stderr, "rtpushd listening on port %s\n", argv[1]);
    ctx->onmessage = onmessage;
    ctx->onopen = onopen;
    ctx->onclose = onclose;
    libwebsock_wait(ctx);
    //perform any cleanup here.
    fprintf(stderr, "Exiting.\n");
    return 0;
}