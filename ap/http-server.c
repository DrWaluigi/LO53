#include "http-server.h"
#include "rssi_list.h"
#include "rssi-display.h"
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>

extern volatile sig_atomic_t got_sigint;
extern Element * rssi_list;
extern sem_t synchro;

//handler of microhttp thread
int microhttp_function(void) {
    struct MHD_Daemon *daemon;
    printf("microhttp daemon : creation of the deamon\n");
    daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);
    printf("microhttp daemon : daemon created\n");
    //deamon is running in a new thread
    if (NULL == daemon) return 1;
    //We let it pause in a processing-time friendly manner by waiting for the enter key to be pressed.
    printf("microhttp daemon : let it in pause\n");
    getchar();
    printf("microhttp daemon : stop deamon\n");
    MHD_stop_daemon (daemon);
    printf("microhttp daemon : daemon stopped\n");
    return 0;
}

//will be called by GNU libmicrohttpd every time an appropriate request comes in.
int answer_to_connection (void *cls,
                          struct MHD_Connection *connection,
                          const char *url,
                          const char *method,
                          const char *version,
                          const char *upload_data,
                          size_t *upload_data_size,
                          void **con_cls) {
    //we don't care about method which should always be a GET
    //get the mac argument from get method
    printf("microhttp daemon : look up for connection\n");
    const char * mac = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "mac");
    u_char byte_mac[6];
    string_to_mac(mac, byte_mac);

    //get all rssi value in memory for this mac
    //compute the mean value
    //build response with mean value and number of samples used = 1
    printf("microhttp daemon : creation of the frame and allocation\n");
    char * frame = (char*) calloc(sizeof(char), 512);
    printf("microhttp daemon : building ot the buffer\n");
    build_buffer(rssi_list, frame, AP_MAC, byte_mac, 1);

    //construct the reponse with the buffer
    struct MHD_Response *response;
    int ret;
    printf("microhttp daemon : creation of the response\n");
    response = MHD_create_response_from_buffer (strlen (frame),(void*) frame, MHD_RESPMEM_PERSISTENT);

    //post the response
    printf("microhttp daemon : posting the response\n");
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);
    return ret;
}
