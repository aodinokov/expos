/*
 * libesyslogctrl_stub.c
 *
 *  Created on: Oct 13, 2010
 *      Author: mralex
 */

#include <unistd.h>

#include "esyslog_utils.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>		/* FILE ...*/
#include <pthread.h>	/* pthread_...*/
#include <unistd.h>		/* fcntl */
#include <fcntl.h>		/* fcntl */


/*----------------------------------------------------------------------------*/
#define DBG_OUT esyslogctrl_stub
ESYSLOG_GROUP_CREATE(esyslogctrl_stub);

#include "esyslogctrl_ps.h"

/* Set the FD_CLOEXEC flag of desc if value is nonzero,
   or clear the flag if value is 0.
   Return 0 on success, or -1 on error with errno set. */

static int set_cloexec_flag (int desc, int value) {
	int oldflags = fcntl (desc, F_GETFD, 0);
	/* If reading the flags failed, return error indication now. */
	if (oldflags < 0)
		return oldflags;
	/* Set just the flag we want to set. */
	if (value != 0) oldflags |= FD_CLOEXEC;
	else oldflags &= ~FD_CLOEXEC;
	/* Store modified flag word in the descriptor. */
	return fcntl (desc, F_SETFD, oldflags);
}


static void _setCommand(EsyslogCtrl_ProcessInfo_t *pProcessInfo) {

	int i;
	FILE * file = NULL;
	char filename[100];

	sprintf(filename,"/proc/%u/cmdline", (unsigned int)pProcessInfo->pid);

	if (!(file = fopen(filename, "r")))
		return;

	for (i = 0 ; i < sizeof(EsyslogCtrlProcessCommand_t)-1; i++) {
		int  sym = fgetc(file);
		if (sym < 0 || sym == EOF ) break;
		if ( sym == 0 )sym = ' ';
		pProcessInfo->command[i] = sym;
	}

	 fclose(file);
}

/*----------------------------------------------------------------------------*/
static int _esyslogctrl_open() {
	int s;
	int len;
	struct sockaddr_un saun;
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    	ESYSLOG_ERR(DBG_OUT, "socket failed\n");
    	return -1;
    }
    saun.sun_family = AF_UNIX;
    strcpy(saun.sun_path, ESYSLOG_URL);

    len = sizeof(saun.sun_family) + strlen(saun.sun_path);

    if (connect(s, &saun, len) < 0) {
    	ESYSLOG_INFO(DBG_OUT, "can't connect. possibly server not started\n");
    	close(s);
    	return -1;
    }

    return s;
}
/*----------------------------------------------------------------------------*/
static void _esyslogctrl_close(int s) {

	if (s>=0)
		close(s);
}
/*----------------------------------------------------------------------------*/
#define _CHECK_FD(_fd) if (_fd < 0) res = -1
#define _CHECK_LEN(msgtype) if (sizeof(msgtype)!=len) { ESYSLOG_HERR(DBG_OUT, "recieved invalid header len %d type %x\n", len, type); res = 0; Esyslog_RecvBody(_fd, len); break;}
#define _CHECK_REQ(recv_call) if ((recv_call)) { ESYSLOG_HERR(DBG_OUT, "%s", "can't recv request\n"); res = -1; break;}
#define _CHECK_RES(send_call) if ((send_call)) { ESYSLOG_HERR(DBG_OUT, "%s", "can't send responce\n"); res = -1; break;}
/*----------------------------------------------------------------------------*/
static int esyslogctrl_stub(int _fd, short	len, int	type, EsyslogCtrl_Handle_t	from, EsyslogCtrl_Handle_t	to) {
	int res = 0;

	switch(type) {
	/*TODO: case MSG_REQ_REGPROCESS:*/

	case MSG_REQ_GETMODULECOUNT: {
		_CHECK_RES(Esyslog_SendRes_getModuleCount(_fd, from, esyslog_getModuleCount()));
	};break;
	case MSG_REQ_GETMODULENAME: {
		int result;
		ESyslog_ModuleId_t moduleId;
		ESyslog_ModuleName_t name;
		_CHECK_LEN(EsyslogReq_getModuleName_t);
		_CHECK_REQ(Esyslog_RecvReq_getModuleName(_fd, &moduleId) !=0 );
		result = esyslog_getModuleName(moduleId, &name);
		_CHECK_RES(Esyslog_SendRes_getModuleName(_fd, from, result, name));
	};break;
	case MSG_REQ_GETGRPCOUNT: {
		ESyslog_ModuleId_t moduleId;
		_CHECK_LEN(EsyslogReq_getGroupCount_t);
		_CHECK_REQ(Esyslog_RecvReq_getGroupCount(_fd, &moduleId) !=0 );
		_CHECK_RES(Esyslog_SendRes_getGroupCount(_fd, from, esyslog_getGroupCount(moduleId)));
	};break;
	case MSG_REQ_GETGRP: {
		int result;
		ESyslog_ModuleId_t moduleId;
		ESyslog_GroupId_t groupId;
		ESyslog_Group_t group;
		_CHECK_LEN(EsyslogReq_getGroup_t);
		_CHECK_REQ(Esyslog_RecvReq_getGroup(_fd, &moduleId, &groupId) !=0 );
		result = esyslog_getGroup(moduleId, groupId, &group);
		_CHECK_RES(Esyslog_SendRes_getGroup(_fd, from, result, &group));
	};break;
	case MSG_REQ_SETGRPCONFIG: {
		ESyslog_ModuleId_t moduleId;
		ESyslog_GroupId_t groupId;
		ESyslog_GroupConfig_t	groupConfig;
		_CHECK_LEN(EsyslogReq_setGroupConfig_t);
		_CHECK_REQ(Esyslog_RecvReq_setGroupConfig(_fd, &moduleId, &groupId, &groupConfig) !=0 );
		_CHECK_RES(Esyslog_SendRes_setGroupConfig(_fd, from, esyslog_setGroupConfig(moduleId, groupId, &groupConfig)));
	};break;
	default:
		ESYSLOG_ERR(DBG_OUT, "Unknown msg len %d type %d from %x to %x\n", len, type, from ,to);
		Esyslog_RecvBody(_fd, len);
	}

	return res;
}
/*----------------------------------------------------------------------------*/
static void * work_thread(void* arg) {
	int * pFd = (int*)arg;
	short	len;
	int	type;
	EsyslogCtrl_Handle_t	from;
	EsyslogCtrl_Handle_t	to;
	EsyslogCtrl_ProcessInfo_t processInfo;

	/* FIXME: lazy start - to prevent quick init/deinit*/
	sleep(1);

	memset(&processInfo, 0, sizeof(EsyslogCtrl_ProcessInfo_t));

	processInfo.pid = getpid();
	_setCommand(&processInfo);
	//processInfo.host[0] = 0;
	do {
		ESYSLOG_INFO(DBG_OUT, "work_thread open socket\n");
		*pFd = -1;
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		*pFd = _esyslogctrl_open();
		set_cloexec_flag(*pFd, 1);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		if (*pFd < 0) { sleep(1); continue; }

		ESYSLOG_INFO(DBG_OUT, "work_thread reg process\n");
		/*now send resitration*/
		Esyslog_SendReq_RegProcess(*pFd, 1, &processInfo);
		/*wait for event*/
		do {
			if (Esyslog_RecvHeader(*pFd, &len, &type, &from, &to)!= 0) {
				ESYSLOG_ERR(DBG_OUT, "Esyslog_RecvHeader failed\n");
				break;
			}
			/*call handler*/
			if (esyslogctrl_stub(*pFd, len, type, from, to)!=0) {
				ESYSLOG_ERR(DBG_OUT, "esyslogctrl_stub failed\n");
				break;
			}
		}while(1);
		ESYSLOG_INFO(DBG_OUT, "work_thread close socket\n");
		/*close and restart*/
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		_esyslogctrl_close(*pFd); *pFd = -1;
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	}while (1);
	Esyslog_SendReq_RegProcess(*pFd, 0, &processInfo);	/* :))) */
	return NULL;
}

/*----------------------------------------------------------------------------*/
static int 			_work_thread_fd = -1;
static pthread_t 	_work_thread;
/*----------------------------------------------------------------------------*/
static int _esyslogctrl_stub_init(){
	   if (pthread_create (
				&_work_thread,                    /* thread id returned (pthread_t *new_thread_ID) */
				NULL,                                   /* thread attributes (const pthread_attr_t *attr) */
				(void *(*)(void *))work_thread, 		/* thread entry point (void * (*start_func)(void *)) */
				&_work_thread_fd                   		/* argument to be passed to the entry point (void *arg) */
				) == -1) { /* error creating the thread */
		   ESYSLOG_ERR(DBG_OUT, " error creating the thread\n");
		   return -1;
		}

	return 0;
}
/*----------------------------------------------------------------------------*/
static void _esyslogctrl_stub_uninit(){
	/* send event to stop thread */
	pthread_cancel(_work_thread);

	/* wait for thread exit */
	pthread_join(_work_thread, NULL);

	/* free resource */
	pthread_detach(_work_thread);

	/* close scoket */
	if (_work_thread_fd>=0) {
		ESYSLOG_INFO(DBG_OUT, "uninit close socket\n");
		close(_work_thread_fd);
		_work_thread_fd = -1;
	}
}
/*----------------------------------------------------------------------------*/
/* to work in daemons we need handlers */
static void _prepare(void)	{ /*_esyslogctrl_stub_uninit();*/ }
static void _reinitParent(void)	{ /*_esyslogctrl_stub_init();*/ }
static void _reinitChid(void)	{ ESYSLOG_INFO(DBG_OUT, "reinit child\n"); _esyslogctrl_stub_uninit(); _esyslogctrl_stub_init(); }
/*----------------------------------------------------------------------------*/
int esyslogctrl_stub_init(){
	int i;
	if ((i = pthread_atfork(_prepare,_reinitParent,_reinitChid))!=0) {
		ESYSLOG_ERR(DBG_OUT, "pthread_atfork error\n");
	}
	return _esyslogctrl_stub_init();
}
/*----------------------------------------------------------------------------*/
void esyslogctrl_stub_uninit(){
	_esyslogctrl_stub_uninit();
}
