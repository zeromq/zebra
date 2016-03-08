#include <zebra.h>

int main()
{
    //  Create and start web server
    zactor_t *zeb_microhttpd = zactor_new (zeb_microhttpd_actor, NULL);

    zstr_send (zeb_microhttpd, "START");
    zsock_wait (zeb_microhttpd);             //  Wait until actor started

    zstr_sendx (zeb_microhttpd, "ENDPOINT", "inproc://http_broker", NULL);
    zsock_wait (zeb_microhttpd);             //  Wait until endpoint configured

    //  Create and start broker
    zactor_t *broker = zactor_new (zeb_broker, "broker");
    zstr_sendx (broker, "BIND", "inproc://http_broker", NULL);

    //  Create and connect handler
    zeb_client_t *handler = zeb_client_new ();
    zeb_client_connect (handler, "inproc://http_broker",  1000, "handler");
    zeb_client_set_handler (handler, "GET", "/hello");

    printf("Enter http://localhost:8888/hello into your browser!\n");
    //  Receive Request
    zmsg_t *request = zeb_client_recv (handler);
    xrap_msg_t *xrap_msg = xrap_msg_decode (&request);
    xrap_msg_destroy (&xrap_msg);

    //  Send Response
    xrap_msg = xrap_msg_new (XRAP_MSG_GET_OK);
    xrap_msg_set_status_code (xrap_msg, 200);
    xrap_msg_set_content_type (xrap_msg, "text/plain");
    xrap_msg_set_content_body (xrap_msg, "Hello World!");
    zmsg_t *response = xrap_msg_encode (&xrap_msg);
    zeb_client_deliver (handler, zeb_client_sender (handler), &response);
    zuuid_t *sender = zeb_client_sender (handler);
    zuuid_destroy (&sender);

    //  Cleanup
    zeb_client_destroy (&handler);
    zactor_destroy (&broker);
    zstr_send (zeb_microhttpd, "STOP");
    zsock_wait (zeb_microhttpd);             //  Wait until actor stopped
    zactor_destroy (&zeb_microhttpd);

    printf("Done!\n");
    return 0;
}
