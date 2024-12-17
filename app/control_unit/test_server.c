#include <libwebsockets.h>
#include <string.h>
#include <unistd.h>

static int callback_client(struct lws *wsi, enum lws_callback_reasons reason,
                           void *user, void *in, size_t len) {
    static int command_count = 0;
    const char *commands[] = {"up", "up", "right", "left", "down", "down"};
    static int sent = 0;

    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            printf("Connected to server\n");
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            if (command_count < 6) {
                char buffer[64];
                sprintf(buffer, "%s", commands[command_count]);
                unsigned char msg[LWS_SEND_BUFFER_PRE_PADDING + 64 + LWS_SEND_BUFFER_POST_PADDING];
                memcpy(&msg[LWS_SEND_BUFFER_PRE_PADDING], buffer, strlen(buffer));
                lws_write(wsi, &msg[LWS_SEND_BUFFER_PRE_PADDING], strlen(buffer), LWS_WRITE_TEXT);
                printf("Sent command: %s\n", buffer);
                command_count++;
                sleep(2);
            } else {
                lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, NULL, 0);
            }
            break;

        default:
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    { "motor-control-protocol", callback_client, 0, 128 },
    { NULL, NULL, 0, 0 }
};

int main() {
    struct lws_context_creation_info info = {0};
    struct lws_client_connect_info ccinfo = {0};
    struct lws_context *context;
    struct lws *wsi;

    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;

    context = lws_create_context(&info);
    ccinfo.context = context;
    ccinfo.address = "localhost";
    ccinfo.port = 8000;
    ccinfo.path = "/";
    ccinfo.protocol = protocols[0].name;

    wsi = lws_client_connect_via_info(&ccinfo);

    while (1) {
        lws_service(context, 1000);
    }

    lws_context_destroy(context);
    return 0;
}

