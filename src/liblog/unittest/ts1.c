#include "check_ext.h"
#include "liblog/log.h"

LOG_MODULE_CREATE(test1);
LOG_GROUP_CREATE_LVL(test1, LOG_LVL_WARN);
LOG_GROUP_CREATE(test2);

START_TEST(log_test_log) {
	LOG_MODULE_INIT(test1);

	LOG_EMERG(test1, "%s", "emerg");
	LOG_ALERT(test1, "%s", "alert");
	LOG_CRIT(test1, "%s", "crit");
	LOG_ERR(test1, "%s", "err");
	LOG_WARN(test1, "%s", "warn");
	LOG_NOTICE(test1, "%s", "notice");
	LOG_INFO(test1, "%s", "info");
	LOG_DEBUG(test1, "%s", "debug");

	LOG_EMERG(test2, "%s", "emerg");
	LOG_ALERT(test2, "%s", "alert");
	LOG_CRIT(test2, "%s", "crit");
	LOG_ERR(test2, "%s", "err");
	LOG_WARN(test2, "%s", "warn");
	LOG_NOTICE(test2, "%s", "notice");
	LOG_INFO(test2, "%s", "info");
	LOG_DEBUG(test2, "%s", "debug");

	LOG_MODULE_UNINIT(test1);
	//fail_unless(0, "test");
//	fail_unless(register_filesystem(&ext3_fs_type)==0, "register filesystem failed");
//	fail_unless(unregister_filesystem(&ext3_fs_type)==0, "unregister filesystem failed");
}END_TEST

int ts1(void){
    return run_suite(
	START_SUITE(liblog_basic){
		ADD_CASE(
			START_CASE(log_basic){
				ADD_TEST(log_test_log);
			}END_CASE
		);
	}END_SUITE
    );
}
