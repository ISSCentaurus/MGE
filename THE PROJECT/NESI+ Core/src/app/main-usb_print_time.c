#include <nesi.h>
#include <string.h>

int main(void)
{
    // initialize all modules
    nesi.init();

    usb.connect();

    char message[128] = {0};

    while(1)
    {
        sprintf(message, "Time is: %s\r\n", dateTime.getStamp());
        usb.print(message);
        wait(1500);
    }

    return 0;
}
