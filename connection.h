#ifndef _JULIA_CONNECTION_H_
#define _JULIA_CONNECTION_H_

#include "buffer.h"
#include "server.h"
#include "string.h"

#include <sys/epoll.h>
#include <stdbool.h> 


#define COMMON_HEADERS              \
    /* General headers */           \
    string_t cache_control;         \
    string_t connection;            \
    string_t date;                  \
    string_t pragma;                \
    string_t trailer;               \
    string_t transfer_encoding;     \
    string_t upgrade;               \
    string_t via;                   \
    string_t warning;               \
    /* Entity headers */            \
    string_t allow;                 \
    string_t content_encoding;      \
    string_t content_language;      \
    string_t content_length;        \
    string_t content_location;      \
    string_t content_md5;           \
    string_t content_range;         \
    string_t content_type;          \
    string_t expires;               \
    string_t last_modified;

extern int epoll_fd;
extern struct epoll_event events[MAX_EVENT_NUM];

typedef struct {
    COMMON_HEADERS
    string_t accept;
    string_t accept_charset;
    string_t accept_encoding;
    string_t authorization;
    string_t expect;
    string_t from;
    string_t host;
    string_t if_match;
    string_t if_modified_since;
    string_t if_none_match;
    string_t if_range;
    string_t if_unmodified_since;
    string_t max_forwards;
    string_t proxy_authorization;
    string_t range;
    string_t referer;
    string_t te;
    string_t user_agent;
} headers_in_t;

typedef struct {
    COMMON_HEADERS
    string_t accept_ranges;
    string_t age;
    string_t etag;
    string_t location;
    string_t proxy_authenticate;
    string_t retry_after;
    string_t server;
    string_t vary;
    string_t www_authenticate;
} headers_out_t;


/********************
 * Request
 ********************/

typedef enum {
    M_CONNECT = 0,
    M_DELETE,
    M_GET,
    M_HEAD,
    M_OPTIONS,
    M_POST,
    M_PUT,
    M_TRACE,
} method_t;

typedef struct {
    method_t method;
    struct {
        int major;
        int minor;
    } version;
    headers_in_t headers;
    int status;

    // For state machine
    string_t request_line;
    string_t uri;
    string_t extension;
    string_t schema;
    string_t host;
    string_t header_name;
    string_t header_value;

    int state;
    int uri_state;
    bool keep_alive;
    bool invalid_header;
    bool request_line_done;
    bool headers_done;
    bool body_done;
    bool saw_eof;
    buffer_t buffer;
} request_t;


/*********************
 * Response
 *********************/

typedef struct {
    int status;
    const char* content_type;
    int content_fd;
    int is_script;

    int send_cur;
    char send_buf[SEND_BUF_SIZE];
    bool ready; // response has been fully constructed
} response_t;


/*******************
 * Connection
 *******************/

 typedef struct {
    int fd; // socket fd
    request_t request;
    response_t response;
    int nrequests;  // # request during this connection

    // TODO(wgtdkp): expire time
} connection_t;


connection_t* new_connection(int fd);
void delete_connection(connection_t* connection);
void connection_close(connection_t* connection);

void epoll_init(void);
void event_add_listen(int* listen_fd);
void event_add(connection_t* connection, int event_flags);
void event_set_to(connection_t* connection, int new_event_flags);
void event_delete(connection_t* connection, int event_flags);

#endif
