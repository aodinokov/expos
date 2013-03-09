/*
 * esyslogctrl_server.h
 *
 *  Created on: Oct 15, 2010
 *      Author: mralex
 */

#ifndef ESYSLOGCTRL_SERVER_H_
#define ESYSLOGCTRL_SERVER_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#include "esyslogctrl_ps.h"
#include "esyslog_helper.h"	/*list*/

typedef struct tagEsyslogctrlClientConnection {

	EsyslogCtrl_Handle_t		handle;		/**<< unique id */
	int s;									/**<< work socket */
	int 						registered;	/**<< is process Id is registered? */
	EsyslogCtrl_ProcessInfo_t	info; 		/**<< process info */
	pthread_t 					thread;		/**<< worker thread */
	pthread_mutex_t 			muxWrOp;	/**<< write mutex*/
	struct CMN_list_head		list;		/**<< list of clients */

}EsyslogctrlClientConnection_t;

#endif /* ESYSLOGCTRL_SERVER_H_ */
