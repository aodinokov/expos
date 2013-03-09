/*
 * libesyslog2syslog_ep.c
 *
 *  Created on: Oct 12, 2010
 *      Author: mralex
 */

#include "libesyslog2syslog.h"
#include "stdio.h" /*NULL*/

static void* _pSyslogHandle = NULL;

__attribute__((constructor))
static void initialize_lib() {
	ESyslog_SyslogBackendConfig_t syslogBackendConfig ={
			ident: NULL,
			option:LOG_NDELAY,
			facility:LOG_USER
	};
	_pSyslogHandle =
		esyslog_registerBackend(ESyslog_SyslogBackendCreate(&syslogBackendConfig));

}

__attribute__((destructor))
static void destroy_lib() {

	if (_pSyslogHandle) {
		esyslog_unregisterBackend(_pSyslogHandle);
		_pSyslogHandle = NULL;
	}
}
