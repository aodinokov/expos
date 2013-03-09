/*
 * libesyslog2stderr.c
 *
 *  Created on: Oct 20, 2010
 *      Author: mralex
 */

/*
 * this module can be used only on user space
 */
#include "libesyslog2stderr.h"
#include "esyslog_be.h"
#include <stdio.h>
#include <string.h>

static void	_onESyslog_BackendSyslog(struct tagESyslog_Backend* pBackend, char* moduleName, ESyslog_Group_t* pGroup, ESyslog_GroupImportance_t importance, const char *format, va_list ap);
static void	_onESyslog_BackendDestroy(struct tagESyslog_Backend* pBackend);

static ESyslog_Backend_t _module = {
		onSyslog: 	_onESyslog_BackendSyslog,
		onDestroy:	_onESyslog_BackendDestroy
};

static void	_onESyslog_BackendSyslog(struct tagESyslog_Backend* pBackend, char* moduleName, ESyslog_Group_t* pGroup, ESyslog_GroupImportance_t importance, const char *format, va_list ap) {

	char bufer[1024];
	int hasNewLine = 0;
	int p = strlen(format);

	if (format[p-1] == '\n' || format[p-1] == '\r')
		hasNewLine = 1;

	vsnprintf(bufer,sizeof(bufer),format, ap);

	fprintf(stderr, "%s.%s %s%s", moduleName?moduleName:"", pGroup->name, bufer, hasNewLine?"":"\n");
}

static void	_onESyslog_BackendDestroy(struct tagESyslog_Backend* pBackend) { /*nothing to do*/}

struct tagESyslog_Backend * ESyslog_StderrBackendCreate(void * pData) {
	return &_module;
}
