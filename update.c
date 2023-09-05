#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF_LEN 4096

typedef struct {
  char *type;
  char *name;
} Arg;

typedef struct {
  char *rettype;
  char *name;
  Arg *args;
  short args_len;
} Func;

void read_file_to_buffer(char *filename, char *buff)
{
  FILE *source = fopen(filename, "r");

  if (source == NULL) {
    fprintf(stderr, "ERROR: can not open `main.c`: %s\n", strerror(errno));
    exit(-1);
  }

  size_t newLen = fread(buff, sizeof(char), MAX_BUF_LEN, source);
  buff[newLen++] = '\0';
  int err = ferror(source);
  fclose(source);

  if (err != 0) {
    fprintf(stderr, "ERROR: can not read `main.c`: %s\n", strerror(err));
    exit(-1);
  }
}

char *find_marker(char *buff, char *marker)
{
  char *ptr = strstr(buff, marker);

  if (ptr == NULL) {
    return NULL;
  }

  do {
    ptr++;
  } while (*ptr != '\n');

  if (ptr == NULL) {
    fprintf(stderr, "ERROR: could not find function declaration\n");
    exit(-1);
  }

  return ptr;
}

// This function is CURSED
char *get_symbol_type(char **cursor)
{
  while (isspace((*cursor[0])) || (*cursor[0]) == ',') {
    (*cursor)++;
  }

  char *_cursor = *cursor;

  int i = 0;
  while (_cursor[i] != '\0' && _cursor[i] != ' ') {
    i++;
  }

  int j = i; // Here I don't move cursor because strncpy below
  while (_cursor[j] != '\0' && _cursor[j] == ' ') {
    j++;
  }

  int stars_len = 0;
  bool is_ptr = _cursor[j] == '*';

  if (is_ptr) {
    while (_cursor[j + stars_len] != '\0' && _cursor[j + stars_len] == '*') {
      stars_len++;
    }
  }

  int type_len = i + (is_ptr ? stars_len + 1 : 1);
  char *type = malloc(type_len);
  strncpy(type, _cursor, i);

  for (int x = 0; x < stars_len; x++) {
    type[i + x] = '*';
  }

  type[type_len - 1] = '\0';
  *cursor += type_len;

  return type;
}

char *get_symbol_name(char **cursor)
{
  char *_cursor = *cursor;
  while (isspace(_cursor[0])) {
    _cursor++;
  }

  int i = 0;
  while (_cursor[i] != '\0' &&
         _cursor[i] != ')' &&
         _cursor[i] != '(' &&
         _cursor[i] != ',' &&
         _cursor[i] != ' ') {
    i++;
  }

  char *name = malloc(i + 1);
  strncpy(name, _cursor, i);
  name[i] = '\0';
  *cursor += i;
  return name;
}

short get_funcion_args_len(char **cursor)
{
  while (isspace((*cursor[0]))) {
    (*cursor)++;
  }

  if ((*cursor)[0] != '(') {
    fprintf(stderr, "ERROR: could not parse function\n");
    exit(-1);
  }

  (*cursor)++;

  while (isspace((*cursor[0]))) {
    (*cursor)++;
  }

  char *_cursor = *cursor;
  short args_len = 1;

  for (int i = 0; _cursor[i] != '\0' && _cursor[i] != '\n'; i++) {
    if (_cursor[i] == ',') {
      args_len++;
    }
  }

  return args_len;
}

Arg *get_funcion_args(char **cursor, short args_len)
{
  Arg *args = malloc(sizeof(Arg) * args_len);

  for (int i = 0; i < args_len; i++) {
    args[i].type = get_symbol_type(cursor);
    args[i].name = get_symbol_name(cursor);
  }

  return args;
}

Func get_func_definition(char *buff)
{
  Func func = {0};
  char *cursor = find_marker(buff, "#include \"common.h\"\n\n");
  func.rettype = get_symbol_type(&cursor);
  func.name = get_symbol_name(&cursor);
  func.args_len = get_funcion_args_len(&cursor);
  func.args = get_funcion_args(&cursor, func.args_len);

  return func;
}

void free_func(Func *func)
{
  for (int i = 0; i < func->args_len; i++) {
    free(func->args[i].type);
    free(func->args[i].name);
  }

  free(func->args);
  free(func->rettype);
  free(func->name);
}

char *get_type_default_value(char *type)
{
  if (strcmp(type, "char*") == 0) {
    return "\"\"";
  } else if (strstr(type, "*") != NULL) {
    return "NULL";
  } else {
    return "0";
  }
}

void print_includes(FILE *out)
{
  fprintf(out, "#include \"common.h\"\n\n");
}

void print_function(FILE *out, Func *func)
{
  fprintf(out, "%s %s(", func->rettype, func->name);

  for (int i = 0; i < func->args_len - 1; i++) {
    fprintf(out, "%s %s, ", func->args[i].type, func->args[i].name);
  }

  fprintf(out, "%s %s)\n", func->args[func->args_len - 1].type, func->args[func->args_len - 1].name);
  fprintf(out, "{\n");

  for (int i = 0; i < func->args_len; i++) {
    fprintf(out, "  UNUSED(%s);\n", func->args[i].name);
  }

  fprintf(out, "  return %s;\n", get_type_default_value(func->rettype));
  fprintf(out, "}\n\n");
}

void print_testcase_def(FILE *out, Func *func)
{
  fprintf(out, "typedef struct Testcase {\n");

  for (int i = 0; i < func->args_len; i++) {
    fprintf(out, "  %s %s;\n", func->args[i].type, func->args[i].name);
  }

  fprintf(out, "  %s expected;\n", func->rettype);
  fprintf(out, "} Testcase;\n\n");
}

void print_testcases(FILE *out, Func *func)
{
  fprintf(out, "  Testcase testcases[] = {\n");
  fprintf(out, "    {");

  for (int i = 0; i < func->args_len; i++) {
    fprintf(out, "%s, ", get_type_default_value(func->args[i].type));
  }

  fprintf(out, "%s}\n", get_type_default_value(func->rettype));
  fprintf(out, "  };\n\n");
}

void print_testcase_loop(FILE *out, Func *func)
{
  fprintf(out, "  for (size_t i = 0; i < ARR_LEN(testcases); i++) {\n");
  fprintf(out, "    NL();\n\n");

  for (int i = 0; i < func->args_len; i++) {
    fprintf(out, "    %1$s %2$s = testcases[i].%2$s;\n", func->args[i].type, func->args[i].name);
    fprintf(out, "    LOGN(%s);\n\n", func->args[i].name);
  }

  fprintf(out, "    %s exp = testcases[i].expected;\n", func->rettype);
  fprintf(out, "    %s res = %s(", func->rettype, func->name);

  for (int i = 0; i < func->args_len - 1; i++) {
    fprintf(out, "%s, ", func->args[i].name);
  }

  fprintf(out, "%s);\n\n", func->args[func->args_len - 1].name);

  if (strcmp(func->rettype, "char*") == 0) {
    fprintf(out, "    ASSERT_STR(exp, res);\n");
  } else {
    fprintf(out, "    ASSERT(exp, res);\n");
  }

  fprintf(out, "  }\n\n");
}

void print_main(FILE *out, Func *func)
{
  fprintf(out, "int main(void)\n");
  fprintf(out, "{\n");

  print_testcases(out, func);
  print_testcase_loop(out, func);

  fprintf(out, "  return 0;\n");
  fprintf(out, "}\n");
}

void update_file(Func *func)
{
  FILE *out = fopen("main.c", "w");
  if (out == NULL) {
    fprintf(stderr, "ERROR: could not write output to main.c: %s", strerror(errno));
    exit(-1);
  }

  print_includes(out);
  print_function(out, func);
  print_testcase_def(out, func);
  print_main(out, func);

  fclose(out);
}

int main(void)
{
  char buff[MAX_BUF_LEN];
  char *filename = "main.c";
  read_file_to_buffer(filename, buff);
  Func func = get_func_definition(buff);
  update_file(&func);
  free_func(&func);
  return 0;
}
