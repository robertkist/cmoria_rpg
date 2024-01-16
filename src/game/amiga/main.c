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

#include "engine_amigabitmap.h"
#include "engine_amigatime.h"

// Game includes
#include "../game_container.h"

#define SCR_DEPTH 2

/*
 * Timer Code: http://amigadev.elowar.com/read/ADCD_2.1/Devices_Manual_guide/node0196.html
 * Double buffering: http://amigadev.elowar.com/read/ADCD_2.1/Libraries_Manual_guide/node05B3.html
 * Window: https://eab.abime.net/showthread.php?t=61692
 */

// TODO
// turn off cursor

const int SCR_WIDTH = 320;
const int SCR_HEIGHT = 256; // 200 for NTSC

// screen and double-buffer globals
static struct Library *IntuitionBase = NULL;
static struct Library *GfxBase = NULL;
static struct BitMap *doubleBufferBmp[2] = {NULL, NULL };
static struct NewScreen newScreen;
struct Screen *screen = NULL;
static struct Window* window = NULL;

void shutdown();
void closeTimer();
void initTimer();

/* ************************************************
 * PROGRAM SETUP
 * ************************************************/

bool setupDoubleBuffer() {
    for (int i = 0; i < 2; i++) {
        doubleBufferBmp[i] = createAmigaBitmap(SCR_DEPTH, SCR_WIDTH, SCR_HEIGHT);
        if (doubleBufferBmp[i] == NULL)
            return false;
    }
    return true;
}

void amigaShutdown() {
    closeTimer();
    if (window != NULL)
        CloseWindow(window);
    if (screen != NULL)
        CloseScreen(screen);
    if (IntuitionBase != NULL)
        CloseLibrary(IntuitionBase);
    if (GfxBase != NULL)
        CloseLibrary(GfxBase);
    for (int i = 0; i < 2; i++) {
        freeAmigaBitmap(doubleBufferBmp[i]);
    }
}

int amigaStartup() {
    IntuitionBase = OpenLibrary("intuition.library", 33L);
    if (IntuitionBase == NULL) {
        fprintf(stderr, "ERROR: cannot load intuition.library\n");
        return -1;
    }
    GfxBase = OpenLibrary("graphics.library", 33L);
    if (GfxBase == NULL) {
        fprintf(stderr, "ERROR: cannot load graphics.library\n");
        amigaShutdown();
        return -1;
    }
    if (!setupDoubleBuffer()) {
        fprintf(stderr, "ERROR: cannot allocated bitmaps for double buffer\n");
        amigaShutdown();
        return -1;
    }
    initTimer();
}

void setPalette() {
    UWORD colorTable[] = { // 4 colors
            0x0ccc, 0x0555, 0x0888, 0x0eee,
    };
    struct ViewPort* vp = &(screen->ViewPort);
    LoadRGB4(vp, &colorTable[0], sizeof((UWORD*)colorTable));
}

/* ************************************************
 * TIME DELAY
 * ************************************************/

//void delete_timer(struct timerequest* tr) {
//    struct MsgPort* tp;
//    if (tr != 0) {
//        tp = tr->tr_node.io_Message.mn_ReplyPort;
//        if (tp != 0)
//            DeletePort(tp);
//        CloseDevice((struct IORequest*)tr);
//        DeleteExtIO((struct IORequest*)tr);
//    }
//}

//struct timerequest *create_timer( ULONG unit ) {
///* return a pointer to a timer request.  If any problem, return NULL */
//    LONG error;
//    struct MsgPort* timerport;
//    struct timerequest* TimerIO;
//    timerport = CreatePort(0, 0);
//    if (timerport == NULL)
//        return NULL;
//    TimerIO = (struct timerequest*)CreateExtIO(timerport, sizeof(struct timerequest));
//    if (TimerIO == NULL) {
//        DeletePort(timerport);   /* Delete message port */
//        return NULL;
//    }
//    error = OpenDevice(TIMERNAME, unit,(struct IORequest*) TimerIO, 0L);
//    if (error != 0 ) {
//        delete_timer(TimerIO);
//        return NULL;
//    }
//    return TimerIO;
//}

//void wait_for_timer(struct timerequest *tr, struct timeval *tv ) {
//    tr->tr_node.io_Command = TR_ADDREQUEST; /* add a new timer request */
//    tr->tr_time = *tv;
//    DoIO((struct IORequest*) tr);
//}

//LONG time_delay(struct timeval *tv, LONG unit) {
//    /* more precise timer than AmigaDOS Delay() */
//    struct timerequest* tr = create_timer(unit);
//    if (tr == NULL)
//        return -1L;
//    wait_for_timer(tr, tv);
//    delete_timer(tr);
//    return 0L;
//}

/* ************************************************
 * MAIN LOOP
 * ************************************************/

void mainLoop() {
    GameContainer_init();
//    unsigned long time1 = getTime(); // TODO
    WORD toggleFrame = 0;
    struct Message* pMsg = NULL;
    struct IntuiMessage* pIMsg = NULL;
    bool running = true;
    bool flip = true;
    while(running) {
        // handle input
        EngineInputEvent key = NO_EVENT;
        while (pMsg = GetMsg(window->UserPort)) {
            pIMsg = (struct IntuiMessage*)pMsg;
            switch (pIMsg->Class) {
                case IDCMP_RAWKEY:
                    switch(pIMsg->Code) {
                        case 69: running = false; return; // Esc
                        case 17: key = KEY_UP; break; // W
                        case 32: key = KEY_LEFT; break; // A
                        case 33: key = KEY_DOWN; break; // S
                        case 34: key = KEY_RIGHT; break; // D
                        case 54: key = KEY_B; break; // N
                        case 55: key = KEY_A; break; // M
                    } break;
            }
            ReplyMsg(pMsg);
        }
        flip = GameContainer_update(key);
        if (flip || key != NO_EVENT) {
            // redraw screen
            MakeScreen(screen);
            RethinkDisplay();
            toggleFrame ^= 1; // flip double buffer
            flip = false;
            // switch the double buffer bitmap so that we are drawing into the correct place
            screen->RastPort.BitMap = doubleBufferBmp[toggleFrame];
            screen->ViewPort.RasInfo->BitMap = doubleBufferBmp[toggleFrame];
            SetRast(&(screen->RastPort), 0);
        }
    }
    GameContainer_delete();
}

int main() {
    int r = amigaStartup();
    if (r != 0)
        return(r);
    newScreen.LeftEdge = 0;
    newScreen.TopEdge = 0;
    newScreen.Width = SCR_WIDTH;
    newScreen.Height = SCR_HEIGHT;
    newScreen.Depth = SCR_DEPTH;
    newScreen.DetailPen = 0;
    newScreen.BlockPen = 1;
    newScreen.ViewModes = SCR_WIDTH > 320 ? HIRES : 0;
    newScreen.Type = CUSTOMSCREEN | CUSTOMBITMAP | SCREENQUIET;
    newScreen.Font = NULL;
    newScreen.DefaultTitle = NULL;
    newScreen.Gadgets = NULL;
    newScreen.CustomBitMap = doubleBufferBmp[0];
    screen = OpenScreen(&newScreen);
    if (screen == NULL) {
        fprintf(stderr, "ERROR: cannot open screen\n");
        amigaShutdown();
        return -1;
    }
    screen->RastPort.Flags = DBUFFER; /* Indicate that the rastport is double buffered. */
    window = OpenWindowTags(NULL,
                              WA_Left, 0,
                              WA_Top, 0,
                              WA_Width, SCR_WIDTH,
                              WA_Height, SCR_HEIGHT,
                              WA_Title, NULL,
                              WA_CustomScreen, (ULONG)screen,
                              WA_Backdrop, TRUE,
                              WA_Borderless, TRUE,
                              WA_DragBar, FALSE,
                              WA_Activate, TRUE,
                              WA_SmartRefresh, TRUE,
                              WA_NoCareRefresh, TRUE,
                              WA_ReportMouse, TRUE,
                              WA_RMBTrap, TRUE,
                              WA_IDCMP, IDCMP_RAWKEY | IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW,
                              TAG_END);
    if (window == NULL) {
        amigaShutdown();
        return -1;
    }
    setPalette();
    mainLoop();
    amigaShutdown();
    printf("done!\n");
    return(0);
}
