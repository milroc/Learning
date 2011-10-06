#include <stdio.h>
#include <string.h>

int main()
{
    char word[80];
    if (scanf("%[^\n]", word) == 1)
    {
        word[0] = 'W';
        word[6] = 'H';
        printf(word);
    }
    return 0;
}
