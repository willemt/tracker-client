
/**
 * @file
 * @brief Manage connection with tracker
 * @author  Willem Thiart himself@willemthiart.com
 * @version 0.1
 *
 * @section LICENSE
 * Copyright (c) 2011, Willem-Hendrik Thiart
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The names of its contributors may not be used to endorse or promote
      products derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL WILLEM-HENDRIK THIART BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <string.h>
#include <stdarg.h>

#include "url_encoder.h"
#include "bt_tracker_client.h"
#include "bt_tracker_client_private.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <time.h>

/* for vargs */
#include <stdarg.h>

#define HTTP_PREFIX "http://"

#if WIN32
static char* strndup(const char* str, const unsigned int len)
{
    char* new;

    new = malloc(len+1);
    strncpy(new,str,len);
    return new;
}
#endif


/**
 * Set a key-value option
 *
 * Options include:
 * pwp_listen_port - The port the client will listen on for PWP connections
 * tracker_interval - Length in seconds that client will communicate with the tracker
 * tracker_url - Set tracker's url
 * infohash - Set the client's infohash
 *
 * @param key The option
 * @param val The value
 * @param val_len Value's length
 *
 */
int bt_trackerclient_set_opt(
                      void *bto,
                      const char *key,
                      const char *val,
                      const int val_len __attribute__((__unused__)))
{
    bt_trackerclient_t *me = bto;

    if (!strcmp(key, "pwp_listen_port"))
    {
        me->cfg.pwp_listen_port = atoi(val);
        return 1;
    }
#if 0
    else if (!strcmp(key, "tracker_interval"))
    {
        me->interval = atoi(val);
        return 1;
    }
    else if (!strcmp(key, "tracker_url"))
    {
        me->cfg.tracker_url = calloc(1, sizeof(char) * (val_len + 1));
        strncpy(me->cfg.tracker_url, val, val_len);
        return 1;
    }
#endif
    else if (!strcmp(key, "infohash"))
    {
        me->cfg.info_hash = strdup(val);
        return 1;
    }
    else if (!strcmp(key, "tracker_backup"))
    {
        return 1;
    }

    return 255;
}

int bt_trackerclient_set_opt_int(void *bto, const char *key, const int val)
{
    bt_trackerclient_t *me = bto;

#if 0
    if (!strcmp(key, "pwp_listen_port"))
    {
        me->cfg.pwp_listen_port = val;
        return 1;
    }
    else
#endif
    if (!strcmp(key, "tracker_interval"))
    {
        me->interval = val;
        return 1;
    }

    return 255;
}


/**
 * Get a key-value option
 *
 * @return string of value if applicable, otherwise NULL
 */
#if 0
char *bt_trackerclient_get_opt_string(
        void *bto,
        const char *key)
{
    bt_trackerclient_t *me = bto;

    if (!strcmp(key, "tracker_url"))
    {
        return me->cfg.tracker_url;
    }
    else if (!strcmp(key, "infohash"))
    {
        return me->cfg.info_hash;
    }
    else
    {
        return NULL;
    }
    return NULL;
}
#endif

/**
 * Get a key-value option
 *
 * @return integer of value if applicable, otherwise -1
 */
int bt_trackerclient_get_opt_int(void *bto, const char *key)
{
    bt_trackerclient_t *me = bto;

    if (!strcmp(key, "pwp_listen_port"))
    {
        return me->cfg.pwp_listen_port;
    }
    else if (!strcmp(key, "tracker_interval"))
    {
        return me->interval;
    }
    else
    {
        return -1;
    }
}

/**
 * Initiliase the tracker client
 *
 * @return tracker client on sucess; otherwise NULL
 *
 */
void *bt_trackerclient_new(
    bt_trackerclient_funcs_t *funcs
    )
{
    bt_trackerclient_t *me;

    me = calloc(1, sizeof(bt_trackerclient_t));
    if (funcs)
        memcpy(&me->funcs,funcs,sizeof(bt_trackerclient_funcs_t));
    return me;
}


#if 0
static int __get_http_tracker_request(void *bto, const char* url)
{
    bt_trackerclient_t *me = bto;
    int status = 0;
    char *host, *port, *default_port = "80";

    host = url2host(url);

    if (!(port = url2port(url)))
    {
        port = default_port;
    }

//    if (1 == me->funcs.tracker_connect(&me->caller, host, port, me->cfg.my_ip))
    int sock;

    sock = tcp_connect(host,port);
//    if (1 == me->funcs.tracker_connect(&me->caller, host, port, me->cfg.my_ip))

    if (0 != sock)
    {
        int rlen;
        char *request, *document, *response;

        __build_tracker_request(&me->cfg, &request);

        if (
               /*  send http request */
               1 == me->funcs.tracker_send(&me->caller, request,
                                           strlen(request)) &&
               /*  receive http response */
               1 == me->funcs.tracker_recv(&me->caller, &response, &rlen))
        {
            int bencode_len;

            document = strstr(response, "\r\n\r\n") + 4;
            bencode_len = response + rlen - document;
            bt_trackerclient_read_tracker_response(me, document, bencode_len);
            free(request);
            free(response);
            status = 1;
        }
    }

    free(host);
    if (port != default_port)
        free(port);
    return status;
}
#endif

/*****f*
 * FUNCTION
 *  Tell if the uri is supported or not.
 * RETURN
 *  1 if the uri is supported, 0 otherwise
 ******/
int bt_trackerclient_supports_uri(void* _me __attribute__((__unused__)), const char* uri)
{
    if (0 == strncmp(uri,"udp://",6))
    {
        return 0;
    }
    else if (0 == strncmp(uri,"http://",7))
    {
        return 1;
    }
    else if (0 == strncmp(uri,"dht://",6))
    {
        return 1;
    }

    return 0;
}

/*****f*
 * FUNCTION
 *  Connect to the uri.
 * RETURN
 *  1 if successful, 0 otherwise
 ******/
int bt_trackerclient_connect_to_uri(void* me_, const char* uri)
{
    bt_trackerclient_t* me = me_;

    if (0 == bt_trackerclient_supports_uri(me_,uri))
    {
        return 0;
    }

    if (0 == strncmp(uri,"udp://",6))
    {
        return 0;
    }
    else if (0 == strncmp(uri,"http://",7))
    {
        me->uri = strdup(uri);
        thttp_connect(me,uri+7);
//        __get_http_tracker_request(_me, uri+7);
        return 1;
    }
    else if (0 == strncmp(uri,"dht://",6))
    {
        return 0;
    }

    return 0;
}


/**
 * Release all memory used by the tracker client
 *
 * @todo add destructors
 */
int bt_trackerclient_release(void *bto)
{
    free(bto);
    return 1;
}

/*----------------------------------------------------------------------------*/


/**
 * Connect to tracker.
 *
 * Send request to tracker and get peer listing.
 *
 * @return 1 on sucess; otherwise 0
 */
#if 0
int bt_trackerclient_connect_to_tracker(void *bto)
{
    bt_trackerclient_t *bt = bto;

    assert(bt);
    assert(bt->cfg.tracker_url);
    assert(bt->cfg.info_hash);
    assert(bt->cfg.p_peer_id);
    return __get_http_tracker_request(bt);
}
#endif

#if 0
void bt_trackerclient_step(void *bto __attribute__((__unused__)))
{
    bt_trackerclient_t *self = bto;
    time_t seconds;

    seconds = time(NULL);

    /*  perform tracker request to get new peers */
    if (self->last_tracker_request + self->cfg.tracker_scrape_interval < seconds)
    {
        self->last_tracker_request = seconds;
        __get_http_tracker_request(self);
    }
}
#endif

/*----------------------------------------------------------------------------*/
/**
 * Set network functions
 */
#if 0
void bt_trackerclient_set_net_funcs(void *bto, bt_net_tracker_funcs_t * net)
{
    bt_trackerclient_t *bt = bto;

    memcpy(&bt->net, net, sizeof(bt_net_tracker_funcs_t));
}
#endif

#if 0

typedef struct {
    uint32_t len;
    unsigned char id;
    unsigned int bytes_read;
    unsigned int tok_bytes_read;
    union {
        msg_have_t have;
        msg_bitfield_t bitfield;
        bt_block_t block;
        msg_piece_t piece;
    };
} msg_t;

static void __endmsg(msg_t* msg)
{
    msg->bytes_read = 0;
    msg->id = 0;
    msg->tok_bytes_read = 0;
    msg->len = 0;
}


static int __read_byte(
        unsigned char* in,
        unsigned int *tot_bytes_read,
        const unsigned char** buf,
        unsigned int *len)
{
    if (*len == 0)
        return 0;

    *in = **buf;
    *tot_bytes_read += 1;
    *buf += 1;
    *len -= 1;
    return 1;
}
#endif

void trackerclient_connected(
    void *me_
)
{
    thttp_connected(me_);
}

/**
 * Receive this much data on this step. */
void trackerclient_dispatch_from_buffer(
        void *me_,
        const unsigned char* buf,
        unsigned int len)
{
    bt_trackerclient_t* me = me_;

    thttp_dispatch_from_buffer(me, buf, len);
}
