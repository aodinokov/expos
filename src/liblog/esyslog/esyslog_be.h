/*
 * esyslog_be.h
 *
 *  Created on: Oct 8, 2010
 *      Author: mralex
 */

#ifndef LIBESYSLOG_BE_H_
#define LIBESYSLOG_BE_H_

#include "esyslog.h"
#include <stdarg.h>	/*va_list*/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* back-end API */
/******************************************************************************/
typedef void			(*onESyslog_BackendSyslog_t)	(struct tagESyslog_Backend* pBackend, char* moduleName, ESyslog_Group_t* pGroup, ESyslog_GroupImportance_t importance, const char *format, va_list ap);
typedef void			(*onESyslog_BackendDestroy_t)	(struct tagESyslog_Backend* pBackend);

typedef struct tagESyslog_Backend{
	onESyslog_BackendSyslog_t 	onSyslog;
	onESyslog_BackendDestroy_t 	onDestroy;

}ESyslog_Backend_t;


#ifdef	__cplusplus
}
#endif

#endif /* LIBESYSLOG_BE_H_ */
