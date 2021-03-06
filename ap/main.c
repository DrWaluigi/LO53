#include <main.h>
#include <rssi_list.h>
#include <pcap_thread.h>
#include <http_daemon.h>

volatile sig_atomic_t got_sigint;
Element * rssi_list;
/* Semaphore dealing with multiaccess from pcap_threads and the http server on
 * the rssi_list.
 */
sem_t synchro;
pthread_t pcap_thread_wlan0, pcap_thread_wlan1;
int * pthread_wlan0_ret;
int * pthread_wlan1_ret;

/******************************************************
*                   sig_callback                      *
* input:                                              *
*       -id the ID of the emitted signal.             *
* output:                                             *
*       no output.                                    *
* desc:                                               *
*       Callback function when a SIGNAL is raised.    *
*                                                     *
*******************************************************/
void sig_callback(int sig) {
    perror("[@] Interruption signal received.");
    exit(-1);
}

int main (int argc, char *argv[]) {
    int err = 0;

    /* Custom signal callbacks. */
    signal(SIGTERM, sig_callback);
    signal(SIGINT, sig_callback);

    /* Initialization of the semaphore. */
    sem_init(&synchro, 3, 0);
    sem_post(&synchro);

    /* Start sniffing on the 'wlan0-1' interface of the AP. */
    printf("[+] Start sniffing on WLAN0-1.\n");
    pthread_create(&pcap_thread_wlan0, NULL, pcap_function, (void *)"wlan0-1");

    /* Start the MicroHTTP daemon. */
    printf("[+] Start MicroHTTP daemon.\n");
    err = start_microhttpd();
    if (err)
        perror("[@] Error: Creation of the MicroHTTP deamon failed!");

    /* Force the main to wait for the sniffing to be done before continuing. */
    pthread_join ( pcap_thread_wlan0, (void**)&pthread_wlan0_ret);

    return 0;
}
