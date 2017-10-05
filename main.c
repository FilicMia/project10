
#include <stdio.h>
#include <stdlib.h>
#include "simplemotion.h"

struct Bus{
  smint32 deviceAddress;
  smbus busHandle;
  
} bus;

struct Bus Bus1;
smint32 deviceAddress;
smbus busHandle;

void on_connect_clicked()
{
    busHandle=smOpenBus("dev/");
    if(busHandle>=0)
    {
        printf("Successfully connected bus ");
    }
    else
        printf("Couldn't connect to bus ");
}

void on_disconnect_clicked()
{
    printf("Closing bus (if open)");
    smCloseBus(busHandle);
}

int main(int argc, char **argv) {
   on_connect_clicked();
   printf("Hello, world!");
   on_disconnect_clicked();
   return 0;
}
