/*
 * libesyslogctrl_ep.c
 *
 *  Created on: Oct 18, 2010
 *      Author: mralex
 */

#include "esyslog.h"

ESYSLOG_MODULE_CREATE(libesyslogctrl);

int esyslogctrl_proxy_init(void);
void esyslogctrl_proxy_uninit(void);
int esyslogctrl_stub_init(void);
void esyslogctrl_stub_uninit(void);

__attribute__((constructor))
static void initialize_lib() {
	esyslogctrl_proxy_init();
	esyslogctrl_stub_init();
	ESYSLOG_MODULE_INIT(libesyslogctrl);
}

__attribute__((destructor))
static void destroy_lib() {
	ESYSLOG_MODULE_UNINIT(libesyslogctrl);
	esyslogctrl_stub_uninit();
	esyslogctrl_proxy_uninit();
}
