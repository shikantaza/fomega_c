/**
  Copyright 2020 Rajesh Jayaprakash <rajesh.jayaprakash@gmail.com>

  This file is part of fomega_c.

  fomega_c is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  fomega_c is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with fomega_c.  If not, see <http://www.gnu.org/licenses/>.
**/

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
