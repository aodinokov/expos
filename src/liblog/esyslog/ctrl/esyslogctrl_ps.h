/*
 * esyslogctrl_ps.h
 *
 *  Created on: Oct 12, 2010
 *      Author: mralex
 */

#ifndef SOCK_SETTINGS_H_
#define SOCK_SETTINGS_H_

#include "esyslogctrl.h"
#include <string.h> /* memcpy */
#include <stdlib.h> /* calloc...*/
#include <errno.h>	/* errno.. */
#include <poll.h>	/* poll.. */

#define __ESYSLOG_PACKED_ATTR 	__attribute__ ((packed))

#ifndef ESYSLOG_URL
#define ESYSLOG_URL     "/tmp/esyslogserv"
#endif /*ESYSLOG_URL*/

/*----------------------------------------------------------------------------*/
/* заголовок нижнего уровня */
/*----------------------------------------------------------------------------*/
typedef struct tagEsyslogHeader {
	short	len;	/**<< длина пакета*/
	int		type;	/**<< тип пакета */
	EsyslogCtrl_Handle_t	from;	/**<< от какого процесса пакет */
	EsyslogCtrl_Handle_t	to;		/**<< от какого процесса пакет */
}__ESYSLOG_PACKED_ATTR EsyslogHeader_t;

static inline ssize_t _Esyslog_Read(int fd, void *buf, size_t count) {
	size_t i = 0;
	ssize_t res;

	ESYSLOG_DEBUG(DBG_OUT, "(IN)_Esyslog_Read fd %d buf %p count %d\n", fd, buf, count);

	do {
#if 0
		struct pollfd fds[1];
        int retval;

		fds[0].fd = fd;
		fds[0].events = POLLRDNORM | POLLPRI| POLLNVAL | POLLERR;

        if ((retval = poll(fds, 1, 3000)) == -1) {	/*если ошибка*/
			int errsv = errno;
			if (errsv == EINTR) {
				ESYSLOG_INFO(DBG_OUT, "poll interrupted by signal\n");
				continue;
			}
			ESYSLOG_ERR(DBG_OUT, "poll returns error %d %s\n", errsv, strerror(errsv));
        	res = -1; break;
        }else if(retval == 1) {
            /* check for descriptor error */
            if (	(fds[0].revents & POLLNVAL) ||
    				(fds[0].revents & POLLERR)
    			){
            	ESYSLOG_INFO(DBG_OUT, "poll invalid revent %x\n", fds[0].revents);
            	res = -1; break;
            }
        }else if (retval == 0 /*&& FD_ISSET(fd, &rfds)*/) {
        	ESYSLOG_INFO(DBG_OUT, "poll timeout\n");

        	/*check if not closed*/
        	fds[0].events |= POLLOUT;
        	if ((retval = poll(fds, 1, 1)) == -1){
        		ESYSLOG_INFO(DBG_OUT, "wr poll error\n");
        	}

        	if (retval == 1) {
                /* check for descriptor error */
                if (	(fds[0].revents & POLLNVAL) ||
        				(fds[0].revents & POLLERR)
        			){
                	ESYSLOG_INFO(DBG_OUT, "wr poll invalid revent %x\n", fds[0].revents);
                	res = -1; break;
                }
        	}
        	continue; /* еще */
        }
#endif
		/* do read - data is available */
		if ((res = read(fd, ((char*)buf)+i, count-i)) == -1) {	/*если ошибка*/
			int errsv = errno;
			if (errsv == EINTR) {
				ESYSLOG_INFO(DBG_OUT, "read interrupted by signal\n");
				continue;
			}
			ESYSLOG_ERR(DBG_OUT, "read returns error %d %s\n", errsv, strerror(errsv));
			break;
		}else if (res == 0) {
			break;
		}

		/* update result*/
		i += res;
		if (i==count) {
			res = i; break;
		}
	}while(1);

	ESYSLOG_DEBUG(DBG_OUT, "(OUT)_Esyslog_Read return %d\n", res);
	return res;
}

static inline ssize_t _Esyslog_Write(int fd, const void *buf, size_t count) {
	return write(fd, buf, count);
}

static inline int Esyslog_SendHeader(int fd, short len, int type, EsyslogCtrl_Handle_t	from, EsyslogCtrl_Handle_t	to) {
	EsyslogHeader_t str;
	str.len = len; str.type=type;
	str.from = from; str.to=to;
	ESYSLOG_INFO(DBG_OUT, "send header fd %d len %d type %d from %d to %d\n", fd, len, type, from, to);
	return _Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline int Esyslog_RecvHeader(int fd, short* pLen, int* pType, EsyslogCtrl_Handle_t	* pFrom, EsyslogCtrl_Handle_t	* pTo) {
	EsyslogHeader_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) { return -1; }
	*pLen = str.len;
	*pType = str.type;
	*pFrom = str.from;
	*pTo = str.to;
	ESYSLOG_INFO(DBG_OUT, "recv header fd %d len %d type %d from %d to %d\n", fd, str.len, str.type, str.from, str.to);
	return 0;
}

static inline int Esyslog_RecvBody(int fd, short len) {
	int res;
	int i = 0;
	static char buf[1024];
	while (i<len) {
		res = _Esyslog_Read(fd, buf, (len-i)<sizeof(buf)?(len-i):sizeof(buf));
		if (res < 0)
			return res;
		if (res>0)
			i+=res;
	}
	return i;
}

static inline int Esyslog_CopyBody(int fdOut, int fdIn, short len) {
	int res;
	int i = 0;
	static char buf[1024];
	while (i<len) {
		res = _Esyslog_Read(fdIn, buf, (len-i)<sizeof(buf)?(len-i):sizeof(buf));
		if (res < 0)
			return res;
		if (res>0) {
			i+=res;
			_Esyslog_Write(fdOut, buf, res);
		}
	}
	return i;
}


/*----------------------------------------------------------------------------*/
#define MSG_RES_ERROR 				-1 /**<< посылаем его, если произошла непоправимая ошибка*/
/*----------------------------------------------------------------------------*/
#define MSG_REQ_REGPROCESS 			0x00
typedef struct tagEsyslogReq_RegProcess {
	int					reg;	/**<< регистрируемся или разрегистрируемся */
	EsyslogCtrl_ProcessInfo_t processInfo;
}__ESYSLOG_PACKED_ATTR EsyslogReq_RegProcess_t;

static inline int Esyslog_SendReq_RegProcess(int fd, int reg, EsyslogCtrl_ProcessInfo_t *pProcessInfo) {
	EsyslogReq_RegProcess_t str;
	str.reg = reg;
	memcpy(&str.processInfo, pProcessInfo, sizeof(EsyslogCtrl_ProcessInfo_t));
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_REQ_REGPROCESS, -1, -1)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline int Esyslog_RecvReq_RegProcess(int fd, int *pReg, EsyslogCtrl_ProcessInfo_t *pProcessInfo) {
	EsyslogReq_RegProcess_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	*pReg = str.reg;
	memcpy(pProcessInfo, &str.processInfo, sizeof(EsyslogCtrl_ProcessInfo_t));
	return 0;
}

/*----------------------------------------------------------------------------*/
#define MSG_REQ_GETPROCESSCOUNT 	0x01

static inline int Esyslog_SendReq_getProcessCount(int fd) {
	return 	Esyslog_SendHeader(fd, 0, MSG_REQ_GETPROCESSCOUNT, -1 , -1)!=0;
}
/*----------------------------------------------------------------------------*/
#define MSG_RES_GETPROCESSCOUNT 	0x02
typedef struct tagEsyslogRes_getProcessCount {
	int result;					/**<< результат выполненя */
}__ESYSLOG_PACKED_ATTR EsyslogRes_getProcessCount_t;

static inline int Esyslog_SendRes_getProcessCount(int fd, int result) {
	EsyslogRes_getProcessCount_t str;
	str.result = result;
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_RES_GETPROCESSCOUNT, -1, -1)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline int Esyslog_RecvRes_getProcessCount(int fd) {
	EsyslogRes_getProcessCount_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	return str.result;
}

/*----------------------------------------------------------------------------*/
#define MSG_REQ_GETPROCESSINFO		0x03
typedef struct tagEsyslogReq_getProcessInfo {
	int Id;						/** << аргумент */
}__ESYSLOG_PACKED_ATTR EsyslogReq_getProcessInfo_t;

static inline int Esyslog_SendReq_getProcessInfo(int fd, int Id) {
	EsyslogReq_getProcessInfo_t str;
	str.Id = Id;
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_REQ_GETPROCESSINFO, -1 , -1)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline int Esyslog_RecvReq_getProcessInfo(int fd, int *pId) {
	EsyslogReq_getProcessInfo_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	*pId = str.Id; return 0;
}

/*----------------------------------------------------------------------------*/
#define MSG_RES_GETPROCESSINFO		0x04
typedef struct tagEsyslogRes_getProcessInfo {
	EsyslogCtrl_Handle_t result;	/**<< результат выполнения */
	EsyslogCtrl_ProcessInfo_t processInfo;
}__ESYSLOG_PACKED_ATTR EsyslogRes_getProcessInfo_t;

static inline int Esyslog_SendRes_getProcessInfo(int fd, EsyslogCtrl_Handle_t result, EsyslogCtrl_ProcessInfo_t *pProcessInfo) {
	EsyslogRes_getProcessInfo_t str;
	str.result = result;
	memcpy(&str.processInfo, pProcessInfo, sizeof(EsyslogCtrl_ProcessInfo_t));
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_RES_GETPROCESSINFO, -1, -1)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline EsyslogCtrl_Handle_t Esyslog_RecvRes_getProcessInfo(int fd, EsyslogCtrl_ProcessInfo_t *pProcessInfo) {
	EsyslogRes_getProcessInfo_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	memcpy(pProcessInfo, &str.processInfo, sizeof(EsyslogCtrl_ProcessInfo_t));
	return str.result;
}

/*----------------------------------------------------------------------------*/
#define MSG_REQ_GETMODULECOUNT		0x05

static inline int Esyslog_SendReq_getModuleCount(int fd, EsyslogCtrl_Handle_t to) {
	return 	Esyslog_SendHeader(fd, 0, MSG_REQ_GETMODULECOUNT, -1, to)!=0;
}

/*----------------------------------------------------------------------------*/
#define MSG_RES_GETMODULECOUNT		0x06
typedef struct tagEsyslogRes_getModuleCount {
	int result;					/**<< результат выполненя */
}__ESYSLOG_PACKED_ATTR EsyslogRes_getModuleCount_t;

static inline int Esyslog_SendRes_getModuleCount(int fd, EsyslogCtrl_Handle_t to, int result) {
	EsyslogRes_getModuleCount_t str;
	str.result = result;
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_RES_GETMODULECOUNT, -1, to)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline EsyslogCtrl_Handle_t Esyslog_RecvRes_getModuleCount(int fd) {
	EsyslogRes_getModuleCount_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	return str.result;
}

/*----------------------------------------------------------------------------*/

#define MSG_REQ_GETMODULENAME 		0x07
typedef struct tagEsyslogReq_getModuleName {
	ESyslog_ModuleId_t moduleId;	/**<< аргумент2  */
}__ESYSLOG_PACKED_ATTR EsyslogReq_getModuleName_t;

static inline int Esyslog_SendReq_getModuleName(int fd, EsyslogCtrl_Handle_t to, ESyslog_ModuleId_t moduleId) {
	EsyslogReq_getModuleName_t str;
	str.moduleId = moduleId;
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_REQ_GETMODULENAME, -1, to)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline EsyslogCtrl_Handle_t Esyslog_RecvReq_getModuleName(int fd, ESyslog_ModuleId_t *pModuleId) {
	EsyslogReq_getModuleName_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	*pModuleId = str.moduleId; return 0;
}

/*----------------------------------------------------------------------------*/
#define MSG_RES_GETMODULENAME 		0x08
typedef struct tagEsyslogRes_getModuleName {
	int result;					/**<< результат выполненя */
	ESyslog_ModuleName_t name;	/**<< аргумент3  */
}__ESYSLOG_PACKED_ATTR EsyslogRes_getModuleName_t;

static inline int Esyslog_SendRes_getModuleName(int fd, EsyslogCtrl_Handle_t to, int result, ESyslog_ModuleName_t name) {
	EsyslogRes_getModuleName_t str;
	str.result = result;
	memcpy(str.name, name, sizeof(ESyslog_ModuleName_t));
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_RES_GETMODULENAME, -1, to)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline EsyslogCtrl_Handle_t Esyslog_RecvRes_getModuleName(int fd, ESyslog_ModuleName_t *pName) {
	EsyslogRes_getModuleName_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	memcpy(&((*pName)[0]), str.name, sizeof(ESyslog_ModuleName_t));
	return str.result;
}

/*----------------------------------------------------------------------------*/
#define MSG_REQ_GETGRPCOUNT 		0x09
typedef struct tagEsyslogReq_getGroupCount {
	ESyslog_ModuleId_t moduleId;	/**<< аргумент2  */
}__ESYSLOG_PACKED_ATTR EsyslogReq_getGroupCount_t;

static inline int Esyslog_SendReq_getGroupCount(int fd, EsyslogCtrl_Handle_t to, ESyslog_ModuleId_t moduleId) {
	EsyslogReq_getGroupCount_t str;
	str.moduleId = moduleId;
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_REQ_GETGRPCOUNT, -1, to)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline EsyslogCtrl_Handle_t Esyslog_RecvReq_getGroupCount(int fd, ESyslog_ModuleId_t *pModuleId) {
	EsyslogReq_getGroupCount_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	*pModuleId = str.moduleId; return 0;
}

/*----------------------------------------------------------------------------*/
#define MSG_RES_GETGRPCOUNT 		0x0A
typedef struct tagEsyslogRes_getGroupCount {
	ESyslog_GroupId_t result;					/**<< результат выполненя */
}__ESYSLOG_PACKED_ATTR EsyslogRes_getGroupCount_t;

static inline int Esyslog_SendRes_getGroupCount(int fd, EsyslogCtrl_Handle_t to, ESyslog_GroupId_t result) {
	EsyslogRes_getGroupCount_t str;
	str.result = result;
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_RES_GETGRPCOUNT, -1, to)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline ESyslog_GroupId_t Esyslog_RecvRes_getGroupCount(int fd) {
	EsyslogRes_getGroupCount_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	return str.result;
}

/*----------------------------------------------------------------------------*/
#define MSG_REQ_GETGRP 				0x0B
typedef struct tagEsyslogReq_getGroup {
	ESyslog_ModuleId_t moduleId;	/**<< аргумент2  */
	ESyslog_GroupId_t groupId;		/**<< аргумент3  */
}__ESYSLOG_PACKED_ATTR EsyslogReq_getGroup_t;

static inline int Esyslog_SendReq_getGroup(int fd, EsyslogCtrl_Handle_t to, ESyslog_ModuleId_t moduleId, ESyslog_GroupId_t groupId) {
	EsyslogReq_getGroup_t str;
	str.moduleId = moduleId;
	str.groupId = groupId;
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_REQ_GETGRP, -1, to)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline EsyslogCtrl_Handle_t Esyslog_RecvReq_getGroup(int fd, ESyslog_ModuleId_t *pModuleId, ESyslog_GroupId_t *pGroupId) {
	EsyslogReq_getGroup_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	*pModuleId = str.moduleId; *pGroupId = str.groupId; return 0;
}

/*----------------------------------------------------------------------------*/
#define MSG_RES_GETGRP 				0x0C
typedef struct tagEsyslogRes_getGroup {
	int result;					/**<< результат выполненя */
	ESyslog_Group_t	group;		/**<< аргумент4 */
}__ESYSLOG_PACKED_ATTR EsyslogRes_getGroup_t;

static inline int Esyslog_SendRes_getGroup(int fd, EsyslogCtrl_Handle_t to, int result, ESyslog_Group_t *pGroup) {
	EsyslogRes_getGroup_t str;
	str.result = result;
	memcpy(&str.group, pGroup, sizeof(ESyslog_Group_t));
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_RES_GETGRP, -1, to)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline EsyslogCtrl_Handle_t Esyslog_RecvRes_getGroup(int fd, ESyslog_Group_t *pGroup) {
	EsyslogRes_getGroup_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	memcpy(pGroup, &str.group, sizeof(ESyslog_Group_t));
	return str.result;
}

/*----------------------------------------------------------------------------*/
#define MSG_REQ_SETGRPCONFIG		0x0D
typedef struct tagEsyslogReq_setGroupConfig {
	ESyslog_ModuleId_t moduleId;		/**<< аргумент2  */
	ESyslog_GroupId_t groupId;			/**<< аргумент3  */
	ESyslog_GroupConfig_t	groupConfig;/**<< аргумент4 */
}__ESYSLOG_PACKED_ATTR EsyslogReq_setGroupConfig_t;

static inline int Esyslog_SendReq_setGroupConfig(int fd, EsyslogCtrl_Handle_t to, ESyslog_ModuleId_t moduleId, ESyslog_GroupId_t groupId, ESyslog_GroupConfig_t *pGroupConfig) {
	EsyslogReq_setGroupConfig_t str;
	str.moduleId = moduleId;
	str.groupId = groupId;
	memcpy(&str.groupConfig, pGroupConfig, sizeof(ESyslog_GroupConfig_t));
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_REQ_SETGRPCONFIG, -1, to)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline EsyslogCtrl_Handle_t Esyslog_RecvReq_setGroupConfig(int fd, ESyslog_ModuleId_t *pModuleId, ESyslog_GroupId_t *pGroupId, ESyslog_GroupConfig_t *pGroupConfig) {
	EsyslogReq_setGroupConfig_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	memcpy(pGroupConfig, &str.groupConfig, sizeof(ESyslog_GroupConfig_t));
	*pModuleId = str.moduleId; *pGroupId = str.groupId; return 0;
}

/*----------------------------------------------------------------------------*/
#define MSG_RES_SETGRPCONFIG		0x0E
typedef struct tagEsyslogRes_setGroupConfig {
	int result;					/**<< результат выполненя */
}__ESYSLOG_PACKED_ATTR EsyslogRes_setGroupConfig_t;

static inline int Esyslog_SendRes_setGroupConfig(int fd, EsyslogCtrl_Handle_t to, int result) {
	EsyslogRes_setGroupConfig_t str;
	str.result = result;
	return 	Esyslog_SendHeader(fd, sizeof(str), MSG_RES_SETGRPCONFIG, -1, to)!=0 ||
			_Esyslog_Write(fd, &str, sizeof(str))!=sizeof(str);
}

static inline EsyslogCtrl_Handle_t Esyslog_RecvRes_setGroupConfig(int fd) {
	EsyslogRes_setGroupConfig_t str;
	if (_Esyslog_Read(fd, &str, sizeof(str))!=sizeof(str)) return -1;
	return str.result;
}

/*----------------------------------------------------------------------------*/

#endif /* SOCK_SETTINGS_H_ */
