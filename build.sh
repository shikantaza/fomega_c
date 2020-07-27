#  Copyright 2020 Rajesh Jayaprakash <rajesh.jayaprakash@gmail.com>
#
#  This file is part of fomega_c.
#
#  fomega_c is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  fomega_c is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with fomega_c.  If not, see <http://www.gnu.org/licenses/>.


#!/bin/sh
bison -d fomega.y
flex fomega.l
gcc util.c env.c fomega.c fomega.tab.c lex.yy.c -lgc -o fomega
