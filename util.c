#include <string.h>
#include <ctype.h>

#include "gc.h"

char *clone_string(char * s)
{
  unsigned int len = strlen(s);

  if(!len)
    return NULL;

  char *s1 = GC_MALLOC(len * sizeof(char));

  memset(s1, '\0', len);
  
  int i;

  for(i=0; i<len; i++)
    s1[i] = s[i];

  return s1;
}

char *convert_to_lower_case(char *s)
{
  char *ret = clone_string(s);

  int i, len = strlen(ret);
  
  for(i=0; i<len; i++)
    ret[i] = tolower(ret[i]);

  return ret;
}
