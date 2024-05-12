#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int s_flag = 0;
  char* target = 0, *fn = 0;
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if (!strcmp(argv[i], "-s")) {
        s_flag = 1;
        continue;
      } else {
        fprintf(2, "Unknown flag: %s\n", argv[i]);
        exit(2);
      }
    } else if (target == 0) {
      target = argv[i];
      continue;
    } else if (fn == 0) {
      fn = argv[i];
    } else {
      fprintf(2, "Usage: ln [-s] old new\n");
      exit(1);
    }
  }
  if (target == 0 || fn == 0) {
    fprintf(2, "Usage: ln [-s] old new\n");
    exit(3);
  }
  if (!s_flag) {
    if (link(target, fn) < 0)
      fprintf(2, "link %s %s: failed\n", target, fn);
  } else {
    if (symlink(target, fn) < 0)
      fprintf(2, "symlink %s %s: failed\n", target, fn);
  }
  exit(0);
}
