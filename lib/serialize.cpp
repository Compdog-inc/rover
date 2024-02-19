#include "serialize.h"

void reverse(char *str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char *str, int str_size, int d)
{
    int i = 0;
    do
    {
        if (i < str_size)
        {
            str[i++] = (x % 10) + '0';
        }
        x = x / 10;
    } while (x);

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
    {
        if (i < str_size)
        {
            str[i++] = '0';
        }
    }

    reverse(str, i < str_size ? i : str_size);
    str[i < str_size ? i : str_size - 1] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char *res, int res_size, int afterpoint)
{
    if (n < 0)
    {
        ftoa(-n, res, res_size - 1, afterpoint);
        for (int i = res_size - 2; i >= 0; i--)
        {
            res[i] = res[i - 1];
        }
        res[0] = '-';
        return;
    }

    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, res_size, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        if (i < res_size)
            res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, res_size - i - 1, afterpoint);
    }
}

struct fuint
{
    fuint(float f)
    {
        this->f = f;
    }

    fuint(uint32_t i)
    {
        this->i = i;
    }

    union
    {
        float f;
        uint32_t i;
    };
};

void encodeFloat(uint8_t *buf, float f)
{
    buf[0] = fuint(f).i & 0xFF;
    buf[1] = (fuint(f).i >> 8) & 0xFF;
    buf[2] = (fuint(f).i >> 16) & 0xFF;
    buf[3] = (fuint(f).i >> 24) & 0xFF;
}

float decodeFloat(uint8_t *buf)
{
    return fuint(((uint32_t)buf[0]) | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)).f;
}