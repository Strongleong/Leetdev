#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define CHOP_CHAR(str) (*str++)
#define CHOP_ARG(argv) (*arv++)
#define MAX(a, b) a > b ? a : b
#define MIN(a, b) a < b ? a : b
#define ARR_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define UNUSED(x) (void)x
#define ASSERT(x, y)                                                           \
  do {                                                                         \
    LOGN(y);                                                                   \
    _GENERIC_PRINT("assert(", x, " == ");                                      \
    _GENERIC_PRINT("", y, ")\n");                                              \
    assert(x == y);                                                            \
  } while (0)

#define ASSERT_STR(x, y)                                                       \
  do {                                                                         \
    LOGN(y);                                                                   \
    _GENERIC_PRINT("assert(", x, " == ");                                      \
    _GENERIC_PRINT("", y, ")\n");                                              \
    assert(strcmp(x, y) == 0);                                                 \
  } while (0)

#define GEN_PRINT(TYPE, SPECIFIER_STR)                                         \
  int print_##TYPE(char *s, TYPE x, char *end) {                               \
    return printf("%s" SPECIFIER_STR "%s", s, x, end);                         \
  }

#define _GENERIC_PRINT(s, x, e)                                                \
  _Generic((x),                                                                \
      int: print_int,                                                          \
      uint8_t: print_int,                                                      \
      uint16_t: print_int,                                                     \
      uint32_t: print_int,                                                     \
      float: print_float,                                                      \
      double: print_float,                                                     \
      size_t: print_size_t,                                                    \
      char: print_char,                                                        \
      char *: print_string,                                                    \
      bool: print_bool,                                                        \
      long: print_long,                                                        \
      void *: print_ptr,                                                       \
      int *: print_ptr,                                                        \
      default: print_unknown)(s, x, e)

#define LOG(x) _GENERIC_PRINT(#x " = ", x, "")
#define LOGN(x) _GENERIC_PRINT(#x " = ", x, "\n")
#define LOGT(x) _GENERIC_PRINT(#x " = ", x, "\t")
#define LOGS(x) _GENERIC_PRINT(#x " = ", x, " ")
#define SLOG(x) _GENERIC_PRINT("", x, "")
#define SLOGN(x) _GENERIC_PRINT("", x, "\n")
#define SLOGT(x) _GENERIC_PRINT("", x, "\t")
#define SLOGS(x) _GENERIC_PRINT("", x, " ")
#define SEP() printf("=====================================\n")
#define NL() printf("\n")

#define ARR_PRINT(arr, len)                                                    \
  do {                                                                         \
    printf(#arr " = {");                                                       \
    for (int i = 0; i < len - 1; i++)                                          \
      _GENERIC_PRINT("", arr[i], ", ");                                        \
    _GENERIC_PRINT("", arr[len - 1], "}\n");                                   \
  } while (0)

char *args_shift(int *argc, char ***argv);

#ifndef COMMON_NO_IMPL

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

GEN_PRINT(int, "%d")
GEN_PRINT(char, "%c")
GEN_PRINT(float, "%f")
// GEN_PRINT(double, "%f")
GEN_PRINT(long, "%ld")
GEN_PRINT(size_t, "%zu")

int print_ptr(char *s, void *x, char *end) {
  return printf("%s%p%s", s, x, end);
}

int print_string(char *s, char *x, char *end) {
  return printf("%s%s%s", s, x, end);
}
int print_bool(char *s, bool x, char *end) {
  return printf("%s%s%s", s, x ? "true" : "false", end);
}

int print_unknown(char *s, ...) {
  (void)s;
  return printf("ERROR: Unknown type\n");
}

char *args_shift(int *argc, char ***argv) {
  assert(*argc > 0);
  char *result = **argv;
  (*argc) -= 1;
  (*argv) += 1;
  return result;
}

#endif /* ifndef COMMON_NO_IMPL */
#endif /* end of include guard: COMMON_H */
