/* Author: Abraham Saiovici */

#ifndef C_POOL

#define C_POOL

#include "instructions.h"

#define UTF_8 1
#define INTEGER 3
#define FLOAT 4
#define LONG 5
#define DOUBLE 6
#define CLASS 7
#define STRING 8
#define FIELD_REF 9
#define METHOD_REF 10
#define INTERFACE_METHOD_REF 11
#define NAME_TYPE 12

typedef struct {
	unsigned char tag;
	void *info;
} cp_entry;

typedef struct {
	unsigned short int cp_count;
	cp_entry *entry;
} const_pool;

typedef struct {
	unsigned short int cp_index;
} class_info, interface_info;

typedef struct {
	unsigned short int class_index;
	unsigned short int nt_index;
} ref_info;

typedef struct {
	unsigned short int string_index;
} string_info;

typedef struct {
	unsigned int bytes;
} int_info, float_info;

typedef struct {
	double bytes;
} long_info, double_info;

typedef struct {
	unsigned short int name_index;
	unsigned short int desc_index;
} nt_info;

typedef struct {
	unsigned short int length;
	char *bytes;
} utf8_info;

typedef struct {
	unsigned short int name_index;
	unsigned int attr_length;
	void *info;
	char *bytes;
} attr_info;

typedef struct {
	unsigned short int start_pc;
	unsigned short int end_pc;
	unsigned short int handler_pc;
	unsigned short int catch_type;
} exception_table;

typedef struct _code_attr {
	unsigned short int max_stack;
	unsigned short int max_locals;
	unsigned int code_length;
	unsigned char *code;
	unsigned short int excep_table_length;
	exception_table *table;
	unsigned short int attr_count;
	attr_info *attributes;
} code_attr;

typedef struct {
	unsigned short int acc_flags;
	unsigned short int name_index;
	unsigned short int desc_index;
	unsigned short int attr_count;
	unsigned short int instr_count;
	instr *instructions;
	attr_info *attr;
} member_info, method_info, field_info;

#endif
