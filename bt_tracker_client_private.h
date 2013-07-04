
typedef struct
{
    /* the length of a piece (from protocol) */
    int piece_len;
    /* number of pieces (from protocol) */
    int npieces;
} bt_piece_info_t;

/** cfg */
typedef struct
{
//    int select_timeout_msec;
//    int max_peer_connections;
//    int max_active_peers;
//    int max_cache_mem;
    int tracker_scrape_interval;
    /*  don't seed, shutdown when complete */
//    int o_shutdown_when_complete;
    /*  the size of the piece, etc */
    bt_piece_info_t pinfo;
    /*  how many seconds between tracker scrapes */
//    int o_tracker_scrape_interval;
    /* listen for pwp messages on this port */
    int pwp_listen_port;
    /*  this holds my IP. I figure it out */
    char *my_ip;
    /* sha1 hash of the info_hash */
    char *info_hash;

    /* 20-byte self-designated ID of the peer */
    char *my_peer_id;

//    char *tracker_url;
} bt_trackerclient_cfg_t;

typedef struct
{
    /* how often we must send messages to the tracker */
    int interval;

    const char* uri;

    /* so that we remember when we last requested the peer list */
    time_t last_tracker_request;

    bt_trackerclient_cfg_t cfg;

    bt_trackerclient_funcs_t funcs;

    void *caller;

    /** callback for initiating read of metafile */
    void (*func_read_metafile) (void *, char *, int len);
    void *udata;

} bt_trackerclient_t;

int bt_trackerclient_read_tracker_response(
    bt_trackerclient_t* me,
    char *buf,
    int len);

void thttp_dispatch_from_buffer(
        void *me_,
        const unsigned char* buf,
        unsigned int len);

void thttp_connected(void *me_);


int net_tcp_connect(const char *host, const char *port);
