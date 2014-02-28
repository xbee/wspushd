
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <websock/websock.h>
#include <pthread.h>

#include "erl_interface.h"
#include "ei.h"
#include "device.pb-c.h"

#define DEF_PORT "5000"

#define MAXBUFLEN 4096
#define BUFSIZE 1024
#define MAXUSERS (17*65536) // C1024K

#define CMD_GETUID      0
#define CMD_GETUID_RESP 1
#define CMD_LOGIN       2 
#define CMD_LOGIN_RESP  3
#define CMD_NEAR        4
#define CMD_NEAR_RESP   5
#define CMD_PING        6
#define CMD_PING_RESP   7

// getuid : uid 
// login : 
// cmdid(1) mac 
// login-response : 
// cmdid(2) uid
// cmdid = near(4) blueid: string 
// cmdid = ad(5) msg: "Welcome to rtmap f3"

// logout : cmdid(3)

unsigned g_UID = 0;

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

int send_msg(libwebsock_client_state *state, 
             unsigned cid, 
             unsigned uid, 
             unsigned cmdid, 
             char * msg) 
{
    printf("Enter send_msg \n");

    Device dev = WSPUSHD__DEVICE__INIT;
    dev.cid = cid;
    dev.uid = uid;
    dev.cmdid = cmdid;
    dev.msg = msg;
    // strncpy(dev.msg, msg, strlen(msg));

    int len=0; 
    void *buf;
    len = wspushd__device__get_packed_size(&dev);
    buf = malloc(len);
    wspushd__device__pack(&dev, buf);

    printf("device__pack ok\n");

    libwebsock_send_binary(state, buf, len);

    free(buf);
    // free(b64buf);

    printf("Exiting send_msg \n");
    return 0;
}

//basic onmessage callback, prints some information about this particular message
//then echos back to the client.
int
onmessage(libwebsock_client_state *state, libwebsock_message *msg)
{
    printf("*********  fd: %d **************\n", state->sockfd);
    printf("sizeof state: %d\n", sizeof(libwebsock_client_state) );
    fprintf(stderr, "Received message from client: %d\n", state->sockfd);
    fprintf(stderr, "Uri: %s\n", state->uri);
    fprintf(stderr, "Message opcode: %d\n", msg->opcode);
    fprintf(stderr, "Payload Length: %llu\n", msg->payload_len);
    // fprintf(stderr, "Payload: %s\n", msg->payload);

    // char buf[MAXBUFLEN] = {0};
    // int xlen = b64_decode_string(trim(msg->payload), buf, sizeof buf);

    // printf("base64 decoded len: %d\n", xlen);
    // printMemBlock(buf, xlen);

    if (msg->opcode == 1) {
        if (strcmp(msg->payload, "ping") == 0) {
            libwebsock_send_text(state, "pong");    
        } else {
            printf("Payload Length: %llu\n", msg->payload_len);
            libwebsock_send_text(state, msg->payload);    
        }
        
        return 0;
    }

    if (strcmp(state->uri, "/ss") == 0) {
        printf("Received ss, Payload Length: %llu\n", msg->payload_len);
        FILE *fp;
        fp=fopen("recv.jpg", "wb");
        if (fp == NULL)
        {
            printf("Error opening file!\n");
            exit(1);
        }
        fwrite(msg->payload, 1, msg->payload_len, fp);
        fclose(fp);
        libwebsock_send_text(state, "received ss");     
        
        return 0;
    }

    Device *dev = NULL;
    dev = wspushd__device__unpack(NULL, msg->payload_len, msg->payload);
    if (NULL == dev) {
        // dev = device__unpack(NULL, xlen-1, buf);
        // if (NULL == dev) {
        //     dev = device__unpack(NULL, xlen+1, buf);
        //     if (NULL == dev) {
        //         fprintf(stderr, "error unpacking incoming message\n");
        //         exit(1);    
        //     }
            
        // }

        fprintf(stderr, "error unpacking incoming message\n");
        exit(1);        
    }
    printf("cid: %zu, uid: %zu, cmdid: %zu\n", dev->cid, dev->uid, dev->cmdid);
    printf("message: %s\n", dev->msg);

    if (dev->cmdid == CMD_GETUID) {

        printf("This is a gutuid cmd\n");

        if (strcmp(dev->msg, "1c:3e:84:3a:ac:a9") == 0) {
            printf("Get test uid from mac\n");

            int uid = 1234;
            send_msg(state, 0, uid, CMD_GETUID_RESP, "getuid\0");
            g_UID = uid;
            libwebsock_close(state);
            return 0;
        }        
        
        printf("Can not get uid from mac\n");
    } else if (dev->cmdid == CMD_LOGIN) {

        printf("Received a login cmd\n");
        send_msg(state, 0, g_UID, CMD_LOGIN_RESP, "login successful"); 

    } else if (dev->cmdid == CMD_NEAR) {

        if (clients[dev->uid]) {
            printf("Received a near cmd\n");
            printf("Near ibeacon id: %s\n", dev->msg);

            libwebsock_client_state * ts = clients[dev->uid];
            // if not equals , that means old connection have closed
            assert(ts->sockfd == state->sockfd);
            assert(ts == state);

            send_msg(clients[dev->uid], 0, g_UID, CMD_NEAR_RESP, "Ad: lastest offers!"); 
        }

        
    } else if (dev->cmdid == CMD_PING) {

        if (clients[dev->uid]) {
            printf("Received a ping cmd\n");
            send_msg(clients[dev->uid], 0, g_UID, CMD_PING_RESP, "Keepalive!"); 
        }
        
    }

    clients[dev->uid] = state;

    // Free the unpacked message
    wspushd__device__free_unpacked(dev, NULL);
    
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
    // libwebsock_send_text(state, "Welcome");
    // add this client to client_table

    return 0;
}

int
onclose(libwebsock_client_state *state)
{
    fprintf(stderr, "onclose: %d\n", state->sockfd);
    return 0;
}

// runs in a thread - the erlang c-node stuff
// expects msgs like {uid, msg} and sends a a 'msg' chunk to uid if connected
void cnode_run()
{
    int fd;                                  /* fd to Erlang node */
    int got;                                 /* Result of receive */
    unsigned char buf[BUFSIZE];              /* Buffer for incoming message */
    ErlMessage emsg;                         /* Incoming message */

    ETERM *uid, *msg;

    erl_init(NULL, 0);

    if (erl_connect_init(1, "secretcookie", 0) == -1)
        erl_err_quit("erl_connect_init");

    if ((fd = erl_connect("httpdmaster@localhost")) < 0)
        erl_err_quit("erl_connect");

    fprintf(stderr, "Connected to httpdmaster@localhost\n\r");

    struct evbuffer *evbuf;

    while (1) {
        got = erl_receive_msg(fd, buf, BUFSIZE, &emsg);
        if (got == ERL_TICK) {
            continue;
        } else if (got == ERL_ERROR) {
            fprintf(stderr, "ERL_ERROR from erl_receive_msg.\n");
            break;
        } else {
            if (emsg.type == ERL_REG_SEND) {
                // get uid and body data from eg: {123, <<"Hello">>}
                uid = erl_element(1, emsg.msg);
                msg = erl_element(2, emsg.msg);
                int userid = ERL_INT_VALUE(uid);
                char *body = (char *) ERL_BIN_PTR(msg);
                int body_len = ERL_BIN_SIZE(msg);
                // Is this userid connected?
                if(clients[userid]){
                    fprintf(stderr, "Sending %d bytes to uid %d\n", body_len, userid);                
                    // evbuf = evbuffer_new();
                    // evbuffer_add(evbuf, (const void*)body, (size_t) body_len);
                    // evhttp_send_reply_chunk(clients[userid], evbuf);
                    // evbuffer_free(evbuf);
                }else{
                    fprintf(stderr, "Discarding %d bytes to uid %d - user not connected\n", 
                            body_len, userid);                
                    // noop
                }
                erl_free_term(emsg.msg);
                erl_free_term(uid); 
                erl_free_term(msg);
            }
        }
    }
    // if we got here, erlang connection died.
    // this thread is supposed to run forever
    // TODO - gracefully handle failure / reconnect / etc
    pthread_exit(0);
}

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

    // Launch the thread that runs the cnode:
    // pthread_attr_t tattr;
    // pthread_t helper;
    // int status;
    // pthread_create(&helper, NULL, cnode_run, NULL);

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
