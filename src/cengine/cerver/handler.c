#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include "cengine/cerver/network.h"
#include "cengine/cerver/packets.h"
#include "cengine/cerver/events.h"
#include "cengine/cerver/errors.h"
#include "cengine/cerver/client.h"
#include "cengine/cerver/cerver.h"
#include "cengine/cerver/connection.h"

#include "cengine/collections/dlist.h"

#include "cengine/utils/log.h"
#include "cengine/utils/utils.h"

// FIXME: create a way to register to different events such as success auth, to avoid creating
// an specific placeholder for every action we might wanna need to trigger when something happens
static void client_auth_packet_handler (Packet *packet) {

    if (packet) {
        if (packet->packet_size >= (sizeof (PacketHeader) + sizeof (RequestData))) {
            char *end = packet->packet;
            RequestData *req = (RequestData *) (end += sizeof (PacketHeader));

            switch (req->type) {
                // we recieve a token from the cerver to use in sessions
                case CLIENT_AUTH_DATA:
                    break;

                // we have successfully authenticated with the server
                case SUCCESS_AUTH:
                    break;

                default: 
                    cengine_log_msg (stderr, LOG_WARNING, LOG_NO_TYPE, "Unknown authentication request.");
                    break;
            }
        }
    }

}

// FIXME: 
void auth_packet_handler (Packet *packet) {

    // char *end = pack_info->packetData;
    // RequestData *reqdata = (RequestData *) (end += sizeof (PacketHeader));

    // switch (reqdata->type) {
    //     case CLIENT_AUTH_DATA: {
    //         Token *tokenData = (Token *) (end += sizeof (RequestData));
    //         #ifdef CLIENT_DEBUG 
    //         cengine_log_msg (stdout, LOG_DEBUG, CLIENT,
    //             c_string_create ("Token recieved from server: %s", tokenData->token));
    //         #endif  
    //         Token *token_data = (Token *) malloc (sizeof (Token));
    //         memcpy (token_data->token, tokenData->token, sizeof (token_data->token));
    //         pack_info->connection->server->token_data = token_data;
    //     } break;
    //     case SUCCESS_AUTH: {
    //         cengine_log_msg (stdout, LOG_SUCCESS, CLIENT, "Client authenticated successfully to server!");
    //         if (pack_info->connection->successAuthAction)
    //             pack_info->connection->successAuthAction (pack_info->connection->successAuthArgs);  
    //     } break;
    //     default: break;
    // }
    // break;

}

// the client handles a packet based on its type
static void client_packet_handler (void *data) {

    if (data) {
        Packet *packet = (Packet *) data;

        if (!packet_check (packet)) {
            switch (packet->header->packet_type) {
                // handles cerver type packets
                case SERVER_PACKET: cerver_packet_handler (packet); break;

                // handles an error from the server
                case ERROR_PACKET: error_packet_handler (packet); break;

                // handles authentication packets
                case AUTH_PACKET: auth_packet_handler (packet); break;

                // handles a request made from the server
                case REQUEST_PACKET: break;

                // handle a game packet sent from the server
                case GAME_PACKET: break;

                // user set handler to handle app specific errors
                case APP_ERROR_PACKET: 
                    if (packet->client->app_error_packet_handler)
                        packet->client->app_error_packet_handler (packet);
                    break;

                // user set handler to handler app specific packets
                case APP_PACKET:
                    if (packet->client->app_packet_handler)
                        packet->client->app_packet_handler (packet);
                    break;

                // custom packet hanlder
                case CUSTOM_PACKET: 
                    if (packet->client->custom_packet_handler)
                        packet->client->custom_packet_handler (packet);
                    break;

                // handles a test packet form the cerver
                case TEST_PACKET: 
                    cengine_log_msg (stdout, LOG_TEST, LOG_NO_TYPE, "Got a test packet from cerver.");
                    break;

                default:
                    #ifdef CLIENT_DEBUG
                    cengine_log_msg (stdout, LOG_WARNING, LOG_NO_TYPE, "Got a packet of unknown type.");
                    #endif
                    break;
            }
        }

        packet_delete (packet);
    }

}

// splits the entry buffer in packets of the correct size
static void client_handle_recieve_buffer (Client *client, i32 socket_fd, 
    char *buffer, size_t total_size) {

    if (buffer && (total_size > 0)) {
        u32 buffer_idx = 0;
        char *end = buffer;

        PacketHeader *header = NULL;
        u32 packet_size;
        char *packet_data = NULL;

        while (buffer_idx < total_size) {
            header = (PacketHeader *) end;

            // check the packet size
            packet_size = header->packet_size;
            if (packet_size > 0) {
                // copy the content of the packet from the buffer
                packet_data = (char *) calloc (packet_size, sizeof (char));
                for (u32 i = 0; i < packet_size; i++, buffer_idx++) 
                    packet_data[i] = buffer[buffer_idx];

                Packet *packet = packet_new ();
                if (packet) {
                    packet->connection = client_connection_get_by_socket (client, socket_fd);
                    packet->header = header;
                    packet->packet_size = packet_size;
                    packet->packet = packet_data;

                    thpool_add_work (client->thpool, (void *) client_packet_handler, packet);
                }

                end += packet_size;
            }

            else break;
        }
    }

}

// FIXME: correctly end server connection
// TODO: add support for handling large files transmissions
static void client_receive (Client *client, i32 fd) {

    ssize_t rc;
    char packet_buffer[MAX_UDP_PACKET_SIZE];
    memset (packet_buffer, 0, MAX_UDP_PACKET_SIZE);

    // do {
        rc = recv (fd, packet_buffer, sizeof (packet_buffer), 0);
        
        if (rc < 0) {
            if (errno != EWOULDBLOCK) {     // no more data to read 
                #ifdef CLIENT_DEBUG 
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Client recv failed!");
                perror ("Error ");
                #endif
            }

            // /break;
        }

        if (rc == 0) {
            // man recv -> steam socket perfomed an orderly shutdown
            // but in dgram it might mean something?
            #ifdef CLIENT_DEBUG
            cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "client_receive () - rc == 0");
            #endif
            // break;
        }

        char *buffer_data = (char *) calloc (MAX_UDP_PACKET_SIZE, sizeof (char));
        if (buffer_data) {
            memcpy (buffer_data, packet_buffer, rc);
            client_handle_recieve_buffer (client, fd, buffer_data, rc);
        }
        
    // } while (true);

}

int client_poll_get_free_idx (Client *client) {

    for (u8 i = 0; i < DEFAULT_MAX_CONNECTIONS; i++)
        if (client->fds[i].fd == -1)
            return i;

    return -1;

}

u8 client_poll (void *data) {

    if (!data) {
        #ifdef CLIENT_DEBUG
        cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Can't poll on a NULL client!");
        #endif
        return 1;
    }

    Client *client = (Client *) data;

    int poll_retval;    

    #ifdef CLIENT_DEBUG
        cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Client poll has started!");
    #endif

    while (client->running) {
        poll_retval = poll (client->fds, client->n_fds, client->poll_timeout);

        // poll failed
        if (poll_retval < 0) {
            #ifdef CLIENT_DEBUG
            cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Client poll failed!");
            perror ("Error");
            #endif
            // FIXME: close all of our active connections...
            break;
        }

        // if poll has timed out, just continue to the next loop... 
        if (poll_retval == 0) {
            // #ifdef CLIENT_DEBUG
            //     cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Poll timeout.");
            // #endif
            continue;
        }

        // one or more fd(s) are readable, need to determine which ones they are
        for (u8 i = 0; i < client->n_fds; i++) {
            if (client->fds[i].revents == 0) continue;
            if (client->fds[i].revents != POLLIN) continue;

            client_receive (client, client->fds[i].fd);
        }
    }

    #ifdef CLIENT_DEBUG
        cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Client poll has ended!");
    #endif

}