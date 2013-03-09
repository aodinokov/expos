/*
 * esyslog.h
 *
 *  Created on: Oct 12, 2010
 *      Author: mralex
 */

#ifndef _LIB_ESYSLOG_H_
#define _LIB_ESYSLOG_H_

#include "esyslog_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* control-plan API */
/******************************************************************************/
#ifndef ESYSLOG_SIMPLE_BACKEND
/** раняя декларация типа Backend */
struct tagESyslog_Backend;

/**
 * @brief добавить модуль логирования в esyslog
 * @param
 * 		pBackend - указатель backend
 * @return
 * 	значение - в случае успеха
 * 	NULL - в случае ошибки
 */
void* esyslog_registerBackend		(struct tagESyslog_Backend *pBackend);

/**
 * @brief удалить модуль логирования из esyslog
 * @param
 * 		pBackend - указатель на результат esyslog_registerBackend
 * @return
 * 	0 - в случае успеха
 * 	-1 - в случае ошибки
 */
int		esyslog_unregisterBackend	(void *pBackendHandle);
#else
#endif /*ESYSLOG_SIMPLE_BACKEND*/

/**
 * @brief выдать текущее количество групп в системе
 * @return количество зарегистрированных модулей в системе
 */
ESyslog_ModuleId_t esyslog_getModuleCount(void);

/**
 * @brief выдать имя модуля
 * @param
 * 	moduleId 	- номер модуля в системе
 *  pName		- указатель, куда надо сохранить имя модуля
 * @return
 * 		 0 - success
 * 		-1 - ошибка
 */
int esyslog_getModuleName(ESyslog_ModuleId_t moduleId, ESyslog_ModuleName_t *pName);


/**
 * @brief выдать текущее количество групп в модуле
 * @param
 * 	moduleId - номер модуля в модуле
 * @return количество групп в модуле
 */
ESyslog_GroupId_t esyslog_getGroupCount(ESyslog_ModuleId_t moduleId);

/**
 * @brief выдать информацию о группе по ее номеру
 * @param
 * 		moduleId - номер модуля в системе в диапазоне [0;esyslog_getModuleCount()-1]
 * 		groupId -  номер группы в диапазоне [0;esyslog_getGroupCount(moduleId)-1]
 * 		pGroup - указатель на структуру, куда сохранить информацию
 * @return
 * 	0 - в случае успеха
 * 	-1 - в случае ошибки
 */
int esyslog_getGroup(ESyslog_ModuleId_t moduleId, ESyslog_GroupId_t groupId, ESyslog_Group_t *pGroup);

/**
 * @brief изменить конфигурацию в группе по ее номеру
 * @param
 * 		moduleId - номер модуля в системе в диапазоне [0;esyslog_getModuleCount()-1]
 * 		groupId -  номер группы в диапазоне [0;esyslog_getGroupCount(moduleId)-1]
 * 		pGroupConfig указатель на новую конфигурацию
 * @return
 * 	0 - в случае успеха
 * 	-1 - в случае ошибки
 */
int esyslog_setGroupConfig(ESyslog_ModuleId_t moduleId, ESyslog_GroupId_t groupId, ESyslog_GroupConfig_t	*pGroupConfig);

/******************************************************************************/
/* front-end API */
/******************************************************************************/
#ifdef __ESYSLOG_IMPL_ATTR
#error __ESYSLOG_IMPL_ATTR redefinition. ESYSLOG internal definition
#endif /*__ESYSLOG_IMPL_ATTR*/

/* префиксы переменных*/
#define __ESYSLOG_GRPNAME(name) esGrp##name
#define __ESYSLOG_MODNAME(name) esMod##name

#ifdef ESYSLOG_NOSECTION
#error ESYSLOG_NOSECTION currently not supported
#else
#define __ESYSLOG_IMPL_ATTR 	__attribute__ ((section ("ESYSLOGSECTION")))
#define	__ESYSLOG_IMPLTYPE_ATTR __attribute__ ((aligned(32)))
#endif

/* работа с magic */
#define __ESYSLOG_MAGIC1 0xe69aa96e
#define __ESYSLOG_MAGIC2 (__ESYSLOG_MAGIC1 ^ 0xaa55ee66)
#if 0
#define __ESYSLOG_IMPLMAGIC magic1: __ESYSLOG_MAGIC1, magic2: __ESYSLOG_MAGIC2
#endif
#define __ESYSLOG_IMPLMAGICCHECK(pImpl) \
	(((pImpl)->magic1 == __ESYSLOG_MAGIC1) && ((pImpl)->magic2 == __ESYSLOG_MAGIC2))

/* мы исползуем cmn_list, но чтобы не влючать его в header переименуем его */
#ifndef ESYSLOG_list_head
/** syslog list implementation */
struct ESYSLOG_list_head {
	struct ESYSLOG_list_head *next, *prev;
};
#endif

/** типы имплементации */
typedef enum enumESyslog_ImplType {
	eitModule = 0,
	eitGroup = 1,
}ESyslog_ImplType_t;

/**данные для реализации модуля*/
typedef struct tagESyslog_ModuleImpl {
	/** todo:*/
	/** имя модуля*/
	ESyslog_ModuleName_t 		name;
	/** добавлен в список или нет */
	int 						usageCounter;
	/** Group count */
	ESyslog_GroupId_t 			grpCount;
	/** Group list */
	struct ESYSLOG_list_head 	grpList;
	/** list for modules*/
	struct ESYSLOG_list_head 	list;
}ESyslog_ModuleImpl_t;

/**данные для реализации группы*/
typedef struct tagESyslog_GroupImpl {
	/** group */
	ESyslog_Group_t 			group;

	/** указатель на модуль*/
	ESyslog_ModuleImpl_t * 		pModule;

	/** list */
	struct ESYSLOG_list_head 	list;
}ESyslog_GroupImpl_t;

/*
 * FIXME: C++ doesn't support union initialization, so we create 2 identical structs with different
 * sequence of union entries to initialize both subibjects
 */
/** for modules */
typedef struct tagESyslog_Impl {
	/* magic 1 */
	unsigned int magic1;
	/*FIXME: add here locks?*/

	ESyslog_ImplType_t type;
	union {
		/** если type == eitModule*/
		ESyslog_ModuleImpl_t moduleImpl;
		/** если type == eitGroup*/
		ESyslog_GroupImpl_t groupImpl;
	}_u;

	/** magic 2 */
	unsigned int magic2;

} __ESYSLOG_IMPLTYPE_ATTR ESyslog_Impl_t;

/** the same for groups */
typedef struct tagESyslog_Impl_G {
	/* magic 1 */
	unsigned int magic1;
	/*FIXME: add here locks?*/

	ESyslog_ImplType_t type;
	union {
		/** если type == eitGroup*/
		ESyslog_GroupImpl_t groupImpl;
		/** если type == eitModule*/
		ESyslog_ModuleImpl_t moduleImpl;
	}_u;

	/** magic 2 */
	unsigned int magic2;

} __ESYSLOG_IMPLTYPE_ATTR ESyslog_Impl_G_t;

/** определение модуля логирования */
#define	ESYSLOG_MODULE_DECLARE(_name) extern ESyslog_Impl_t __ESYSLOG_MODNAME(_name)
/** создание модуля логирования */
#if 0
#define	ESYSLOG_MODULE_CREATE(_name) ESyslog_Impl_t __ESYSLOG_MODNAME(_name) __ESYSLOG_IMPL_ATTR = \
	{ type : eitModule, _u: { moduleImpl: { name: #_name, usageCounter: 0}, }, __ESYSLOG_IMPLMAGIC}
#endif
#define	ESYSLOG_MODULE_CREATE(_name) ESyslog_Impl_t __ESYSLOG_MODNAME(_name) __ESYSLOG_IMPL_ATTR = \
	{ __ESYSLOG_MAGIC1, eitModule, { { #_name, 0,}, }, __ESYSLOG_MAGIC2}


/** определение группы логирования */
#define	ESYSLOG_GROUP_DECLARE(_name) extern ESyslog_Impl_G_t __ESYSLOG_GRPNAME(_name)
#if 0
#define	ESYSLOG_GROUP_CREATE_LVL(_name, _level) ESyslog_Impl_t __ESYSLOG_GRPNAME(_name) __ESYSLOG_IMPL_ATTR = \
	{ type : eitGroup, _u: { groupImpl: { group: {name: #_name, config: { mask : _level }, }, }, }, __ESYSLOG_IMPLMAGIC}
#endif
/** создание группы логирования */
#define	ESYSLOG_GROUP_CREATE_LVL(_name, _level) ESyslog_Impl_G_t __ESYSLOG_GRPNAME(_name) __ESYSLOG_IMPL_ATTR = \
	{ __ESYSLOG_MAGIC1, eitGroup, { { { #_name , {_level} } } }, __ESYSLOG_MAGIC2}

/** создание группы логирования */
#define	ESYSLOG_GROUP_CREATE(_name) ESYSLOG_GROUP_CREATE_LVL(_name, ESYSLOG_LVL_ERR)

/** взять уровень*/
#define	ESYSLOG_GROUP_GET_LVL(_name) (__ESYSLOG_GRPNAME(_name)._u.groupImpl.group.config.mask)
/** установить уровень*/
#define	ESYSLOG_GROUP_SET_LVL(_name, _level) {__ESYSLOG_GRPNAME(_name)._u.groupImpl.group.config.mask = _level;  }

/*do not use this function directly*/
int 	_esyslog_initImpl	(ESyslog_Impl_t* pImpl);
void 	_esyslog_uninitImpl	(ESyslog_Impl_t* pImpl);

#define ESYSLOG_MODULE_INIT(_name) _esyslog_initImpl(&__ESYSLOG_MODNAME(_name))
#define ESYSLOG_MODULE_UNINIT(_name) _esyslog_uninitImpl(&__ESYSLOG_MODNAME(_name))

#ifndef ESYSLOG_SIMPLE_BACKEND
void 	_esyslog			(ESyslog_GroupImpl_t* pGroupImpl,
					ESyslog_GroupImportance_t importance, __const char *__fmt, ...);
void	_esyslog_dump(ESyslog_GroupImpl_t* pGroupImpl,
					ESyslog_GroupImportance_t importance, void* ptr, int size);

/** логирование */
#define ESYSLOG(_name, _importance, _arg...) _esyslog(&(__ESYSLOG_GRPNAME(_name)._u.groupImpl), _importance, _arg)
#define ESYSLOG_DUMP(_name, _importance, _ptr, _size) _esyslog_dump(&(__ESYSLOG_GRPNAME(_name)._u.groupImpl), _importance, _ptr, _size)

#ifdef ESYSLOG_V_ARGS
void 	_vesyslog(ESyslog_GroupImpl_t* pGroupImpl,
					ESyslog_GroupImportance_t importance, __const char *__fmt, va_list ap);
#define VESYSLOG(_name, _importance, _fmt, _ap) _esyslog(&(__ESYSLOG_GRPNAME(_name)._u.groupImpl), _importance, _fmt, _ap);
#endif

#else
#define ESYSLOG(_name, _importance, _arg...) {if (ESYSLOG_IMP_ENABLED(__ESYSLOG_GRPNAME(_name)._u.groupImpl.group.config.mask, _importance))ESYSLOG_PRINTF(#_name ": " _arg); }
#endif


#ifdef	__cplusplus
}
#endif

#endif /*_LIB_ESYSLOG_H_*/
