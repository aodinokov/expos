/*
 * esyslog_utils.h
 *
 *  Created on: Oct 12, 2010
 *      Author: mralex
 */

#ifndef LIBESYSLOG_UTILS_H_
#define LIBESYSLOG_UTILS_H_

#include "esyslog.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#define __LOG_FUNCTION__ __PRETTY_FUNCTION__
#else /* ! __cplusplus*/
#ifdef __GNUC__
#define __LOG_FUNCTION__ __func__
#endif
#endif /*  #ifdef __cplusplus */


/** вывод с заголовком */
#define ESYSLOG_H(_name, _importance, _fmt, ...) \
	ESYSLOG(_name,_importance, __FILE__ ":%d:%s: " _fmt, __LINE__, __LOG_FUNCTION__, ## __VA_ARGS__ )

/** вход в функцию */
#define ESYSLOG_CI(_name, _importance, _arg...) \
	ESYSLOG(_name, _importance, "(IN)"__LOG_FUNCTION__ _arg)

/** выход из функции */
#define ESYSLOG_CO(_name, _importance, _arg...) \
	ESYSLOG(_name, _importance, "(OUT)"__LOG_FUNCTION__ _arg)

#define ESYSLOG_EMERG(_name, _arg...) ESYSLOG(_name, esgiEmerg, _arg)
#define ESYSLOG_ALERT(_name, _arg...) ESYSLOG(_name, esgiAlert, _arg)
#define ESYSLOG_CRIT(_name, _arg...) ESYSLOG(_name, esgiCrit, _arg)
#define ESYSLOG_ERR(_name, _arg...) ESYSLOG(_name, esgiErr, _arg)
#define ESYSLOG_WARN(_name, _arg...) ESYSLOG(_name, esgiWarn, _arg)
#define ESYSLOG_NOTICE(_name, _arg...) ESYSLOG(_name, esgiNotice, _arg)
#define ESYSLOG_INFO(_name, _arg...) ESYSLOG(_name, esgiInfo, _arg)
#define ESYSLOG_DEBUG(_name, _arg...) ESYSLOG(_name, esgiDebug, _arg)

#define ESYSLOG_HEMERG(_name, _arg...) ESYSLOG_H(_name, esgiEmerg, _arg)
#define ESYSLOG_HALERT(_name, _arg...) ESYSLOG_H(_name, esgiAlert, _arg)
#define ESYSLOG_HCRIT(_name, _arg...) ESYSLOG_H(_name, esgiCrit, _arg)
#define ESYSLOG_HERR(_name, _arg...) ESYSLOG_H(_name, esgiErr, _arg)
#define ESYSLOG_HWARN(_name, _arg...) ESYSLOG_H(_name, esgiWarn, _arg)
#define ESYSLOG_HNOTICE(_name, _arg...) ESYSLOG_H(_name, esgiNotice, _arg)
#define ESYSLOG_HINFO(_name, _arg...) ESYSLOG_H(_name, esgiInfo, _arg)
#define ESYSLOG_HDEBUG(_name, _arg...) ESYSLOG_H(_name, esgiDebug, _arg)



#ifdef	__cplusplus
}
#endif

#endif /* LIBESYSLOG_UTILS_H_ */
