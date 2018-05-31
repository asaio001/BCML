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

void print_instr(jclass *class);

int main(int argc, char **argv)
{
	
	jclass class;
	if (!read_class(&class, CLASS_FILE_NAME)) {
		return 0;
	}


	print_instr(&class);

	if (!write_class(class, CLASS_FILE_NAME)) {
		return 0;
	}
	dispose_class(&class);

	return 0;
}

void print_instr(jclass *class)
{
	int i, j;
	for (i = 0; i < class->method_count; i++) {
		method_info m = class->methods[i];
		char *m_name= ((utf8_info *) class->cpool.entry[m.name_index].info)->bytes;
		char *m_sig = ((utf8_info *) class->cpool.entry[m.desc_index].info)->bytes;
		printf("- Method: %s %s\n", m_name, m_sig);

		for (j = 0; j < m.instr_count; j++) {
			printf("\t - %s\n", instr_string(m.instructions[j].opcode));
		}
	}
}
