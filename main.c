
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplemotion.h"

struct Bus{
  smint32 deviceAddress;
  smbus busHandle;
  
} bus;

struct Bus Bus1;
smint32 deviceAddress;
smbus busHandle = -1;

/*
 Append stre to str1.
 */
void appendString(char** str1, char* str2){
    char *newstring;
    if((newstring = malloc(strlen(*str1)+strlen(str2)+1)) != NULL){
           strcat(newstring,*str1);
	   strcat(newstring,str2);
    } else {
        printf("malloc failed!\n");
    }
    *str1 = newstring;
  
}

//MIAMIAMAIMAI TU JE SEGM fault
char* stringifySMBusErrors(SM_STATUS smStat, smint32 smDeviceErrors)
{
    char* errorString = (char *) malloc(4);
    errorString = strcpy(errorString,"");

    if( ((smStat!=SM_OK && smStat!=SM_NONE) || smDeviceErrors!=SMP_CMD_STATUS_ACK ))
    {
            char* errorFlags = (char *) malloc(4);
            errorFlags = strcpy(errorFlags,"");
            char* smErrorFlags = (char *) malloc(4);
	    smErrorFlags = strcpy(smErrorFlags,"");
            //these faults are from SM bus host side
            if(smStat&SM_ERR_NODEVICE) appendString(&errorFlags,"* NoDevice (check port name)");
	    if(smStat&SM_ERR_PARAMETER) appendString(&errorFlags, "* InvalidParameter (API)");
            if(smStat&SM_ERR_COMMUNICATION) appendString(&errorFlags, "* Communication (cheksum mismatch)<br>");
            if(smStat&SM_ERR_LENGTH) appendString(&errorFlags, "* DataLegth (timeout or app error)");
            if(smStat&SM_ERR_BUS)  appendString(&errorFlags,"* BusError");
            

            if(!(smStat&SM_ERR_NODEVICE))//ignore device side faults if nodevice is active because it would make no sense
            {
                //device errors are read from the device (so connection must be working). these are error flags of device side of SM bus
                if(smDeviceErrors&SMP_CMD_STATUS_NACK) appendString(&smErrorFlags,"* Command fail (NACK)<br>");
                if(smDeviceErrors&SMP_CMD_STATUS_INVALID_ADDR) appendString(&smErrorFlags, "* Invalid param address<br>");
                if(smDeviceErrors&SMP_CMD_STATUS_INVALID_VALUE) appendString(&smErrorFlags,"* Invalid param value<br>");
                if(smDeviceErrors&SMP_CMD_STATUS_VALUE_TOO_HIGH) appendString(&smErrorFlags, "* Value too high<br>");
                if(smDeviceErrors&SMP_CMD_STATUS_VALUE_TOO_LOW) appendString(&smErrorFlags,"* Value too low<br>");

            }
            
            if(strlen(errorFlags)){
                errorString = "Bus error flags:";
		appendString(&errorString,errorFlags);
	    }else
                errorString="Communication error. Device errors:";
            if(strlen(smErrorFlags)) strcat(errorString,smErrorFlags);
    }
    return errorString;
}

int checkAndReportSMBusErrors(int fast)
{
     char* errs = "";
 
    /*SM BUS & SM DEVICES HAVE THREE CATEGORIES OF STATUS & ERROR BITS:
     *
     *1) BUS STATUS & ERROR BITS. THESE ARE RETURNED ON EACH SM LIBRARY CALL (THE SM_STAUTS TYPE)
     *   AND ACCUMULATED INTO A INTERNAL VARIABLE THAT MAY BE READ BY GETCUMULATIVESTATUS FUNCTION.
     *   THIS VALUE REPORTS ERRORS THAT HAPPEN ON WITH COMMUNICATION LAYER (PHYISCAL DEVICE PROBLEMS
     *   SUCH AS NOT AVAILABLE OF BUS DEVICE OR CHECKSUM ERROR).
     *
     *2) DEVICE SIDE SM STATUS & ERROR BITS. READING THIS REQUIRES WORKING CONNECTION TO A TARGET DEVICE
     *   IN ORDER TO RE 
AD SMP_CUMULATIVE_STATUS PARAMETER.
     *   THIS VALUE CONTAINS ERRORS THAT SUCCESSFULLY WERE TRANSFERRED TO TARGET BUT WERE NOT ACCEPTED
     *   BY SOME REASON (I.E. IF INVALID PARAMTER ADDRESS OR VALUE WAS USED).
     *
     *3) DEVICE SPECIFIC STATE & ERRORS, SUCH AS SERVO DRIVE STAUTS AND FAULT BITS ON SMP_STATUS AND
     *   SMP_FAULTS PARAMETERS. THESE STATES ARE NOT CHECKED IN THIS FUNCTION.
     *
     */
// 
// 
//     //read SMP_CUMULATIVE_STATUS
    smint32 SMDeviceSideCommStatus;
    if(fast==0)
    {
        smRead1Parameter(busHandle,deviceAddress,SMP_CUMULATIVE_STATUS,&SMDeviceSideCommStatus);
        //if we have some error bits on, reset them, so we can spot new errors later
        if(SMDeviceSideCommStatus!=0)
        {
            smSetParameter(busHandle,deviceAddress,SMP_CUMULATIVE_STATUS,0);
        
        }
    }
    else
    {
        SMDeviceSideCommStatus=0;//a cludge to avoid false errors being reported in stringifySMBusErrors
    }

    //read cumulative bus status errors and all convert (1) and (2) bits to human readable form:
    errs=stringifySMBusErrors(getCumulativeStatus(busHandle), SMDeviceSideCommStatus);

    //reset local errors bits
    resetCumulativeStatus(busHandle);
// 
    //if there were errors, log them
    if(strlen(errs) != 0)
    {
       printf("%s\n",errs);
       return 1;
    }

    return 0;
}

void on_connect_clicked()
{
    busHandle=smOpenBus("/dev/ttyUSB0");
    if(busHandle>=0)
    {
        printf("Successfully connected bus %ld \n", busHandle);
    }
    else
        printf("Couldn't connect to bus %ld \n", busHandle);
    checkAndReportSMBusErrors(0);
}

void on_disconnect_clicked()
{
    printf("Closing bus (if open) %ld", busHandle);
    smCloseBus(busHandle);
}

int main(int argc, char **argv) {
   on_connect_clicked();
   printf("Hello, world BUses!\n");
   on_disconnect_clicked();
   return 0;
}
