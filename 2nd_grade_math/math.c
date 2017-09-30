#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define  STD_NULL         ((void*)0)
#define  MAX_NUMBERS	10
#define  MAX_SECTIONS	10

typedef int (*print_const_t)(char *buf, int id);

typedef enum{
	ADDITION,
	SUBTRACTION,
	MIXED_ADD_SUB,
	UNKNOWN
} equation_type_t;

typedef struct{
	int integer[MAX_NUMBERS];
	char operator[MAX_NUMBERS-1];
	int n_integer;
	int result;
} equation_t;

typedef enum{
	EQUATION,
	CONST,
} section_type_t;

typedef struct {
	section_type_t s_type;

	equation_type_t e_type;
	int n_integer;
	int range;
	int n_equation;
	int n_per_line;
	int b_left_blank;

	print_const_t print_const;
	int const_id;

	int n_line_feed;
} print_section_t;

typedef struct {
	print_section_t sections[MAX_SECTIONS];
	int n_section;
} print_profile_t;

int _2grade_test_print_const(char *buf, int id)
{
	int len = 0;

	if(id == 0) {
		len += sprintf(buf+len, "苏州市平江实验学校二年级上册计算过关\n");
		len += sprintf(buf+len, "班级_______   姓名_______   学号_______   成绩\n");
		len += sprintf(buf+len, "一、直接写出得数。（66分）\n");
	} else if(id == 1) {
		len += sprintf(buf+len, "二、填空。（10分）\n");
	} else if(id == 2) {
		len += sprintf(buf+len, "三、竖式计算（24分）\n");
	} else {
		printf("[_2grade_test_print_const] invalid id!\n");
	}

	return len;
}

static print_profile_t g_profiles[] = {
	{
		{
		//s_type, e_type, n_integer, range, n_equation, n_per_line, b_left_blank, print_const, const_id, n_line_feed;
			{CONST, UNKNOWN, 0, 0, 0, 0, 0, _2grade_test_print_const, 0, 0},
			{EQUATION, MIXED_ADD_SUB, 2, 100, 54, 6, 0, NULL, 0, 0},
			{EQUATION, MIXED_ADD_SUB, 3, 100, 6, 6, 0, NULL, 0, 0},
			{CONST, UNKNOWN, 0, 0, 0, 0, 0, _2grade_test_print_const, 1, 0},
			{EQUATION, MIXED_ADD_SUB, 2, 100, 10, 4, 1, NULL, 0, 0},
			{CONST, UNKNOWN, 0, 0, 0, 0, 0, _2grade_test_print_const, 2, 0},
			{EQUATION, MIXED_ADD_SUB, 3, 100, 4, 4, 0, NULL, 0, 2},
			{EQUATION, MIXED_ADD_SUB, 3, 100, 4, 4, 0, NULL, 0, 2}
		},
		8
	}
};

void generate_single_equation(equation_type_t type, unsigned int range, unsigned int result, equation_t * equation)
{
	unsigned int a;
	equation_type_t t;

	t = type;
	if(t == MIXED_ADD_SUB) {
		if(result < (range/3))
			t = SUBTRACTION;
		else if(result > (range*2/3))
			t = ADDITION;
		else
			t = (rand()&1) ? ADDITION : SUBTRACTION;
	}

	if(t == ADDITION) {
		a = 0;
		while(a < range/10 || (result-a) < range/10)
			a = (unsigned int)rand()%result;
		equation->integer[0] = a;
		equation->integer[1] = result-a;
		equation->operator[0] = '+';
	} else {
		a = result;
		while((a-result) < range/10)
			a = ((unsigned int)rand()%(range-result))+result;
		equation->integer[0] = a;
		equation->integer[1] = a-result;
		equation->operator[0] = '-';
	}

	equation->result = result;
	equation->n_integer = 2;
}

void generate_equation(equation_type_t type, int n_integer, unsigned int range, equation_t *equation)
{
	int i;
	equation_t  e;
	unsigned int result;

	if(n_integer > MAX_NUMBERS || n_integer < 2) {
		printf("[generate_equation] length not supported!\n");
		return;
	}

	result = 0;
	while(result < range/10) {
		result = (unsigned int)rand()%range;
	}

	i = n_integer - 1;
	equation->integer[i] = result;
	while(i > 0) {
		generate_single_equation(type, range, equation->integer[i], &e);

		equation->integer[i-1] = e.integer[0];
		equation->integer[i] = e.integer[1];
		equation->operator[i-1] = e.operator[0];
		i--;
	}

	equation->n_integer = n_integer;
	equation->result = result;
}

int print_equation_section(char *buf, equation_t *equation,
	int n_equation, int n_per_line, int b_left_blank, int line_feed)
{
	int i, j, n, len = 0;
	int blank;
	n = n_per_line;
	for(i=0; i<n_equation; i++) {
		equation_t *e = equation+i;
		if(b_left_blank)
			blank = (unsigned int)rand()%e->n_integer;
		for(j=0; j<e->n_integer; j++) {
			if(b_left_blank && blank == j)
				len += sprintf(buf+len, "(    )");
			else
				len += sprintf(buf+len, "%d", e->integer[j]);
			if(j < e->n_integer-1)
				len += sprintf(buf+len, "%c", e->operator[j]);
		}
		if(b_left_blank)
			len += sprintf(buf+len, "=%d         ", e->result);
		else
			len += sprintf(buf+len, "=         ");
		n--;
		if(n == 0) {
			len += sprintf(buf+len, "\n");
			n = n_per_line;
		}
	}

	if(n != n_per_line)
		len += sprintf(buf+len, "\n");
	for(i=0; i<line_feed; i++)
		len += sprintf(buf+len, "\n");

	return len;
}

int main()
{
	FILE * pFile;
	char *buffer;
	int profile, page, i, j, len;

	/*
	printf("Profile: \n");
	scanf("%d", &profile);
	if(profile > sizeof(g_profiles)/sizeof(print_profile_t)) {
		printf("[main] bad profile id!\n");
		return 0;
	}
	*/
	profile = 0;

	printf("Page: \n");
	scanf("%d", &page);
	if(page == 0 || page > 100) {
		printf("[main] page between 1~100!\n");
		return 0;
	}

	pFile = fopen ("math.txt", "wb");
	buffer = (char*)malloc(1024*1024);
	len = 0;

	srand( (unsigned)time(STD_NULL) );

	while(page > 0) {
		for(i=0; i<g_profiles[profile].n_section; i++) {
			print_section_t *p_section = &(g_profiles[profile].sections[i]);

			if(p_section->s_type == EQUATION) {
				equation_t *p_equation = (equation_t*)malloc(sizeof(equation_t)*p_section->n_equation);
				for(j=0; j<p_section->n_equation; j++) {
					generate_equation(p_section->e_type, p_section->n_integer, p_section->range, p_equation+j);
				}
				len += print_equation_section(buffer+len, p_equation,
					p_section->n_equation, p_section->n_per_line, p_section->b_left_blank, p_section->n_line_feed);
				free(p_equation);
			} else if(p_section->s_type == CONST) {
				len += p_section->print_const(buffer+len, p_section->const_id);
			} else {
				printf("[main] unknown section!\n");
			}
		}
		//len += sprintf(buffer+len, "\n\n\n");
		page--;
	}

	fwrite (buffer , sizeof(char), len, pFile);

	free(buffer);
	fclose (pFile);

	printf("Check math.txt\n\r");

	system("pause");
	return 0;
}

/*
int main()
{
	FILE * pFile;
	char *buffer;

	int start, stop, range, number, size;
	unsigned int i, t, r, a, s;
	printf("Range Start: \n\r");
	scanf("%d", &start);
	printf("Range Stop: \n\r");
	scanf("%d", &stop);
	if(start > stop) {
		printf("Start bigger than stop!\n\r");
		return 0;
	}
	if((start+100) > stop) {
		printf("Too close!\n\r");
		return 0;
	}

	printf("Count(1-5000): \n\r");
	scanf("%d", &number);
	if(number<1 || number>5000) {
		printf("Error!\n\r");
		return 0;
	}

	pFile = fopen ("math.txt", "wb");
	buffer = (char*)malloc(number*50);
	size = 0;
	range = stop-start;
	srand( (unsigned)time(STD_NULL) );

	for(i=0; i<number; i++) {
		t = (unsigned int)rand()&1;
		r = 0;
		//printf("xxx %d, %d\n\r", t, 1<<(sizeof(int)-1));
		while(r<(range/3)) {
			r = (unsigned int)rand()%range+1;
		}
		a = (unsigned int)rand()%r;
		if(a<(range/10) || (r-a)<(range/10))
			a = (unsigned int)rand()%r;
		if(a<(range/10) || (r-a)<(range/10))
			a = (unsigned int)rand()%r;
		if(t)
			size += sprintf(buffer+size, "%d + %d =", a, r-a);
		else
			size += sprintf(buffer+size, "%d - %d =", r, a);

		if(((i+1)%4) == 0)
			size += sprintf(buffer+size, "\n");
		else
			size += sprintf(buffer+size, "                      ");
	}

	fwrite (buffer , sizeof(char), size, pFile);

	free(buffer);
	fclose (pFile);

	printf("Check math.txt\n\r");

	system("pause");
	return 0;
}
*/

