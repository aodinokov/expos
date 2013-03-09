/*
 * libesyslog_impl.h
 *
 *  Created on: Oct 12, 2010
 *      Author: mralex
 */

#ifndef LIBESYSLOG_IMPL_H_
#define LIBESYSLOG_IMPL_H_

#include "esyslog_helper.h"

/* используем CMN_list, но переименуем его, т.к. он переименован в нашем header*/
#define ESYSLOG_list_head CMN_list_head

#include "esyslog.h"

#ifndef ESYSLOG_SIMPLE_BACKEND
#include "esyslog_be.h"

typedef struct tagESyslogEngine_Backend {
	/** указатель на созданный backend */
	ESyslog_Backend_t *pBackend;

	/** структура для объединения в backendLst */
	struct CMN_list_head 	list;
}ESyslogEngine_Backend_t;

#endif

typedef struct tagESyslogEngine{

	/** количество модулей в системе (для ускорения) */
	ESyslog_ModuleId_t 		moduleCnt;
	/** список модулей */
	struct CMN_list_head 	moduleLst;

#ifndef ESYSLOG_SIMPLE_BACKEND
	/*TODO: mutex??? - тогда придется инициализировать в функции */
	/** список бэкэндов */
	struct CMN_list_head 	backendLst;
#endif

}ESyslogEngine_t;

#ifndef ESYSLOG_SIMPLE_BACKEND
#define ESYSLOGENGINE_INIT(name) {moduleCnt: 0, CMN_LIST_HEAD_INIT(name.moduleLst) ,CMN_LIST_HEAD_INIT(name.backendLst) }
#else
#define ESYSLOGENGINE_INIT(name) {moduleCnt: 0, CMN_LIST_HEAD_INIT(name.moduleLst)}
#endif /*ESYSLOG_SIMPLE_BACKEND*/

#endif /* LIBESYSLOG_IMPL_H_ */
