#include <stdio.h>

void main()
{
    float current = 0;
    int end = 300;
    int step = 20;
    float celsius;

    while (current <= end)
    {
        celsius = (5.0 / 9.0) * (current - 32.0);
        printf("%3.0f\t%6.1f\n", current, celsius);

        current += 20;
    }
}
