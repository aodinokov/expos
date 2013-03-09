/*
 * log.h
 *
 *  Created on: Jan 27, 2013
 *      Author: mralex
 */

#ifndef LOG_H_
#define LOG_H_

#ifdef HAVE_CONFIG
#include "config.h"
#endif /*HAVE_CONFIG*/

#ifndef LOG_ENGINE

/* log importancy */
#define LOG_IMP_EMERG	(0)
#define LOG_IMP_ALERT	(1)
#define LOG_IMP_CRIT	(2)
#define LOG_IMP_ERR 	(3)
#define LOG_IMP_WARN 	(4)
#define LOG_IMP_NOTICE 	(5)
#define LOG_IMP_INFO 	(6)
#define LOG_IMP_DEBUG 	(7)

#define _LOG_IMP_MASK(importance) ((1<<(importance)))

/* log level */
#define	LOG_LVL_NONE 	(0)
#define	LOG_LVL_EMERG 	(LOG_LVL_NONE	| _LOG_IMP_MASK(LOG_IMP_EMERG))
#define	LOG_LVL_ALERT 	(LOG_LVL_EMERG 	| _LOG_IMP_MASK(LOG_IMP_ALERT))
#define	LOG_LVL_CRIT 	(LOG_LVL_ALERT 	| _LOG_IMP_MASK(LOG_IMP_CRIT))
#define	LOG_LVL_ERR 	(LOG_LVL_CRIT 	| _LOG_IMP_MASK(LOG_IMP_ERR))
#define	LOG_LVL_WARN 	(LOG_LVL_ERR 	| _LOG_IMP_MASK(LOG_IMP_WARN))
#define	LOG_LVL_NOTICE 	(LOG_LVL_WARN 	| _LOG_IMP_MASK(LOG_IMP_NOTICE))
#define	LOG_LVL_INFO 	(LOG_LVL_NOTICE	| _LOG_IMP_MASK(LOG_IMP_INFO))
#define	LOG_LVL_DEBUG 	(LOG_LVL_INFO	| _LOG_IMP_MASK(LOG_IMP_DEBUG))

#define	LOG_MODULE_DECLARE(_name)
#define	LOG_GROUP_DECLARE(_name)
#define	LOG_MODULE_CREATE(_name)
#define	LOG_GROUP_CREATE_LVL(_name, _level)
#define	LOG_GROUP_CREATE(_name) LOG_GROUP_CREATE_LVL(_name, LOG_LVL_DEFAULT)

#define LOG_GROUP_GET_LVL(_name) LOG_LVL_DEFAULT
#define LOG_GROUP_SET_LVL(_name, _level)

#define LOG_MODULE_INIT(_name)
#define LOG_MODULE_UNINIT(_name)

#define LOG(_name, _importance, _arg...)
#define LOG_V(_name, _importance, _fmt, _ap)
#else /*LOG_ENGINE*/
#if LOG_ENGINE == esyslog

#include "esyslog/esyslog.h"

/* log importancy */
#define LOG_IMP_EMERG	esgiEmerg
#define LOG_IMP_ALERT	esgiAlert
#define LOG_IMP_CRIT	esgiCrit
#define LOG_IMP_ERR 	esgiErr
#define LOG_IMP_WARN 	esgiWarn
#define LOG_IMP_NOTICE 	esgiNotice
#define LOG_IMP_INFO 	esgiInfo
#define LOG_IMP_DEBUG 	esgiDebug

/* log level */
#define	LOG_LVL_NONE 	ESYSLOG_LVL_NONE
#define	LOG_LVL_EMERG 	ESYSLOG_LVL_EMERG
#define	LOG_LVL_ALERT 	ESYSLOG_LVL_ALERT
#define	LOG_LVL_CRIT 	ESYSLOG_LVL_CRIT
#define	LOG_LVL_ERR 	ESYSLOG_LVL_ERR
#define	LOG_LVL_WARN 	ESYSLOG_LVL_WARN
#define	LOG_LVL_NOTICE 	ESYSLOG_LVL_NOTICE
#define	LOG_LVL_INFO 	ESYSLOG_LVL_INFO
#define	LOG_LVL_DEBUG 	ESYSLOG_LVL_DEBUG

#define	LOG_MODULE_DECLARE(_name) ESYSLOG_MODULE_DECLARE(_name)
#define	LOG_GROUP_DECLARE(_name) ESYSLOG_GROUP_DECLARE(_name)
#define	LOG_MODULE_CREATE(_name) ESYSLOG_MODULE_CREATE(_name)
#define	LOG_GROUP_CREATE_LVL(_name, _level) ESYSLOG_GROUP_CREATE_LVL(_name, _level)
#define	LOG_GROUP_CREATE(_name) LOG_GROUP_CREATE_LVL(_name, LOG_LVL_DEFAULT)

#define LOG_GROUP_GET_LVL(_name) ESYSLOG_GROUP_GET_LVL(_name)
#define LOG_GROUP_SET_LVL(_name, _level) ESYSLOG_GROUP_SET_LVL(_name, _level)

#define LOG_MODULE_INIT(_name) ESYSLOG_MODULE_INIT(_name)
#define LOG_MODULE_UNINIT(_name) ESYSLOG_MODULE_UNINIT(_name)

#define LOG(_name, _importance, _arg...) ESYSLOG(_name, _importance, _arg)
#define LOG_V(_name, _importance, _fmt, _ap) VESYSLOG(_name, _importance, _fmt, _ap)

#endif /*LOG_ENGINE == esyslog*/
#endif /*LOG_ENGINE*/

#ifndef LOG_LVL_DEFAULT
#define LOG_LVL_DEFAULT LOG_LVL_ERR
#endif

#ifdef __cplusplus
#define __LOG_FUNCTION__ __PRETTY_FUNCTION__
#else /* ! __cplusplus*/
#ifdef __GNUC__
#define __LOG_FUNCTION__ __func__
#endif
#endif /*  #ifdef __cplusplus */

#define LOG_H(_name, _importance, _fmt, ...) \
	LOG(_name,_importance, __FILE__ ":%d:%s: " _fmt, __LINE__, __LOG_FUNCTION__, ## __VA_ARGS__ )

#define LOG_EMERG(_name, _arg...) LOG(_name, LOG_IMP_EMERG, _arg)
#define LOG_ALERT(_name, _arg...) LOG(_name, LOG_IMP_ALERT, _arg)
#define LOG_CRIT(_name, _arg...) LOG(_name, LOG_IMP_CRIT, _arg)
#define LOG_ERR(_name, _arg...) LOG(_name, LOG_IMP_ERR, _arg)
#define LOG_WARN(_name, _arg...) LOG(_name, LOG_IMP_WARN, _arg)
#define LOG_NOTICE(_name, _arg...) LOG(_name, LOG_IMP_NOTICE, _arg)
#define LOG_INFO(_name, _arg...) LOG(_name, LOG_IMP_INFO, _arg)
#define LOG_DEBUG(_name, _arg...) LOG(_name, LOG_IMP_DEBUG, _arg)

#define LOG_EMERG_H(_name, _arg...) LOG_H(_name, LOG_IMP_EMERG, _arg)
#define LOG_ALERT_H(_name, _arg...) LOG_H(_name, LOG_IMP_ALERT, _arg)
#define LOG_CRIT_H(_name, _arg...) LOG_H(_name, LOG_IMP_CRIT, _arg)
#define LOG_ERR_H(_name, _arg...) LOG_H(_name, LOG_IMP_ERR, _arg)
#define LOG_WARN_H(_name, _arg...) LOG_H(_name, LOG_IMP_WARN, _arg)
#define LOG_NOTICE_H(_name, _arg...) LOG_H(_name, LOG_IMP_NOTICE, _arg)
#define LOG_INFO_H(_name, _arg...) LOG_H(_name, LOG_IMP_INFO, _arg)
#define LOG_DEBUG_H(_name, _arg...) LOG_H(_name, LOG_IMP_DEBUG, _arg)

#define LOG_V_EMERG(_name, _fmt, _ap) LOG_V(_name, LOG_IMP_EMERG, _fmt, _ap)
#define LOG_V_ALERT(_name, _fmt, _ap) LOG_V(_name, LOG_IMP_ALERT, _fmt, _ap)
#define LOG_V_CRIT(_name, _fmt, _ap) LOG_V(_name, LOG_IMP_CRIT, _fmt, _ap)
#define LOG_V_ERR(_name, _fmt, _ap) LOG_V(_name, LOG_IMP_ERR, _fmt, _ap)
#define LOG_V_WARN(_name, _fmt, _ap) LOG_V(_name, LOG_IMP_WARN, _fmt, _ap)
#define LOG_V_NOTICE(_name, _fmt, _ap) LOG_V(_name, LOG_IMP_NOTICE, _fmt, _ap)
#define LOG_V_INFO(_name, _fmt, _ap) LOG_V(_name, LOG_IMP_INFO, _fmt, _ap)
#define LOG_V_DEBUG(_name, _fmt, _ap) LOG_V(_name, LOG_IMP_DEBUG, _fmt, _ap)

#endif /* LOG_H_ */
