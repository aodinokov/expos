/*
 * libesyslog2stderr_ep.c
 *
 *  Created on: Oct 20, 2010
 *      Author: mralex
 */

#include "libesyslog2stderr.h"
#include "stdio.h" /*NULL*/

static void* _pSyslogHandle = NULL;

__attribute__((constructor))
static void initialize_lib() {
	_pSyslogHandle =
		esyslog_registerBackend(ESyslog_StderrBackendCreate(NULL));

}

__attribute__((destructor))
static void destroy_lib() {

	if (_pSyslogHandle) {
		esyslog_unregisterBackend(_pSyslogHandle);
		_pSyslogHandle = NULL;
	}
}

