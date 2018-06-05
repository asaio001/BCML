# BCML (Bytecode Manipulation Library)

Minimalist library written in C capable of directly interfacing with Java bytecode. 
Based on Oracle's VM specification of the Java class file format: https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html


Parses a full Java class file into the following structure:
```
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
```

## Instruction parsing
According to the VM spec, bytecode instructions are encapsulated in an attribute_info structure (whose name_index corresponds to a UTF-8 entry in the constant pool of value "Code") inside of each method_info structure of a class that contains all of its executable instructions.

At the moment, instruction parsing consists of just parsing the opcode of each instruction while skipping its payload. This means that things like modifying unique attributes of an instruction are not yet able to be done. The extent of instruction support consists of something like the following code snippet: 
```
	jclass class;
	if (!read_class(&class, CLASS_FILE_NAME)) {
		return 0;
	}


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

	dispose_class(&class);
```

Running it on a classic HelloWorld class gives the following output:
```
- Method: <init> ()V
	 - aload_0
	 - invokespecial
	 - return
- Method: main ([Ljava/lang/String;)V
	 - getstatic
	 - ldc
	 - invokevirtual
	 - return
```


In the future I will hopefully implement an individual structure per instruction, and so the ability to modify each instruction's attributes at will.

## Test case
A small code snippet showing a test case of the library that iterates through the constant pool of a standard HelloWorld class, finds the UTF8 entry that matches the HELLO_WORLD constant, and replaces it with a modified string, NEW_STRING:
```
	jclass class;
	if (!read_class(&class, CLASS_FILE_NAME)) {
		return 0;
	}


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

	if (!write_class(class, CLASS_FILE_NAME)) {
		return 0;
	}

	dispose_class(&class);
```

