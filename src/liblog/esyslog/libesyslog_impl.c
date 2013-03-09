/*
 * libesyslog_impl.c
 *
 *  Created on: Oct 8, 2010
 *      Author: mralex
 */


/******************************************************************************/

#include "libesyslog_impl.h"

static ESyslogEngine_t st_ESyslogEngine =
	ESYSLOGENGINE_INIT(st_ESyslogEngine);

/******************************************************************************/
#ifndef ESYSLOG_SIMPLE_BACKEND
static void _ESyslogEngine_AddBacked(ESyslogEngine_t * pESyslogEngine, ESyslogEngine_Backend_t *pEngine_Backend){
	CMN_list_add_tail(&pEngine_Backend->list, &pESyslogEngine->backendLst);
}
/*----------------------------------------------------------------------------*/
static void _ESyslogEngine_RemBacked(ESyslogEngine_t * pESyslogEngine, ESyslogEngine_Backend_t *pEngine_Backend){
	CMN_list_del(&pEngine_Backend->list);
}
#endif /*ESYSLOG_SIMPLE_BACKEND*/
/*----------------------------------------------------------------------------*/
static void _ESyslogEngine_AddImpl(ESyslogEngine_t * pESyslogEngine, ESyslog_Impl_t* pImpl){
	CMN_list_add_tail((struct CMN_list_head *)&pImpl->_u.moduleImpl.list, &pESyslogEngine->moduleLst);
	pESyslogEngine->moduleCnt++;
}
/*----------------------------------------------------------------------------*/
static void _ESyslogEngine_RemImpl(ESyslogEngine_t * pESyslogEngine, ESyslog_Impl_t* pImpl){
	pESyslogEngine->moduleCnt--;
	CMN_list_del((struct CMN_list_head *)&pImpl->_u.moduleImpl.list);
}
/*----------------------------------------------------------------------------*/
static ESyslog_ModuleImpl_t * _ESyslogEngine_GetModuleImpl(ESyslogEngine_t * pESyslogEngine,  ESyslog_ModuleId_t moduleId) {
	ESyslog_ModuleId_t i = 0;
	struct CMN_list_head * pCur;

	CMN_list_for_each(pCur, &pESyslogEngine->moduleLst) {
		ESyslog_ModuleImpl_t * pModuleImpl = CMN_GetContainer(pCur, ESyslog_ModuleImpl_t, list);
		if ((i++) == moduleId)
			return pModuleImpl;
	}
	return NULL;
}
/******************************************************************************/
#ifndef ESYSLOG_SIMPLE_BACKEND
void 	_vesyslog(ESyslog_GroupImpl_t* pGroupImpl,
					ESyslog_GroupImportance_t importance, __const char *__fmt, va_list ap) {
	struct CMN_list_head * pCur;

	if ( ESYSLOG_IMP_ENABLED(pGroupImpl->group.config.mask, importance)) {
		CMN_list_for_each(pCur, &st_ESyslogEngine.backendLst) {
			ESyslogEngine_Backend_t *pEngine_Backend = CMN_GetContainer(pCur, ESyslogEngine_Backend_t, list);
			ESyslog_Backend_t *pBackend = pEngine_Backend->pBackend;
			va_list t_ap;
			va_copy(t_ap, ap);

			if (pBackend && pBackend->onSyslog)
				pBackend->onSyslog(pBackend,
						pGroupImpl->pModule?pGroupImpl->pModule->name: NULL, &pGroupImpl->group, importance, __fmt, t_ap);
		}
	}
}
/*----------------------------------------------------------------------------*/
void 	_esyslog(ESyslog_GroupImpl_t* pGroupImpl,
					ESyslog_GroupImportance_t importance, __const char *__fmt, ...) {
		va_list ap;
		va_start(ap, __fmt);
		_vesyslog(pGroupImpl, importance, __fmt, ap);
		va_end(ap);
}
/*----------------------------------------------------------------------------*/
void	_esyslog_dump(ESyslog_GroupImpl_t* pGroupImpl,
		ESyslog_GroupImportance_t importance, void* ptr, int size) {
	if ( ESYSLOG_IMP_ENABLED(pGroupImpl->group.config.mask, importance)) {
		/* TODO: if (!supported by backend) { */
		char buf[1024];
		int p = 0, s = 0;

		while( p<size ) {
			if ((p & 0xf) == 0){
				if ( p!=0 ) _esyslog(pGroupImpl, importance, "%s", buf);
				s = sprintf(buf, "%04x:", p);
			}

			s += sprintf(buf+s, " %02x", (int)((unsigned char*)ptr)[p]);
			++p;
		}
		if ((p & 0xf) != 0) _esyslog(pGroupImpl, importance, "%s", buf);
		/* TODO: } else { ... } */
	}
}
#endif /*ESYSLOG_SIMPLE_BACKEND*/
/******************************************************************************/
static ESyslog_GroupImpl_t * _ESyslogModuleImpl_GetGroupImpl(ESyslog_ModuleImpl_t *pModuleImpl, ESyslog_GroupId_t groupId) {
	ESyslog_GroupId_t i = 0;
	struct CMN_list_head * pCur;

	CMN_list_for_each(pCur, (struct CMN_list_head *)&pModuleImpl->grpList) {
		ESyslog_GroupImpl_t * pGroupImpl = CMN_GetContainer(pCur, ESyslog_GroupImpl_t, list);
		if ((i++) == groupId)
			return pGroupImpl;
	}
	return NULL;
}
/******************************************************************************/
#ifndef ESYSLOG_SIMPLE_BACKEND

void * esyslog_registerBackend		(struct tagESyslog_Backend *pBackend){
	ESyslogEngine_Backend_t *pEngine_Backend;

	if (!pBackend) {	/*неправильный параметр*/
		return NULL;
	}

	if (!( pEngine_Backend = (ESyslogEngine_Backend_t *)OS_ALLOC(sizeof(ESyslogEngine_Backend_t)))) {
		return NULL;
	}

	/**/
	pEngine_Backend->pBackend = pBackend;

	_ESyslogEngine_AddBacked(&st_ESyslogEngine, pEngine_Backend);

	return (void*)pEngine_Backend;
}
/*----------------------------------------------------------------------------*/
int		esyslog_unregisterBackend	(void *pBackendHandle) {
	ESyslogEngine_Backend_t *pEngine_Backend = (ESyslogEngine_Backend_t *)pBackendHandle;

	if (!pBackendHandle)
		return -1;

	_ESyslogEngine_RemBacked(&st_ESyslogEngine, pEngine_Backend);
	OS_FREE(pEngine_Backend);

	return 0;
}
#endif /* ESYSLOG_SIMPLE_BACKEND*/
/******************************************************************************/
static void _esyslog_findAndAddGroups(ESyslog_Impl_t* pImpl) {
#ifdef ESYSLOG_NOSECTION
	/*not implemented*/
#else
	ESyslog_Impl_t * pGroupImpl = pImpl;

	if (	pImpl->type != eitModule ||
			pImpl->_u.moduleImpl.usageCounter != 0)
		return;

	while (1) {
		ESyslog_Impl_t * pTmp = pGroupImpl - 1;
		if (!__ESYSLOG_IMPLMAGICCHECK(pTmp))
			break;
		pGroupImpl = pTmp;
	}

	/*ok - pGroupImplBegin содержит начало секции*/
	pImpl->_u.moduleImpl.grpCount = 0;
	CMN_INIT_LIST_HEAD((struct CMN_list_head *)&pImpl->_u.moduleImpl.grpList);

	while (__ESYSLOG_IMPLMAGICCHECK(pGroupImpl)) {
		if (	pGroupImpl->type == eitGroup &&
				!pGroupImpl->_u.groupImpl.pModule) {
			pGroupImpl->_u.groupImpl.pModule = &pImpl->_u.moduleImpl;

			CMN_list_add_tail((struct CMN_list_head *)&pGroupImpl->_u.groupImpl.list, (struct CMN_list_head *)&pImpl->_u.moduleImpl.grpList);

			pImpl->_u.moduleImpl.grpCount++;
		}
		pGroupImpl++;
	}
}
#endif
/*----------------------------------------------------------------------------*/
int 	_esyslog_initImpl	(ESyslog_Impl_t* pImpl) {

	if (	pImpl->type != eitModule)
		return -1;

	if (pImpl->_u.moduleImpl.usageCounter == 0) {
		/*инициализируем секцию со всеми группами*/
		_esyslog_findAndAddGroups(pImpl);
		/*добавить в общий список*/
		_ESyslogEngine_AddImpl(&st_ESyslogEngine, pImpl);
	}

	pImpl->_u.moduleImpl.usageCounter++;

	return 0;
}
/*----------------------------------------------------------------------------*/
void 	_esyslog_uninitImpl	(ESyslog_Impl_t* pImpl){

	if (	pImpl->type != eitModule ||
			pImpl->_u.moduleImpl.usageCounter == 0)
		return;

	if ((pImpl->_u.moduleImpl.usageCounter--) == 1){
		_ESyslogEngine_RemImpl(&st_ESyslogEngine, pImpl);
	}
}
/******************************************************************************/
ESyslog_ModuleId_t esyslog_getModuleCount(void) {
	return st_ESyslogEngine.moduleCnt;
}
/*----------------------------------------------------------------------------*/
int esyslog_getModuleName(ESyslog_ModuleId_t moduleId, ESyslog_ModuleName_t *pName) {
	ESyslog_ModuleImpl_t * pModuleImpl;

	if (!pName)
		return -1;
	if (!(pModuleImpl = _ESyslogEngine_GetModuleImpl(&st_ESyslogEngine, moduleId)))
		return -1;

	memcpy(&((*pName)[0]), &(pModuleImpl->name[0]), sizeof(ESyslog_ModuleName_t));
	return 0;
}
/*----------------------------------------------------------------------------*/
ESyslog_GroupId_t esyslog_getGroupCount(ESyslog_ModuleId_t moduleId) {
	ESyslog_ModuleImpl_t * pModuleImpl;

	if (!(pModuleImpl = _ESyslogEngine_GetModuleImpl(&st_ESyslogEngine, moduleId)))
		return -1;

	return pModuleImpl->grpCount;
}
/*----------------------------------------------------------------------------*/
int esyslog_getGroup(ESyslog_ModuleId_t moduleId, ESyslog_GroupId_t groupId, ESyslog_Group_t *pGroup) {
	ESyslog_ModuleImpl_t * pModuleImpl;
	ESyslog_GroupImpl_t * pGroupImpl;

	if (!pGroup) { /*invalid argument*/
		return -1;
	}

	if (!(pModuleImpl = _ESyslogEngine_GetModuleImpl(&st_ESyslogEngine, moduleId))) {
		return -1;
	}

	if (!(pGroupImpl = _ESyslogModuleImpl_GetGroupImpl(pModuleImpl, groupId))) {
		return -1;
	}

	memcpy(pGroup, &pGroupImpl->group, sizeof(ESyslog_Group_t));
	return 0;
}
/*----------------------------------------------------------------------------*/
int esyslog_setGroupConfig(ESyslog_ModuleId_t moduleId, ESyslog_GroupId_t groupId, ESyslog_GroupConfig_t	*pGroupConfig) {
	ESyslog_ModuleImpl_t * pModuleImpl;
	ESyslog_GroupImpl_t * pGroupImpl;

	if (!pGroupConfig) { /*invalid argument*/
		return -1;
	}

	if (!(pModuleImpl = _ESyslogEngine_GetModuleImpl(&st_ESyslogEngine, moduleId))) {
		return -1;
	}

	if (!(pGroupImpl = _ESyslogModuleImpl_GetGroupImpl(pModuleImpl, groupId))) {
		return -1;
	}

	memcpy(&pGroupImpl->group.config, pGroupConfig, sizeof(ESyslog_GroupConfig_t));
	return 0;
}
/*----------------------------------------------------------------------------*/
