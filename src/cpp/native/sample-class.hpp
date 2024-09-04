
#include <stdio.h>

class SampleClass
{
public:
    SampleClass()
    {
        printf("SampleClass::SampleClass() Constructor\n");
    }

    double test_fn_1(int x)
    {
        printf("SampleClass::test_fn_1() %d\n", x);
        double y = 0;
        for (int i = 0; i < x; i++)
        {
            for (int j = 0; j < x; j++)
            {
                y = y + 1;
            }
        }
        printf("SampleClass::test_fn_1() %f\n", y);
        return y;
    }
};