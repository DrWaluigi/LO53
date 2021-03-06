#include <pcap.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <pcap_thread.h>
#include <rssi_list.h>

extern volatile sig_atomic_t got_sigint;
extern Element * rssi_list;
extern sem_t synchro;

/******************************************************
*                    pcap_function                    *
* input:                                              *
*       -name of the interface.                       *
* output:                                             *
*       no output.                                    *
* desc:                                               *
*       This function will listen on an interface and *
*       sniff its packets. If the packet is a WiFi    *
*       one, it will extract the RSSI value and add   *
*       it to the rssi_list.                          *
*                                                     *
*******************************************************/
void *pcap_function(void *arg) {
    char *iface = (char *) arg;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t * handle = NULL;
    struct ieee80211_radiotap_header * rtap_head;
    struct ieee80211_header * eh;
    struct pcap_pkthdr header;
    const u_char * packet;
    u_char * mac;
    u_char first_flags;
    int offset = 0;
    char rssi;
    Element * dev_info;

    /* Open a pcap handler to sniff traffic on the interface. */
    handle = pcap_open_live(iface, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        printf("[@] Could not open pcap on %s", iface);
        pthread_exit((void *)-1);
    }

    while (!got_sigint) {
        packet = pcap_next(handle, &header);
        if (!packet)
            continue;
        rtap_head = (struct ieee80211_radiotap_header *) packet;
        int len = (int) rtap_head->it_len[0] + 256 * (int) rtap_head->it_len[1];
        eh = (struct ieee80211_header *) (packet + len);
        if ((eh->frame_control & 0x03) == 0x01) {
          mac = eh->source_addr;
          first_flags = rtap_head->it_present[0];
          offset = 8;
          offset += ((first_flags & 0x01) == 0x01) ? 8 : 0 ;
          offset += ((first_flags & 0x02) == 0x02) ? 1 : 0 ;
          offset += ((first_flags & 0x04) == 0x04) ? 1 : 0 ;
          offset += ((first_flags & 0x08) == 0x08) ? 4 : 0 ;
          offset += ((first_flags & 0x10) == 0x10) ? 2 : 0 ;
          offset += ((first_flags & 0x80000000) == 0x80000000) ? 4 : 0 ;
          rssi = *((char *) rtap_head + offset) - 0x100;
          printf("%d bytes -- %02X:%02X:%02X:%02X:%02X:%02X -- RSSI: %d dBm\n",
                 len, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], (int) rssi);
          sem_wait(&synchro);
          if ((dev_info = find_mac(rssi_list, mac)) == NULL)
            dev_info = add_element(&rssi_list, mac);
          clear_outdated_values(&dev_info->measurements);
          add_value(&dev_info->measurements, (int) rssi);
          sem_post(&synchro);
        }
    }
    pcap_close(handle);
    pthread_exit((void *) 0);
}
