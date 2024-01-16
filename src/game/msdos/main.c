#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <dos.h>
#include <go32.h>
#include <dpmi.h>
#include <time.h>
#include <sys/nearptr.h>
#include "engine_bitmap_msdos.h"
#include "vesa_api.h"
#include "../engine.h"
#include "../game_container.h"
#include "../helpers/h_memory.h"

/* README
 * Demo for VESA 2.0 for DJGPP and DOSBox (or MS DOS) - requires a 386 CPU
 * If this program doesn't run...
 *   1. switch from dosbox-staging to regular dosbox (brew install dosbox)
 *   2. ensure a SVGA chipset with VESA support (e.g. s3) has been selected in DOSBox
 *   3. ensure enough VRAM is available for the SVGA card to enter the chosen mode
 */

int main(void) {
    if (!__djgpp_nearptr_enable())
        return -1;
    // switch to VESA mode
    if (VBE_set_vesa_mode() != 0) {
        printf("ERROR setting graphics mode");
        return -1;
    }
    // Load and draw a bitmap with a palette
    VBE_set_palette_entry(0, 0, 0, 0);
    VBE_set_palette_entry(1, 0, 0, 0);
    VBE_set_palette_entry(2, 80, 80, 80);
    VBE_set_palette_entry(3, 148, 148, 148);
    VBE_set_palette_entry(4, 210, 210, 210);
    VBE_set_palette_entry(5, 255, 0, 0);
    GameContainer_init();
    // game loop
    time_t start;
    int fpsDelay = 1000 / 60;
    char key;
    while(true) {
        EngineInputEvent keyCode = NO_EVENT;
        start = time(NULL);
        // keyboard input
        if(kbhit()) {
            key = getkey();
            if (key == 119) // w
                keyCode = KEY_UP;
            else if (key == 97) // a
                keyCode = KEY_LEFT;
            else if (key == 115) // s
                keyCode = KEY_DOWN;
            else if (key == 100) // d
                keyCode = KEY_RIGHT;
            else if (key == 110) // n
                keyCode = KEY_B;
            else if (key == 109) // m
                keyCode = KEY_A;
            else if (key == 27)  // esc
                break;
        }
        // draw bitmap
        bool flipPage = false;
        flipPage = GameContainer_update(keyCode);
        if (flipPage) {
            // flip video page and delay
            VBE_flip();
        }

        time_t d = fpsDelay - (time(NULL) - start);
        delay(d < 0? 0 : d);
    }
    // go back to text mode, quit program
    VBE_set_text_mode();
    GameContainer_delete();
    printf("memory ref count: %li\n", getMemCount());
    printf("done\n");
    return 0;
}