#include "check_ext.h"
#include "libelf/libelf.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

START_TEST(elf_ver_ck) {
	fail_unless(elf_version(EV_CURRENT) != EV_NONE, "can't set version");
}END_TEST

START_TEST(elf_kind_object) {
	int fd;
	Elf *e;
	Elf_Kind ek;

	fail_unless(elf_version(EV_CURRENT) != EV_NONE, "can't set version");

	if ((fd = open("begin.o", O_RDONLY, 0)) == -1) {
	    ck_error("can't find file begin.o");
	}

	if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
	    fail("elf_begin returned NULL: %s", elf_errmsg(-1));
	}

	ek = elf_kind(e);

	fail_unless(ek == ELF_K_ELF, "kind MUST BE object file");

	elf_end(e);
	close(fd);

}END_TEST

int ts1(void){
    return run_suite(
	START_SUITE(ts1){
		ADD_CASE(
			START_CASE(elf_basic){
				ADD_TEST(elf_ver_ck);
				ADD_TEST(elf_kind_object);
			}END_CASE
		);
	}END_SUITE
    );
}
