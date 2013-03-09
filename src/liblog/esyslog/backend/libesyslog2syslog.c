/*
 * libesyslog2syslog.c
 *
 *  Created on: Oct 8, 2010
 *      Author: mralex
 */

/*
 * this module can be used only on user space
 */

#include "libesyslog2syslog.h"
#include <stdio.h>
#include "esyslog_be.h"

static void	_onESyslog_BackendSyslog(struct tagESyslog_Backend* pBackend, char* moduleName, ESyslog_Group_t* pGroup, ESyslog_GroupImportance_t importance, const char *format, va_list ap);
static void	_onESyslog_BackendDestroy(struct tagESyslog_Backend* pBackend);

static int _linkCounter = 0;
static ESyslog_Backend_t _module = {
		onSyslog: 	_onESyslog_BackendSyslog,
		onDestroy:	_onESyslog_BackendDestroy
};

static int _GroupImportance2Syslog(ESyslog_GroupImportance_t importance) {
	switch (importance) {
	case esgiEmerg: return LOG_EMERG;
	case esgiAlert: return LOG_ALERT;
	case esgiCrit: return LOG_CRIT;
	case esgiErr: return LOG_ERR;
	case esgiWarn: return LOG_WARNING;
	case esgiNotice: return LOG_NOTICE;
	case esgiInfo: return LOG_INFO;
	case esgiDebug: return LOG_DEBUG;
	}
	return LOG_ERR;
}

static void	_onESyslog_BackendSyslog(struct tagESyslog_Backend* pBackend, char* moduleName, ESyslog_Group_t* pGroup, ESyslog_GroupImportance_t importance, const char *format, va_list ap) {
	char bufer[1000];

	vsnprintf(bufer,sizeof(bufer),format, ap);

	syslog(_GroupImportance2Syslog(importance), "%s.%s %s", moduleName?moduleName:"", pGroup->name, bufer);
}

static void	_onESyslog_BackendDestroy(struct tagESyslog_Backend* pBackend) {

	if ((--_linkCounter) == 0) {
		closelog();
	}
}

struct tagESyslog_Backend * ESyslog_SyslogBackendCreate(void * pData) {

	ESyslog_SyslogBackendConfig_t *pSyslogBackendConfig = (ESyslog_SyslogBackendConfig_t *)pData;
	if (!pData)
		return NULL;

	if ((_linkCounter++)==0) {
		openlog(pSyslogBackendConfig->ident, pSyslogBackendConfig->option, pSyslogBackendConfig->facility);
	}

	return &_module;
}


