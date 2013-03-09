/*
 * esyslogctrl_server.c
 *
 *  Created on: Oct 13, 2010
 *      Author: mralex
 */

#include <pthread.h>

#include "esyslog_utils.h"

/*----------------------------------------------------------------------------*/
ESYSLOG_GROUP_CREATE(esyslogctrl_server);
#define DBG_OUT esyslogctrl_server

#include "esyslogctrl_server.h"

/*----------------------------------------------------------------------------*/
static CMN_LIST_HEAD(_connectionList);
static pthread_mutex_t _connectionMux;
/*----------------------------------------------------------------------------*/
static int esyslogctrl_stub(EsyslogctrlClientConnection_t *pClientConnection, short	len, int type, EsyslogCtrl_Handle_t	from, EsyslogCtrl_Handle_t	to);
/*----------------------------------------------------------------------------*/
static int _esyslogctrl_initConnList(){
	CMN_INIT_LIST_HEAD(&_connectionList);
	pthread_mutex_init(&_connectionMux, NULL);
	return 0;
}
/*----------------------------------------------------------------------------*/
static void _esyslogctrl_uninitConnList(){
	pthread_mutex_destroy(&_connectionMux);
}
/*----------------------------------------------------------------------------*/
static void _esyslogctrl_lockConnList(){
	pthread_mutex_lock(&_connectionMux);
}
static void _esyslogctrl_unlockConnList(){
	pthread_mutex_unlock(&_connectionMux);
}
/*----------------------------------------------------------------------------*/
/**list must be locked*/
static EsyslogctrlClientConnection_t* _esyslogctrl_findConnection(EsyslogCtrl_Handle_t handle) {
	struct CMN_list_head * pCur;

	CMN_list_for_each(pCur, &_connectionList) {
		EsyslogctrlClientConnection_t *pClientConnection = CMN_list_entry(pCur, EsyslogctrlClientConnection_t, list);
		if (pClientConnection->handle == handle)
			return pClientConnection;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static int _esyslogctrl_getConnectionNumber(void) {
	struct CMN_list_head * pCur;
	int number = 0;
	CMN_list_for_each(pCur, &_connectionList) {
		EsyslogctrlClientConnection_t *pClientConnection = CMN_list_entry(pCur, EsyslogctrlClientConnection_t, list);
		if (pClientConnection->registered)
			number++;
	}
	return number;
}
/*----------------------------------------------------------------------------*/
static EsyslogctrlClientConnection_t* _esyslogctrl_getConnectionById(int Id) {
	struct CMN_list_head * pCur;
	int number = 0;
	CMN_list_for_each(pCur, &_connectionList) {
		EsyslogctrlClientConnection_t *pClientConnection = CMN_list_entry(pCur, EsyslogctrlClientConnection_t, list);
		if (pClientConnection->registered) {
			if (Id == number)
				return pClientConnection;
			number++;
		}
	}
	return NULL;
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

    unlink(ESYSLOG_URL);
    if (bind(s, &saun, len) < 0) {
    	ESYSLOG_ERR(DBG_OUT, "bind failed\n");
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
static void * work_thread(void* arg) {
	EsyslogctrlClientConnection_t *pClientConnection = (EsyslogctrlClientConnection_t *)arg;
	short len;
	int type;
	EsyslogCtrl_Handle_t	from;
	EsyslogCtrl_Handle_t	to;

	while(1){
		if (Esyslog_RecvHeader(pClientConnection->s, &len, &type, &from, &to)!=0)
			break;
		esyslogctrl_stub(pClientConnection,len,type,from,to);
	}

    _esyslogctrl_lockConnList();
    CMN_list_del(&pClientConnection->list);
    _esyslogctrl_unlockConnList();

	close(pClientConnection->s);
	pthread_mutex_destroy(&pClientConnection->muxWrOp);
	pthread_detach(pClientConnection->thread);
	free(pClientConnection);
	return NULL;
}
/*----------------------------------------------------------------------------*/
static int _esyslogctrl_server(int s) {

    if (listen(s, 5) < 0) {
    	ESYSLOG_ERR(DBG_OUT, "listen failed\n");
    	return -1;
    }

	while (1) {
		int ns;
		unsigned int fromlen;
		struct sockaddr_un fsaun;

		EsyslogctrlClientConnection_t *pClientConnection;

	    fromlen = sizeof(fsaun);
	    if ((ns = accept(s, &fsaun, &fromlen)) < 0) {
	    	ESYSLOG_ERR(DBG_OUT, "accept failed\n");
	    }

	    /* create object */
	    if (!(pClientConnection = calloc(1,sizeof(EsyslogctrlClientConnection_t)))) {
	    	close(ns);
	    	continue;
	    }
	    /* fill object */
	    pClientConnection->s = ns;
	    pClientConnection->registered = 0;
	    pClientConnection->handle = (EsyslogCtrl_Handle_t)pClientConnection;	/*uniq id*/

	    /*create mutext */
	    pthread_mutex_init(&pClientConnection->muxWrOp, NULL);

	    /* add to list*/
	    _esyslogctrl_lockConnList();
	    CMN_list_add_tail(&pClientConnection->list, &_connectionList);
	    _esyslogctrl_unlockConnList();
	    /* send ns to client thread*/
	    if (pthread_create (
	 			&pClientConnection->thread,             /* thread id returned (pthread_t *new_thread_ID) */
	 			NULL,                                   /* thread attributes (const pthread_attr_t *attr) */
	 			(void *(*)(void *))work_thread, 		/* thread entry point (void * (*start_func)(void *)) */
	 			pClientConnection                           /* argument to be passed to the entry point (void *arg) */
	 			) == -1) {
	 		/* error creating the thread */
		    _esyslogctrl_lockConnList();
		    CMN_list_del(&pClientConnection->list);
		    _esyslogctrl_unlockConnList();

	    	close(ns);
	    	pthread_mutex_destroy(&pClientConnection->muxWrOp);
	    	free(pClientConnection);
	    	continue;
	 	}
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
#define _SEND_ERR(pCon) { Esyslog_SendHeader(pCon->s, 0, MSG_RES_ERROR, -1, -1); }
#define _CHECK_FD(_fd) if (_fd < 0) res = -1
#define _CHECK_LEN(msgtype) if (sizeof(msgtype)!=len) { ESYSLOG_HERR(DBG_OUT, "recieved invalid header len %d type %x\n", len, type); res = 0; Esyslog_RecvBody(_fd, len); break;}
#define _CHECK_REQ(recv_call) if ((recv_call)) { ESYSLOG_HERR(DBG_OUT, "%s", "can't recv request\n"); res = -1; break;}
#define _CHECK_RES(send_call) if ((send_call)) { ESYSLOG_HERR(DBG_OUT, "%s", "can't send responce\n"); res = -1; break;}
/*----------------------------------------------------------------------------*/
static int esyslogctrl_stub(EsyslogctrlClientConnection_t *pClientConnection, short	len, int type, EsyslogCtrl_Handle_t	from, EsyslogCtrl_Handle_t	to) {
	int res = 0;

	if (from == -1)
		from = pClientConnection->handle;
	else {
		/*TODO: CHECK*/
		from = pClientConnection->handle;
	}
	switch(type) {
	case MSG_REQ_REGPROCESS : {
		int reg;
		Esyslog_RecvReq_RegProcess(pClientConnection->s, &reg, &pClientConnection->info);
		pClientConnection->registered = reg;
		/* todo: send to all registered */
	}; break;
	case MSG_REQ_GETPROCESSCOUNT: {
		int process_count;
		_esyslogctrl_lockConnList();
		process_count = _esyslogctrl_getConnectionNumber();
		_esyslogctrl_unlockConnList();
		pthread_mutex_lock(&pClientConnection->muxWrOp);
		Esyslog_SendRes_getProcessCount(pClientConnection->s, process_count);
		pthread_mutex_unlock(&pClientConnection->muxWrOp);
	}; break;
	case MSG_REQ_GETPROCESSINFO: {
		int id = -1;
		EsyslogctrlClientConnection_t *pTmpClientConnection;
		Esyslog_RecvReq_getProcessInfo(pClientConnection->s, &id);
		_esyslogctrl_lockConnList();

		pthread_mutex_lock(&pClientConnection->muxWrOp);
		if ((pTmpClientConnection = _esyslogctrl_getConnectionById(id))!=NULL) {
			Esyslog_SendRes_getProcessInfo(pClientConnection->s,
					pTmpClientConnection->handle,
					&pTmpClientConnection->info);
		}else {
			_SEND_ERR(pClientConnection);
		}
		pthread_mutex_unlock(&pClientConnection->muxWrOp);
		_esyslogctrl_unlockConnList();

	}; break;
	default: {	/* retransmit */
			int new_fd;
			EsyslogctrlClientConnection_t *pToClientConnection;
			_esyslogctrl_lockConnList();
			if (!( pToClientConnection = _esyslogctrl_findConnection(to))) {
				_SEND_ERR(pClientConnection);
				_esyslogctrl_unlockConnList();
				break;
			}
			new_fd = pToClientConnection->s;

			//printf("send %d=>%d %d %d from %d to %d\n", pClientConnection->s, new_fd, len, type, from, to );
			pthread_mutex_lock(&pToClientConnection->muxWrOp);
			Esyslog_SendHeader(new_fd,len, type, from, to );
			Esyslog_CopyBody(new_fd, pClientConnection->s, len);
			pthread_mutex_unlock(&pToClientConnection->muxWrOp);
			_esyslogctrl_unlockConnList();
		}
	}

	return res;
}

int main(){
	int res = 1;
	int s;

	_esyslogctrl_initConnList();

	s = _esyslogctrl_open();
	if (s >= 0) {
		_esyslogctrl_server(s);
		_esyslogctrl_close(s);
		res = 0;
	}
	_esyslogctrl_uninitConnList();
	return res;
}
