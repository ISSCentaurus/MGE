/*
 * File:   main-UART_receive_terminal.c
 * Author:  Dean Tate
 * Created on July 20, 2014
 */

#include <nesi.h>
#include <uart2.h>

/* This program acts as a terminal between the NESI and a PC
 * for debugging/demonstration purposes
 */

/*Functions*/
//------------------------------------------------------------------------------
void terminal_info(void);       // Prints intro message
void LED(char color, int DC);   // Inputs LED color R or B and duty cycle
void PD(char channel, int status);  // Power driver, inputs A or B and either on or off
void Q(int quadrant, char *type);   // Quadrant, inputs quad number and either value or voltage output
void pic(char *filename);   // Takes picture with input filename
int stringcompare(char *str1, char *str2);  // Compares two strings
void printz(char *string);   // Replaces uart2.send



// Stores received message and parsed values
struct parsevalues{
	char command;   //commaned issued from user over uart2
        char recd[40]; //received data from uart2
	char string[40];    //parsed char string from rdata
	int value;  //parsed int value from rdata
};

typedef struct parsevalues PARSE_struct;


int main(void)
{

    nesi.init();    // initialize all NESI modules
    uart2.init();           // UART 2 is ready to be used
    uart2.baudrate(9600);   // Set baudrate to 9600   
    
    PARSE_struct PARSE;
    char *rdata = &PARSE.recd;  // Stores the address of the message received
    terminal_info();    // Print intro message

    while(1)
    {
        int i = 0;
        char bracket[1] = {'>'};
        uart2.send(bracket,1);

        //Reset struct values back to 0x00 at each loop
        PARSE.command = '\0';
        PARSE.value = 0;
        for(i=0; i<40; ++i){
            PARSE.recd[i] = '\0';
            PARSE.string[i] = '\0';
        }
        rdata = &PARSE.recd;
        //End resset
        
        
        //Receive input string
        //---------------------------------------
        while(1){
            char abc[1] = {'/0'};
            if(uart2.receive(abc,1)){   //Wait until I get an input
                if(abc[0]==0x0D) break;     //Break from loop when input is 'enter'
                uart2.send(abc,1);  //Echo input
                *rdata = abc[0];    //Store input character into PARSE.recd string
                ++rdata;    //Increment PARSE.recd string placement
            }
        }

        //Parsing input string
        //--------------------------------------
        rdata = &PARSE.recd;    //Reset rdata to point to beginning of PARSE.recd
        PARSE.command = *rdata; //Store first value in PARSE.recd as the command
        ++rdata;
        i=0;
        while(*rdata){  //Continue loop till the end of input string
            while(*rdata== ' ') ++rdata;    //Skip spaces from input string
            if(*rdata >='0' && *rdata <= '9'){  //If value from string is an integer
                PARSE.value = PARSE.value*10 + *rdata - '0';    //Converts char to integer and stores into PARSE.value
            }
            else {
                PARSE.string[i] = *rdata;   //Store non-integer values into PARSE.string
                ++i;    //Increment PARSE.string string placement
            }
            ++rdata;
        }
        //End parsing-------------------------------

         //Lookup table for input commands
        //Passes parsed values into functions
        switch(PARSE.command){
            case 'L':  LED(*PARSE.string, PARSE.value); break;  //Inputs color (r or b) and duty cycle
            case 'D':  PD(*PARSE.string, PARSE.value); break;   //Inputs channel (a or b) and state (0 or 1)
            case 'Q':  Q(PARSE.value, PARSE.string); break;     //Inputs quad # (1 - 4) and state (val or volt)
            case 'P':  pic(PARSE.string); break;    //Inputs filename to save picture as
            default:  uart2.send("\n\rError:  incorrect command\n\r",40); terminal_info(); break;
        }

    }
     return(0);
}

/*Terminal_info*/
//------------------------------------------------------------------------------
void terminal_info(void){
    char message[128] = {};
    sprintf(message,"\n\rTerminal commands:\n\r");
    printz(message);
    sprintf(message,"  *LEDs:  (L)(color)(Duty cycle) | example:  L r 53 or L b 100\n\r");
    printz(message);
    sprintf(message,"  *Power drivers:  (D)(channel)(status) | example:  D a 1 or D b 0\n\r");
    printz(message);
    sprintf(message,"  *QX value:  (Q)(channel)(val) | example:  Q 1 val\n\r");
    printz(message);
    sprintf(message,"  *QX voltage:  (Q)(channel)(volt) | example:  Q 3 volt\n\r");
    printz(message);
    sprintf(message,"  *Camera:  (P)(filename) | example:  P myimage\n\r");
    printz(message);
}

/*LED*/
//------------------------------------------------------------------------------
void LED(char color, int DC){
    char message[128] = {0};
    if(color == 'r' || color == 'R'){   //Checks color of LED
        ledR.dutycycle(DC);
        sprintf(message, "\n\rRed LED at %d%s duty cycle\n\r",DC,"%");
        printz(message);
    }
    else if(color =='b' || color == 'B'){
        ledB.dutycycle(DC);
        sprintf(message, "\n\rBlue LED at %d%s duty cycle\n\r",DC,"%");
        printz(message);
    }
    else {
        sprintf(message, "\n\rError:  incorrect LED input\n\r");
        printz(message);
    }
}

/*Power driver*/
//------------------------------------------------------------------------------
void PD(char channel, int status){
    char message[128] = {0};
    if(channel == 'a' || channel == 'A'){   //Checks channel of power driver
        if(status == 1){    //Power driver on
            powerDriverA.on();
            sprintf(message,"\n\rPower driver A is on\n\r");
            printz(message);
        }
        else if(status == 0){   //Power driver off
            powerDriverA.off();
            sprintf(message,"\n\rPower driver A is off\n\r");
            printz(message);
        }
        else {
            sprintf(message,"\n\rError:  Power driver A status must be 1 or 0\n\r");
            printz(message);
        }
    }

    else if(channel == 'b' || channel == 'B'){  //Checks power driver channel
            if(status == 1) {       //Power driver on
                powerDriverB.on();  
                sprintf(message,"\n\rPower driver B is on\n\r");
                printz(message);
            }
            else if(status == 0){   //Power driver off
                powerDriverB.off();
                sprintf(message,"\n\rPower driver B is off\n\r");
                printz(message);
            }
    }
        else {
            sprintf(message,"\n\rError:  Power driver B status must be 1 or 0\n\r");
            printz(message);
        }
}

/*Quadrant resistive sensors*/
//------------------------------------------------------------------------------
void Q(int quadrant, char *type){
    int valQ;   //Stores raw input value from resistive sensor
    double d_val;   //Stores voltage from input resistive sensor
    char message[128] = {0};
    char status[1] = {'\0'};    //Stores input character

    while(!status[0]){  //Continue to output values until received input from keyboard
        switch(quadrant){   //Lookup quadrant number
            case 1:{
                valQ = resistiveSensors.getQ1(5,50); break; //Record raw data from resisitve sensor
            }

            case 2:{
                valQ = resistiveSensors.getQ2(5,50); break;
            }

            case 3:{
                valQ = resistiveSensors.getQ3(5,50); break;
            }

            case 4:{
                valQ = resistiveSensors.getQ4(5,50); break;
            }
            default:{
                sprintf(message, "Error:  incorrect quadrant number.  Must be between 1-4\n\r");
                printz(message); 
                break;
            }
        }

        if(stringcompare(type, "val")){ //Outputs sensor data as raw value
                sprintf(message,"Q%d value = %d\n\r",quadrant,valQ);
                printz(message);
                wait(300);
        }
        else if(stringcompare(type, "volt")){   //Outputs sensor data as voltage
                d_val = 3.3*valQ/(double)1024;  // Converts 10 bit ADC value to a voltage
                sprintf(message,"Q%d voltage = %.3f\n\r",quadrant,d_val);   //Round decimal to 3 places
                printz(message);
                wait(300);
        }
        else{ 
            sprintf(message,"\n\rError:  Q type must be val or volt\n\r");
            printz(message);
            break;
        }
        uart2.receive(status,1);    //Check if input from keyboard and store to "status"
    }

}

/*Take picture*/
//------------------------------------------------------------------------------
void pic(char *filename){
    char message[128] = {0};
    char savedas[40] = {0};    //Addes ".jpg" to the end of filename


    sprintf(savedas,"%s.jpg",filename);     //Saves file as .jpg

    // disconnect USB SD card (in case routine writes to SD memory)
    usb.eject();
    
    sprintf(message,"\n\rTaking picture...\n\r");
    printz(message);

    // take a picture
    camera.on();
    camera.getPix(savedas);
    camera.off();
    sprintf(message,"Picture saved as %s\n\r", savedas);
    printz(message);

    usb.connect();
}

/*String compare*/
//------------------------------------------------------------------------------
int stringcompare(char *str1, char *str2){
    int i=0;
    while(str1[i]==str2[i]){		//While each char is equal to each other, continue
            if(str1[i]=='\0' && str2[i]=='\0') return 1;	//If they've both reached a null,
            ++i;						//then they're equal
    }
    return 0;
}

/*Print strings*/
//------------------------------------------------------------------------------
void printz(char *string){
    int psize = 0;  //Size of string
    int i;
    char *ptr = string; //Pointer to input string

    while(*ptr){    //Counts size of the string and stores to psize
        ++psize;
        ++ptr;
    }

    uart2.send(string,psize);   //Sends string

    for(i=0; i<128; ++i){   //Resets input string so another value can be stored without overwriting
        string[i] = '\0';
    }    
}