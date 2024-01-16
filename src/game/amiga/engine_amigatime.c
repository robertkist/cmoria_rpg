#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <proto/dos.h>
#include <proto/timer.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>

// C includes
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "engine_amigatime.h"

/* ************************************************
 * TIME MEASUREMENT
 * ************************************************/

static struct timerequest* TimerIO;
static struct EClockVal* time1;
static ULONG E_Freq;
static struct Library* timerBaseLibrary;

void initTimer() {
    struct Library* TimerBase;
    TimerIO  = (struct timerequest*)AllocMem(sizeof(struct timerequest), MEMF_CLEAR | MEMF_PUBLIC);
    time1 = (struct EClockVal*)AllocMem(sizeof(struct EClockVal), MEMF_CLEAR | MEMF_PUBLIC);
    if ((OpenDevice("timer.device",UNIT_ECLOCK, (struct IORequest*)TimerIO, 0L))) {
        printf("ERROR opening timer.device\n");
        return;
    }
    TimerBase = (struct Library*)TimerIO->tr_node.io_Device;
    timerBaseLibrary= TimerBase;
}

unsigned long getTime() {
    struct Library* TimerBase = timerBaseLibrary;
    E_Freq = ReadEClock((struct EClockVal*) time1); /* Get initial */
    return time1->ev_lo;
}

void closeTimer() {
    FreeMem(time1, (LONG)sizeof(struct EClockVal));
    FreeMem(TimerIO, (LONG)sizeof(struct timerequest));
    CloseDevice((struct IORequest*) TimerIO);
}