#include "check_ext.h"
#include "libelf/gelf.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

START_TEST(elf_kind_object) {
	int fd;
	Elf *e;
	Elf_Kind ek;

	fail_unless(elf_version(EV_CURRENT) != EV_NONE, "can't set version");

	if ((fd = open("begin.o", O_RDONLY, 0)) == -1) {
	    ck_error("can't find file begin.o");
	}

	e = elf_begin(fd, ELF_C_READ, NULL);
	fail_unless(e != NULL, "elf_begin returned NULL: %s", elf_errmsg(-1));

	ek = elf_kind(e);
	fail_unless(ek == ELF_K_ELF, "kind MUST BE object file");

	elf_end(e);
	close(fd);

}END_TEST

START_TEST(elf_read_exec) {
	int fd;
	Elf *e;
	Elf_Kind ek;
	int i;
	char *id, bytes[5];

	GElf_Ehdr ehdr;

	fail_unless(elf_version(EV_CURRENT) != EV_NONE, "can't set version");

	if ((fd = open("test_libelf", O_RDONLY, 0)) == -1) {
	    ck_error("can't find file test_libelf");
	}

	e = elf_begin(fd, ELF_C_READ, NULL);
	fail_unless(e != NULL, "elf_begin returned NULL: %s", elf_errmsg(-1));

	ek = elf_kind(e);
	fail_unless(ek == ELF_K_ELF, "kind MUST be object file");
	fail_unless((gelf_getehdr(e, &ehdr) != NULL), "gethdr failed: %s", elf_errmsg(-1));
	fail_unless((i = gelf_getclass(e)) != ELFCLASSNONE, "getclass failed: %s", elf_errmsg(-1));
	fail_unless((id = elf_getident(e, NULL)) != NULL, "getident failed: %s", elf_errmsg(-1));

	elf_end(e);
	close(fd);

}END_TEST


int ts1(void){
    return run_suite(
	START_SUITE(ts1){
		ADD_CASE(
			START_CASE(elf_basic){
				ADD_TEST(elf_kind_object);
				ADD_TEST(elf_read_exec);
			}END_CASE
		);
	}END_SUITE
    );
}
