#ifndef __MY_FUNCTIONS_H__
#define __MY_FUNCTIONS_H__

void my_put_char(char c);
void my_put_string(const char* str);
char const* find_string(char const* to_find, char const* str);
void my_put_number(int num);
void sort_char(char* tab, unsigned int tab_length);
int string_compare(char const* str1, char const* str2);
unsigned int string_length(char const* str);
void swap_char(char* a, char* b);
int nb_square(int sqr, int nb);
char* string_duplicate(char const* str);
char* int_to_alpha(int num);



#endif
