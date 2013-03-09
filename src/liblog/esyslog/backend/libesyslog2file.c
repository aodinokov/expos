/*
 * libesyslog2file.c
 *
 *  Created on: Oct 20, 2010
 *      Author: mralex
 */

/*
 * this module can be used only on user space
 */
#include "libesyslog2file.h"
#include "esyslog_be.h"
#include "esyslog_helper.h"

#include <stdlib.h> /*calloc...*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h> /*getpid*/
#include <unistd.h>

extern char	*__progname;		/* Program name, from crt0. */

struct tagFileBacked {

	/** backend */
	ESyslog_Backend_t backend;

	/** file to store */
	FILE*file;
};

static void	_onESyslog_BackendSyslog(struct tagESyslog_Backend* pBackend, char* moduleName, ESyslog_Group_t* pGroup, ESyslog_GroupImportance_t importance, const char *format, va_list ap);
static void	_onESyslog_BackendDestroy(struct tagESyslog_Backend* pBackend);

static ESyslog_Backend_t _module = {
		onSyslog: 	_onESyslog_BackendSyslog,
		onDestroy:	_onESyslog_BackendDestroy
};

static void	_onESyslog_BackendSyslog(struct tagESyslog_Backend* pBackend, char* moduleName, ESyslog_Group_t* pGroup, ESyslog_GroupImportance_t importance, const char *format, va_list ap) {
	int hasNewLine = 0;
	int p = strlen(format);
	time_t t;
	struct tagFileBacked * pFileBacked = CMN_GetContainer(pBackend, struct tagFileBacked, backend);

	if (format[p-1] == '\n' || format[p-1] == '\r')
		hasNewLine = 1;

	if (pFileBacked->file) {
		char buf[100];
		time(&t);
		ctime_r(&t, buf);
		while(buf[strlen(buf)-1] == '\n') buf[strlen(buf)-1] = 0;
		fprintf(pFileBacked->file, "%s.%s %s ", moduleName?moduleName:"", pGroup->name, buf);
		vfprintf(pFileBacked->file, format, ap);
		if (!hasNewLine)fprintf(pFileBacked->file, "\n");
		fflush(pFileBacked->file);
	}
}

static void	_onESyslog_BackendDestroy(struct tagESyslog_Backend* pBackend) {
	struct tagFileBacked * pFileBacked = CMN_GetContainer(pBackend, struct tagFileBacked, backend);
	if (pFileBacked->file)
		fclose(pFileBacked->file);
	free(pFileBacked);
}

struct tagESyslog_Backend * ESyslog_FileBackendCreate(void * pData) {

	char filename[1000];

	struct tagFileBacked * pFileBacked = calloc(1, sizeof(struct tagFileBacked));

	if (!pFileBacked){
		return NULL;
	}

	snprintf(filename, sizeof(filename), "%s.%d.log", __progname, (int)getpid());

	if (!(pFileBacked->file = fopen(filename, "a+"))) {
		free(pFileBacked);
		return NULL;
	}

	memcpy(&pFileBacked->backend, &_module, sizeof(ESyslog_Backend_t));

	return &pFileBacked->backend;
}

