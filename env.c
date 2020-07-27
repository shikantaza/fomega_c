#include "util.h"
#include "env.h"

void *get_val(env_t *e, char *key)
{
  int i;

  //searching from the end to simulate
  //addition of key/val to front
  for(i=e->nof_elements-1; i >= 0; i--)
  {
    if(e->entries[i].delete_flag)
      continue;
    
    if(!strcmp(e->entries[i].key, key))
      return e->entries[i].val;
  }
  
  return NULL;
}

void add_entry(env_t *e, char *key, void *val)
{
  e->nof_elements++;
  
  if(e->nof_elements == 1)
    e->entries = GC_MALLOC(sizeof(env_entry_t));
  else
    e->entries = GC_REALLOC(e->entries, e->nof_elements * sizeof(env_entry_t));

  e->entries[e->nof_elements-1].delete_flag = 0;
  e->entries[e->nof_elements-1].key = clone_string(key);
  e->entries[e->nof_elements-1].val = val;
  
}
  
void remove_entry(env_t *e, char *key)
{
  int i;

  for(i=0; i<e->nof_elements; i++)
  {
    if(e->entries[i].delete_flag)
      continue;

    if(!strcmp(key, e->entries[i].key))
    {
      e->entries[i].delete_flag = 1;
      return;
    }
  }
}

env_t *create_env()
{
  env_t *e = GC_MALLOC(sizeof(env_t));

  e->nof_elements = 0;
  e->entries = NULL;

  return e;
}

env_t *clone_env(env_t *e)
{
  env_t *new_env = create_env();

  new_env->nof_elements = e->nof_elements;
  new_env->entries = GC_MALLOC(new_env->nof_elements * sizeof(env_entry_t));

  int i;

  for(i=0; i < e->nof_elements; i++)
  {
    new_env->entries[i].delete_flag = e->entries[i].delete_flag;
    new_env->entries[i].key         = clone_string(e->entries[i].key);
    new_env->entries[i].val         = e->entries[i].val;
  }

  return new_env;
}

env_t *extend_env(env_t *e, char *key, void *val)
{
  env_t *new_env = clone_env(e);
  add_entry(new_env, key, val);
  return new_env;
}

#ifdef TEST
int main(int argc, char **argv)
{
  env_t *e = create_env();

  add_entry(e, "key", "val1");

  printf("%s\n", (char *)get_val(e, "key"));

  env_t *new_env = extend_env(e, "key2", "val2");
  
  printf("%s\n", (char *)get_val(new_env, "key2"));
  printf("%s\n", (char *)get_val(new_env, "key"));
  printf("%s\n", (char *)get_val(e, "key"));
    
  return 0;
}
#endif
