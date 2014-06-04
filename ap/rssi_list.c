#include "rssi_list.h"
 
/******************************************************
*                     string_to_mac                   *
* input :                                             *
* 		-buf : string containing the mac address to   *
*		convert in the form of "AA:BB:CC:DD:EE:FF"    *
*		-mac_value : array containing the mac	      *
*		converted in the form of 6 bytes in the form  *
*		{170, 187, 204, 221, 238, 255}                *
* output :                                            *
*		-Pointer on the input byte array              *
* desc :                                              *
*		This function convert a mac from its string   *
*	from to a byte form.                              *
*                                                     *
*******************************************************/
u_char * string_to_mac(const char const * buf, u_char * byte_mac) {
    sscanf(buf, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &byte_mac[0], &byte_mac[1],&byte_mac[2],&byte_mac[3],&byte_mac[4],&byte_mac[5]);
    return byte_mac;
}

/******************************************************
*                     mac_to_string                   *
* input :                                             *
* 		-buf : string containing the mac address      *
*		converted in the form of "AA:BB:CC:DD:EE:FF"  *
*		-mac_value : array containing the mac address *
*		to convert in the form of 6 bytes in the form *
*		{170, 187, 204, 221, 238, 255}                *
* output :                                            *
*		-Pointer on the input string                  *
* desc :                                              *
*		This function convert a mac from its byte     *
*	from to a string form.                            *
*                                                     *
*******************************************************/
char * mac_to_string(const u_char const * byte_mac, char * buf) {
    sprintf(buf, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", byte_mac[0], byte_mac[1], byte_mac[2], byte_mac[3], byte_mac[4], byte_mac[5]);
    return buf;
}

/******************************************************
*                     is_equal                        *
* input :                                             *
* 		-array1 : first array to compare.             *
* 		-array2 : second array to compare.            *
* output :                                            *
*		-boolean value (0 = false, 1 = true)          *
* desc :                                              *
*		This function compare to u_char array         *
*                                                     *
*******************************************************/
int is_equal(u_char * array1, u_char * array2) {
    if (sizeof(array1) != sizeof(array2))
        return 0;
    int i = 0;
    while (i < 6) {
        if(array1[i] != array2[i])
            return 0;
        ++i;
    }
    return 1;
}


/********************** RSSI SAMPLE FUNCTION **********************/

/******************************************************
*                 clear_outdated_values               *
* input :                                             *
* 		-list : Deque of the list to clear            *
* output :                                            *
*		-NONE                                         *
* desc :                                              *
*		This function delete all Rssi_samples from    *
* the linked list pointed to by the input which have  *
* a deadline expired.                                 *
*                                                     *
*******************************************************/
void clear_outdated_values(Deque * list) {
    //if the list is not empty
    struct timeval timer;
    unsigned long long ull_timer = 0;
    //get current time
    gettimeofday(&timer, NULL);
    ull_timer = timer.tv_sec;
    if (list != NULL && list->head != NULL) {
        //go through the rssi list
        Rssi_sample * pCurrentElem = list->head;
        Rssi_sample * pPrevElem = NULL;
        while (pCurrentElem != NULL) {
            //if the element date has expired then delete it and relink the list properly
            if (pCurrentElem->deadline + DEFAULT_KEEP_DELAY_S < ull_timer ) {
                //list contains only one element
                if (pCurrentElem == list->head && pCurrentElem->next == NULL) {
                    free(pCurrentElem);
                    list->head = NULL;
                    list->tail = NULL;
                    pCurrentElem = NULL;
                }
                //first element but not alone
                else if (pCurrentElem == list->head) {
                    //reassign the head pointer
                    list->head = pCurrentElem->next;
                    //free element
                    free(pCurrentElem);
                    //reassign pointer
                    pCurrentElem = list->head;
                }
                //last element but not alone
                else if (pCurrentElem == list->tail){
                    //reassign the tail pointer
                    list->tail = pPrevElem;
                    list->tail->next = NULL;
                    //free element
                    free(pCurrentElem);
                    //reassign pointer
                    pCurrentElem = NULL;
                }
                //otherwise
                else {
                    //relink previous element to next element
                    pPrevElem->next = pCurrentElem->next;
                    //free current element
                    free(pCurrentElem);
                    //reassign current element to next element
                    pCurrentElem = pPrevElem->next;
                }
            }
            //no need to modify. get next elem
            else {
                //get next element
                pPrevElem = pCurrentElem;
                pCurrentElem = pCurrentElem->next;
            }
        }
    }
    return;
}


//clear the whole rssi samples list
/******************************************************
*                    clear_values                     *
* input :                                             *
* 		-list : Deque of the list to clear            *
* output :                                            *
*		-NONE                                         *
* desc :                                              *
*		This function delete the whole linked list    *
* pointed to by the input and set the head and tails  *
* to point to NULL.                                   *
*                                                     *
*******************************************************/
void clear_values(Deque * list) {
    //if the list of rssi samples is not empty
    if (list != NULL && list->head != NULL) {
        //while we have not reached the last rssi sample
        while (list->head != NULL) {
            //get the next element in pTmp
            Rssi_sample * pTmp = list->head->next;
            //clear rssi sample
            free(list->head);
            //get the saved next rssi sample address
            list->head = pTmp;
        }
    }
    list->tail = list->head;
    return;
}

/******************************************************
*                      add_value                      *
* input :                                             *
* 		-list : Deque of the list where to add the new*
*				element.                              *
*		-value : int value of the rssi sample to store*
* output :                                            *
*		-NONE                                         *
* desc :                                              *
*		This function add a new rssi sample ad the end*
*	of the rssi list and relink the list properly.    *
*                                                     *
*******************************************************/
void add_value(Deque * list, int value) {
    if (list == NULL)
        return;
    //initialize the new rssi_sample
    Rssi_sample * new_rssi_sample = NULL;
    /***********************************************************
     *Note :                                                    *
     *using malloc causes memory corruption                     *
     *It seems to be because of a static declaration of new_el  *
     * ! need an investigation !                                *
     ************************************************************/
    new_rssi_sample = (Rssi_sample*) calloc(sizeof(Rssi_sample), 1);
    new_rssi_sample->rssi_mW = value;
    struct timeval timer;
    gettimeofday(&timer, NULL);
    new_rssi_sample->deadline = timer.tv_sec;
    new_rssi_sample->next = NULL;

    //if the list is empty then just assign the head to the element we have created
    if (list->head == NULL) {
        list->head = new_rssi_sample;
        list->tail = list->head;
    }
    //otherwise reassign pointer of the last element to point to the new element and the tail to point too
    else {
        list->tail->next = new_rssi_sample;
        list->tail = new_rssi_sample;
    }
    return;
}
/********************** END RSSI SAMPLE FUNCTION **********************/



/********************** ELEMENT FUNCTION **********************/
/******************************************************
*                      clear_list                     *
* input :                                             *
* 		-list : pointer to the address of the list    *
*		to clear.	                                  *
* output :                                            *
*		-NONE                                         *
* desc :                                              *
*		This function clear the whole list pointed to *
*	by the input.                                     *
*                                                     *
*******************************************************/
void clear_list(Element ** list) {
    //if the list is not empty
    if (*list != NULL) {
        //while we have not reached the last element
        while (*list != NULL) {
            //save next element address  in pTmp
            Element * pTmp = (*list)->next;
            //clear rssi list of the device to free
            clear_values(&(*list)->measurements);
            //free device
            free(*list);
            //get saved next device address
            *list = pTmp;
        }
    }
    return;
}


/******************************************************
*                       find_mac                      *
* input :                                             *
* 		-list : pointer to the address of the list    *
*		-mac_value : array containing the new mac to  *
* 		assign to the new element.				      *
* output :                                            *
*		-Pointer on the element found.                *
* desc :                                              *
*		This function search for an element which has *
* the mac value given in parameter.                   *
*                                                     *
*******************************************************/
Element * find_mac(Element * list, u_char * mac_value) {
    if (list == NULL)
        return NULL;
    Element * pTmp = list;
    while (!is_equal(pTmp->mac_addr,mac_value) && (pTmp->next != NULL)) {
        pTmp = pTmp->next;
    }

    //check that the element pointed to by pTmp is the one we searched for and not a default element
    if (!is_equal(pTmp->mac_addr, mac_value))
        return NULL;
    return pTmp;
}

/******************************************************
*                       add_element                   *
* input :                                             *
* 		-list : pointer to the address of the list    *
*		-mac_value : array containing the new mac to  *
* 		assign to the new element.				      *
* output :                                            *
*		-Pointer on the element created.              *
* desc :                                              *
*		This function create a new element and link it*
* at the end of the input list.                       *
*                                                     *
*******************************************************/
Element * add_element(Element ** list, u_char * mac_value) {
    int i =0;
    //initialize new element
    Element * el = NULL;
    el = (Element*)calloc(sizeof(Element), 1);
    for (i =0; i < 6; i++)
        el->mac_addr[i] = mac_value[i];
    el->measurements.head = NULL;
    el->measurements.tail = NULL;
    el->next = NULL;

    if (*list != NULL) {
        Element * pTmp = *list;
        //get last element
        while (pTmp->next != NULL)
            pTmp = pTmp->next;
        //assign next element of the last element to point to the new element created
        pTmp->next = el;
    }
    else
        *list = el;

    //return a pointer to the element created
    return el;
}


/******************************************************
*                     delete_element                  *
* input :                                             *
* 		-list : pointer to the address of the list    *
*		to clear.                                     *
*		-e : pointer to an element to delete          *
* output :                                            *
*		-NONE                                         *
* desc :                                              *
*		This function delete and element from the     *
* input list.                                         *
*                                                     *
*******************************************************/
void delete_element(Element ** list, Element * e) {
    //list is empty or element is not readable
    if (*list == NULL || e == NULL)
        return;

    Element * pTmp = *list;
    //if it's the first element
    if ((*list)->mac_addr == e->mac_addr) {
        *list = pTmp->next;
        //before deleting element, clear the rssi list
        Rssi_sample * pRSSIL = pTmp->measurements.head;
        while (pRSSIL != pTmp->measurements.tail) {
            pTmp->measurements.head = pRSSIL->next;
            free(pRSSIL);
            pRSSIL = pTmp->measurements.head;
        }
        //here measurements contains only the last element
        free(pTmp->measurements.tail);
        pTmp->measurements.head = NULL;
        pTmp->measurements.tail = NULL;

        //now delete the element itself
        free(pTmp);
        return;
    }
    //Note :
    //We consider each element has an unique mac_addr and cannot be listed twice
    while (pTmp->next != NULL && pTmp->next->mac_addr != e->mac_addr)
        pTmp = pTmp->next;

    //we are at the element before the one we want to delete
    //get the element after the one to delete
    Element * pTmp2 = pTmp->next->next;

    //before deleting element, clear the rssi list
    Rssi_sample * pRSSIL = pTmp->next->measurements.head;
    while (pRSSIL != pTmp->next->measurements.tail) {
        pTmp->next->measurements.head = pRSSIL->next;
        free(pRSSIL);
        pRSSIL = pTmp->next->measurements.head;
    }
    //here measurements contains only the last element
    free(pTmp->next->measurements.tail);
    pTmp->next->measurements.head = NULL;
    pTmp->next->measurements.tail = NULL;

    //now delete the element itself
    free(pTmp->next);
    //reassign pointer to relink the list
    pTmp->next = pTmp2;
    return;
}

/******************************************************
*                     clear_empty_macs                *
* input :                                             *
* 		-list : pointer to the address of the list    *
*		to clear.                                     *
* output :                                            *
*		-NONE                                         *
* desc :                                              *
*		This function delete all element of the       *
* input list which has their rssi list empty.         *
*                                                     *
*******************************************************/
void clear_empty_macs(Element ** list) {
    Element * pTmp = *list;
    Element * pTmp2 = NULL;
    //go through the list
    while (pTmp != NULL) {
        //if the current element has an empty rssi sample list
        if (pTmp->measurements.head == NULL && pTmp->measurements.tail == NULL) {
            //get next element
            pTmp2 = pTmp->next;
            //deleete element from the list
            delete_element(list, pTmp);
            //restore the element
            pTmp = pTmp2;
        }
        else
            //get next element
            pTmp= pTmp->next;
    }
    return ;
}

/********************** END ELEMENT FUNCTION **********************/



/********************** COMMUNICATION FUNCTION **********************/
/******************************************************
*                     build_element                   *
* input :                                             *
* 		-e : pointer to the element to build the rssi *
*		http part from                                *
* 		-buff : pointer to the buffer to fill         *
* output :                                            *
*		-pointer to the buffer filled                 *
* desc :                                              *
*		This function compute the mean value of rssi  *
*       for the element and build the frame into buff *
*                                                     *
*******************************************************/
char * build_element(Element * e, char * buf) {
    double sum = 0.0;
    double mean_value = 0.0;
    int number_samples = 0;
    char mac[17];
    mac_to_string(e->mac_addr, mac);
    //compute the mean value
    Rssi_sample * pTmp = e->measurements.head;
    while (pTmp != NULL) {//for each sample of the device
        ++number_samples;
        sum += pTmp->rssi_mW;
        pTmp = pTmp->next;
    }
    mean_value = sum / number_samples;
    if(number_samples > 0)
        sprintf(buf,"{\"%s\":\"%.2f\",\"samples\":\"%d\"}", mac, mean_value, number_samples);

    return buf;
}


/******************************************************
*                     build_buffer                    *
* input :                                             *
* 		-list : pointer to the first element to build *
*		http part from                                *
* 		-buffer : pointer to the buffer to fill       *
* 		-my_name : human readable mac of the AP       *
* 		-nb_macs : number of macs requested in http   *
* 		GET request                                   *
* 		-macs_requested : array of nb_macs * 6 byte   *
* 		containing the byte macs requested by http    *
* 		request.                                      *
* output :                                            *
*		-pointer to the buffer filled                 *
* desc :                                              *
*		This function build the response buffer to    *
*       send back as a response to an http GET request*
*       in the form :                                 *
*  {"ap":"ap:ap:ap:ap:ap:ap", "rssi":["{1},{2},...]}  *
*   with each sample of rssi in the form :            *
*       {"XX:XX:XX:XX:XX:XX":"1", "samples":"2"}      *
*       - 1 : rssi mean value                         *
*       - 2 : number of samples used for mean value   *
*                                                     *
*******************************************************/
char * build_buffer(Element * list, char * buffer, char * my_name, u_char * macs_requested, unsigned short nb_macs) {
    int i =0, j=0;
    char * tmp =(char*) malloc(sizeof(char)*248);
    sprintf(buffer, "{\"ap\":\"%s\", \"rssi\":[", my_name);
    //get first element
    Element * TmpEl = NULL;
    u_char mac_requested_i[6];
    for (i = 0; i < nb_macs; ++i) {
        for (j = 0; j < 6; j++)
            mac_requested_i[j] = macs_requested[i+j];
        TmpEl = find_mac( list, mac_requested_i);
        //if we found the element
        if (TmpEl != NULL)
            strcat(buffer, build_element(TmpEl, tmp));
    }
    strcat(buffer, "]}");
    return buffer;
}

/********************** END COMMUNICATION FUNCTION **********************/
