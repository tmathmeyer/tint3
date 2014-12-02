/*
 * Copyright (C) 2014 Ted Meyer
 * A utility for printing all the special characters used by sakamoto and tint3
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
▁
▂
▃
▄
▅
▆
▇
█
▉
▊
▋
▌
▍
▎
▏
▐
░
▒
▓
▔
*/


int main() {
	char * v = malloc(4);
	strncpy(v, "▁", 3);

	v[2] = v[2] - 9;	

	int i = 0;
	for(; i< 27; i++) {
		printf("%s",v);
		printf("\n%x %x %x\n", v[0], v[1], v[2]);
		v[2] = v[2] + 1;
	}
	
}