#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <microhttpd.h>

#define PORT 8888

struct timeval startTime;

const char* errorpage="<html><body>This doesn't seem to be right.</body></html>";

const char *waitpage = "<html><body>Countdown %d</body></html>";

zresponse_t *
zrest_handle_get (zrequest_t *request)
{
    assert (request);
    zresponse_t *response = NULL;

    if (0 == strcmp (request->path, "/")) {
        const char *page = "<html><body>Hello, Dude!</body></html>";
        response = zresponse_new (page, "text/html", 200);
    }
    else
    if (0 == strcmp (request->path, "/countdown")) {
        struct timeval currentTime;
        gettimeofday (&currentTime, 0);
        int countdown = 20 - (currentTime.tv_sec - startTime.tv_sec);
        if (countdown < 0) {
            const char *donepage = "<html><body>Done!</body></html>";
            response = zresponse_new (donepage, "text/html", MHD_HTTP_OK);
        }
        else {
            char *waitstr = malloc (250);
            sprintf (waitstr, waitpage, countdown);
            response = zresponse_new (waitstr, "text/html", MHD_HTTP_OK);
            free (waitstr);
        }
    }
    else {
        const char *bla = "<bla><blub></blub></bla>";
        response = zresponse_new (bla, "text/xml", MHD_HTTP_OK);
    }
    return response;
}

zresponse_t *
zrest_handle_post (zrequest_t *request)
{
    assert (request);
    zresponse_t *response = NULL;

    printf ("%.*s\n", request->data_size, request->data);

    const char *page = "<html><body>Thanks, POST!</body></html>";
    response = zresponse_new (page, "text/html", 201);

    return response;
}

zresponse_t *
zrest_handle_put (zrequest_t *request)
{
    assert (request);
    zresponse_t *response = NULL;

    printf ("%.*s\n", request->data_size, request->data);

    const char *page = "<html><body>Thanks, PUT!</body></html>";
    response = zresponse_new (page, "text/html", 202);

    return response;
}

zresponse_t *
zrest_handle_delete (zrequest_t *request)
{
    assert (request);
    zresponse_t *response = NULL;

    const char *page = "<html><body>Thanks, DELETE!</body></html>";
    response = zresponse_new (page, "text/html", 200);

    return response;
}

int
print_out_key (void *cls,
               enum MHD_ValueKind kind,
               const char *key, const char *value)
{
    printf ("%s: %s\n", key, value);
    return MHD_YES;
}

int
send_page(struct MHD_Connection *con, const char *page, const char *mime, int status_code)
{
    int ret = 0;
    struct MHD_Response *response;
    response = MHD_create_response_from_buffer (strlen (page) , (void *) page, MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE, mime);
    if (!response)
        return MHD_NO;
    ret = MHD_queue_response (con, status_code, response);
    printf ("Send Page: %s, %d\n", mime, status_code);
    MHD_destroy_response (response);
    return ret;
}

void request_completed (void *cls, struct MHD_Connection *con, void **con_cls, enum MHD_RequestTerminationCode toe)
{
    assert (con_cls);
    if (*con_cls) {
        zconnection_t *connection = (zconnection_t *) *con_cls;
        zconnection_destroy (&connection);
    }
    *con_cls = NULL;
}

int
answer_to_connection (void *cls,
                      struct MHD_Connection *con,
                      const char *url,
                      const char *method,
                      const char *version,
                      const char *uploaded_data, size_t *uploaded_data_size,
                      void **con_cls)
{
    printf ("URL: %s\n", url);
    printf ("Method: %s\n", method);
    printf ("Version: %s\n", version);
    printf ("Data: %s\n", uploaded_data);

    MHD_get_connection_values (con, MHD_HEADER_KIND, &print_out_key, NULL);

    const char *content_type = MHD_lookup_connection_value (con, MHD_HEADER_KIND, "Content-Type");
    zconnection_t *connection = NULL;

    //  New http request will have nil con_cls
    if (NULL == *con_cls) {
        zrequest_t *request = zrequest_new (url, method, content_type);
        connection = zconnection_new (request);
        *con_cls = (void *) connection;

        if (0 == strcmp (method, MHD_HTTP_METHOD_POST) || 0 == strcmp (method, MHD_HTTP_METHOD_PUT)) {
            if (strcmp ("application/xml", content_type)) {
                //  POST and PUT data is provided on second call to this method,
                //  thus return MHD_YES which indicates that the request is not yet finished.
                return MHD_YES;
            }
        }
    }
    //  Get connection for an already running request
    else {
        connection = (zconnection_t *) *con_cls;
    }

    //  Start request processing
    if (0 == strcmp (method, MHD_HTTP_METHOD_GET)) {
        connection->response = zrest_handle_get (connection->request);
        zresponse_t *response = connection->response;
        return send_page (con, response->data, response->mime, response->status_code);
    }
    else
    if (0 == strcmp (method, MHD_HTTP_METHOD_POST)) {
        if (*uploaded_data_size) {
            zrequest_set_data (connection->request, uploaded_data, *uploaded_data_size);
            connection->response = zrest_handle_post (connection->request);
            *uploaded_data_size = 0;
            return MHD_YES;
        }
        else {
            zresponse_t *response = connection->response;
            return send_page (con, response->data, response->mime, response->status_code);
        }
    }
    else
    if (0 == strcmp (method, MHD_HTTP_METHOD_PUT)) {
        if (*uploaded_data_size) {
            zrequest_set_data (connection->request, uploaded_data, *uploaded_data_size);
            connection->response = zrest_handle_put (connection->request);
            *uploaded_data_size = 0;
            return MHD_YES;
        }
        else {
            zresponse_t *response = connection->response;
            return send_page (con, response->data, response->mime, response->status_code);
        }
    }
    else
    if (0 == strcmp (method, MHD_HTTP_METHOD_DELETE)) {
        connection->response = zrest_handle_delete (connection->request);
        zresponse_t *response = connection->response;
        return send_page (con, response->data, response->mime, response->status_code);
    }

    return send_page (con, errorpage, "text/html", MHD_HTTP_BAD_REQUEST);
}

static int on_client_connect (void *cls,
                              const struct sockaddr *addr,
                              socklen_t addrlen)
{
    struct sockaddr_in *client_addr = (struct sockaddr_in *) addr;
    printf ("Client-Addr: %s:%d\n", inet_ntoa (client_addr->sin_addr), ntohs (client_addr->sin_port));
    return MHD_YES;
}

int
main ()
{
    gettimeofday(&startTime, 0);

    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT, &on_client_connect, NULL,
                               &answer_to_connection, NULL,
                               MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
                               MHD_OPTION_END);

    if (NULL == daemon)
        return 1;

    getchar ();

    MHD_stop_daemon (daemon);
    return 0;
}
