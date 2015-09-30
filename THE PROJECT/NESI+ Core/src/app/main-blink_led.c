#include <nesi.h>

int main(void)
{
    // initialize all modules
    nesi.init();

    while(1)
    {
        ledB.dutycycle(50);
        ledR.dutycycle(50);
        wait(500);
        ledB.dutycycle(0);
        ledR.dutycycle(0);
        wait(500);
    }

    return 0;
}
