/*
 * print.c
 *
 *  Created on: Apr 22, 2024
 *      Author: James DESTINE
 */

#include "print.h"

void TestPrinter(void) 
{
    int status = 0;
    prnInit();

    lcdclean();
	lcdprintf(ALG_LEFT, "Prepare for print...");
	lcdFlip();


    prnPrintf("Mgn&Co\n");
    prnPrintf("Printer test is OK\n");
    prnPrintf("Close!!!\n");
    prnStart();
    do
    {
        usleep(100000);
        status = prnStatus();
        if (((status  >> PRINTER_STATUS_BUSY) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = busy");
        else if (((status  >> PRINTER_STATUS_HIGHT_TEMP) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = tempreture high");
        else if (((status  >> PRINTER_STATUS_PAPER_LACK) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = no paper");
        else if (((status  >> PRINTER_STATUS_FEED) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = feed paper");
        else if (((status  >> PRINTER_STATUS_PRINT) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = printing");
        else if (((status  >> PRINTER_STATUS_FORCE_FEED) & 0x01) == 0x01)
                lcdprintf(ALG_LEFT, "printer status = force feed paper");
        else if (((status  >> PRINTER_STATUS_POWER_ON) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = power on");
        lcdFlip();

    } while (status != 0);
    prnStep(5);

    prnClose();
	lcdprintf(ALG_LEFT, "Press any key.");
    lcdFlip();

    kbGetKey();
    return;
}
