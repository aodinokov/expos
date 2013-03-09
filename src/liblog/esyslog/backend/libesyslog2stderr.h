/*
 * libesyslog2stderr.h
 *
 *  Created on: Oct 20, 2010
 *      Author: mralex
 */

#ifndef LIBESYSLOG2STDERR_H_
#define LIBESYSLOG2STDERR_H_

#include "esyslog.h"

struct tagESyslog_Backend * ESyslog_StderrBackendCreate(void * pData);

#endif /* LIBESYSLOG2STDERR_H_ */
