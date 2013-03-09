/*
 * libesyslog2file.h
 *
 *  Created on: Oct 20, 2010
 *      Author: mralex
 */

#ifndef LIBESYSLOG2FILE_H_
#define LIBESYSLOG2FILE_H_

#include "esyslog.h"

struct tagESyslog_Backend * ESyslog_FileBackendCreate(void * pData);

#endif /* LIBESYSLOG2FILE_H_ */
