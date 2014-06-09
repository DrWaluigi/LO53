#include "http_server.h"
#include "rssi_list.h"
#include "rssi_display.h"
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>

extern volatile sig_atomic_t got_sigint;
extern Element * rssi_list;
extern sem_t synchro;

/* Starts the MicroHTTP deamon.
 *
 * Input: None.
 * Output: 0 if success, -1 otherwise.
 */
int start_microhttpd(void) {
    struct MHD_Daemon *daemon;
    printf("[+] MicroHTTP daemon: creation.\n");
    daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &connection_callback, NULL, MHD_OPTION_END);

    /* The deamon could not be created. */
    if (!daemon) {
        perror("[@] MicroHTTP daemon: creation error.");
        return -1;
    }

    printf("[+] MicroHTTP daemon: creation success.\n");
    /* HACK: Avoid the daemon to complete. */
    getchar();

    MHD_stop_daemon(daemon);
    printf("[+] MicroHTTP daemon: stopped.\n");
    return 0;
}

/* Callback function which is triggered when a new connection is created in the
 * MicroHTTP daemon.
 */
int connection_callback(void *cls,
                        struct MHD_Connection *connection,
                        const char *url,
                        const char *method,
                        const char *version,
                        const char *upload_data,
                        size_t *upload_data_size,
                        void **con_cls) {
    int err = 0;
    const char *mac = NULL;
    char *frame = NULL;
    struct MHD_Response *response = NULL;
    u_char byte_mac[6] = {0};

    /* TODO: Check if it is a GET method only. */
    //get the mac argument from get method
    printf("[+] MicroHTTP daemon: new connection.\n");
    /* Get the mac argument from the get request. */
    mac = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "mac");
    /* Convert the mac string to an array of byte. */
    string_to_mac(mac, byte_mac);

    frame = (char*) calloc(sizeof(char), 512);
    printf("[+] MicroHTTP daemon: creation of the buffer.\n");
    build_buffer(rssi_list, frame, AP_MAC, byte_mac, 1, 512);

    printf("[+] MicroHTTp daemon: creation of the response.\n");
    response = MHD_create_response_from_buffer(strlen(frame), (void*)frame, MHD_RESPMEM_PERSISTENT);

    printf("[+] MicroHTTP daemon: send the response.\n");
    err = MHD_queue_response (connection, MHD_HTTP_OK, response);

    /* Free the response structure. */
    MHD_destroy_response(response);
    return err;
}
