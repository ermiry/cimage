#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/cerver/cerver.h"
#include "cengine/cerver/connection.h"
#include "cengine/cerver/packets.h"

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#pragma region cerver stats

static CerverStats *cerver_stats_new (void) {

    CerverStats *cerver_stats = (CerverStats *) malloc (sizeof (CerverStats));
    if (cerver_stats) {
        memset (cerver_stats, 0, sizeof (CerverStats));
        cerver_stats->received_packets = packets_per_type_new ();
        cerver_stats->sent_packets = packets_per_type_new ();
    } 

    return cerver_stats;

}

static void cerver_stats_delete (CerverStats *cerver_stats) {

    if (cerver_stats) {
        packets_per_type_delete (cerver_stats->received_packets);
        packets_per_type_delete (cerver_stats->sent_packets);
        
        free (cerver_stats);
    } 

}

void cerver_stats_print (Cerver *cerver) {

    if (cerver) {
        if (cerver->stats) {
            printf ("\nCerver's %s stats: ", cerver->name->str);
            printf ("\nThreshold time:            %ld\n", cerver->stats->threshold_time);

            if (cerver->auth_required) {
                printf ("\nClient packets received:       %ld\n", cerver->stats->client_n_packets_received);
                printf ("Client receives done:          %ld\n", cerver->stats->client_receives_done);
                printf ("Client bytes received:         %ld\n\n", cerver->stats->client_bytes_received);

                printf ("On hold packets received:       %ld\n", cerver->stats->on_hold_n_packets_received);
                printf ("On hold receives done:          %ld\n", cerver->stats->on_hold_receives_done);
                printf ("On hold bytes received:         %ld\n\n", cerver->stats->on_hold_bytes_received);
            }

            printf ("Total packets received:        %ld\n", cerver->stats->total_n_packets_received);
            printf ("Total receives done:           %ld\n", cerver->stats->total_n_receives_done);
            printf ("Total bytes received:          %ld\n\n", cerver->stats->total_bytes_received);

            printf ("N packets sent:                %ld\n", cerver->stats->n_packets_sent);
            printf ("Total bytes sent:              %ld\n", cerver->stats->total_bytes_sent);

            printf ("\nCurrent active client connections:         %ld\n", cerver->stats->current_active_client_connections);
            printf ("Current connected clients:                 %ld\n", cerver->stats->current_n_connected_clients);
            printf ("Current on hold connections:               %ld\n", cerver->stats->current_n_hold_connections);
            printf ("Total clients:                             %ld\n", cerver->stats->total_n_clients);
            printf ("Unique clients:                            %ld\n", cerver->stats->unique_clients);
            printf ("Total client connections:                  %ld\n", cerver->stats->total_client_connections);

            printf ("\nReceived packets:\n");
            packets_per_type_print (cerver->stats->received_packets);

            printf ("\nSent packets:\n");
            packets_per_type_print (cerver->stats->sent_packets);
        }

        else {
            char *status = c_string_create ("Cerver %s does not have a reference to cerver stats!",
                cerver->name->str);
            if (status) {
                cengine_log_msg (stderr, LOG_ERROR, LOG_ERROR, status);
                free (status);
            }
        }
    }

    else {
        cengine_log_msg (stderr, LOG_WARNING, LOG_NO_TYPE, 
            "Cant print stats of a NULL cerver!");
    }

}

#pragma endregion

Cerver *cerver_new (void) {

    Cerver *cerver = (Cerver *) malloc (sizeof (Cerver));
    if (cerver) {
        memset (cerver, 0, sizeof (Cerver));
        cerver->ip = NULL;
        cerver->name = NULL;
        cerver->token = NULL;
        cerver->stats = NULL;
    }

    return cerver;

}

void cerver_delete (void *ptr) {

    if (ptr) {
        Cerver *cerver = (Cerver *) ptr;

        str_delete (cerver->name);
        str_delete (cerver->ip);

        free (cerver);
    }

}

static Cerver *cerver_deserialize (SCerver *scerver) {
    
    Cerver *cerver = NULL;

    if (scerver) {
        cerver = cerver_new ();
        if (cerver) {
            cerver->use_ipv6 = scerver->use_ipv6;
            cerver->protocol = scerver->protocol;
            cerver->port = scerver->port;

            cerver->name = str_new (scerver->name);
            cerver->type = scerver->type;
            cerver->auth_required = scerver->auth_required;
            cerver->uses_sessions = scerver->uses_sessions;

            cerver->stats = cerver_stats_new ();
        }
    }

    return cerver;

}

// compare the info the server sent us with the one we expected 
// and ajust our connection values if necessary
static u8 cerver_check_info (Cerver *cerver, Connection *connection) {

    u8 retval = 1;

    if (cerver && connection) {
        #ifdef CLIENT_DEBUG
        cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, 
            c_string_create ("Connected to cerver %s.", cerver->name->str));
        switch (cerver->protocol) {
            case PROTOCOL_TCP: 
                cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Cerver using TCP protocol."); 
                break;
            case PROTOCOL_UDP: 
                cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Cerver using UDP protocol.");
                break;

            default: 
                cengine_log_msg (stdout, LOG_WARNING, LOG_NO_TYPE, "Cerver using unknown protocol."); 
                break;
        }
        #endif

        if (cerver->use_ipv6) {
            #ifdef CLIENT_DEBUG
            cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Cerver is configured to use ipv6");
            #endif
        }

        #ifdef CLIENT_DEBUG
        switch (cerver->type) {
            case CUSTOM_CERVER:
                cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Cerver is of type: custom cerver");
                break;
            case FILE_CERVER:
                cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Cerver is of type: file cerver");
                break;
            case WEB_CERVER:
                cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Cerver is of type: web cerver");
                break;
            case GAME_CERVER:
                cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Cerver is of type: game cerver");
                break;

            default: 
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Cerver is of unknown type."); 
                break;
        }
        #endif

        if (cerver->auth_required) {
            #ifdef CLIENT_DEBUG
            cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Cerver requires authentication.");
            #endif
            if (connection->auth_data) {
                #ifdef CLIENT_DEBUG
                cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Sending auth data to cerver...");
                #endif

                if (!connection->auth_packet) connection_generate_auth_packet (connection);

                if (packet_send (connection->auth_packet, 0, NULL, false)) {
                    cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to send connection auth packet!");
                }

                retval = 0;
            }
        }

        else {
            #ifdef CLIENT_DEBUG
            cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Cerver does not requires authentication.");
            #endif
        }
        
        retval = 0;
    }

    return retval;

}

// handles cerver type packets
void cerver_packet_handler (Packet *packet) {

    if (packet) {
        if (packet->data_size >= sizeof (RequestData)) {
            char *end = (char *) packet->data;
            RequestData *req = (RequestData *) (end);

            switch (req->type) {
                case CERVER_INFO: {
                    #ifdef CLIENT_DEBUG
                    cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Recieved a cerver info packet.");
                    #endif
                    packet->connection->cerver = cerver_deserialize ((SCerver *) (end += sizeof (RequestData)));
                    if (cerver_check_info (packet->connection->cerver, packet->connection))
                        cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to correctly check cerver info!");
                } break;

                // the cerves is going to be teardown, we have to disconnect
                case CERVER_TEARDOWN:
                    #ifdef CLIENT_DEBUG
                    cengine_log_msg (stdout, LOG_WARNING, LOG_NO_TYPE, "---> Server teardown! <---");
                    #endif
                    client_got_disconnected (packet->client);
                    client_event_trigger (packet->client, EVENT_DISCONNECTED);
                    break;

                case CERVER_INFO_STATS:
                    // #ifdef CLIENT_DEBUG
                    // cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Received a cerver stats packet.");
                    // #endif
                    break;

                case CERVER_GAME_STATS:
                    // #ifdef CLIENT_DEBUG
                    // cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, "Received a cerver game stats packet.");
                    // #endif
                    break;

                default: 
                    cengine_log_msg (stderr, LOG_WARNING, LOG_NO_TYPE, "Unknown cerver type packet."); 
                    break;
            }
        }
    }

}