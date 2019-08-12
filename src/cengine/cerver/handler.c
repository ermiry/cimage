#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include "cengine/types/types.h"

#include "cengine/cerver/network.h"
#include "cengine/cerver/packets.h"
#include "cengine/cerver/events.h"
#include "cengine/cerver/errors.h"
#include "cengine/cerver/client.h"
#include "cengine/cerver/cerver.h"
#include "cengine/cerver/connection.h"
#include "cengine/cerver/handler.h"
#include "cengine/cerver/game.h"

#include "cengine/threads/thread.h"

#include "cengine/collections/dlist.h"

#include "cengine/utils/log.h"
#include "cengine/utils/utils.h"

#pragma region auxiliary

SockReceive *sock_receive_new (void) {

    SockReceive *sr = (SockReceive *) malloc (sizeof (SockReceive));
    if (sr) {
        sr->spare_packet = NULL;
        sr->missing_packet = 0;
    } 

    return sr;

}

void sock_receive_delete (void *sock_receive_ptr) {

    if (sock_receive_ptr) {
        packet_delete (((SockReceive *) sock_receive_ptr)->spare_packet);
        free (sock_receive_ptr);
    }

}

ClientConnection *client_connection_aux_new (Client *client, Connection *connection) {

    ClientConnection *cc = (ClientConnection *) malloc (sizeof (ClientConnection));
    if (cc) {
        cc->client = client;
        cc->connection = connection;
    }

    return cc;

}

void client_connection_aux_delete (ClientConnection *cc) { if (cc) free (cc); }

#pragma endregion

#pragma region handlers

static void client_auth_packet_handler (Packet *packet) {

    if (packet) {
        if (packet->packet_size >= (sizeof (PacketHeader) + sizeof (RequestData))) {
            char *end = (char *) packet->packet;
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

// handles a request made from the cerver
static void client_request_packet_handler (Packet *packet) {

    if (packet) {

    }

}

// the client handles a packet based on its type
static void client_packet_handler (void *data) {

    if (data) {
        Packet *packet = (Packet *) data;
        packet->client->stats->n_packets_received += 1;

        // if (!packet_check (packet)) {
            switch (packet->header->packet_type) {
                // handles cerver type packets
                case CERVER_PACKET:
                    packet->client->stats->received_packets->n_cerver_packets += 1; 
                    cerver_packet_handler (packet); 
                    break;

                // handles an error from the server
                case ERROR_PACKET: 
                    packet->client->stats->received_packets->n_error_packets += 1;
                    error_packet_handler (packet); 
                    break;

                // handles authentication packets
                case AUTH_PACKET: 
                    packet->client->stats->received_packets->n_auth_packets += 1;
                    client_auth_packet_handler (packet); 
                    break;

                // handles a request made from the server
                case REQUEST_PACKET: 
                    packet->client->stats->received_packets->n_request_packets += 1; 
                    client_request_packet_handler (packet);
                    break;

                // handles a game packet sent from the server
                case GAME_PACKET: 
                    packet->client->stats->received_packets->n_game_packets += 1;
                    client_game_packet_handler (packet);
                    break;

                // user set handler to handle app specific errors
                case APP_ERROR_PACKET: 
                    packet->client->stats->received_packets->n_app_error_packets += 1;
                    if (packet->client->app_error_packet_handler)
                        packet->client->app_error_packet_handler (packet);
                    break;

                // user set handler to handler app specific packets
                case APP_PACKET:
                    packet->client->stats->received_packets->n_app_packets += 1;
                    if (packet->client->app_packet_handler)
                        packet->client->app_packet_handler (packet);
                    break;

                // custom packet hanlder
                case CUSTOM_PACKET: 
                    packet->client->stats->received_packets->n_custom_packets += 1;
                    if (packet->client->custom_packet_handler)
                        packet->client->custom_packet_handler (packet);
                    break;

                // handles a test packet form the cerver
                case TEST_PACKET: 
                    packet->client->stats->received_packets->n_test_packets += 1;
                    cengine_log_msg (stdout, LOG_TEST, LOG_NO_TYPE, "Got a test packet from cerver.");
                    break;

                default:
                    packet->client->stats->received_packets->n_bad_packets += 1;
                    #ifdef CLIENT_DEBUG
                    cengine_log_msg (stdout, LOG_WARNING, LOG_NO_TYPE, "Got a packet of unknown type.");
                    #endif
                    break;
            }
        // }

        packet_delete (packet);
    }

}

#pragma endregion

#pragma region receive

static SockReceive *client_receive_handle_spare_packet (Client *client, Connection *connection, 
    size_t buffer_size, char **end, size_t *buffer_pos) {

    if (connection->sock_receive) {
        if (connection->sock_receive->spare_packet) {
            size_t copy_to_spare = 0;
            if (connection->sock_receive->missing_packet < buffer_size) 
                copy_to_spare = connection->sock_receive->missing_packet;

            else copy_to_spare = buffer_size;

            // append new data from buffer to the spare packet
            if (copy_to_spare > 0) {
                packet_append_data (connection->sock_receive->spare_packet, *end, copy_to_spare);

                // check if we can handler the packet 
                size_t curr_packet_size = connection->sock_receive->spare_packet->data_size + sizeof (PacketHeader);
                if (connection->sock_receive->spare_packet->header->packet_size == curr_packet_size) {
                    connection->sock_receive->spare_packet->client = client;
                    connection->sock_receive->spare_packet->connection = connection;
                    client_packet_handler (connection->sock_receive->spare_packet);

                    connection->sock_receive->spare_packet = NULL;
                    connection->sock_receive->missing_packet = 0;
                }

                else connection->sock_receive->missing_packet -= copy_to_spare;

                // offset for the buffer
                if (copy_to_spare < buffer_size) *end += copy_to_spare;
                *buffer_pos += copy_to_spare;
            }
        }
    }

    return connection->sock_receive;

}

// splits the entry buffer in packets of the correct size
static void client_receive_handle_buffer (Client *client, Connection *connection, 
    char *buffer, size_t buffer_size) {

    if (buffer && (buffer_size > 0)) {
        char *end = buffer;
        size_t buffer_pos = 0;

        SockReceive *sock_receive = client_receive_handle_spare_packet (client, connection, 
            buffer_size, &end, &buffer_pos);

        if (buffer_pos >= buffer_size) return;

        PacketHeader *header = NULL;
        size_t packet_size = 0;
        char *packet_data = NULL;

        size_t remaining_buffer_size = 0;
        size_t packet_real_size = 0;
        size_t to_copy_size = 0;

        while (buffer_pos < buffer_size) {
            remaining_buffer_size = buffer_size - buffer_pos;
            if (remaining_buffer_size >= sizeof (PacketHeader)) {
                header = (PacketHeader *) end;

                // check the packet size
                packet_size = header->packet_size;
                if ((packet_size > 0) && (packet_size < 65536)) {
                    end += sizeof (PacketHeader);
                    buffer_pos += sizeof (PacketHeader);

                    Packet *packet = packet_new ();
                    if (packet) {
                        // packet->sock_fd = connection->sock_fd;
                        packet_header_copy (&packet->header, header);
                        packet->packet_size = packet->header->packet_size;

                        // check for packet size and only copy what is in the current buffer
                        packet_real_size = header->packet_size - sizeof (PacketHeader);
                        to_copy_size = 0;
                        if ((remaining_buffer_size - sizeof (PacketHeader)) < packet_real_size) {
                            sock_receive->spare_packet = packet;
                            to_copy_size = remaining_buffer_size - sizeof (PacketHeader);
                            sock_receive->missing_packet = packet_real_size - to_copy_size;
                        }

                        else {
                            to_copy_size = packet_real_size;
                            packet_delete (sock_receive->spare_packet);
                            sock_receive->spare_packet = NULL;
                        } 

                        packet_set_data (packet, (void *) end, to_copy_size);

                        end += to_copy_size;
                        buffer_pos += to_copy_size;

                        if (!sock_receive->spare_packet) {
                            packet->client = client;
                            packet->connection = connection;
                            client_packet_handler (packet);
                        }
                    }

                    else {
                        #ifdef CLIENT_DEBUG
                        cengine_log_msg (stderr, LOG_ERROR, LOG_CLIENT, 
                            c_string_create ("Failed to create a new packet in client_handle_receive_buffer () in connection %s.", 
                            connection->name->str));
                        #endif
                    }
                }

                else {
                    #ifdef CLIENT_DEBUG 
                    cengine_log_msg (stderr, LOG_WARNING, LOG_CLIENT, 
                        c_string_create ("Got a packet of invalid size: %ld in connection %s.",
                        packet_size, connection->name->str));
                    #endif
                    break;
                }
            }

            else break;
        }
    }

}

// handles a failed recive from a connection associatd with a client
// end sthe connection to prevent seg faults or signals for bad sock fd
static void client_receive_handle_failed (Client *client, Connection *connection) {

    if (client && connection) {
        if (!client_connection_end (client, connection)) {
            // check if the client has any other active connection
            if (client->connections->size <= 0) {
                client->running = false;
            }
        }
    }

}

// receives incoming data from the socket
void client_receive (Client *client, Connection *connection) {

    if (client && connection) {
        char *packet_buffer = (char *) calloc (RECEIVE_PACKET_BUFFER_SIZE, sizeof (char));
        if (packet_buffer) {
            ssize_t rc = recv (connection->sock_fd, packet_buffer, RECEIVE_PACKET_BUFFER_SIZE, 0);

            if (rc < 0) {
                if (errno != EWOULDBLOCK) {     // no more data to read 
                    #ifdef CLIENT_DEBUG 
                    cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
                        c_string_create ("client_receive () - rc < 0 - sock fd: %d", connection->sock_fd));
                    perror ("Error");
                    #endif

                    client_receive_handle_failed (client, connection);
                }
            }

            else if (rc == 0) {
                // man recv -> steam socket perfomed an orderly shutdown
                // but in dgram it might mean something?
                #ifdef CLIENT_DEBUG
                cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, 
                    c_string_create ("client_receive () - rc == 0 - sock fd: %d",
                    connection->sock_fd));
                // perror ("Error");
                #endif

                switch (errno) {
                    case EAGAIN: 
                        printf ("Is the connection still opened?\n"); 
                        client_receive_handle_failed (client, connection);
                        break;
                    case EBADF:
                    case ENOTSOCK: {
                        #ifdef CLIENT_DEBUG
                        perror ("Error ");
                        #endif
                        client_receive_handle_failed (client, connection);
                    }
                }
            }

            else {
                // cengine_log_msg (stdout, LOG_DEBUG, LOG_CLIENT, 
                //     c_string_create ("Connection %s rc: %ld",
                //     connection->name->str, rc));
                client->stats->n_receives_done += 1;
                client->stats->total_bytes_received += rc;

                // handle the recived packet buffer -> split them in packets of the correct size
                client_receive_handle_buffer (client, connection, 
                    packet_buffer, rc);
            }

            free (packet_buffer);
        }

        else {
            #ifdef CLIENT_DEBUG
            cengine_log_msg (stderr, LOG_ERROR, LOG_CLIENT, 
                "Failed to allocate a new packet buffer!");
            #endif
        }
    }

}

#pragma endregion