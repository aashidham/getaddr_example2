#include <arpa/inet.h>
#include <errno.h> 
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppb_var_array.h"
#include "ppapi/c/ppb_var_dictionary.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_messaging.h"


#ifndef   NI_MAXHOST
#define   NI_MAXHOST 1025
#endif

static PPB_GetInterface g_get_browser_interface = NULL;

int main0(void);
void* main2(void* p);

PP_EXPORT int32_t PPP_InitializeModule(PP_Module a_module_id, 
				PPB_GetInterface get_browser) {
	g_get_browser_interface = get_browser;
	return PP_OK;
}

static PP_Bool Instance_DidCreate(PP_Instance instance,
                                  uint32_t argc,
                                  const char* argn[],
                                  const char* argv[]) {
	nacl_io_init_ppapi(instance, g_get_browser_interface);
	//main2(NULL);
	main0();
	return PP_TRUE;
}

static void Instance_DidDestroy(PP_Instance instance) {}
static void Instance_DidChangeView(PP_Instance instance, PP_Resource view_resource) {}
static void Instance_DidChangeFocus(PP_Instance instance, PP_Bool has_focus) {}
static PP_Bool Instance_HandleDocumentLoad(PP_Instance instance,
                                           PP_Resource url_loader) {
  return PP_FALSE;
}

int main0(void)
{
	//nacl_io_init();
	pthread_t main2_thread;
	if(pthread_create(&main2_thread, NULL, main2, NULL)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	fprintf(stdout,"Could create thread\n");
	if(pthread_join(main2_thread, NULL)) {

		fprintf(stderr, "Error joining thread\n");
		return 2;
	}
	fprintf(stdout,"Could join thread\n");
	return 0;
}

void* main2(void* p)
{
    //nacl_io_init();
    fprintf(stdout,"entered thread1\n");
    struct addrinfo* result;
    struct addrinfo* res;
    int error;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints)); 
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_CANONNAME;

    /* resolve the domain name into a list of addresses */
    fprintf(stdout,"before getaddrinfo\n");
    error = getaddrinfo("www.benuku2.com", NULL, &hints, &result);
    fprintf(stdout,"after getaddrinfo\n");
    if (error != 0 ) {   
        if (error == EAI_SYSTEM) {
            fprintf(stdout,"getaddrinfo error EAI_SYSTEM in syscall %s\n", strerror(errno));
        } else {
            fprintf(stdout, "error in getaddrinfo: %s\n", gai_strerror(error));
        }   
        exit(EXIT_FAILURE);
    }   
    fprintf(stdout,"was able to go through syscall");
    /* loop over all returned results and do inverse lookup */
    for (res = result; res != NULL; res = res->ai_next) {   
        char hostname[NI_MAXHOST];
        error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
        if (error != 0) {
            fprintf(stdout, "error in getnameinfo: %s\n", gai_strerror(error));
            continue;
        }
        if (*hostname != '\0')
            fprintf(stdout,"hostname: %s\n", hostname);
    }   
    freeaddrinfo(result);
    return 0;
}

PP_EXPORT const void* PPP_GetInterface(const char* interface_name) {
  if (strcmp(interface_name, PPP_INSTANCE_INTERFACE) == 0) {
    static PPP_Instance instance_interface = {
        &Instance_DidCreate,
        &Instance_DidDestroy,
        &Instance_DidChangeView,
        &Instance_DidChangeFocus,
        &Instance_HandleDocumentLoad,
    };
    return &instance_interface;
  } 
  return NULL;
}


PP_EXPORT void PPP_ShutdownModule() {
}

