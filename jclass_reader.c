/* 
Author: Abraham Saiovici
Specs: https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
*/

#include "jclass.h"

int read_class(jclass *class, char *file_name)
{

	FILE *f = fopen(file_name, "rb");
	if (!f) {
		printf("Error. Could not read from %s\n", file_name);
		return 0;
	}

	unsigned int magic = (unsigned int) read_bytes(f, 4);
	if(magic != MAGIC_NUM) {
		printf("Error. This file is not a Java class.\n");
		return 0;
	}

	class->minor_ver = read_bytes(f, 2);
	class->major_ver = read_bytes(f, 2);
	class->cpool.cp_count = read_bytes(f, 2);
	class->cpool.entry = malloc(sizeof(cp_entry) * class->cpool.cp_count);
	read_cpool(class, f);

	class->acc_flag  = read_bytes(f, 2);
	class->this_class = read_bytes(f, 2);
	class->super_class = read_bytes(f, 2);

	class->interface_count = read_bytes(f, 2);
	class->read_interfaces = malloc(sizeof(interface_info) * class->interface_count);
	read_interfaces(class, f);

	class->field_count = read_bytes(f, 2);
	class->fields = malloc(sizeof(field_info) * class->field_count);
	read_member_info(class, class->fields, f, class->field_count);

	class->method_count = read_bytes(f, 2);
	class->methods = malloc(sizeof(method_info)  * class->method_count);
	read_member_info(class, class->methods, f, class->method_count);

	class->attr_count = read_bytes(f, 2);
	class->attributes = malloc(sizeof(attr_info) * class->attr_count);
	read_attr(f, class->attributes, class->attr_count);

	fclose(f);

	return 1;
}

void read_cpool(jclass *class, FILE *f)
{
	int i;
	class->cpool.entry[0].info = NULL;
	for (i = 1; i < class->cpool.cp_count; i++) {
		unsigned char tag = fgetc(f);
		class->cpool.entry[i].tag = tag;
		switch (tag) {
			case UTF_8: {
				utf8_info *utf = malloc(sizeof(utf8_info));
				utf->length = read_bytes(f, 2);
				utf->bytes = malloc(sizeof(char) * utf->length + 1);
				fread(utf->bytes, sizeof(char), utf->length, f);
				utf->bytes[utf->length] = 0;
				class->cpool.entry[i].info = (void *) utf;
				break;
			}
			case INTEGER: {
				case FLOAT:
				{
					int_info *fint = malloc(sizeof(int_info));
					fint->bytes = read_bytes(f, 4);
					class->cpool.entry[i].info = (void *) fint;
					break;
				}
			}
			case LONG: {
				case DOUBLE:
				{
					long_info *dlong = malloc(sizeof(dlong));
					dlong->bytes = read_bytes(f, 8);
					class->cpool.entry[i].info = (void *) dlong;
					i++;
					break;
				}
			}
			case CLASS: {
				class_info *classinfo = malloc(sizeof(class_info));
				classinfo->cp_index = read_bytes(f, 2);
				class->cpool.entry[i].info = (void *) classinfo;
				break;
			}
			case STRING: {
				string_info *string = malloc(sizeof(string_info));
				string->string_index = read_bytes(f, 2);
				class->cpool.entry[i].info = (void *) string;
				break;
			}

			case FIELD_REF: {
				case METHOD_REF:
				{
					case INTERFACE_METHOD_REF:
					{
						ref_info *refinfo = malloc(sizeof(ref_info));
						refinfo->class_index = read_bytes(f, 2);
						refinfo->nt_index = read_bytes(f, 2);
						class->cpool.entry[i].info = (void *) refinfo;
						break;
					}
				}
			}
			case NAME_TYPE: {
				nt_info *ntinfo = malloc(sizeof(nt_info));
				ntinfo->name_index = read_bytes(f, 2);
				ntinfo->desc_index = read_bytes(f, 2);
				class->cpool.entry[i].info = (void *) ntinfo;
				break;
			}
		}
	}
}

void read_interfaces(jclass *class, FILE *f)
{
	int i;
	for (i = 0; i < class->interface_count; i++) {
		interface_info intr;
		intr.cp_index = read_bytes(f, 2);
		class->read_interfaces[i] = intr;
	}
}

void read_member_attr(jclass *class, member_info *fmr, FILE *f)
{
	int i;
	for (i = 0; i < fmr->attr_count; i++) {
		attr_info attr;
		attr.name_index = read_bytes(f, 2);
		attr.attr_length = read_bytes(f, 4);
		attr.bytes = malloc(sizeof(char) * attr.attr_length);

		char *attr_name = ((utf8_info *) class->cpool.entry[attr.name_index].info)->bytes;

		if (strncmp("Code", attr_name, 4) == 0) {
			read_code_attr(class, f, fmr, &attr);
			fmr->attr[i] = attr;
			continue;
		}

		fread(attr.bytes, sizeof(char), attr.attr_length, f);
		fmr->attr[i] = attr;
	}
}

void read_code_attr(jclass *class, FILE *f, member_info *fmr, attr_info *attr)
{
	fpos_t fpos;
	code_attr *code = malloc(sizeof(code_attr));
	fgetpos(f, &fpos);

	code->max_stack = read_bytes(f, 2);
	code->max_locals = read_bytes(f, 2);
	code->code_length = read_bytes(f, 4);
	code->code = malloc(sizeof(unsigned char) * code->code_length + 1);

	fread(code->code, sizeof(unsigned char), code->code_length, f);

	code->code[code->code_length] = 0;
	code->excep_table_length = read_bytes(f, 2);
	code->table = malloc(sizeof(exception_table) * code->excep_table_length);

	int b;
	for (b = 0; b < code->excep_table_length; b++) {
		exception_table exc;
		exc.start_pc = read_bytes(f, 2);
		exc.end_pc = read_bytes(f, 2);
		exc.handler_pc = read_bytes(f, 2);
		exc.catch_type = read_bytes(f, 2);
		code->table[b] = exc;
	}

	code->attr_count = read_bytes(f, 2);
	code->attributes = malloc(sizeof(attr_info) * code->attr_count);

	for (b = 0; b < code->attr_count; b++) {
		attr_info attri;
		attri.name_index = read_bytes(f, 2);
		attri.attr_length = read_bytes(f, 4);
		read_bytes(f, attri.attr_length);
		code->attributes[b] = attri;
	}

	attr->info = (void *) code;
	parse_instr(class, code, fmr);

	fsetpos(f, &fpos);
	fread(attr->bytes, sizeof(char), attr->attr_length, f);
}

void read_member_info(jclass *class, member_info *c, FILE *f, unsigned short int count)
{
	int i;
	for (i = 0; i < count; i++) {
		member_info fmi;

		fmi.acc_flags = read_bytes(f, 2);
		fmi.name_index = read_bytes(f, 2);
		fmi.desc_index = read_bytes(f, 2);
		fmi.attr_count = read_bytes(f, 2);
		fmi.attr = malloc(sizeof(attr_info) * fmi.attr_count);

		read_member_attr(class, &fmi, f);
		c[i] = fmi;
	}
}

void read_attr(FILE *f, attr_info *attri, unsigned short int attr_count)
{
	int i;
	for (i = 0; i < attr_count; i++) {
		attr_info attr;

		attr.name_index = read_bytes(f, 2);
		attr.attr_length = read_bytes(f, 4);
		attr.bytes = malloc(sizeof(char) * attr.attr_length);

		fread(attr.bytes, sizeof(char), attr.attr_length, f);
		attri[i] = attr;
	}
}

/* Read high-endian unsigned data type of 'length' bytes starting from bytes+offset address*/

long double format_bytes(unsigned char *bytes, int offset, int length)
{
	int i, ch = offset;
	long double res = 0;

	for (i = ((length - 1) * 8); i >= 0; i -= 8) {
		res += ((bytes[ch++] & 0xFF) << i);
	}

	return res;
}


/* Read high-endian unsigned data type of 'length' length from file */

long double read_bytes(FILE *f, int bytes)
{
	int i;
	unsigned char *c = malloc(sizeof(unsigned char) * bytes);

	for (i = 0; i < bytes; i++) {
		c[i] = fgetc(f);
	}

	long double res = format_bytes(c, 0, bytes);
	free(c);

	return res;
}

void parse_instr(jclass *class, code_attr *code, method_info *method)
{
	int i, instru, length;
	for (i = 0, instru = 0; i < code->code_length; instru++) {
		i += get_instr_size(code, i);
	}

	method->instr_count = instru;


	method->instructions = malloc(sizeof(instr) * instru);
	for (i = 0, instru = 0; i < code->code_length; instru++) {
		instr instruction;

		instruction.opcode = code->code[i];	
		method->instructions[instru] = instruction;

		i += get_instr_size(code, i);
	}
}


