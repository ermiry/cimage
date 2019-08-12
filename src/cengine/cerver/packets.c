#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/cerver/network.h"
#include "cengine/cerver/packets.h"
#include "cengine/cerver/cerver.h"
#include "cengine/cerver/client.h"

#ifdef CLIENT_DEBUG
#include "cengine/utils/log.h"
#endif

static ProtocolID protocol_id = 0;
static ProtocolVersion protocol_version = { 0, 0 };

void packets_set_protocol_id (ProtocolID proto_id) { protocol_id = proto_id; }

void packets_set_protocol_version (ProtocolVersion version) { protocol_version = version; }

PacketsPerType *packets_per_type_new (void) {

    PacketsPerType *packets_per_type = (PacketsPerType *) malloc (sizeof (PacketsPerType));
    if (packets_per_type) memset (packets_per_type, 0, sizeof (PacketsPerType));
    return packets_per_type;

}

void packets_per_type_delete (void *ptr) { if (ptr) free (ptr); }

void packets_per_type_print (PacketsPerType *packets_per_type) {

    if (packets_per_type) {
        printf ("Cerver:            %ld\n", packets_per_type->n_cerver_packets);
        printf ("Error:             %ld\n", packets_per_type->n_error_packets);
        printf ("Auth:              %ld\n", packets_per_type->n_auth_packets);
        printf ("Request:           %ld\n", packets_per_type->n_request_packets);
        printf ("Game:              %ld\n", packets_per_type->n_game_packets);
        printf ("App:               %ld\n", packets_per_type->n_app_packets);
        printf ("App Error:         %ld\n", packets_per_type->n_app_error_packets);
        printf ("Custom:            %ld\n", packets_per_type->n_custom_packets);
        printf ("Test:              %ld\n", packets_per_type->n_test_packets);
        printf ("Unknown:           %ld\n", packets_per_type->n_unknown_packets);
        printf ("Bad:               %ld\n", packets_per_type->n_bad_packets);
    }

}

static PacketHeader *packet_header_new (PacketType packet_type, size_t packet_size) {

    PacketHeader *header = (PacketHeader *) malloc (sizeof (PacketHeader));
    if (header) {
        memset (header, 0, sizeof (PacketHeader));
        header->protocol_id = protocol_id;
        header->protocol_version = protocol_version;
        header->packet_type = packet_type;
        header->packet_size = packet_size;
    }

    return header;

}

// allocates space for the dest packet header and copies the data from source
// returns 0 on success, 1 on error
u8 packet_header_copy (PacketHeader **dest, PacketHeader *source) {

    u8 retval = 1;

    if (source) {
        *dest = (PacketHeader *) malloc (sizeof (PacketHeader));
        if (*dest) {
            memcpy (*dest, source, sizeof (PacketHeader));
            retval = 0;
        }
    }

    return retval;

}

static inline void packet_header_delete (PacketHeader *header) { if (header) free (header); }

u8 packet_append_data (Packet *packet, void *data, size_t data_size);

Packet *packet_new (void) {

    Packet *packet = (Packet *) malloc (sizeof (Packet));
    if (packet) {
        memset (packet, 0, sizeof (Packet));
        packet->cerver = NULL;
        packet->client = NULL;
        packet->connection = NULL;

        packet->custom_type = NULL;

        packet->data = NULL;
        packet->data_end = NULL;

        packet->header = NULL;  
        packet->packet = NULL;
    }

    return packet;

}

// create a new packet with the option to pass values directly
// data is copied into packet buffer and can be safely freed
Packet *packet_create (PacketType type, void *data, size_t data_size) {

    Packet *packet = packet_new ();
    if (packet) {
        packet->packet_type = type;
        if (data) packet_append_data (packet, data, data_size);
    }

    return packet;

}

void packet_delete (void *ptr) {

    if (ptr) {
        Packet *packet = (Packet *) ptr;

        packet->cerver = NULL;
        packet->client = NULL;
        packet->connection = NULL;

        str_delete (packet->custom_type);
        if (packet->data) free (packet->data);
        packet_header_delete (packet->header);
        if (packet->packet) free (packet->packet);

        free (packet);
    }

}

// sets the pakcet destinatary is directed to and the protocol to use
void packet_set_network_values (Packet *packet, Client *client, Connection *connection) {

    if (packet) {
        packet->client = client;
        packet->connection = connection;
    }

}

// sets the data of the packet -> copies the data into the packet
// if the packet had data before it is deleted and replaced with the new one
// returns 0 on success, 1 on error
u8 packet_set_data (Packet *packet, void *data, size_t data_size) {

    u8 retval = 1;

    if (packet && data) {
        // check if there was data in the packet before
        if (packet->data) free (packet->data);

        packet->data_size = data_size;
        packet->data = malloc (packet->data_size);
        if (packet->data) {
            memcpy (packet->data, data, data_size);
            packet->data_end = (char *) packet->data;
            packet->data_end += packet->data_size;

            retval = 0;
        }
    }

    return retval;

}

// appends the data to the end if the packet already has data
// if the packet is empty, creates a new buffer
// it creates a new copy of the data and the original can be safely freed
u8 packet_append_data (Packet *packet, void *data, size_t data_size) {

    u8 retval = 1;

    if (packet && data) {
        // append the data to the end if the packet already has data
        if (packet->data) {
            size_t new_size = packet->data_size + data_size;
            void *new_data = realloc (packet->data, new_size);
            if (new_data) {
                packet->data_end = (char *) new_data;
                packet->data_end += packet->data_size;

                // copy the new buffer
                memcpy (packet->data_end, data, data_size);
                packet->data_end += data_size;

                packet->data = new_data;
                packet->data_size = new_size;

                retval = 0;
            }

            else {
                #ifdef CLIENT_DEBUG
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to realloc packet data!");
                #endif
                packet->data = NULL;
                packet->data_size = 0;
            }
        }

        // if the packet is empty, create a new buffer
        else {
            packet->data_size = data_size;
            packet->data = malloc (packet->data_size);
            if (packet->data) {
                // copy the data to the packet data buffer
                memcpy (packet->data, data, data_size);
                packet->data_end = (char *) packet->data;
                packet->data_end += packet->data_size;

                retval = 0;
            }

            else {
                #ifdef CLIENT_DEBUG
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to allocate packet data!");
                #endif
                packet->data = NULL;
                packet->data_size = 0;
            }
        }
    }

    return retval;

}

// sets a the packet's packet using by copying the passed data
// deletes the previuos packet's packet
// returns 0 on succes, 1 on error
u8 packet_set_packet (Packet *packet, void *data, size_t data_size) {

    u8 retval = 1;

    if (packet && data) {
        if (packet->packet) free (packet->packet);

        packet->packet_size = data_size;
        packet->packet = malloc (packet->packet_size);
        if (packet->packet) {
            memcpy (packet->packet, data, data_size);

            retval = 0;
        }
    }

    return retval;

}   

// prepares the packet to be ready to be sent
// returns 0 on sucess, 1 on error
u8 packet_generate (Packet *packet) {

    u8 retval = 0;

    if (packet) {
        if (packet->packet) {
            free (packet->packet);
            packet->packet = NULL;
            packet->packet_size = 0;
        }   

        packet->packet_size = sizeof (PacketHeader) + packet->data_size;
        packet->header = packet_header_new (packet->packet_type, packet->packet_size);

        // create the packet buffer to be sent
        packet->packet = malloc (packet->packet_size);
        if (packet->packet) {
            char *end = (char *) packet->packet;
            memcpy (end, packet->header, sizeof (PacketHeader));
            end += sizeof (PacketHeader);
            memcpy (end, packet->data, packet->data_size);

            retval = 0;
        }
    }

    return retval;

}

// generates a simple request packet of the requested type reday to be sent, 
// and with option to pass some data
Packet *packet_generate_request (PacketType packet_type, u32 req_type, 
    void *data, size_t data_size) {

    Packet *packet = packet_new ();
    if (packet) {
        packet->packet_type = packet_type;

        // generate the request
        packet->data = malloc (sizeof (RequestData));
        ((RequestData *) packet->data)->type = req_type;

        packet->data_size = sizeof (RequestData);
        packet->data_end = (char *) packet->data;
        packet->data_end += sizeof (RequestData);

        // if there is data, append it to the packet data buffer
        if (data) {
            if (packet_append_data (packet, data, data_size)) {
                // we failed to appedn the data into the packet
                packet_delete (packet);
                packet = NULL;
            }
        } 

        // make the packet ready to be sent, and check for errors
        if (packet_generate (packet)) {
            packet_delete (packet);
            packet = NULL;
        }
    }

    return packet;

}

// TODO: check for errno appropierly
// sends a packet using the tcp protocol and the packet sock fd
// returns 0 on success, 1 on error
u8 packet_send_tcp (const Packet *packet, int flags, size_t *total_sent) {

    if (packet) {
        ssize_t sent;
        const char *p = (char *) packet->packet;
        size_t packet_size = packet->packet_size;

        while (packet_size > 0) {
            sent = send (packet->connection->sock_fd, p, packet_size, flags);
            if (sent < 0) return 1;
            p += sent;
            packet_size -= sent;
        }

        if (total_sent) *total_sent = sent;

        return 0;
    }

    return 1;

}

// FIXME: correctly send an udp packet!!
u8 packet_send_udp (const void *packet, size_t packet_size) {

    ssize_t sent;
    const void *p = packet;
    // while (packet_size > 0) {
    //     sent = sendto (server->serverSock, begin, packetSize, 0, 
    //         (const struct sockaddr *) &address, sizeof (struct sockaddr_storage));
    //     if (sent <= 0) return -1;
    //     p += sent;
    //     packetSize -= sent;
    // }

    return 0;

}

static void packet_send_update_stats (PacketType packet_type, size_t sent,
    Client *client, Connection *connection) {

    client->stats->n_packets_sent += 1;
    client->stats->total_bytes_sent += sent;

    // connection->stats->n_packets_sent += 1;
    // connection->stats->total_bytes_sent += sent; 

    switch (packet_type) {
        case ERROR_PACKET: 
            client->stats->sent_packets->n_error_packets += 1;
            // connection->stats->sent_packets->n_error_packets += 1;
            break;

        case AUTH_PACKET: 
            client->stats->sent_packets->n_auth_packets += 1;
            // connection->stats->sent_packets->n_auth_packets += 1;
            break;

        case REQUEST_PACKET: 
            client->stats->sent_packets->n_request_packets += 1;
            // connection->stats->sent_packets->n_request_packets += 1;
            break;

        case GAME_PACKET:
            client->stats->sent_packets->n_game_packets += 1;
            // connection->stats->sent_packets->n_game_packets += 1;
            break;

        case APP_PACKET:
            client->stats->sent_packets->n_app_packets += 1;
            // connection->stats->sent_packets->n_app_packets += 1;
            break;

        case APP_ERROR_PACKET: 
            client->stats->sent_packets->n_app_error_packets += 1;
            // connection->stats->sent_packets->n_app_error_packets += 1;
            break;

        case CUSTOM_PACKET:
            client->stats->sent_packets->n_custom_packets += 1;
            // connection->stats->sent_packets->n_custom_packets += 1;
            break;

        case TEST_PACKET: 
            client->stats->sent_packets->n_test_packets += 1;
            // connection->stats->sent_packets->n_test_packets += 1;
            break;
    }

}

// sends a packet using its network values
u8 packet_send (const Packet *packet, int flags, size_t *total_sent) {

    u8 retval = 1;

    if (packet) {
        switch (packet->connection->protocol) {
            case PROTOCOL_TCP: {
                size_t sent = 0;
                if (!packet_send_tcp (packet, flags, &sent)) {
                    packet_send_update_stats (packet->packet_type, sent,
                        packet->client, packet->connection);

                    retval = 0;
                }

                else {
                    packet->client->stats->sent_packets->n_bad_packets += 1;
                    // packet->connection->stats->sent_packets->n_bad_packets += 1;

                    if (total_sent) *total_sent = 0;
                }
            } break;
            case PROTOCOL_UDP:
                break;

            default: break;
        }
    }

    return retval;

}

// FIXME:
// check for packets with bad size, protocol, version, etc
u8 packet_check (Packet *packet) {

    /*if (packetSize < sizeof (PacketHeader)) {
        #ifdef CLIENT_DEBUG
        cengine_log_msg (stderr, LOG_WARNING, LOG_NO_TYPE, "Recieved a to small packet!");
        #endif
        return 1;
    } 

    PacketHeader *header = (PacketHeader *) packetData;

    if (header->protocolID != PROTOCOL_ID) {
        #ifdef CLIENT_DEBUG
        logMsg (stdout, LOG_WARNING, LOG_PACKET, "Packet with unknown protocol ID.");
        #endif
        return 1;
    }

    Version version = header->protocolVersion;
    if (version.major != PROTOCOL_VERSION.major) {
        #ifdef CLIENT_DEBUG
        logMsg (stdout, LOG_WARNING, LOG_PACKET, "Packet with incompatible version.");
        #endif
        return 1;
    }

    // compare the size we got from recv () against what is the expected packet size
    // that the client created 
    if (packetSize != header->packetSize) {
        #ifdef CLIENT_DEBUG
        logMsg (stdout, LOG_WARNING, LOG_PACKET, "Recv packet size doesn't match header size.");
        #endif
        return 1;
    } 

    if (expectedType != DONT_CHECK_TYPE) {
        // check if the packet is of the expected type
        if (header->packetType != expectedType) {
            #ifdef CLIENT_DEBUG
            logMsg (stdout, LOG_WARNING, LOG_PACKET, "Packet doesn't match expected type.");
            #endif
            return 1;
        }
    } */

    return 0;   // packet is fine

}