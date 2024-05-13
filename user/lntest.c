#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define BUFSIZE 128
#define rd1 "tr1"
#define rd2 "tr2"
#define rd3 "tr3"
#define RECLIMCODE -42
#define NOSUCHFILECODE -89

static int create_file(const char* fn, const char* data, const char* testname) {
	int fd = open(fn, O_CREATE | O_WRONLY);
	if (fd < 0) {
		fprintf(2, "[TEST\t%s]: Failed to create file \"%s\".\n", testname, fn);
		return 1;
	}
	if (write(fd, data, strlen(data)) < 0) {
		fprintf(2, "[TEST\t%S]: Failed to write data to file \"%s\".\n", testname, fn);
		unlink(fn);
		return 2;
	}
	close(fd);
	return 0;
}

static int create_link(const char* fn, const char* sn, const char* testname) {
	if (symlink(fn, sn) < 0) {
		fprintf(2, "[TEST\t%s]: Failed to create symlink \"%s\" -> \"%s\".\n", testname, sn, fn);
		return 3;
	}
	return 0;
}

static int check_link_content(const char* sn, const char* data, const char* testname) {
	int fd = open(sn, 0);
	if (fd < 0) {
		fprintf(2, "[TEST\t%s]: Failed to open symlink \"%s\".\n", testname, sn);
		return 4;
	}
	char buf[BUFSIZE];
	int bytes;
	if ((bytes = read(fd, buf, BUFSIZE - 1)) < 0) {
		fprintf(2, "[TEST\t%s]: Failed to read data from symlink's target.\n",
				testname);
		close(fd);
		return 5;
	}
	close(fd);
	buf[bytes] = 0;
	if (strcmp(buf, data)) {
		fprintf(2,
				"[TEST\t%s]: Incorrect data, read from symlink's target. "
				"Expected: \"%s\"; Actual: \"%s\"\n",
				testname, data, buf);
		return 6;
	}
	return 0;
}

static int common_test(const char* fn, const char* sn, const char* data,
					   const char* testname) {
	if (create_file(fn, data, testname) != 0) {
		return 1;
	}
	if (create_link(fn, sn, testname) != 0) {
		unlink(fn);
		return 2;
	}
	if (check_link_content(sn, data, testname) != 0) {
		unlink(fn);
		unlink(sn);
		return 3;
	}
	fprintf(2, "[TEST\t%s]: Test success.\n", testname);
	unlink(fn);
	unlink(sn);
	return 0;
}

static int test_1() {
	/* Корректная абсолютная ссылка на файл. */
	int t1 = common_test("/" rd1 "/f1", "/" rd1 "/l1", "35UINzqHOc", "1");
	return t1;
}

static int test_2() {
	/* Корректная относительная ссылка на файл того
	же каталога. */
	int t2_1 =
		common_test("./f21", "l21", "ouJQfsqtWN", "2.1");
	int t2_2 =
		common_test("f22", "l22", "PI1ACMEaAa", "2.2");
	return t2_1 | t2_2;
}

static int test_3() {
	/* Корректная относительная ссылка на файл каталога
	на 2-3 уровня выше. */

#define d2 "t3d"
#define d3 "t3dd"
	
    int prep = mkdir(rd1 "/" d2) |
			   mkdir(rd1 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t3]: Failed to create directories.\n");
        return 1;
    } 
	int t3_1 = common_test(rd1 "/" d2 "/" d3 "/f31", "l31", "hPSArXfFD4", "3.1");
    int t3_2 = common_test("./" rd1 "/" d2 "/" d3 "/f32", "l32", "2Y5lqqwnQS", "3.2");
    return t3_1 | t3_2;

#undef d2
#undef d3

}

static int test_4() {
	/* Корректная относительная ссылка на файл каталога
	на 2-3 уровня ниже. */

#define d2 "t4d"
#define d3 "t4dd"
	
    int prep = mkdir(rd1 "/" d2) |
			   mkdir(rd1 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t4]: Failed to create directories.\n");
        return 1;
    } 
	int t4_1 = common_test("../../../f41", rd1 "/" d2 "/" d3 "/l41", "IL1jxxRgqw", "4.1");
    int t4_2 = common_test("../../../f42", "./" rd1 "/" d2 "/" d3 "/l42", "qJQw0lXWjS", "4.2");
    return t4_1 | t4_2;

	return 0;

#undef d2
#undef d3

}

static int test_5() {
	/* Корректная абсолютная ссылка на абсолютную символическую
	ссылку (глубина рекурсии 2-3, конечная цель - существующий файл). */

#define d2 "t5d"
#define d3 "t5dd"
	
	const char* fp = "/" rd1 "/" d2 "/" d3 "/f51";
	const char* s1 = "/" rd1 "/" d2 "/l51" ;
	const char* s2 = "/" rd1 "/" d2 "/" d3 "/l52" ;
	const char* s3 = "/" rd1 "/l53";
	const char* data = "NuPy7LOMU5";
	const char* testname = "5";

    int prep = mkdir(rd1 "/" d2) |
			   mkdir(rd1 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t%s]: Failed to create directories.\n", testname);
        return 1;
    } 
	if (create_file(fp, "NuPy7LOMU5", testname) != 0) {
		return 2;
	}
	if (create_link(fp, s1, testname) != 0) {
		unlink(fp);
		return 3;
	}
	if (create_link(s1, s2, testname) != 0) {
		unlink(fp);
		unlink(s1);
		return 3;
	}
	if (create_link(s2, s3, testname) != 0) {
		unlink(fp);
		unlink(s1);
		unlink(s2);
		return 3;
	}
	if (check_link_content(s3, data, testname) != 0) {
		unlink(fp);
		unlink(s1);
		unlink(s2);
		unlink(s3);
		return 4;
	}
	if (check_link_content(s2, data, testname) != 0) {
		unlink(fp);
		unlink(s1);
		unlink(s2);
		unlink(s3);
		return 4;
	}
	if (check_link_content(s1, data, testname) != 0) {
		unlink(fp);
		unlink(s1);
		unlink(s2);
		unlink(s3);
		return 4;
	}

	unlink(fp);
	unlink(s1);
	unlink(s2);
	unlink(s3);
	fprintf(2, "[TEST\t%s]: Test success.\n", testname);
	return 0;

#undef d2
#undef d3

}

static int test_6() {
	/* Корректная абсолютная ссылка на относительную
	символическую ссылку (глубина рекурсии 2-3, конечная цель
	- существующий файл). */

#define d2 "t6d"
#define d3 "t6dd"
	
	const char* fp = d3 "/f61";
	const char* afp = "/" rd1 "/" d2 "/" d3 "/f61";
	const char* s1 = "/" rd1 "/" d2 "/l61";
	const char* s2 = d2 "/" d3 "/l62";
	const char* as2 = "/" rd1 "/" d2 "/" d3 "/l62";
	const char* s3 = "/" rd1 "/l63";
	const char* data = "h30aeMdblO";
	const char* testname = "6";

    int prep = mkdir(rd1 "/" d2) |
			   mkdir(rd1 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t%s]: Failed to create directories.\n", testname);
        return 1;
    } 
	if (create_file(afp, data, testname) != 0) {
		return 2;
	}
	if (create_link(fp, s1, testname) != 0) {
		unlink(afp);
		return 3;
	}
	if (create_link(s1, as2, testname) != 0) {
		unlink(afp);
		unlink(s1);
		return 3;
	}
	if (create_link(s2, s3, testname) != 0) {
		unlink(afp);
		unlink(s1);
		unlink(as2);
		return 3;
	}
	if (check_link_content(s3, data, testname) != 0) {
		unlink(afp);
		unlink(s1);
		unlink(as2);
		unlink(s3);
		return 4;
	}
	if (check_link_content(as2, data, testname) != 0) {
		unlink(afp);
		unlink(s1);
		unlink(as2);
		unlink(s3);
		return 4;
	}
	if (check_link_content(s1, data, testname) != 0) {
		unlink(afp);
		unlink(s1);
		unlink(as2);
		unlink(s3);
		return 4;
	}

	unlink(afp);
	unlink(s1);
	unlink(as2);
	unlink(s3);
	fprintf(2, "[TEST\t%s]: Test success.\n", testname);
	return 0;

#undef d2
#undef d3

}

static int test_7() {
	/* Корректная относительная ссылка на относительную
	символическую ссылку (глубина рекурсии 2-3, конечная цель
	- существующий файл). */

#define d2 "t7d"
#define d3 "t7dd"
	
	const char* fp = "f71";
	const char* afp = "/" rd2 "/" d2 "/f71";
	const char* as1 = "/" rd2 "/" d2 "/l71";
	const char* s1 = d2 "/l71";
	const char* as2 = "/" rd2 "/l72";
	const char* s2 = "../../l72";
	const char* as3 = "/" rd2 "/" d2 "/" d3 "/l73";
	const char* data = "FiShBXj3ki";
	const char* testname = "7";

    int prep = mkdir(rd2 "/" d2) |
			   mkdir(rd2 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t%s]: Failed to create directories.\n", testname);
        return 1;
    } 
	if (create_file(afp, data, testname) != 0) {
		return 2;
	}
	if (create_link(fp, as1, testname) != 0) {
		unlink(afp);
		return 3;
	}
	if (create_link(s1, as2, testname) != 0) {
		unlink(afp);
		unlink(as1);
		return 3;
	}
	if (create_link(s2, as3, testname) != 0) {
		unlink(afp);
		unlink(as1);
		unlink(as2);
		return 3;
	}
	if (check_link_content(as3, data, testname) != 0) {
		unlink(afp);
		unlink(as1);
		unlink(as2);
		unlink(as3);
		return 4;
	}
	if (check_link_content(as2, data, testname) != 0) {
		unlink(afp);
		unlink(as1);
		unlink(as2);
		unlink(as3);
		return 4;
	}
	if (check_link_content(as1, data, testname) != 0) {
		unlink(afp);
		unlink(as1);
		unlink(as2);
		unlink(as3);
		return 4;
	}

	unlink(afp);
	unlink(as1);
	unlink(as2);
	unlink(as3);
	fprintf(2, "[TEST\t%s]: Test success.\n", testname);
	return 0;

#undef d2
#undef d3

}

static int test_8() {
	/* Косвенная ссылка на себя (бесконечная рекурсия через
	2-3 перехода) */

#define d2 "t8d"
#define d3 "t8dd"
	
	const char* as1 = "/" rd2 "/" d2 "/l81";
	const char* as2 = "/" rd2 "/l82";
	const char* as3 = "/" rd2 "/" d2 "/" d3 "/l83";
	const char* data = "FiShBXj3ki";
	const char* testname = "8";

    int prep = mkdir(rd2 "/" d2) |
			   mkdir(rd2 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t%s]: Failed to create directories.\n", testname);
        return 1;
    } 
	if (create_file(as1, data, testname) != 0) {
		return 2;
	}
	if (create_link(as1, as2, testname) != 0) {
		unlink(as1);
		return 3;
	}
	if (create_link(as2, as3, testname) != 0) {
		unlink(as1);
		unlink(as2);
		return 3;
	}
	// remove file as1
	unlink(as1);
	// create link as1 -> as3 -> as2 -> as1
	if (create_link(as3, as1, testname) != 0) {
		unlink(as2);
		unlink(as3);
		return 3;
	}
	if (open(as1, O_RDONLY) != RECLIMCODE) {
		fprintf(2, "[TEST\t%d]: Self-recursion link open() didn't return RECLIMCODE.\n", testname);
		unlink(as1);
		unlink(as2);
		unlink(as3);
		return 5;
	}
	if (open(as2, O_RDONLY) != RECLIMCODE) {
		fprintf(2, "[TEST\t%d]: Self-recursion link open() didn't return RECLIMCODE.\n", testname);
		unlink(as1);
		unlink(as2);
		unlink(as3);
		return 5;
	}
	if (open(as3, O_RDONLY) != RECLIMCODE) {
		fprintf(2, "[TEST\t%d]: Self-recursion link open() didn't return RECLIMCODE.\n", testname);
		unlink(as1);
		unlink(as2);
		unlink(as3);
		return 5;
	}

	unlink(as1);
	unlink(as2);
	unlink(as3);
	fprintf(2, "[TEST\t%s]: Test success.\n", testname);
	return 0;

#undef d2
#undef d3

}

static int test_9() {
	/* Абсолютная ссылка на несуществующий файл */

#define d2 "t9d"
#define d3 "t9dd"
	
	const char* afp = "/" rd2 "/" d2 "/" d3 "/f91";
	const char* as1 = "/" rd2 "/" d2 "/l91";
	const char* data = "FiShBXj3ki";
	const char* testname = "9";

    int prep = mkdir(rd2 "/" d2) |
			   mkdir(rd2 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t%s]: Failed to create directories.\n", testname);
        return 1;
    } 
	if (create_file(afp, data, testname) != 0) {
		return 2;
	}
	if (create_link(afp, as1, testname) != 0) {
		unlink(afp);
		return 3;
	}
	unlink(afp);
	if (open(as1, O_RDONLY) != NOSUCHFILECODE) {
		fprintf(2, "[TEST\t%d]: Unexisting file link open() didn't return NOSUCHFILECODE.\n", testname);
		unlink(as1);
		return 5;
	}
	
	unlink(as1);
	fprintf(2, "[TEST\t%s]: Test success.\n", testname);
	return 0;

#undef d2
#undef d3

}

static int test_10() {
	/* Относительная ссылка на несуществующий файл того же
	каталога (при этом файл существует в каталоге на 2-3 уровня
	выше или ниже) */

#define d2 "t10d"
#define d3 "t10dd"
	
	const char* afp1 = "/" rd2 "/" d2 "/" d3 "/f101";
	const char* fp1 = d2 "/" d3 "/f101";
	const char* afp2 = "/" rd2 "/f101";
	const char* as1 = "/" rd2 "/l101";
	const char* data = "FiShBXj3ki";
	const char* testname = "10";

    int prep = mkdir(rd2 "/" d2) |
			   mkdir(rd2 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t%s]: Failed to create directories.\n", testname);
        return 1;
    } 
	if (create_file(afp1, data, testname) != 0) {
		return 2;
	}
	if (create_file(afp2, data, testname) != 0) {
		return 2;
	}
	if (create_link(fp1, as1, testname) != 0) {
		unlink(afp1);
		unlink(afp2);
		return 3;
	}
	unlink(afp1);
	if (open(as1, O_RDONLY) != NOSUCHFILECODE) {
		fprintf(2, "[TEST\t%d]: Unexisting file link open() didn't return NOSUCHFILECODE.\n", testname);
		unlink(as1);
		return 5;
	}
	
	unlink(as1);
	unlink(afp2);
	fprintf(2, "[TEST\t%s]: Test success.\n", testname);
	return 0;

#undef d2
#undef d3

}

static int test_11() {
	/* Относительная ссылка на несуществующий файл каталога
	на 2-3 уровня выше (при этом файл в том же каталоге существует) */

#define d2 "t11d"
#define d3 "t11dd"
	
	const char* afp1 = "/" rd2 "/" d2 "/" d3 "/f111";
	const char* afp2 = "/" rd2 "/f111";
	const char* fp2 = "f111";
	const char* as1 = "/" rd2 "/l111";
	const char* data = "FiShBXj3ki";
	const char* testname = "11";

    int prep = mkdir(rd2 "/" d2) |
			   mkdir(rd2 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t%s]: Failed to create directories.\n", testname);
        return 1;
    } 
	if (create_file(afp1, data, testname) != 0) {
		return 2;
	}
	if (create_file(afp2, data, testname) != 0) {
		return 2;
	}
	if (create_link(fp2, as1, testname) != 0) {
		unlink(afp1);
		unlink(afp2);
		return 3;
	}
	unlink(afp2);
	if (open(as1, O_RDONLY) != NOSUCHFILECODE) {
		fprintf(2, "[TEST\t%d]: Unexisting file link open() didn't return NOSUCHFILECODE.\n", testname);
		unlink(as1);
		return 5;
	}
	
	unlink(as1);
	unlink(afp1);
	fprintf(2, "[TEST\t%s]: Test success.\n", testname);
	return 0;

#undef d2
#undef d3

}

static int test_12() {
	/* Относительная ссылка на несуществующий файл каталога
	на 2-3 уровня ниже (при этом файл в том же каталоге существует) */

#define d2 "t12d"
#define d3 "t12dd"
	
	const char* afp1 = "/" rd2 "/" d2 "/" d3 "/f121";
	const char* afp2 = "/" rd2 "/f121";
	const char* fp2 = "../../f121";
	const char* as1 = "/" rd2 "/" d2 "/" d3 "/l121";
	const char* data = "FiShBXj3ki";
	const char* testname = "12";

    int prep = mkdir(rd2 "/" d2) |
			   mkdir(rd2 "/" d2 "/" d3);
    if (prep != 0) {
        fprintf(2, "[TEST\t%s]: Failed to create directories.\n", testname);
        return 1;
    } 
	if (create_file(afp1, data, testname) != 0) {
		return 2;
	}
	if (create_file(afp2, data, testname) != 0) {
		return 2;
	}
	if (create_link(fp2, as1, testname) != 0) {
		unlink(afp1);
		unlink(afp2);
		return 3;
	}
	unlink(afp2);
	if (open(as1, O_RDONLY) != NOSUCHFILECODE) {
		fprintf(2, "[TEST\t%d]: Unexisting file link open() didn't return NOSUCHFILECODE.\n", testname);
		unlink(as1);
		return 5;
	}
	
	unlink(as1);
	unlink(afp1);
	fprintf(2, "[TEST\t%s]: Test success.\n", testname);
	return 0;

#undef d2
#undef d3

}

static void d1prep() {
    if (mkdir(rd1)) {
        fprintf(2, "Failed to create directory \"%s\" for testing.\n", rd1);
        exit(1);
    }
	if (mkdir(rd2)) {
        fprintf(2, "Failed to create directory \"%s\" for testing.\n", rd2);
        exit(1);
    }
	if (mkdir(rd3)) {
        fprintf(2, "Failed to create directory \"%s\" for testing.\n", rd3);
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

	if (ret == 0) {
		printf("All tests successed.\n");
	}

	exit(ret);
}