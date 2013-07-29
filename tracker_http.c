#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/* for uint32_t */
#include <stdint.h>

#include <assert.h>

/* for vargs */
#include <stdarg.h>

#include <uv.h>
#include <http_parser.h>

#include "config.h"
#include "url_encoder.h"
#include "bt_tracker_client.h"
#include "bt_tracker_client_private.h"

#define HTTP_PREFIX "http://"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

typedef struct {
    bt_trackerclient_t *tc;

    /*  response so far */
    char* response;

    /*  response length */
    int rlen;
} connection_attempt_t;

#if WIN32
static char* strndup(const char* str, const unsigned int len)
{
    char* new;

    new = malloc(len+1);
    strncpy(new,str,len);
    new[len] = 0;
    return new;
}

static int asprintf(char **resultp, const char *format, ...)
{
    char buf[1024];
    va_list args;

    va_start (args, format);
    vsprintf(buf, format, args);
    *resultp = strdup(buf);
    va_end (args);
    return 1;
}
#endif

/**
 * @return *  1 on success otherwise, 0
 */
int url2host_and_port(
    const char *url,
    char** host_o,
    char** port_o
)
{
    const char* ptr = url, *host, *port;

    host = url;

    port = strpbrk(host, ":/");

    if (!port)
    {
        return 0;
    }

    *host_o = strndup(host,port - host);
    if (*port == ':')
    {
        port += 1;
        *port_o = strndup(port,strpbrk(port, "/") - port);
    }
    else
    {
        *port_o = NULL;
    }

    return 1;
}

static void __build_tracker_request(bt_trackerclient_t* me, const char* url, char **request)
{
    char *info_hash_encoded;

    assert(config_get(me->cfg,"piece_length"));
    assert(config_get(me->cfg,"infohash"));
    assert(config_get(me->cfg,"my_peerid"));
    assert(config_get(me->cfg,"npieces"));

    info_hash_encoded = url_encode(config_get(me->cfg,"infohash"), 20);

    asprintf(request,
             "GET %s"
             "?info_hash=%s"
             "&peer_id=%s"
             "&port=%d"
             "&uploaded=%d"
             "&downloaded=%d"
             "&left=%d"
             "&event=started"
             "&compact=1"
             " HTTP/1.0"
             "\r\n"
             "\r\n\r\n",
             url,
             info_hash_encoded,
             config_get(me->cfg,"my_peerid"),
             config_get_int(me->cfg,"pwp_listen_port"),
             0,
             0,
             config_get_int(me->cfg,"npieces") * config_get_int(me->cfg,"piece_length")
             );

    free(info_hash_encoded);

#if 1 /*  debugging */
    printf("%s\n", *request);
#endif
}

static void __write_cb(uv_write_t* req, int status) {

  if (status)
  {
    uv_err_t err = uv_last_error(uv_default_loop());
    fprintf(stderr, "uv_write error: %s\n", uv_strerror(err));
    assert(0);
  }
}

int __on_httpbody(http_parser* parser, const char *p, size_t len)
{
    printf("body: '%.*s'\n", len, p);
    return 0;
}

static void __read_cb(uv_stream_t* tcp, ssize_t nread, uv_buf_t buf)
{
    connection_attempt_t *ca = tcp->data;

    if (nread >= 0)
    {
//        printf("read some data %.*s\n", buf.len, buf.base);
        ca->response = realloc(ca->response, ca->rlen + nread);
        strncpy(ca->response+ca->rlen, buf.base, nread);
        ca->rlen += nread;
    }
    else
    {
        int nparsed;
        http_parser_settings settings;
        http_parser *parser;

        memset(&settings,0,sizeof(http_parser_settings));

        settings.on_body = __on_httpbody;

        parser = malloc(sizeof(http_parser));
        http_parser_init(parser, HTTP_RESPONSE);
        nparsed = http_parser_execute(parser, &settings, ca->response, ca->rlen);

        if (parser->upgrade)
        {
          /* handle new protocol */
        }
        else if (nparsed != ca->rlen)
        {
          /* Handle error. Usually just close the connection. */
            printf("ERROR: couldn't parse http response: %s\n",
                    http_errno_description(HTTP_PARSER_ERRNO(parser)));
        }

        assert(uv_last_error(uv_default_loop()).code == UV_EOF);
        //uv_close((uv_handle_t*)tcp, close_cb);
    }

    free(buf.base);
}

static uv_buf_t __alloc_cb(uv_handle_t* handle, size_t size)
{
  return uv_buf_init(malloc(size), size);
}

static void __on_connect(uv_connect_t *req, int status)
{
    connection_attempt_t *ca = req->data;
    int r;
    char *request;
    uv_buf_t buf;
    uv_write_t *write_req;

    assert(req->data);
    assert(ca->tc);


    if (status == -1)
    {
        fprintf(stderr, "connect callback error %s\n",
                uv_err_name(uv_last_error(uv_default_loop())));
        return;
    }

    __build_tracker_request(ca->tc, ca->tc->uri, &request);


    buf.base = request;
    buf.len = strlen(request);
    //req->handle = req->data;
    write_req = malloc(sizeof(uv_write_t));
    r = uv_write(write_req, req->handle, &buf, 1, __write_cb);
    r = uv_read_start(req->handle, __alloc_cb, __read_cb);
}

static void __on_resolved(uv_getaddrinfo_t *req, int status, struct addrinfo *res)
{
    char addr[17] = {'\0'};
    uv_tcp_t *socket;
    uv_connect_t *connect_req;

    if (status == -1)
    {
        fprintf(stderr, "getaddrinfo callback error %s\n",
                uv_err_name(uv_last_error(uv_default_loop())));
        return;
    }

    uv_ip4_name((struct sockaddr_in*) res->ai_addr, addr, 16);
    connect_req = malloc(sizeof(uv_connect_t));
    connect_req->data = req->data;
    socket = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), socket);
    uv_tcp_connect(connect_req, socket,
            *(struct sockaddr_in*) res->ai_addr, __on_connect);

    free(req);
    uv_freeaddrinfo(res);
}

/**
 * Connect to URL
 * @return 1 on success; 0 otherwise
 */
int thttp_connect(
        void *me_,
        const char* url)
{
    bt_trackerclient_t *me = me_;
    char *host, *port, *default_port = "80";
    uv_getaddrinfo_t *req;
    struct addrinfo hints;
    connection_attempt_t *ca;
    int r;

    ca = malloc(sizeof(connection_attempt_t));
    ca->tc = me;
    ca->rlen = 0;

    if (0 == url2host_and_port(url,&host,&port))
    {
        assert(0);
    }

    if (!port)
    {
        port = default_port;
    }

    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;
    req = malloc(sizeof(uv_getaddrinfo_t));
    req->data = ca;
    r = uv_getaddrinfo(uv_default_loop(), req, __on_resolved, host, port, &hints);

    if (r)
    {
        fprintf(stderr, "getaddrinfo call error %s\n",
                uv_err_name(uv_last_error(uv_default_loop())));
        return 0;
    }

    return 1;
}

/**
 * Receive this much data on this step.
 * @param me_ Tracker client
 * @param buf Buffer to dispatch events from
 * @param len Length of buffer
 */
void thttp_dispatch_from_buffer(
        void *me_ __attribute__((__unused__)),
        const unsigned char* buf,
        unsigned int len)
{
//    bt_trackerclient_t* me = _me;
//    httptracker_dispatch_from_buffer(me->ht);
//    printf("%.*s\n", len, buf);
}
