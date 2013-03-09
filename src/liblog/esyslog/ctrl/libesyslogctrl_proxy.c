/*
 * libesyslogctrl_proxy.c
 *
 *  Created on: Oct 13, 2010
 *      Author: mralex
 */

#include <unistd.h>

#include "esyslog_utils.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
/*----------------------------------------------------------------------------*/
ESYSLOG_GROUP_CREATE(esyslogctrl_proxy);
#define DBG_OUT esyslogctrl_proxy

#include "esyslogctrl_ps.h"

/*----------------------------------------------------------------------------*/
static int _fd = -1;
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
//    	perror("connect failed");
    	ESYSLOG_ERR(DBG_OUT, "connect failed\n");
    	return -1;
    }
    _fd = s;

    return 0;
}
/*----------------------------------------------------------------------------*/
static void _esyslogctrl_close() {
	int s = _fd;
	_fd = -1;

	if (s>=0)
		close(s);
}
/*----------------------------------------------------------------------------*/
#define _CHECK_FD(_fd) if (_fd < 0) res = -1
#define _CHECK_REQ(send_call) if ((send_call)) { ESYSLOG_HERR(DBG_OUT, "%s", "can't send request\n"); res = -1; }
#define _CHECK_RESP(msgtype, structtype) \
	{ \
		short	len; int	type; \
		if (Esyslog_RecvHeader(_fd, &len, &type, &from, &to)!= 0) { res = -1; ESYSLOG_ERR(DBG_OUT, "Esyslog_RecvHeader failed\n");} \
		else if (len != sizeof(structtype) || type!= msgtype) \
		{ res = -1; ESYSLOG_HERR(DBG_OUT, "recieved invalid header len %d type %x\n", len, type); Esyslog_RecvBody(_fd, len);} \
	}
/*----------------------------------------------------------------------------*/
int esyslogctrl_getProcessCount	() {
	int res = 0;
	EsyslogCtrl_Handle_t	from; EsyslogCtrl_Handle_t	to;
	_CHECK_FD(_fd);
	if (res != 0)return -1;
	_CHECK_REQ(Esyslog_SendReq_getProcessCount(_fd) != 0);
	if (res != 0)return -1;
	_CHECK_RESP(MSG_RES_GETPROCESSCOUNT, EsyslogRes_getProcessCount_t);
	if (res != 0)return -1;
	return Esyslog_RecvRes_getProcessCount(_fd);
}
/*----------------------------------------------------------------------------*/
EsyslogCtrl_Handle_t 	esyslogctrl_getProcessInfo	(int Id, EsyslogCtrl_ProcessInfo_t* pProcessInfo) {
	int res = 0;
	EsyslogCtrl_Handle_t	from; EsyslogCtrl_Handle_t	to;
	_CHECK_FD(_fd);
	if (res != 0)return -1;
	_CHECK_REQ(Esyslog_SendReq_getProcessInfo(_fd, Id) != 0);
	if (res != 0)return -1;
	_CHECK_RESP(MSG_RES_GETPROCESSINFO, EsyslogRes_getProcessInfo_t);
	if (res != 0)return -1;
	return Esyslog_RecvRes_getProcessInfo(_fd, pProcessInfo);
}
/*----------------------------------------------------------------------------*/
ESyslog_ModuleId_t		esyslogctrl_getModuleCount	(EsyslogCtrl_Handle_t pId) {
	int res = 0;
	EsyslogCtrl_Handle_t	from; EsyslogCtrl_Handle_t	to;
	_CHECK_FD(_fd);
	if (res != 0)return -1;
	_CHECK_REQ(Esyslog_SendReq_getModuleCount(_fd, pId) != 0);
	if (res != 0)return -1;
	_CHECK_RESP(MSG_RES_GETMODULECOUNT, EsyslogRes_getModuleCount_t);
	if (res != 0)return -1;
	return Esyslog_RecvRes_getModuleCount(_fd);
}
/*----------------------------------------------------------------------------*/
int 					esyslogctrl_getModuleName	(EsyslogCtrl_Handle_t pId, ESyslog_ModuleId_t moduleId, ESyslog_ModuleName_t *pName){
	int res = 0;
	EsyslogCtrl_Handle_t	from; EsyslogCtrl_Handle_t	to;
	_CHECK_FD(_fd);
	if (res != 0)return -1;
	_CHECK_REQ(Esyslog_SendReq_getModuleName(_fd, pId, moduleId) != 0);
	if (res != 0)return -1;
	_CHECK_RESP(MSG_RES_GETMODULENAME, EsyslogRes_getModuleName_t);
	if (res != 0)return -1;
	return Esyslog_RecvRes_getModuleName(_fd, pName);
}
/*----------------------------------------------------------------------------*/
ESyslog_GroupId_t 		esyslogctrl_getGroupCount	(EsyslogCtrl_Handle_t pId, ESyslog_ModuleId_t moduleId) {
	int res = 0;
	EsyslogCtrl_Handle_t	from; EsyslogCtrl_Handle_t	to;
	_CHECK_FD(_fd);
	if (res != 0)return -1;
	_CHECK_REQ(Esyslog_SendReq_getGroupCount(_fd, pId, moduleId) != 0);
	if (res != 0)return -1;
	_CHECK_RESP(MSG_RES_GETGRPCOUNT, EsyslogRes_getGroupCount_t);
	if (res != 0)return -1;
	return Esyslog_RecvRes_getGroupCount(_fd);
}
/*----------------------------------------------------------------------------*/
int 					esyslogctrl_getGroup		(EsyslogCtrl_Handle_t pId, ESyslog_ModuleId_t moduleId, ESyslog_GroupId_t groupId, ESyslog_Group_t *pGroup) {
	int res = 0;
	EsyslogCtrl_Handle_t	from; EsyslogCtrl_Handle_t	to;
	_CHECK_FD(_fd);
	if (res != 0)return -1;
	_CHECK_REQ(Esyslog_SendReq_getGroup(_fd, pId, moduleId, groupId) != 0);
	if (res != 0)return -1;
	_CHECK_RESP(MSG_RES_GETGRP, EsyslogRes_getGroup_t);
	if (res != 0)return -1;
	return Esyslog_RecvRes_getGroup(_fd, pGroup);
}
/*----------------------------------------------------------------------------*/
int 					esyslogctrl_setGroupConfig	(EsyslogCtrl_Handle_t pId, ESyslog_ModuleId_t moduleId, ESyslog_GroupId_t groupId, ESyslog_GroupConfig_t	*pGroupConfig) {
	int res = 0;
	EsyslogCtrl_Handle_t	from; EsyslogCtrl_Handle_t	to;
	_CHECK_FD(_fd);
	if (res != 0)return -1;
	_CHECK_REQ(Esyslog_SendReq_setGroupConfig(_fd, pId, moduleId, groupId, pGroupConfig) != 0);
	if (res != 0)return -1;
	_CHECK_RESP(MSG_RES_SETGRPCONFIG, EsyslogRes_setGroupConfig_t);
	if (res != 0)return -1;
	return Esyslog_RecvRes_setGroupConfig(_fd);
}
/*----------------------------------------------------------------------------*/
int esyslogctrl_proxy_init(){
	return _esyslogctrl_open();
}
/*----------------------------------------------------------------------------*/
void esyslogctrl_proxy_uninit(){
	_esyslogctrl_close();
}
/*----------------------------------------------------------------------------*/
