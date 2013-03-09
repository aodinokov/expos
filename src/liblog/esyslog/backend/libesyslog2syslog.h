/*
 * libesyslog2syslog.h
 *
 *  Created on: Oct 8, 2010
 *      Author: mralex
 */

#ifndef LIBESYSLOG2SYSLOG_H_
#define LIBESYSLOG2SYSLOG_H_

#include <syslog.h>
#include "esyslog.h"

typedef struct tagESyslog_SyslogBackendConfig{

	const char *ident;
	int option;
	int facility;
}ESyslog_SyslogBackendConfig_t;

struct tagESyslog_Backend * ESyslog_SyslogBackendCreate(void * pData);

#endif /* LIBESYSLOG2SYSLOG_H_ */
