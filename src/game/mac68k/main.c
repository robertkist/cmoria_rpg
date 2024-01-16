#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <Quickdraw.h>
#include "../game_container.h"
#include "engine_macbitmap.h"

// TODO: react to external quit signal, e.g. from system shutdown
// TODO: implement basic menu with a quit action
// TODO: moving the window erases the rest of the desktop (not good!)
// http://preserve.mactech.com/articles/mactech/Vol.09/09.09/Bitmapper/index.html
// http://preserve.mactech.com/articles/mactech/Vol.03/03.07/Animation/index.html

const int WINDOW_X = 400;
const int WINDOW_Y = 240;

static WindowPtr window;  // window pointer
GrafPtr doubleBufferPort;   // GraphPort to double buffer bitmap
static BitMap* doubleBufferBitmap;
static bool redraw = true;

void initToolbox() {
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(NULL);
    InitCursor();
    FlushEvents(everyEvent, 0); // Empty event queue of stray or leftover events
}

void openWindow() {
    // create new window
    Rect rect = qd.screenBits.bounds;
    SetRect(&rect, rect.left + 5, rect.top + 45, WINDOW_X + 5, WINDOW_Y + 45); // get screen dimensions
    window = NewWindow(NULL,
                       &rect,       // boundRect
                       "\pcmoria",  // title
                       true,        // visible
                       4,           // procID
                       (WindowPtr) - 1, // behind
                       true,        // goAwayFlag (close button)
                       0L);         // refCon
    SetPort(window);         // set current drawing port to 'window'
    EraseRect(&window->portRect);        // clear window
}

void CreateDoubleBuffer() {
    Rect r;
    SetRect(&r, 0, 0, WINDOW_X, WINDOW_Y);
    doubleBufferPort = (GrafPtr)calloc(sizeof(GrafPort), 1);
    if (doubleBufferPort == nil)
        SysBeep(20);
    doubleBufferBitmap = (BitMap*)calloc(sizeof(BitMap), 1);
    if (doubleBufferBitmap == nil)
        SysBeep(20);
    doubleBufferBitmap->bounds = r;
    doubleBufferBitmap->rowBytes = (r.right - r.left + 7) / 8;
    doubleBufferBitmap->baseAddr = (char*)calloc(doubleBufferBitmap->rowBytes * (r.bottom - r.top), 1);
    if (doubleBufferBitmap->baseAddr == nil)
        SysBeep( 20 );
    OpenPort(doubleBufferPort);
    SetPortBits(doubleBufferBitmap);
}

void FreeDoubleBuffer() {
    free(doubleBufferBitmap->baseAddr);
    free(doubleBufferBitmap);
    free(doubleBufferPort);
}

void flipScreen() {
    // copy buffer to screen
    Rect r;
    SetRect(&r, 0, 0, WINDOW_X, WINDOW_Y);
    CopyBits(&doubleBufferPort->portBits,
             &(window->portBits),
             &r, //&doubleBufferPort->portBits.bounds,
             &r, //&doubleBufferPort->portBits.bounds,
             srcCopy, //transparent + patCopy,
             NULL);
    // erase double buffer
    EraseRect(&doubleBufferPort->portRect);
}

void gameLoop() {
    // Elements of C++ Macintosh Programming, p127
    // Using the Macintosh Toolbox with C, p41, 64
    GameContainer_init();

    Rect dragRect;
    SetRect(&dragRect, -32767, -32767, 32767, 32767);
    char macKey;
    EventRecord theEvent;
    bool running = true;
    WindowPtr whichWindow;
    EngineInputEvent key = NO_EVENT;
    while(running) {
        key = NO_EVENT;
        if (GetNextEvent(everyEvent, &theEvent)) {
            switch (theEvent.what) {
                case keyDown :
//                case autoKey : // TODO
                    macKey = (char)(theEvent.message & charCodeMask);
                    switch (macKey) {
                        case 'q' : case 'Q': running = false; break;
                        case 'w' : case 'W': key = KEY_UP; break;
                        case 'a' : case 'A': key = KEY_LEFT; break;
                        case 's' : case 'S': key = KEY_DOWN; break;
                        case 'd' : case 'D': key = KEY_RIGHT; break;
                        case 'n' : case 'N': key = KEY_B; break;
                        case 'm' : case 'M': key = KEY_A; break;
                    } break;
                case mouseDown:
                    int windowCode = FindWindow(theEvent.where, &whichWindow);
                    switch (windowCode) {
                        case inSysWindow: // window activated
                            SystemClick(&theEvent, whichWindow);
                            redraw = true; // TODO this doesnt work because we have to trigger a
                                           //      redraw event in GameContainer_update() without changing
                                           //      game state
                            break;
                        case inDrag:
                            DragWindow(window, theEvent.where, &dragRect);
                            break;
                        case inDesk:  // window focus lost
                            if (window != NULL)
                                HiliteWindow(window, 0);
                            break;
                        case inContent:  // click inside window
                            HiliteWindow(whichWindow, true);
                            break;
                        case inGoAway:  // window close button
                            char stillinGoAway = TrackGoAway(window, theEvent.where);
                            if (stillinGoAway)
                                running = false;
                            break;
                    } break;
            }
        }
        // TODO delay!
        if (GameContainer_update(key))
            redraw = true;
        if (redraw) {
            flipScreen();
            redraw = false;
        }
        SystemTask();
    }
    GameContainer_delete();
}

int main(int argc, char** argv) {
    initToolbox();
    openWindow();
    CreateDoubleBuffer();
    gameLoop();
    FlushEvents(everyEvent, -1);
    FreeDoubleBuffer();
    ExitToShell();
    return 0;
}