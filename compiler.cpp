#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


void add_label(char*name, int ins_no);
void if_handle(int table_index, int flag);

struct identifier
{
	char*name;
	int address;
	int size;
	bool isConstant;
};

struct label_struct
{
	char*name;
	int line_no;
};

char*registers = (char*)malloc(8 * sizeof(sizeof(char)));
label_struct** labels;
int id_index = 0,table_index=0,top=-1,lable_index=0;
identifier** data ;
int*stack =(int*)malloc(100*sizeof(int));

void push(int data)
{
	stack[++top] = data;
}
int pop()
{
	return stack[top--];
}

struct intermediate_code
{
	int op_code;
	int parameters[5];
};
intermediate_code** table;

void alloc()
{
	data = (identifier**)malloc(10 * sizeof(identifier*));
	for (int i = 0; i < 10; i++)
		data[i] = (identifier*)malloc(sizeof(identifier));
	table = (intermediate_code**)malloc(20* sizeof(intermediate_code*));
	for (int i = 0; i < 20; i++)
		table[i] = (intermediate_code*)malloc(sizeof(intermediate_code));
	labels = (label_struct**)malloc(10 * sizeof(label_struct*));
	for (int i = 0; i < 10; i++)
		labels[i] = (label_struct*)malloc(sizeof(label_struct));
}

void* memory = malloc(1000);

void strip(char *s)
{
	char *p2 = s;
	while (*s != '\0')
	{
		if (*s != '\t' && *s != '\n')
			*p2++ = *s++;
		else
			++s;
	}
	*p2 = '\0';
}

char* mystrsep(char** stringp, const char* delim)
{
	char* start = *stringp;
	char* p;
	p = (start != NULL) ? strpbrk(start, delim) : NULL;
	if (p == NULL)
	{
		*stringp = NULL;
	}
	else
	{
		*p = '\0';
		*stringp = p + 1;
	}
	return start;
}

void alloc_data(char* cmd)
{
	char* buffer = (char*)malloc(100 * sizeof(char));
	strcpy(buffer, cmd);
	if (strcmp(mystrsep(&buffer, " "), "DATA") == 0)
	{

		data[id_index]->name = (char*)malloc(10 * sizeof(char));
		strcpy(data[id_index]->name, mystrsep(&buffer, "["));
		if (buffer != NULL)
		{
			data[id_index]->size = atoi(mystrsep(&buffer, "]"));
		}
		else
			data[id_index]->size = 1;
			
		if (id_index == 0)
			data[id_index]->address = 0;
		data[id_index]->isConstant = false;
		data[id_index + 1]->address = data[id_index]->address + data[id_index]->size;
		id_index++;
	}
	else
	{
		data[id_index]->name = (char*)malloc(10 * sizeof(char));
		strcpy(data[id_index]->name, mystrsep(&buffer, "="));
		*((char*)(&memory + data[id_index]->address))= atoi(mystrsep(&buffer, "="));
		data[id_index + 1]->address = data[id_index]->address + 1;
		data[id_index]->size = 0;
		data[id_index]->isConstant = true;
		id_index++;		
	}
}

void print_symboltable()
{
	for (int i = 0; i < id_index; i++)
	{
		printf("%s %d %d %d\n", data[i]->name,data[i]->size, data[i]->address, data[i]->isConstant);
	}
}
void load_data(FILE* fp)
{
	char* buffer = (char*)malloc(100 * sizeof(char));
	while (fgets(buffer, 256, fp) != NULL)
	{
		strip(buffer);
		if (strcmp(buffer, "START:") == 0)

			break;
		else
		{
			alloc_data(buffer);
		}
	}	
}
int find_register(char* reg)
{
	if (strcmp(reg, "AX") == 0)
		return 0;
	if (strcmp(reg, "BX") == 0)
		return 1;
	if (strcmp(reg, "CX") == 0)
		return 2;
	if (strcmp(reg, "DX") == 0)
		return 3;
	if (strcmp(reg, "EX") == 0)
		return 4;
	if (strcmp(reg, "FX") == 0)
		return 5;
	if (strcmp(reg, "GX") == 0)
		return 6;
	if (strcmp(reg, "HX") == 0)
		return 7;

}
int find_identifier(char* name)
{
	for (int i = 0; i < id_index; i++)
	{
		strip(data[i]->name);
		if (strcmp(data[i]->name, name) == 0)
			return i;
	}
	return -1;
}
int find_opcode(char* op)
{
	if (strcmp(op, "MOV_R") == 0)
		return 1;
	if (strcmp(op, "MOV_M") == 0)
		return 2;
	if (strcmp(op, "ADD") == 0)
		return 3;
	if (strcmp(op, "SUB") == 0)
		return 4;
	if (strcmp(op, "MUL") == 0)
		return 5;
	if (strcmp(op, "JUMP") == 0)
		return 6;
	if (strcmp(op, "IF") == 0)
		return 7;
	if (strcmp(op, "EQ") == 0)
		return 8;
	if (strcmp(op, "LT") == 0)
		return 9;
	if (strcmp(op, "GT") == 0)
		return 10;
	if (strcmp(op, "LTEQ") == 0)
		return 11;
	if (strcmp(op, "GTEQ") == 0)
		return 12;
	if (strcmp(op, "PRINT") == 0)
		return 13;
	if (strcmp(op, "READ") == 0)
		return 14;
	if (strcmp(op, "ELSE") == 0)
		return 15;
}
void execute_cmds(FILE* fp)
{
	char* buffer = (char*)malloc(100 * sizeof(char));
	while (fgets(buffer, 256, fp) != NULL)
	{
		strip(buffer);
		if (strcmp(buffer, "START:") == 0)
			break;
		else
		{
			alloc_data(buffer);
		}
	}
}
int find_label(char*name)
{
	for (int i = 0; i < lable_index; i++)
	{
		if (strcmp(labels[i]->name, name) == 0)
			return (labels[i]->line_no);
	}
}
void add_to_intermediatecode(char* param,int ins_no,int no_of_parameters,char* opname)
{
	int op_code = find_opcode(opname);
	int i = 0;
	if (op_code == 1)
	{
		table[table_index]->op_code = op_code;
		char* name = mystrsep(&param, " ");

		table[table_index]->parameters[0] = find_identifier(mystrsep(&name,"["));
		if (name != NULL)
			table[table_index]->parameters[2] = atoi(mystrsep(&name, "]"));
		else
			table[table_index]->parameters[2] = 0;
		table[table_index]->parameters[1] = find_register(mystrsep(&param, " "));	
		table_index++;
	}
	if (op_code == 2)
	{
		table[table_index]->op_code = op_code;
		table[table_index]->parameters[0] = find_register(mystrsep(&param, " "));
		char* name = mystrsep(&param, " ");

		table[table_index]->parameters[1] = find_identifier(mystrsep(&name, "["));
		if (name != NULL)
			table[table_index]->parameters[2] = atoi(mystrsep(&name, "]"));
		else
			table[table_index]->parameters[2] = 0;
		
		
		table_index++;
	}
	if (op_code == 6)
	{
		table[table_index]->op_code = op_code;
		table[table_index]->parameters[0] = find_label(mystrsep(&param, " "));
		
		table_index++;
	}
	if (op_code == 3 || op_code == 4 || op_code == 5)
	{
		table[table_index]->op_code = op_code;
		table[table_index]->parameters[0] = find_register(mystrsep(&param, " "));
		table[table_index]->parameters[1] = find_register(mystrsep(&param, " "));
		table[table_index]->parameters[2] = find_register(mystrsep(&param, " "));
		table_index++;
	}
	if (op_code == 13 || op_code == 14)
	{
		table[table_index]->op_code = op_code;
		table[table_index]->parameters[0] = find_register(mystrsep(&param, " "));
		
		table_index++;
	}
	if (op_code == 7)
	{
		table[table_index]->op_code = op_code;
		table[table_index]->parameters[0] = find_register(mystrsep(&param, " "));
		table[table_index]->parameters[1] = find_opcode(mystrsep(&param, " "));
		table[table_index]->parameters[2] = find_register(mystrsep(&param, " "));
		if_handle(table_index,0);
		table_index++;
	}
	if (op_code == 15)
	{
		table[table_index]->op_code = op_code;
		if_handle(table_index, 1);
		table_index++;
	}
}
void if_handle(int table_index,int flag)
{
	if (flag==0)
		push(table_index);
	else if (flag==1)
	{
		
		table[pop()]->parameters[3] = table_index+1;
		push(table_index);
	}
	else if (flag == 2)
	{
		table[pop()]->parameters[0] = table_index;
	}
}

void intermediate_generation(FILE* fp)
{
	int ins_no = 0;
	char* buffer = (char*)malloc(100 * sizeof(char));
	while (fgets(buffer, 256, fp) != NULL)
	{
		strip(buffer);
		char* temp = (char*)malloc(100 * sizeof(char));
		strcpy(temp,buffer);
		char* name = (char*)malloc(100 * sizeof(char));
		strcpy(name, mystrsep(&temp, " "));	
		if (strcmp(name, "MOV_R") == 0)
		{
			add_to_intermediatecode(temp,ins_no,3,name);
			ins_no++;
		}
		else if (strcmp(name, "MOV_M") == 0)
		{
			add_to_intermediatecode(temp,ins_no,3,name);
			ins_no++;
		}
		else if (strcmp(name, "ADD") == 0 || strcmp(name, "SUB") == 0 || strcmp(name, "MUL") == 0)
		{
			add_to_intermediatecode(temp,ins_no,2,name);
			ins_no++;
		}
		else if (strcmp(name, "READ") == 0 || strcmp(name, "PRINT") == 0)
		{
			add_to_intermediatecode(temp,ins_no,1,name);
			ins_no++;
		}
		else if (name[strlen(name)-1]==':')
		{
			add_label(name,ins_no);
		}
		else if (strcmp(name, "JUMP") == 0)
		{
			add_to_intermediatecode(temp, ins_no, 1, name);
			ins_no++;
		}
		else if (strcmp(name, "IF") == 0)
		{
			add_to_intermediatecode(temp, ins_no, 4, name);
			ins_no++;
		}
		else if (strcmp(name, "ELSE") == 0)
		{
			add_to_intermediatecode(temp, ins_no, 1, name);
			ins_no++;
			
		}
		else if (strcmp(name, "ENDIF") == 0)
		{
			if_handle(table_index,2);
		}
	}
}
void add_label(char*name,int ins_no)
{
	name[strlen(name) - 1] = '\0';
	labels[lable_index]->name = (char*)malloc(10 * sizeof(char*));
	strcpy(labels[lable_index]->name, name);
	labels[lable_index]->line_no = ins_no;
	lable_index++;

}
void display_intermediatecode()
{
	for (int i = 0; i < table_index; i++)
	{
		printf("%d ", table[i]->op_code);
		for (int j = 0; j < 5; j++)
			printf("%d  ", table[i]->parameters[j]);
		printf("\n");
	}
}
int eval_expression(char op1, char op2, int operat)
{
	if (operat==8)
	{
		return op1==op2;
	}
	if (operat==9)
	{
		return op1 < op2;
	}
	if (operat == 10)
	{
		return op1> op2;
	}
	if (operat == 11)
	{
		return op1 <= op2;
	}
	if (operat == 12)
	{
		return op1 >= op2;
	}

}
void execute_intermediatecode()
{
	for (int i = 0; i < table_index; i++)
	{
		if (table[i]->op_code == 1)
		{	
			memcpy(&memory + (table[i]->parameters[0] + table[i]->parameters[2])*sizeof(char), &registers[table[i]->parameters[1]], sizeof(char));
		}
		if (table[i]->op_code==2)
		{
			memcpy(&registers[table[i]->parameters[0]], &memory + (table[i]->parameters[1] + table[i]->parameters[2])*sizeof(char), sizeof(char));
		}
		if (table[i]->op_code == 3)
		{
			registers[table[i]->parameters[0]] = registers[table[i]->parameters[1]] + registers[table[i]->parameters[2]];
		}
		if (table[i]->op_code == 4)
		{
			registers[table[i]->parameters[0]] = registers[table[i]->parameters[1]] - registers[table[i]->parameters[2]];
		}
		if (table[i]->op_code == 5)
		{
			registers[table[i]->parameters[0]] = registers[table[i]->parameters[1]] * registers[table[i]->parameters[2]];
		}
		if (table[i]->op_code == 13)
		{
			printf("%d\n", registers[table[i]->parameters[0]]);
		}
		if (table[i]->op_code == 14)
		{
			printf("Enter the value:");
			scanf("%d", &registers[table[i]->parameters[0]]);
		}
		if (table[i]->op_code == 6)
		{
			i = table[i]->parameters[0];
		}
		if (table[i]->op_code == 7)
		{
			if (eval_expression(table[i]->parameters[0], table[i]->parameters[2], table[i]->parameters[1]))
			i = table[i]->parameters[3];
		}
		if (table[i]->op_code == 15)
		{
			
				i = table[i]->parameters[0];
				
		}

	}
}
void write_intermediate_code_to_file(char* file_name)
{
		FILE*fp = fopen(file_name, "wb");
		fwrite(&table_index, sizeof(int), 1, fp);
		fwrite(&lable_index, sizeof(int), 1, fp);
		fwrite(&id_index, sizeof(int), 1, fp);
		fwrite(table,sizeof(intermediate_code),table_index,fp);
		fwrite(labels, sizeof(label_struct), lable_index, fp);
		fwrite(data, sizeof(identifier), id_index, fp);
		fclose(fp);

}
void read_intermediate_code(char* file_name)
{
	FILE*fp = fopen(file_name, "rb");
	int rows=0,lable_count=0,data_index;
	

	fread(&rows, sizeof(int), 1, fp);
	fread(&lable_count, sizeof(int), 1, fp);
	fread(&data_index, sizeof(int), 1, fp);

	identifier**data = (identifier**)malloc(data_index * sizeof(identifier*));
	for (int i = 0; i < data_index; i++)
		data[i] = (identifier*)malloc(sizeof(identifier));
	intermediate_code**table = (intermediate_code**)malloc(rows*sizeof(intermediate_code*));
	for (int i = 0; i < rows;i++)
		table[i] = (intermediate_code*)malloc(sizeof(intermediate_code));
	label_struct**labels = (label_struct**)malloc(lable_count * sizeof(label_struct*));
	for (int i = 0; i < lable_count; i++)
		labels[i] = (label_struct*)malloc(sizeof(label_struct));

	
	fread(table, sizeof(intermediate_code), rows, fp);
	fread(labels, sizeof(label_struct), lable_count, fp);
	fread(data, sizeof(identifier), id_index, fp);
	fclose(fp);
}

int main()
{
	alloc();
	FILE* fp = fopen("C:\\Users\\ThatavarthiS\\Desktop\\code.txt", "r");
	load_data(fp);
	print_symboltable();
	intermediate_generation(fp);
	display_intermediatecode();
	
	execute_intermediatecode();
	
	system("pause");
	return 0;
}




