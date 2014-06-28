
/*
 * Convert ascii byte streams into url encoded character streams as per rfc1738
 *
 *
 * http://en.wikipedia.org/wiki/Percent-encoding
 * http://www.faqs.org/rfcs/rfc1738.html
 *
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* Converts a hex character to its integer value */
static char __from_hex(
    char ch
)
{
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
static char __to_hex(
    char code
)
{
    static char hex[] = "0123456789ABCDEF";

    return hex[code & 15];
}

static int __is_uri_unreserved(unsigned char ch)
{
    if (50 <= ch && ch <= 90)
    {
        return 1;
    }
    else if (ch == '-' || ch == '_' || ch == '.' || ch == '~')
    {
        return 1;
    }
    return 0;
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(
    const char *str,
    int len
)
{
    int i;
    const char *pstr = str;
    char *buf, *pbuf;
    
    pbuf = buf = malloc(strlen(str) * 3 + 1);

    for (i=0;i<len;i++)
    {
        if (__is_uri_unreserved(*pstr))
        {
            *pbuf++ = *pstr;
        }
//        else if (*pstr == ' ')
//        {
//            *pbuf++ = '+';
//        }
        else
        {
            *pbuf++ = '%';
            *pbuf++ = __to_hex(*pstr >> 4);
            *pbuf++ = __to_hex(*pstr & 15);
        }
        pstr++;
    }

    *pbuf = '\0';
    return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
static char *url_decode(
    const char *str
)
{
    const char *pstr = str;

    char *buf = malloc(strlen(str) + 1), *pbuf = buf;

    while (*pstr)
    {
        if (*pstr == '%')
        {
            if (pstr[1] && pstr[2])
            {
                *pbuf++ = __from_hex(pstr[1]) << 4 | __from_hex(pstr[2]);
                pstr += 2;
            }
        }
        else if (*pstr == '+')
        {
            *pbuf++ = ' ';
        }
        else
        {
            *pbuf++ = *pstr;
        }
        pstr++;
    }
    *pbuf = '\0';
    return buf;
}

