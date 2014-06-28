
#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

#include <stdint.h>

#include "bt_tracker_client.h"

void TestURL2Host_with_suffix(
    CuTest * tc
)
{
    char *url;

    url = url2host("http://www.abc.com/other");
    CuAssertTrue(tc, 0 == strcmp(url, "www.abc.com"));
    free(url);
}

void TestURL2Host_without_suffix(
    CuTest * tc
)
{
 //   char *url;

    CuAssertTrue(tc, 0);
//    url = url2host("http://www.abc.com");
  //  CuAssertTrue(tc, 0 == strcmp(url, "www.abc.com"));
   // free(url);
}

void TestURL2Host_without_http_prefix(
    CuTest * tc
)
{
    CuAssertTrue(tc, 0);
#if 0
    char *url;

    url = url2host("www.abc.com");
    CuAssertTrue(tc, 0 == strcmp(url, "www.abc.com"));
    free(url);
#endif
}

void TestURL2Host_with_port(
    CuTest * tc
)
{
    char *url;

    url = url2host("www.abc.com:9000");
    CuAssertTrue(tc, 0 == strcmp(url, "www.abc.com"));
    free(url);
}

void TestURL2HostPort_with_suffix(
    CuTest * tc
)
{
    char *port;

    port = url2port("http://www.abc.com:9000/other");
    CuAssertTrue(tc, 0 == strcmp(port, "9000"));
    free(port);
}

void TestURL2HostPort_without_suffix(
    CuTest * tc
)
{
    CuAssertTrue(tc, 0);
#if 0
    char *port;

    port = url2port("http://www.abc.com:9000");
    CuAssertTrue(tc, 0 == strcmp(port, "9000"));
    free(port);
#endif
}

void TestURL2HostPort_without_http_prefix(
    CuTest * tc
)
{
    CuAssertTrue(tc, 0);
#if 0
    char *port;

    port = url2port("www.abc.com:9000");
    CuAssertTrue(tc, 0 == strcmp(port, "9000"));
    free(port);
#endif
}

