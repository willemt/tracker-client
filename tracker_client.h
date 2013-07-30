


void *trackerclient_new(
    void (*on_work_done)(void* callee, int status),
    void (*on_add_peer)(void* callee,
        char* peer_id,
        unsigned int peer_id_len,
        char* ip,
        unsigned int ip_len,
        unsigned int port),
    void* callee);


int bt_trackerclient_set_opt_int(void *bto, const char *key, const int val);

int trackerclient_supports_uri(void* _me, const char* uri);

int trackerclient_connect_to_uri(void* _me, const char* uri);

void trackerclient_set_cfg(void *me_, void *cfg);
