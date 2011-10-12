#include "check_ext.h"

START_TEST(test_div_by_zero) {
    int i = 0;
    fail_unless (i == 0,"Zero is a valid amount of money");
}END_TEST

START_TEST(test_div_by_zero1) {
    int i = 1;
    fail_unless (i == 0,"Zero is a valid amount of money");
}END_TEST

START_TEST(test_div_by_zero2) {
    ck_error("something goes wrong");
}END_TEST


void
_setup(void) {
	printf("setup\n");
}

void
_teardown(void) {
	printf("teardown\n");
}

//START_MAIN_CASE(test1, core){
//	FIXTURE(_setup, _teardown, 1);
//	TIMEOUT(5);
//	ADD_TEST(test_div_by_zero);
//	ADD_TEST(test_div_by_zero1);
//}END_MAIN_CASE

int main(void){
	return run_suite(
		START_SUITE(test1){
			ADD_CASE(
				START_CASE(core){
					FIXTURE(_setup, _teardown);
					TIMEOUT(5);
					ADD_TEST(test_div_by_zero);
					ADD_TEST(test_div_by_zero2);
				}END_CASE
			);
			ADD_CASE(
				START_CASE(core1){
					FIXTURE(_setup, _teardown);
					TIMEOUT(5);
					ADD_TEST(test_div_by_zero1);
				}END_CASE
			);
		}END_SUITE
	);
}
