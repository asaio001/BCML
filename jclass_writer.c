/* 
Author: Abraham Saiovici
Specs: https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
*/

#include "jclass.h"


int write_class(jclass class, char *file_name)
{
	FILE *f = fopen(file_name, "wb");
	if (!f) {
		printf("Error. Could not write to %s\n", file_name);
		return 0;
	}

	write_bytes(f, MAGIC_NUM, 4);

	write_bytes(f, class.minor_ver, 2);
	write_bytes(f, class.major_ver, 2);
	write_bytes(f, class.cpool.cp_count, 2);
	write_cpool(class, f);

	write_bytes(f, class.acc_flag, 2);
	write_bytes(f, class.this_class, 2);
	write_bytes(f, class.super_class, 2);
	write_bytes(f, class.interface_count, 2);
	write_inter(class, f);

	write_bytes(f, class.field_count, 2);
	write_member_info(class, class.fields, f, class.field_count);

	write_bytes(f, class.method_count, 2);
	write_member_info(class, class.methods, f, class.method_count);

	write_bytes(f, class.attr_count, 2);
	write_attr(class.attributes, class.attr_count, f);

	fflush(f);
	fclose(f);

	return 1;
}

void write_cpool(jclass class, FILE *f)
{
	int i;
	for (i = 1; i < class.cpool.cp_count; i++) {
		unsigned char tag = class.cpool.entry[i].tag;
		void *constant = class.cpool.entry[i].info;
		fputc(tag, f);

		switch(tag) {
			case UTF_8: {
				utf8_info *utf = (utf8_info *) constant;
				write_bytes(f, utf->length, 2);
				fwrite(utf->bytes, sizeof(char), utf->length, f);
				break;
			}
			case INTEGER: {
				case FLOAT:
				{
					int_info *fint = (int_info *) constant;
					write_bytes(f, fint->bytes, 4);
					break;
				}
			}
			case LONG: {
				case DOUBLE:
				{
					long_info *dlong = (long_info *) constant;
					write_bytes(f, dlong->bytes, 8);
					i++;
					break;
				}
			}
			case CLASS: {
				class_info *classinfo = (class_info *) constant;
				write_bytes(f, classinfo->cp_index, 2);
				break;
			}
			case STRING: {
				string_info *string = (string_info *) constant;
				write_bytes(f, string->string_index, 2);
				break;
			}

			case FIELD_REF: {
				case METHOD_REF:
				{
					case INTERFACE_METHOD_REF:
					{
						ref_info *refinfo = (ref_info *) constant;
						write_bytes(f, refinfo->class_index, 2);
						write_bytes(f, refinfo->nt_index, 2);
						break;
					}
				}
			}
			case NAME_TYPE: {
				nt_info *ntinfo = (nt_info *) constant;
				write_bytes(f, ntinfo->name_index, 2);
				write_bytes(f, ntinfo->desc_index, 2);
				break;
			}
		}
	}
}

void write_inter(jclass class, FILE *f)
{
	int i;
	for (i = 0; i < class.interface_count; i++) {
		write_bytes(f, class.read_interfaces[i].cp_index, 2);
	}
}

void write_member_info(jclass class, member_info *c, FILE *f, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		member_info fmi = c[i];

		write_bytes(f, fmi.acc_flags, 2);
		write_bytes(f, fmi.name_index, 2);
		write_bytes(f, fmi.desc_index, 2);
		write_bytes(f, fmi.attr_count, 2);
		write_attr(fmi.attr, fmi.attr_count, f);
	}
}

void write_attr(attr_info *attri, int count, FILE *f)
{
	int i;
	for (i = 0; i < count; i++) {
		attr_info attr = attri[i];

		write_bytes(f, attr.name_index, 2);
		write_bytes(f, attr.attr_length, 4);
		fwrite(attr.bytes, sizeof(char), attr.attr_length, f);
	}
}

void write_bytes(FILE *f, unsigned long long byte, int byte_len)
{
	int i, bit = ((byte_len - 1)* 8);

	for (i = 0; i < byte_len; i++, bit -= 8) {
		fputc(byte >> bit, f);
	}
}

void dispose_class(jclass *class)
{
	int i, b;
	for (i = 0; i < class->cpool.cp_count; i++) {
		void *constant = class->cpool.entry[i].info;
		if (class->cpool.entry[i].tag == UTF_8) {
			free(((utf8_info *) constant)->bytes);
		}
		free(constant);
	}

	for (i = 0; i < class->method_count; i++) {
		for (b = 0; b < class->methods[i].attr_count; b++) {
			int name_index = class->methods[i].attr[b].name_index;
			char *attr_name = ((utf8_info *) class->cpool.entry[name_index].info)->bytes;

			if (strncmp("Code", attr_name, 4) == 0) {
				code_attr *code = (code_attr *) class->methods[i].attr[b].info;
				free(class->methods[i].instructions);
				free(code->attributes);
				free(code->code);
				free(code->table);
				free(code);
			}
			free(class->methods[i].attr[b].bytes);
		}
		free(class->methods[i].attr);
	}
	
	for (i = 0; i < class->field_count; i++) {
		free(class->fields[i].attr);
	}

	for (i = 0; i < class->attr_count; i++) {
		free(class->attributes[i].bytes);
	}

	free(class->cpool.entry);
	free(class->read_interfaces);
	free(class->fields);
	free(class->methods);
	free(class->attributes);
}


