#include <stdio.h>

int main()
{
    int c;
    long spaces = 0;
    long tabs = 0;
    long newlines = 0;

    while ((c = getchar()) != EOF)
    {
        if (c == ' ')
        {
            spaces++;
        }
        else if (c == '\t')
        {
            tabs++;
        }
        else if (c == '\n')
        {
            newlines++;
        }

        putchar(c);
    }

    printf("\n\nSpaces\tTabs\tNewlines\n%ld\t%ld\t%ld\n", spaces, tabs, newlines);
}
