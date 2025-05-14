#include "./fat.h"
#include "./io.h"

char stringcompare(char *string0, char *string1, int length)
{
	for(int i = 0; i < length; i++)
	{
		if(string0[i] != string1[i])
		{
			return 0;
		}
	}

	return 1;
}

void stringcopy(char *src, char *dest, int length)
{
    for(int i = 0; i < length; i++)
    {
        dest[i] = src[i];
    }
}