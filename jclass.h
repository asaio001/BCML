/* Author: Abraham Saiovici */

#ifndef JCLASS

#define JCLASS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constpool.h"

#define MAGIC_NUM 0xCAFEBABE

#define ACC_PUBLIC 0x0001
#define ACC_PRIVATE 0x0002
#define ACC_PROTECTED 0x0004
#define ACC_STATIC 0x0008
#define ACC_FINAL 0x0010
#define ACC_SYNCHRONIZED 0x0020
#define ACC_NATIVE 0x0100
#define ACC_ABSTRACT 0x0400
#define ACC_INTERFACE 0x0200
#define ACC_STRICT 0x0800

typedef struct {
	const_pool cpool;
	unsigned short int minor_ver;
	unsigned short int major_ver;
	unsigned short int acc_flag;
	unsigned short int this_class;
	unsigned short int super_class;
	unsigned short int interface_count;
	unsigned short int field_count;
	unsigned short int method_count;
	unsigned short int attr_count;
	interface_info *read_interfaces;
	field_info *fields;
	method_info *methods;
	attr_info *attributes;
} jclass;


extern int read_class(jclass *class, char *file_name);
extern void read_cpool(jclass *class, FILE *f);
extern void read_interfaces(jclass *class, FILE *f);
extern void read_member_attr(jclass *class, member_info *fmr, FILE *f);
extern void read_code_attr(jclass *class, FILE *f, member_info *fmr, attr_info *attr);
extern void read_member_info(jclass *class, member_info *c, FILE *f, unsigned short int count);
extern void read_attr(FILE *f, attr_info *attri, unsigned short int attr_count);
extern void parse_instr(jclass *class, code_attr *code, method_info *method);
extern long double format_bytes(unsigned char *bytes, int offset, int length);
extern long double read_bytes(FILE *f, int bytes);

extern int write_class(jclass class, char *file_name);
extern void write_cpool(jclass class, FILE *f);
extern void write_inter(jclass class, FILE *f);
extern void write_member_info(jclass class, member_info *c, FILE *f, int count);
extern void write_attr(attr_info *attri, int count, FILE *f);
extern void write_bytes(FILE *f, unsigned long long byte, int byte_len);
extern void dispose_class(jclass *class);



#endif

