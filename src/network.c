
/**
 * Copyright (c) 2011, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file. 

 * @file
 * @brief Manage connection with tracker
 * @author  Willem Thiart himself@willemthiart.com
 * @version 0.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>

#if WIN32
#define _WIN32_WINNT 0x0501

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>

#endif

#include <unistd.h>     /* for sleep */

#include <fcntl.h>

#include <errno.h>

#include "bt_tracker_client.h"
#include "bt_tracker_client_private.h"

#include <fcntl.h>

static int __set_nonblocking(int fd)
{
#ifdef WIN32
   unsigned long mode;
   
   mode = 1;
   return ioctlsocket(fd, FIONBIO, &mode);
#else
   int flags;
   
   flags = fcntl(fd, F_GETFL, 0);

   assert(flags == 0);

   flags = flags|O_NONBLOCK;
   return fcntl(fd, F_SETFL, flags);
#endif
}

int net_tcp_connect(const char *host, const char *port)
{
    int sock = 0, err;
    struct addrinfo hints, *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    //hints.ai_family = AF_UNSPEC;// AF_INET;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    //printf("connecting to: %s %s\n", host, port);

    if (0 != (err = getaddrinfo(host, port, &hints, &result)))
    {
        fprintf(stderr, "getaddrinfo: %s (%s,%s)\n",
                gai_strerror(err), host, port);
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        int ret;

        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        ret = __set_nonblocking(sock);

        assert(ret == 0);

        if (sock == -1)
            continue;

        assert(rp);
        assert(rp->ai_addr);

        if (-1 == connect(sock, rp->ai_addr, rp->ai_addrlen))
        {
#if WIN32
            if (WSAGetLastError() !=  WSAEWOULDBLOCK)
            {
                perror("could not connect");
                close(sock);
                return 0;
            }
#else

#endif
            goto cleanup;
        }
        else
        {
            printf("[connected to %s:%s]\n", host, port);
            goto cleanup;
        }
    }

    if (rp == NULL)
    {
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }

cleanup:
    freeaddrinfo(result);
    return sock;
}


