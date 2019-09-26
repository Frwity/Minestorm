#include <stdio.h>
#include "my_functions.h"
#include <stdlib.h>

void my_put_char(char c)
{
        fwrite(&c, sizeof(char), 1, stdout);
	return;
}

void my_put_string(const char* str)
{
	if (str == NULL)
		return;
	for (int i = 0; str[i] != '\0'; i++)
	{
		my_put_char(str[i]);
	}
}

char const* find_string(char const* to_find, char const* str)
{
	if (to_find == NULL || str == NULL)
		return NULL;
	int lto_find = string_length(to_find);
	int lstri = string_length(str);
	if (lstri < lto_find)
		return NULL;
	int start = 0;
	int max = 0;
	while ( start < lstri - lto_find)
	{
		for (int i = 0; i < lto_find ;++i)
		{
			if (to_find[i] != str[i + start])
				break;
			++max;
			if (max == lto_find)
				return &str[start];
		}
		++start;
	}
	return NULL;
}

void my_put_number(int num)
{
	if (num == -2147483648)
	{
		my_put_string("-2147483648");
		return;
	}
	if (num < 0)
	{
		my_put_char('-');
		num = num - num - num;
	}
	int length = 1;
	for (int remainder = num; remainder >= 10 ; remainder /= 10)
		++length;
	
	char nb[length+1];
	nb[length] = '\0';
	int lastnb;
	for (int i = length; i >= 0 ; --i)
	{
		lastnb = num % 10;
		num = (num - lastnb) / 10;
		nb[i-1] = lastnb + 48;
	}
	my_put_string(nb);
	return;
}

void sort_char(char* tab, unsigned int tab_length)
{
	for (unsigned int i = 0; i < tab_length; ++i)
	{
		for (unsigned int i2 = 1; i2 + i < tab_length; ++i2)
		{
			if (tab[tab_length - i2] < tab[tab_length - i2 - 1])
				swap_char(&tab[tab_length - i2], &tab[tab_length - i2 - 1]);
		}
	}
}

int string_compare(char const* str1, char const* str2)
{
	if (str1[0] == '\0'&& str2[0] == '\0')
		return 0;
	int length1 = string_length(str1);
	int length2 = string_length(str2);
	int length = 0;
	if (length1 < length2)
		length = length1;
	else
		length = length2;
	for (int i = 0; i < length; i++)
	{
		if (str1[i] < str2[i])
			return -1;
		if (str1[i] > str2[i])
			return 1;
	}
	if (length1 < length2)
		return -1;
	else if (length1 > length2) 
		return 1;
	return 0;
}

unsigned int string_length(char const* str)
{
	if (str == NULL)
		return 0;
	int lgth = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		++lgth;
	}
	return lgth;
}

void swap_char(char* a, char* b)
{
	char temp = *a;
	*a = *b;
	*b = temp;
	return;	
}

int nb_square(int sqr, int nb)
{
	int nb2 = nb;
	if (sqr == 0)
		return 1;
	for (int i = 1; i < sqr; ++i)
		nb *= nb2;
	return nb;
}

char* string_duplicate(char const* str)
{
	if (str == NULL)
		return NULL;
	char* to_return = malloc((string_length(str) + 1) * sizeof(char));
	for(unsigned int i = 0; i < string_length(str); ++i)
		to_return[i] = str[i];
	to_return[string_length(str)] = '\0';
	return to_return;
}

char* int_to_alpha(int num)
{
	if (num == 0)
	{
		char* zero = malloc(2 * sizeof(char));
		zero[0] = '0';
		zero[1] = '\0';
		return zero;
	}
	int length = 0;
	int neg = 0;
	long nb = num;
	if (nb < 0)
	{
		++length;
		nb *= -1;
		neg = 1;
	}
	for (long i = 1; i <= nb; i *= 10)
		++length;	
	char* to_return = malloc(length + 1 * sizeof(char));
	if (nb == 0)
		to_return[0] = '0';
	int lastnb;
	for (int i = length; i > 0 ; --i)
	{
		lastnb = nb % 10;
		nb = (nb - lastnb) / 10;
		to_return[i - 1] = lastnb + 48;
	}
	if(neg)
		to_return[0] = '-';
	to_return[length] = '\0';
	return to_return;	
}
