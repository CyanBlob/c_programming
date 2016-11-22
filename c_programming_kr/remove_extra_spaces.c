#include <stdio.h>

int main()
{
    int c;
    long spaces = 0;
    long tabs = 0;
    long newlines = 0;
    int lastCharWasSpace = 0;

    while ((c = getchar()) != EOF)
    {
        if (lastCharWasSpace == 0 || c != ' ')
        {
            putchar(c);
        }

        if (c == ' ')
        {
            spaces++;
            lastCharWasSpace = 1;
        }
        else if (c == '\t')
        {
            tabs++;
            lastCharWasSpace = 0;
        }
        else if (c == '\n')
        {
            newlines++;
            lastCharWasSpace = 0;
        }
        else
        {
            lastCharWasSpace = 0;
        }

    }

    printf("\n\nSpaces\tTabs\tNewlines\n%ld\t%ld\t%ld\n", spaces, tabs, newlines);
}
