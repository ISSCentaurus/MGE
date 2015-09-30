#include <nesi.h>

int main(void)
{
    // initialize all modules
    nesi.init();

    usb.connect();

    char input[64] = {0};
    int bytesRead = 0;

    while(1)
    {
        if((bytesRead = usb.read(input,64)))
        {
            input[bytesRead] = '\0'; // terminate string
            usb.printf("At time %s, %d bytes of data received:\r\n\"%s\"\r\n\r\n",
                    dateTime.getStamp(), bytesRead, input);
        }
    }

    return 0;
}
