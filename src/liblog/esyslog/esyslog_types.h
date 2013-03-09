/*
 * esyslog_types.h
 *
 *  Created on: Oct 13, 2010
 *      Author: mralex
 */

#ifndef LIBESYSLOG_TYPES_H_
#define LIBESYSLOG_TYPES_H_

/*
 * типы, используемые в API
 */
/******************************************************************************/
/* общие типы данных */
/******************************************************************************/

/** уникальный идентификатор группы */
typedef int 		ESyslog_ModuleId_t;
typedef int 		ESyslog_GroupId_t;

/** имя группы логирования */
typedef char 		ESyslog_GroupName_t[40];
/** имя модуля логирования */
typedef ESyslog_GroupName_t ESyslog_ModuleName_t;

/** важность сообщения */
typedef enum enumESyslog_MsgImportance{

	esgiEmerg 	= 0,	/**<< system is unusable */
	esgiAlert 	= 1,	/**<< action must be taken immediately */
	esgiCrit 	= 2,	/**<< critical conditions */
	esgiErr 	= 3,	/**<< error conditions */
	esgiWarn 	= 4,	/**<< warning conditions */
	esgiNotice 	= 5,	/**<< normal, but significant, condition */
	esgiInfo 	= 6,	/**<< informational message */
	esgiDebug 	= 7,	/**<< debug-level message*/

}ESyslog_GroupImportance_t;

/** маска важности сообщения */
typedef unsigned int	ESyslog_MsgImportanceMask_t;

/* таким образом солгасуются важность и его маска*/
#define ESYSLOG_IMP_MASK(importance) ((1<<(importance)))
/* так проверяется, что маска есть */
#define ESYSLOG_IMP_ENABLED(mask, importance) (mask & ESYSLOG_IMP_MASK(importance))

/* levels b00000000 b00000001 b00000011 b00000111 ...*/
#define	ESYSLOG_LVL_NONE 	(0)
#define	ESYSLOG_LVL_EMERG 	(ESYSLOG_LVL_NONE	| ESYSLOG_IMP_MASK(esgiEmerg))
#define	ESYSLOG_LVL_ALERT 	(ESYSLOG_LVL_EMERG 	| ESYSLOG_IMP_MASK(esgiAlert))
#define	ESYSLOG_LVL_CRIT 	(ESYSLOG_LVL_ALERT 	| ESYSLOG_IMP_MASK(esgiCrit))
#define	ESYSLOG_LVL_ERR 	(ESYSLOG_LVL_CRIT 	| ESYSLOG_IMP_MASK(esgiErr))
#define	ESYSLOG_LVL_WARN 	(ESYSLOG_LVL_ERR 	| ESYSLOG_IMP_MASK(esgiWarn))
#define	ESYSLOG_LVL_NOTICE 	(ESYSLOG_LVL_WARN 	| ESYSLOG_IMP_MASK(esgiNotice))
#define	ESYSLOG_LVL_INFO 	(ESYSLOG_LVL_NOTICE	| ESYSLOG_IMP_MASK(esgiInfo))
#define	ESYSLOG_LVL_DEBUG 	(ESYSLOG_LVL_INFO	| ESYSLOG_IMP_MASK(esgiDebug))

/** настройки группы логирования */
typedef struct tagESyslog_GroupConfig{

	/** маска важности */
	ESyslog_MsgImportanceMask_t mask;

}ESyslog_GroupConfig_t;

/** группа логирования */
typedef struct tagESyslog_Group{

	/**имя группы*/
	ESyslog_GroupName_t		name;

	/** конфигурация группы*/
	ESyslog_GroupConfig_t	config;

}ESyslog_Group_t;


#endif /* LIBESYSLOG_TYPES_H_ */
