/*
 * esyslogctrl.h
 *
 *  Created on: Oct 13, 2010
 *      Author: mralex
 */

#ifndef ESYSLOGCTRL_H_
#define ESYSLOGCTRL_H_

#include "esyslog_types.h"

/** Process Id */
typedef unsigned int 	EsyslogCtrl_Handle_t;
typedef char 			EsyslogCtrlHostName_t[100];
typedef char 			EsyslogCtrlProcessCommand_t[100];
typedef struct 	tagEsyslogCtrl_ProcessInfo {
	EsyslogCtrlHostName_t		host;
	pid_t						pid;
	EsyslogCtrlProcessCommand_t	command;
}EsyslogCtrl_ProcessInfo_t;

int 					esyslogctrl_getProcessCount	(void);
EsyslogCtrl_Handle_t 	esyslogctrl_getProcessInfo	(int Id, EsyslogCtrl_ProcessInfo_t* pProcessInfo);
ESyslog_ModuleId_t		esyslogctrl_getModuleCount	(EsyslogCtrl_Handle_t to);
int 					esyslogctrl_getModuleName	(EsyslogCtrl_Handle_t to, ESyslog_ModuleId_t moduleId, ESyslog_ModuleName_t *pName);
ESyslog_GroupId_t 		esyslogctrl_getGroupCount	(EsyslogCtrl_Handle_t to, ESyslog_ModuleId_t moduleId);
int 					esyslogctrl_getGroup		(EsyslogCtrl_Handle_t to, ESyslog_ModuleId_t moduleId, ESyslog_GroupId_t groupId, ESyslog_Group_t *pGroup);
int 					esyslogctrl_setGroupConfig	(EsyslogCtrl_Handle_t to, ESyslog_ModuleId_t moduleId, ESyslog_GroupId_t groupId, ESyslog_GroupConfig_t	*pGroupConfig);


#endif /* ESYSLOGCTRL_H_ */
