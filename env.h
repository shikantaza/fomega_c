#include <string.h>
#include <stdio.h>

#include "gc.h"

typedef struct
{
  int delete_flag;
  char *key;
  void *val;
} env_entry_t;

typedef struct
{
  unsigned int nof_elements;
  env_entry_t *entries;
} env_t;
