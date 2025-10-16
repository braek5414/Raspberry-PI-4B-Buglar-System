// Burglar Alarm Application

// gcc -std=gnu11 -Wall -g  -o alarm_client balarm_client.c udp3.c -lpigpio
// sudo ./alarm_client ###.###.#.#


#include <stdlib.h>          // EXIT_ codes
#include <stdbool.h>         // bool
#include <stdio.h>           // printf, scanf
#include <string.h>          // strlen, strcmp
#include <poll.h>            // poll
#include <unistd.h>          // sleep
#include <assert.h>          // assert
#include "udp3.h"            // custom udp library
#include "pigpio.h"          // RPI GPIO library
#include "kbhit.h"

#define SERVER_LISTENER_PORT 4096

#define TIMEOUT_SECONDS 25

// GPIO pins
// TODO.  Set the constants for the GPIO pins
//        that will be used
#define DOOR       2
#define PIR        14
#define ARM_DISARM 3

#define TIMER 1         // The first timer
#define IO_DEBUG false  // Turn on for debug prints

// global variables
bool armed = false;
char *remoteIp = NULL;
int remotePort = SERVER_LISTENER_PORT;

bool isAnyKeyPressed()
{
    struct pollfd pfd;
    pfd.fd = 1;
    pfd.events = POLLIN;
    poll(&pfd, 1, 1);
    return pfd.revents == POLLIN;
}

// This function is called when the door GPIO pin changes value
void door(int gpio, int level, uint32_t tick)
{
    if (IO_DEBUG)
       printf("%d %d\n",gpio,level);
    assert(gpio==DOOR);
    char strD[] = {"door"};
    sendUdpData(remoteIp, remotePort, strD);
    printf("Door open\n");
    
}

// This function is called when the arm disarm button is pressed
void armdisarm(int gpio, int level, uint32_t tick)
{
    if (IO_DEBUG)
        printf("%d %d\n",gpio,level);
    assert(gpio==ARM_DISARM);
    if (!level)
    {
        armed=!armed;
        if (armed)
        {
            printf("Arming system\n");
            sendUdpData(remoteIp, remotePort, "armed");
        }
        else
        {
            printf("Disrming system\n");
            sendUdpData(remoteIp, remotePort, "disarmed");
        }
    }
}

// This function is called when the PIR sensor changes value
void pir(int gpio, int level, uint32_t tick)
{
    if (IO_DEBUG)
       printf("%d %d\n",gpio,level);
    assert(gpio == PIR);
    if (level && armed)
    {
       printf("motion\n");
       sendUdpData(remoteIp, remotePort, "motion");
    }
}

void timer()
{
    printf("sending pulse\n");
    sendUdpData(remoteIp, remotePort, "pulse");
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // turn buffering off for stdout
    // needed due to the asynchronous nature of the callbacks
    setbuf(stdout, NULL);

    bool quit = false;
    char str[100];
    char quitcheck[] = {"q"};

    // Verify arguments are good
    bool goodArguments = (argc == 2);
    if (goodArguments)
        remoteIp = argv[1];
    if (!goodArguments)
    {
        printf("usage: alarm IPV4_ADDRESS\n");
        printf("  where:\n");
        printf("  IPV4_ADDRESS is address of the remote machine\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the library
    gpioInitialise();

    
    gpioSetMode(DOOR, 0);
    gpioSetMode(ARM_DISARM, 0);
    gpioSetMode(PIR, 0);

 

    gpioSetAlertFunc(DOOR, door);
    gpioSetAlertFunc(ARM_DISARM, armdisarm);
    gpioSetAlertFunc(PIR, pir);

    // Debounce the switch(s)
    gpioGlitchFilter(DOOR, 100 );
    gpioGlitchFilter(ARM_DISARM, 100);

    // And set up the timer callback
    gpioSetTimerFunc(TIMER, TIMEOUT_SECONDS * 1e3, timer);

    // Send disarmed message
    sendUdpData(remoteIp, remotePort, "disarmed");
    printf("disarmed\n");

    // Loop until 'q' is pressed on keyboard
    while (!quit)
    {
      
        if (isAnyKeyPressed())
        {
           
            fgets(str, sizeof(str), stdin);
            if (strncmp(quitcheck, str, 1) == 0)
            {
                quit = true;
            }
        }
        if (!quit)
        {
            // Sleep for a second
            sleep(1);
        }
    }
    // Shut down the library
    gpioTerminate();
    return EXIT_SUCCESS;
}
