 /*
Author: Abraham Saiovici

Description: Test case of the Bytecode Manipulation Library (BCML) that iterates through the constant pool of a standard
HelloWorld class, finds the UTF8 entry that matches the HELLO_WORLD constant, and replaces it with a 
modified string, NEW_STRING.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../jclass.h"

#define CLASS_FILE_NAME "HelloWorld.class"
#define HELLO_WORLD "Hello World!"
#define NEW_STRING "ModifiedString"

void modify_utf8(jclass *class, char *, char *);

int main(int argc, char **argv)
{
	
	jclass class;
	if (!read_class(&class, CLASS_FILE_NAME)) {
		return 0;
	}


	modify_utf8(&class, HELLO_WORLD, NEW_STRING);

	if (!write_class(class, CLASS_FILE_NAME)) {
		return 0;
	}
	dispose_class(&class);

	return 0;
}

void modify_utf8(jclass *class, char *old, char *new)
{
	int i;
	for (i = 1; i < class->cpool.cp_count; i++) {
		cp_entry entry = class->cpool.entry[i];
		if (entry.tag == UTF_8) {
			utf8_info *utf = (utf8_info *) entry.info;
			if (strncmp(old, utf->bytes, strlen(old)) == 0) {
				utf->length = strlen(new);
				utf->bytes = realloc(utf->bytes, utf->length + 1);
				strncpy(utf->bytes, new, utf->length + 1);
				printf("Found string: \"%s\" and replaced it with string: \"%s\"\n", old, new);
			}
		}
	}
}
