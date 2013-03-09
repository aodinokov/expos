/*
 * esyslogctrl_client.c
 *
 *  Created on: Oct 18, 2010
 *      Author: mralex
 */
#include <stdlib.h> /*atoi*/
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h> /*strcmp*/

#include "esyslogctrl.h"

/* возможные аргументы */
static int	list = 		0;
static char *pid = 		NULL;
static char *module = 	NULL;
static char *group = 	NULL;
static char *mask = 	NULL;

static void _show_help(char * progname) {

	static char * _help_msg = "\nUsage:\n"
		"	%s --help		вывод подсказки\n"
		"	%s --list		вывод всех доступных процессов\n"
		"	%s --pid <pid> [--module <moduleName>][--group <groupName>]	вывод настроек групп процесса\n"
		"	%s --pid <pid> --module <moduleName> --group <groupName> --mask <mask>	настройка группы процесса\n"
		"\n"
		"Options:\n"
		"	--help\t\t-h	опция вывода подсказки\n"
		"	--list\t\t-l	опция вывода списка процессов\n"
		"	--pid\t\t-p	идентификатор процесса\n"
		"	--module\t-d	имя модуля\n"
		"	--group\t\t-g	имя группы\n"
		"	--mask\t\t-m	маска [+/-]{dinweclm}|{a}\n"
		"				использование +/- означает модификацию текущего значения\n"
		"				dinweclma == Debug Info Notice Warning Error Critical aLert eMergency All.\n"
		"\n";

	printf(_help_msg, progname, progname, progname, progname);
}

static int _ConvertMask(char *mask, char * pSign, ESyslog_MsgImportanceMask_t *pMask) {

	int i;
	*pSign =' ';	/*not inited*/
	*pMask = 0;

	if (!mask[0])
		return -1;

	if (mask[0]=='+'){
		*pSign='+';
		mask++;
	}else
	if (mask[0]=='-'){
		*pSign='-';
		mask++;
	}
	/*ok*/
	for (i = 0 ; mask[i]; i++) {
		switch (mask[i]) {
		case 'a': (*pMask) = ESYSLOG_LVL_DEBUG; break;
		case 'd': (*pMask) |= ESYSLOG_IMP_MASK(esgiDebug); 	break;
		case 'i': (*pMask) |= ESYSLOG_IMP_MASK(esgiInfo); 	break;
		case 'n': (*pMask) |= ESYSLOG_IMP_MASK(esgiNotice); break;
		case 'w': (*pMask) |= ESYSLOG_IMP_MASK(esgiWarn); 	break;
		case 'e': (*pMask) |= ESYSLOG_IMP_MASK(esgiErr); 	break;
		case 'c': (*pMask) |= ESYSLOG_IMP_MASK(esgiCrit); 	break;
		case 'l': (*pMask) |= ESYSLOG_IMP_MASK(esgiAlert); 	break;
		case 'm': (*pMask) |= ESYSLOG_IMP_MASK(esgiEmerg); 	break;
		default:
			return -1;
		}
	}
	return 0;
}

static char* _sprintMask(char*buf, ESyslog_MsgImportanceMask_t mask) {
	int i=0;

#if 0
	if (mask == ESYSLOG_LVL_DEBUG) {
		buf[i++]='a';
		buf[i++]=0;
		return buf;
	}
	if (mask & ESYSLOG_IMP_MASK(esgiEmerg)) buf[i++]='m';
	if (mask & ESYSLOG_IMP_MASK(esgiAlert)) buf[i++]='l';
	if (mask & ESYSLOG_IMP_MASK(esgiCrit)) buf[i++]='c';
	if (mask & ESYSLOG_IMP_MASK(esgiErr)) buf[i++]='e';
	if (mask & ESYSLOG_IMP_MASK(esgiWarn)) buf[i++]='w';
	if (mask & ESYSLOG_IMP_MASK(esgiNotice)) buf[i++]='n';
	if (mask & ESYSLOG_IMP_MASK(esgiInfo)) buf[i++]='i';
	if (mask & ESYSLOG_IMP_MASK(esgiDebug)) buf[i++]='d';
#else

	if (mask & ESYSLOG_IMP_MASK(esgiEmerg)) buf[i++]='m'; else buf[i++]= '-';
	if (mask & ESYSLOG_IMP_MASK(esgiAlert)) buf[i++]='l'; else buf[i++]= '-';
	if (mask & ESYSLOG_IMP_MASK(esgiCrit)) buf[i++]='c'; else buf[i++]= '-';
	if (mask & ESYSLOG_IMP_MASK(esgiErr)) buf[i++]='e'; else buf[i++]= '-';
	if (mask & ESYSLOG_IMP_MASK(esgiWarn)) buf[i++]='w'; else buf[i++]= '-';
	if (mask & ESYSLOG_IMP_MASK(esgiNotice)) buf[i++]='n'; else buf[i++]= '-';
	if (mask & ESYSLOG_IMP_MASK(esgiInfo)) buf[i++]='i'; else buf[i++]= '-';
	if (mask & ESYSLOG_IMP_MASK(esgiDebug)) buf[i++]='d'; else buf[i++]= '-';
#endif
	buf[i++]=0;

	return buf;
}

static int _setMask(void) {
	char sign = ' ';
	ESyslog_MsgImportanceMask_t impMask = ESYSLOG_LVL_NONE;

	if (_ConvertMask(mask, &sign, &impMask)!=0) {
		printf("invalid mask %s\n", mask);
		return -1;
	}
	int result = 1;
	int i;
	pid_t p_id;
	int count;
	EsyslogCtrl_Handle_t 		handle;
	EsyslogCtrl_ProcessInfo_t 	info;
	ESyslog_ModuleId_t 			modCount;
	ESyslog_GroupId_t			grpCount;
	ESyslog_ModuleName_t		modName;
	ESyslog_Group_t				grp;
	ESyslog_ModuleId_t 	j;
	ESyslog_GroupId_t	k;

	p_id = atoi(pid);
	count = esyslogctrl_getProcessCount();
	for (i = 0 ; i < count ; i++) {
		if (	(handle = esyslogctrl_getProcessInfo(i, &info))!=-1 &&
				info.pid == p_id
				) {
			modCount = esyslogctrl_getModuleCount(handle);
			for (j=0;j<modCount; j++) {
				esyslogctrl_getModuleName(handle,j, &modName);
				grpCount = esyslogctrl_getGroupCount(handle,j);
				for (k=0;k<grpCount;k++) {
					if (esyslogctrl_getGroup(handle,j,k, &grp)==0){
						if ((!module || strcmp(module, modName)==0) && (!group || strcmp(group, grp.name)==0) ) {
							if (sign == '+')grp.config.mask |= impMask;
							else if (sign == '-')grp.config.mask &= ~impMask;
							else grp.config.mask = impMask;
							if (esyslogctrl_setGroupConfig(handle,j,k, &grp.config)!=0){
								printf("error to set mask");
							}
							result = 0;
							if (module && group)
								return result;
						}
					}
				}
			}
		}
	}

	if (module && group)
		printf ("Process %d module %s group %s not found\n", (int) p_id, module, group);
	return 1;
}

static int _showPid() {
	int i;
	char buf[30];
	pid_t p_id;
	int count;
	EsyslogCtrl_Handle_t 		handle;
	EsyslogCtrl_ProcessInfo_t 	info;
	ESyslog_ModuleId_t 			modCount;
	ESyslog_GroupId_t			grpCount;
	ESyslog_ModuleName_t		modName;
	ESyslog_Group_t				grp;
	ESyslog_ModuleId_t 	j;
	ESyslog_GroupId_t	k;

	p_id = atoi(pid);
	count = esyslogctrl_getProcessCount();
	for (i = 0 ; i < count ; i++) {
		if (	(handle = esyslogctrl_getProcessInfo(i, &info))!=-1 &&
				info.pid == p_id
				) {
#if 0
			/*show filters*/
			if (module || group) {
				printf("using filter:");
				if (module) printf(" module==%s", module);
				if (group) printf(" group==%s", group);
				printf("\n");
			}
#endif
			/*found - show info*/
			printf("%-25s %-40s %-10s\n", "module", "group", "mask");
			modCount = esyslogctrl_getModuleCount(handle);
			if (modCount>0) {
				for (j=0;j<modCount; j++) {
					esyslogctrl_getModuleName(handle,j, &modName);
					grpCount = esyslogctrl_getGroupCount(handle,j);
					for (k=0;k<grpCount;k++) {
						if (esyslogctrl_getGroup(handle,j,k, &grp)==0){
							if (	(!module || strcmp(module, modName)==0) &&
									(!group || strcmp(group, grp.name)==0) )
							printf("%-25s %-40s %-10s\n", modName, grp.name, _sprintMask(buf, grp.config.mask));
						}
					}
				}
			}
			return 0;
		}
	}

	printf ("Process %d not found\n", (int) p_id);
	return 1;
}

static int _showlist() {
	int i;
	int count = esyslogctrl_getProcessCount();

	printf("%8s %8s %s\n", "PID", "HANDLE", "COMMAND");

	for (i = 0 ; i < count ; i++) {
		EsyslogCtrl_Handle_t res;
		EsyslogCtrl_ProcessInfo_t info;
		if ((res = esyslogctrl_getProcessInfo(i, &info))!=-1) {
			printf("%8d %08x %s\n", info.pid, res, info.command);
		}
	}

	return 0;
}

static int parse(int argc, char **argv) {
	int rez;
	int option_index;
	/*cmd line parsing data*/
	const char* short_options = "hlp:d:g:m:";
#ifdef _GNU_SOURCE
	const struct option long_options[] =
	{
		{"help",no_argument,NULL,'h'},
		{"list",no_argument,NULL,'l'},
		{"pid",required_argument,NULL,'p'},
		{"module",required_argument,NULL,'d'},
		{"group",required_argument,NULL,'g'},
		{"mask",required_argument,NULL,'m'},
		{NULL,0,NULL,0}
	};

	while ((rez=getopt_long(argc, argv, short_options,
			long_options,&option_index))!=-1){
#else
	while ((rez=getopt(argc, argv, short_options))!=-1){
#endif
		switch(rez){
			case 'l':{
				list = 1;
				break;
			}
			case 'p': pid = optarg; break;
			case 'd': module = optarg; break;
			case 'g': group = optarg; break;
			case 'm': mask = optarg; break;
			case 'h':
			default:
				_show_help(argv[0]);
				return -1;
		}
	}

	if (optind < argc) {
		printf("Unknown parameter: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		printf ("\n");
		return 1;
	}
	return 0;
}

int main(int argc, char **argv) {

	if (argc == 1) {
		_show_help(argv[0]);
		return 1;
	}
	if (parse(argc, argv)!=0)
		return 1;

	/**try to determine mode*/
	if(list && (pid || module || group || mask)) {
		printf("Invalid usage of --list\n");
		return 1;
	}
	if ((module || group) && !pid) {
		printf("Invalid usage of --module or --group without --pid\n");
		return 1;
	}
//	if(mask && (!module || !group)) {
//		printf("Invalid usage of --mask without --module or --group\n");
//		return 1;
//	}
	if (mask)
		return _setMask();
	if (pid)
		return _showPid();
	if (list)
		return _showlist();

	return 0;
}
