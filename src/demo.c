#include <stdio.h>
#include <math.h>

int main()
{
    int a = 1, d = 2, c;
    if(a <= d)
    {
        c += a;
        a = d;
        d -= c;
    }
    /* 定义字符串 */
    char ch[10] = "OK";
    char x, y = 'a';
    // 各种运算符操作
    c = a + d;
    c = a % d;
    a >> 1;
    if(a == d)
    {
        printf("%d", d);
    }
    else if(a == c)
    {
        printf("%f", 1.123);
    }

    double b = 1e-10;
    double e = 1E10;

    return 0;
}