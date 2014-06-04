#include "rssi-display.h"
#include "rssi_list.h"
#include "pcap-thread.h"
#include "http-server.h"

volatile sig_atomic_t got_sigint;
Element * rssi_list;
//sempahore is used to control access to rssi_list between pcap_threads and  http server
sem_t synchro;
pthread_t pcap_thread_wlan0, pcap_thread_wlan1;
int * pthread_wlan0_ret;
int * pthread_wlan1_ret;
int microhttpd_ret;

//catch signla interruption (^C)
void catch_signal(int sig) {
    printf("\nInterruption Signal Received.\n");
    exit(-1);
}


int main (int argc, char *argv[]) {
    signal(SIGTERM, catch_signal);
    signal(SIGINT, catch_signal);

    //initialization of the semaphore
    //..&synchro : & sem_t
    //..3 : number of simultaneous access to this sem_t
    //..0 : initial value
    sem_init(&synchro, 3, 0);
    sem_post(&synchro);

    /*TEST PART*/
    //build rssi_list
    /*
    u_char mac1[6];
    string_to_mac("11:11:11:11:11:11", mac1);
    add_element(&rssi_list, mac1);
    add_value(&rssi_list->measurements, 10);
    add_value(&rssi_list->measurements, 20);
    add_value(&rssi_list->measurements, 30);
    */
    /*END TEST PART*/

    //create and start threads to sniff wlan0 and wlan1
    printf("creation of the pcap thread\n");
    pthread_create ( &pcap_thread_wlan0, NULL, pcap_function, (void *) "wlan0-1" );
    //	pthread_create ( &pcap_thread_wlan1, NULL, pcap_function, (void *) "wlan1-0" );
    //run microhttp daemon
    printf("run microhttp daemon\n");
    microhttpd_ret = microhttp_function();

    if(microhttpd_ret == 1) printf("E : Error with microhttpd daemon\n");


    //wait for thread to be terminated
    pthread_join ( pcap_thread_wlan0, (void**)&pthread_wlan0_ret);
    // pthread_join ( pcap_thread_wlan1, (void**)&pthread_wlan1_ret);

    return 0;
}
