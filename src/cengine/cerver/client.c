#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include "cengine/cerver/network.h"
#include "cengine/cerver/packets.h"
#include "cengine/cerver/events.h"
#include "cengine/cerver/errors.h"
#include "cengine/cerver/client.h"
#include "cengine/cerver/handler.h"
#include "cengine/cerver/cerver.h"
#include "cengine/cerver/connection.h"

#include "cengine/collections/dlist.h"

#include "cengine/utils/log.h"
#include "cengine/utils/utils.h"

Connection *client_connection_get_by_socket (Client *client, i32 sock_fd);

static Client *client_new (void) {

    Client *client = (Client *) malloc (sizeof (Client));
    if (client) {
        memset (client, 0, sizeof (Client));

        client->connections = NULL;

        memset (client->fds, 0, sizeof (client->fds));
        for (u8 i = 0; i < DEFAULT_MAX_CONNECTIONS; i++)    
            client->fds[i].fd = -1;

        client->thpool = NULL;

        client->registered_actions = NULL;
    }

    return client;

}

static void client_delete (Client *client) {

    if (client) {
        dlist_destroy (client->connections);
        // TODO: check with new thpool code --- 14/06/2019
        if (client->thpool) thpool_destroy (client->thpool);

        client_events_end (client);

        free (client);
    }

}

void client_set_poll_timeout (Client *client, u32 timeout) { 
    
    if (client) client->poll_timeout = timeout; 
    
}

// sets a cutom app packet hanlder and a custom app error packet handler
void cerver_set_app_handlers (Client *client, Action app_handler, Action app_error_handler) {

    if (client) {
        client->app_packet_handler = app_handler;
        client->app_error_packet_handler = app_error_handler;
    }

}

// sets a custom packet handler
void cerver_set_custom_handler (Client *client, Action custom_handler) {

    if (client) client->custom_packet_handler = custom_handler;

}

Client *client_create (void) {

    Client *client = client_new ();
    if (client) {
        client->connections = dlist_init (connection_delete, NULL);
        client->poll_timeout = DEFAULT_POLL_TIMEOUT;
        client->running = false;
        client->in_lobby = false;
        client->owner = false;
        client_events_init (client);
        // client->thpool = thpool_create ("client", DEFAULT_THPOOL_INIT);
        client->thpool = thpool_init (2);
        if (!client->thpool) {
            #ifdef CLIENT_DEBUG
            cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to init client thpool!");
            #endif
        }
    }

    return client;

}

// stop any on going process and destroys the client
u8 client_teardown (Client *client) {

    u8 retval = 1;

    if (client) {
        client->running = false;

        // end any ongoing connection
        for (ListElement *le = dlist_start (client->connections); le; le = le->next) {
            connection_end ((Connection *) le->data);
            le->data = NULL;
        }

        if (client->thpool) {
            #ifdef CLIENT_DEBUG
                cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE,
                    c_string_create ("Active threads in thpool: %i", 
                    thpool_num_threads_working (client->thpool)));
            #endif

            thpool_destroy (client->thpool);
            #ifdef CLIENT_DEBUG
                cengine_log_msg (stdout, LOG_SUCCESS, LOG_NO_TYPE, "Client thpool got destroyed!");
            #endif

            client->thpool = NULL;
        } 

        client_delete (client);

        retval = 0;
    }

    return retval;

}

// returns a connection (registered to a client) by its name
Connection *client_connection_get_by_name (Client *client, const char *name) {

    Connection *retval = NULL;

    if (client && name) {
        Connection *connection = NULL;
        for (ListElement *le = dlist_start (client->connections); le; le = le->next) {
            connection = (Connection *) le->data;
            if (!strcmp (connection->name->str, name)) {
                retval = connection;
                break;
            }
        }
    }

    return retval;

}

// returns a connection assocaited with a socket
Connection *client_connection_get_by_socket (Client *client, i32 sock_fd) {

    Connection *retval = NULL;

    if (client) {
        Connection *connection = NULL;
        for (ListElement *le = dlist_start (client->connections); le; le = le->next) {
            connection = (Connection *) le->data;
            if (connection->sock_fd == sock_fd) {
                retval = connection;
                break;
            }
        }
    }

    return retval;

}

// creates a new connection and registers it to the specified client;
// the connection should be ready to be started
// returns 0 on success, 1 on error
int client_connection_create (Client *client, const char *name,
    const char *ip_address, u16 port, u8 protocol, bool use_ipv6, bool async) {

    int retval = 1;

    if (client) {
        if (ip_address) {
            Connection *connection = connection_create (name, ip_address, port, protocol, use_ipv6, async);
            if (connection) {
                dlist_insert_after (client->connections, dlist_end (client->connections), connection);
                retval = 0;
            }

            else cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to create new connection!");
        }

        else 
            cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to create new connection, no ip provided!");
    }

    return retval;

}

// registers an existing connection to a client
// retuns 0 on success, 1 on error
int client_connection_register (Client *client, Connection *connection) {

    int retval = 1;

    if (client && connection) {
        dlist_insert_after (client->connections, dlist_end (client->connections), connection);
        retval = 0;
    }

    return retval;

}

// FIXME: end connections on errors!!
// starts a client connection
// returns 0 on success, 1 on error
int client_connection_start (Client *client, Connection *connection) {

    int retval = 1;

    if (client && connection) {
        if (!connection_start (connection)) {
            if (connection->async) {
                // add the new socket to the poll structure
                u8 idx = client_poll_get_free_idx (client);

                if (idx >= 0) {
                    client->fds[idx].fd = connection->sock_fd;
                    client->fds[idx].events = POLLIN;
                    client->n_fds++;

                    // check if we walready have the client poll running
                    if (!client->running) {
                        if (!thpool_add_work (client->thpool, (void *) client_poll, client)) {
                            client->running = true;
                            retval = 0;
                        }

                        else {
                            cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
                                "Failed to add client_poll () to client thpool!");
                        } 
                    }
                }     

                else cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to get free client poll idx!");
            }

            else retval = 1;
        }
    }

    return retval;

}

// terminates and destroy a connection registered to a client
// returns 0 on success, 1 on error
int client_connection_end (Client *client, Connection *connection) {

    int retval = 1;

    if (client && connection) {
        // end the connection
        if (!connection_end (connection)) {
            // remove the connection from the client
            for (u8 i = 0; i < client->n_fds; i++) 
                if (client->fds[i].fd == connection->sock_fd)
                    client->fds[i].fd = -1;

            connection_delete (dlist_remove_element (client->connections, 
                dlist_get_element (client->connections, connection)));

            retval = 0;
        }
    }

    return retval;

}

/*** REQUESTS ***/

// These are the requests that we send to the server and we expect a response 

// void *generateRequest (PacketType packetType, RequestType reqType) {

//     size_t packetSize = sizeof (PacketHeader) + sizeof (RequestData);
//     void *begin = client_generatePacket (packetType, packetSize);
//     char *end = begin;

//     RequestData *reqdata = (RequestData *) (end += sizeof (PacketHeader));
//     reqdata->type = reqType;

//     return begin;

// }

// u8 client_makeTestRequest (Client *client, Connection *connection) {

//     // if (client && connection) {
//     //     size_t packetSize = sizeof (PacketHeader);
//     //     void *req = client_generatePacket (TEST_PACKET, packetSize);
//     //     if (req) {
//     //         if (client_sendPacket (connection, req, packetSize) < 0) 
//     //             cengine_log_msg (stderr, LOG_ERROR, PACKET, "Failed to send test packet!");

//     //         else cengine_log_msg (stdout, TEST, PACKET, "Sent test packet to server.");

//     //         free (req);

//     //         return 0;
//     //     }
//     // }

//     // return 1;

// }

#pragma endregion

/*** FILE SERVER ***/

#pragma region FILE SERVER

// TODO:
// request a file from the server
i8 client_file_get (Client *client, Connection *connection, const char *filename) {

    if (client && connection) {}

}

// TODO:
// send a file to the server
i8 client_file_send (Client *client, Connection *connection, const char *filename) {

    if (client && connection) {}

}

#pragma endregion

/*** GAME SERVER ***/

#pragma region GAME SERVER

// request to create a new multiplayer game
// void *client_game_createLobby (Client *owner, Connection *connection, GameType gameType) {

//     Lobby *new_lobby = NULL;

//     // create a new connection
//     Connection *new_con = client_make_new_connection (owner, connection->cerver->ip, 
//         connection->cerver->port, false);

//     if (new_con) {
//         char buffer[1024];
//         memset (buffer, 0, 1024);
//         int rc = read (new_con->sock_fd, buffer, 1024);

//         if (rc > 0) {
//             char *end = buffer;
//             PacketHeader *header = (PacketHeader *) end;
//             #ifdef CLIENT_DEBUG
//                 if (header->packetType == SERVER_PACKET)
//                     cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "New connection - got a server packet.");
//             #endif

//             // authenticate using our server token
//             size_t token_packet_size = sizeof (PacketHeader) + sizeof (RequestData) + sizeof (Token);
//             void *token_packet = client_generatePacket (AUTHENTICATION, token_packet_size);
//             if (token_packet) {
//                 char *end = token_packet;
//                 RequestData *req = (RequestData *) (end += sizeof (PacketHeader));
//                 req->type = CLIENT_AUTH_DATA;

//                 Token *tok = (Token *) (end += sizeof (RequestData));
//                 memcpy (tok->token, connection->server->token_data->token, sizeof (tok->token));

//                 client_sendPacket (new_con, token_packet, token_packet_size);
//                 free (token_packet);
//             }

//             else {
//                 // cengine_log_msg (stderr, LOG_ERROR, CLIENT, "New connection - failed to create auth packet!");
//                 client_end_connection (owner, new_con);
//                 return NULL;
//             }

//             memset (buffer, 0, 1024);
//             rc = read (new_con->sock_fd, buffer, 1024);

//             if (rc > 0) {
//                 end = buffer;
//                 RequestData *reqdata = (RequestData *) (end + sizeof (PacketHeader));
//                 if (reqdata->type == SUCCESS_AUTH) {
//                     #ifdef CLIENT_DEBUG
//                         cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, 
//                             "New connection - authenticated to server.");
//                     #endif

//                     sleep (1);

//                     // make the create lobby request
//                     size_t create_packet_size = sizeof (PacketHeader) + sizeof (RequestData);
//                     void *lobby_req = generateRequest (GAME_PACKET, LOBBY_CREATE);
//                     if (lobby_req) {
//                         client_sendPacket (new_con, lobby_req, create_packet_size);
//                         free (lobby_req);
//                     }

//                     else {
//                         cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
//                             "New connection - failed to create lobby packet!");
//                         client_end_connection (owner, new_con);
//                         return NULL;
//                     }
                    
//                     memset (buffer, 0, 1024);
//                     rc = read (new_con->sock_fd, buffer, 1024);

//                     if (rc > 0) {
//                         end = buffer;
//                         RequestData *reqdata = (RequestData *) (end += sizeof (PacketHeader));
//                         if (reqdata->type == LOBBY_UPDATE) {
//                             SLobby *got_lobby = (SLobby *) (end += sizeof (RequestData));
//                             new_lobby = (Lobby *) malloc (sizeof (SLobby));
//                             memcpy (new_lobby, got_lobby, sizeof (SLobby));
//                         }
//                     }
//                 }
                   
//             }
//         }

//         client_end_connection (owner, new_con);
//     }

//     return new_lobby;

// }

// // FIXME: send game type to server
// // request to join an on going game
// void *client_game_joinLobby (Client *client, Connection *connection, GameType gameType) {

//     if (client && connection) {
//         // create & send a join lobby req packet to the server
//         size_t packetSize = sizeof (PacketHeader) + sizeof (RequestData);
//         void *req = generateRequest (GAME_PACKET, LOBBY_JOIN);

//         if (req) {
//             i8 retval = client_sendPacket (connection, req, packetSize);
//             free (req);
//             // return retval;
//         }
//     }

//     return NULL;

// }

// // request the server to leave the lobby
// i8 client_game_leaveLobby (Client *client, Connection *connection) {

//     if (client && connection) {
//         if (client->in_lobby) {
//             // create & send a leave lobby req packet to the server
//             size_t packetSize = sizeof (PacketHeader) + sizeof (RequestData);
//             void *req = generateRequest (GAME_PACKET, LOBBY_LEAVE);

//             if (req) {
//                 i8 retval = client_sendPacket (connection, req, packetSize);
//                 free (req);
//                 return retval;
//             }
//         }
//     }

//     return -1;

// }

// // request to destroy the current lobby, only if the client is the owner
// i8 client_game_destroyLobby (Client *client, Connection *connection) {

//     if (client && connection) {
//         if (client->in_lobby) {
//             // create & send a leave lobby req packet to the server
//             size_t packetSize = sizeof (PacketHeader) + sizeof (RequestData);
//             void *req = generateRequest (GAME_PACKET, LOBBY_DESTROY);

//             if (req) {
//                 i8 retval = client_sendPacket (connection, req, packetSize);
//                 free (req);
//                 return retval;
//             }
//         }
//     }

//     return -1;

// }

// // the owner of the lobby can request to init the game
// i8 client_game_startGame (Client *client, Connection *connection) {

//     if (client && connection) {
//         if (client->in_lobby) {
//             // create & send a leave lobby req packet to the server
//             size_t packetSize = sizeof (PacketHeader) + sizeof (RequestData);
//             void *req = generateRequest (GAME_PACKET, GAME_INIT);

//             if (req) {
//                 i8 retval = client_sendPacket (connection, req, packetSize);
//                 free (req);
//                 return retval;
//             }
//         }
//     }

//     return -1;

// }