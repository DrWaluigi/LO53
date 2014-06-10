#ifndef _MICROHTTP_THREAD_
#define _MICROHTTP_THREAD_

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>

#define PORT 8080

/* Mac address of the Access Point emitting the reponse. */
#define AP_MAC  "08:00:27:07:57:21"

/*!
 * \brief start_microhttpd creates the HTTP daemon.
 * \return 0 if success, -1 otherwise.
 */
int start_microhttpd(void);
/*!
 * \brief connection_callback callback function that is triggered when a new
 *        connection is created
 */
int connection_callback(void *cls,
                        struct MHD_Connection *connection,
                        const char *url,
                        const char *method,
                        const char *version,
                        const char *upload_data,
                        size_t *upload_data_size,
                        void **con_cls);

#endif
