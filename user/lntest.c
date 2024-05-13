#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define BUFSIZE 128
#define d1 "lntest_testdir"

static int common_test(const char* fn, const char* sn, const char* data,
					   const char* testname) {
	int fd = open(fn, O_CREATE | O_WRONLY);
	if (fd < 0) {
		fprintf(2, "[TEST\t%s]: Failed to create file.\n", testname);
		return 1;
	}
	if (write(fd, data, strlen(data)) < 0) {
		fprintf(2, "[TEST\t%S]: Failed to write data to file.\n", testname);
		unlink(fn);
		return 2;
	}
	close(fd);
	if (symlink(fn, sn) < 0) {
		fprintf(2, "[TEST\t%s]: Failed to create symlink.\n", testname);
		unlink(fn);
		return 3;
	}
	fd = open(sn, 0);
	if (fd < 0) {
		fprintf(2, "[TEST\t%s]: Failed to open symlink.\n", testname);
		unlink(fn);
		return 4;
	}
	char buf[BUFSIZE];
	int bytes;
	if ((bytes = read(fd, buf, BUFSIZE - 1)) < 0) {
		fprintf(2, "[TEST\t%s]: Failed to read data from symlink's target.\n",
				testname);
		unlink(fn);
		unlink(sn);
		return 5;
	}
	buf[bytes] = 0;
	if (strcmp(buf, data)) {
		fprintf(2,
				"[TEST\t%s]: Incorrect data, read from symlink's target. "
				"Expected: \"%s\"; Actual: \"%s\"\n",
				data, buf, testname);
		unlink(fn);
		unlink(sn);
		return 6;
	}
	fprintf(2, "[TEST\t%s]: Test success.\n", testname);
	unlink(fn);
	unlink(sn);
	return 0;
}

static int test_1() {
	/* Корректная абсолютная ссылка на файл. */
	int t1 = common_test("/test_1_file", "test_1_link", "35UINzqHOc", "1");
	return t1;
}

static int test_2() {
	/* Корректная относительная ссылка на файл того
	же каталога. */
	int t2_1 =
		common_test("./test_2_1_file", "test_2_1_link", "ouJQfsqtWN", "2.1");
	int t2_2 =
		common_test("test_2_2_file", "test_2_2_link", "PI1ACMEaAa", "2.2");
	return t2_1 | t2_2;
}

static int test_3() {
	/* Корректная относительная ссылка на файл каталога
	на 2-3 уровня выше. */

#define d2 "test_3_dir_2"
#define d3 "test_3_dir_3"
	
    int prep = mkdir(d1 "/" d2) |
			   mkdir(d1 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t3]: Failed to create directories.\n");
        return 1;
    } 
	int t3_1 = common_test(d1 "/" d2 "/" d3 "/test_3_1_file", "test_3_1_link", "hPSArXfFD4", "3.1");
    int t3_2 = common_test("./" d1 "/" d2 "/" d3 "/test_3_2_file", "test_3_2_link", "2Y5lqqwnQS", "3.2");
    return t3_1 | t3_2;

#undef d2
#undef d3

}

static int test_4() {
	/* Корректная относительная ссылка на файл каталога
	на 2-3 уровня ниже. */

#define d2 "test_4_dir_2"
#define d3 "test_4_dir_3"
	
    int prep = mkdir(d1 "/" d2) |
			   mkdir(d1 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t4]: Failed to create directories.\n");
        return 1;
    } 
	int t4_1 = common_test("../../../test_4_1_file", d1 "/" d2 "/" d3 "/test_3_1_link", "IL1jxxRgqw", "4.1");
    int t4_2 = common_test("../../../test_3_2_file", "./" d1 "/" d2 "/" d3 "/test_3_2_link", "qJQw0lXWjS", "4.2");
    return t4_1 | t4_2;

	return 0;

#undef d2
#undef d3

}

static int test_5() {
	/* Корректная абсолютная ссылка на абсолютную символическую
	ссылку (глубина рекурсии 2-3, конечная цель - существующий файл). */
	return 0;
}

static int test_6() {
	/* Корректная абсолютная ссылка на относительную
	символическую ссылку (глубина рекурсии 2-3, конечная цель
	- существующий файл). */
	return 0;
}

static int test_7() {
	/* Корректная относительная ссылка на относительную
	символическую ссылку (глубина рекурсии 2-3, конечная цель
	- существующий файл). */
	return 0;
}

static int test_8() {
	/* Косвенная ссылка на себя (бесконечная рекурсия через
	2-3 перехода) */
	return 0;
}

static int test_9() {
	/* Абсолютная ссылка на несуществующий файл */
	return 0;
}

static int test_10() {
	/* Относительная ссылка на несуществующий файл того же
	каталога (при этом файл существует в каталоге на 2-3 уровня
	выше или ниже) */
	return 0;
}

static int test_11() {
	/* Относительная ссылка на несуществующий файл каталога
	на 2-3 уровня выше (при этом файл в том же каталоге существует) */
	return 0;
}

static int test_12() {
	/* Относительная ссылка на несуществующий файл каталога
	на 2-3 уровня ниже (при этом файл в том же каталоге существует) */
	return 0;
}

static void d1prep() {
    if (mkdir(d1)) {
        fprintf(2, "Failed to create directory for testing.\n");
        exit(1);
    }
}

int main() {
    d1prep();
    
	int ret = 0;
	ret |= test_1();
	ret |= test_2();
	ret |= test_3();
	ret |= test_4();
	ret |= test_5();
	ret |= test_6();
	ret |= test_7();
	ret |= test_8();
	ret |= test_9();
	ret |= test_10();
	ret |= test_11();
	ret |= test_12();

	exit(ret);
}