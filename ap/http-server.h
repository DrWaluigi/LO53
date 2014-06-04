#ifndef _MICROHTTP_THREAD_
#define _MICROHTTP_THREAD_

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>

#define PORT 8080

#define AP_MAC  "08:00:27:07:57:21"

int microhttp_function(void);
int answer_to_connection (void *cls,
                          struct MHD_Connection *connection,
                          const char *url,
                          const char *method,
                          const char *version,
                          const char *upload_data,
                          size_t *upload_data_size,
                          void **con_cls);

#endif
