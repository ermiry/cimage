#ifndef _CEVER_CLIENT_CONNECTION_H_
#define _CEVER_CLIENT_CONNECTION_H_

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/cerver/network.h"
#include "cengine/cerver/cerver.h"
#include "cengine/cerver/handler.h"

#define CONNECTION_MAX_SLEEP                60        // used for connection with exponential backoff (secs)     

struct _Cerver;
struct _Client;

struct _Connection {

    String *name;

    i32 sock_fd;
    bool use_ipv6;  
    Protocol protocol;
    u16 port; 

    u32 max_sleep;
    bool connected;                     // is the socket connected?

    String *ip;                         // cerver's ip
    struct sockaddr_storage address;    // cerver's address

    // info about the cerver we are connected to
    struct _Cerver *cerver;

    SockReceive *sock_receive;

    Action auth_action;                 // cerver requires authentication
    void *auth_data;                    // maybe auth credentials
    Action destroy_auth_data;           // destroys the auth data when the connection ends
    
    Action success_auth_action;         // what happens if the authentication was successfull
    void *success_auth_args;            // passed to the success auth method

};

typedef struct _Connection Connection;

extern Connection *connection_new (void);
extern void connection_delete (void *ptr);

extern int connection_comparator (void *one, void *two);

// sets the connection max sleep (wait time) to try to connect to the cerver
extern void connection_set_max_sleep (Connection *connection, u32 max_sleep);

// sets the auth action to be triggered if the server asks for authentication
// also takes a method to free the auth data when the connection is destroyed
extern void connection_set_auth_action (Connection *connection,
    Action auth_method, void *auth_data, Action destroy_auth_data);

// sets the connection auth data and a method to destroy it once the connection has ended
extern void connection_set_auth_data (Connection *connection, void *auth_data, Action destroy_auth_data);

// removes the connection auth data and its destroy method
extern void connection_remove_auth_data (Connection *connection);

// sets the connection success auth action that is executed if the authentication to a server
// was successfull
extern void connection_set_success_auth (Connection *connection, Action succes_action, void *args);

// creates a new connection that is ready to be started
extern Connection *connection_create (const char *name,
     const char *ip_address, u16 port, Protocol protocol, bool use_ipv6);

// starts a connection -> connects to the specified ip and port
// returns 0 on success, 1 on error
extern int connection_start (Connection *connection);

// starts listening and receiving data in the connection sock
extern void client_connection_update (void *ptr);

// ends a connection
extern void connection_end (struct _Client *client, Connection *connection);

#endif