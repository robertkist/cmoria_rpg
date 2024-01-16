#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "sdl_core.h"
#include "../engine.h"
#include "../config.h"

static bool window_dirty = false;
static bool window_running = false;
static SDL_Window *window = NULL;
static SDL_Surface *window_surface = NULL;
static SDL_Surface *window_framebuffer = NULL;
static SDL_Surface *scaled_fb = NULL;
static SDL_Surface *p32BPPSurface = NULL;
static struct timeval fps_start_time;
static struct timeval frame_start_time;
static struct timeval last_key_press;

static uint8_t FPS_DELAY = 0;
static const uint16_t KEYBOARD_DELAY = 250;

bool GSDL_isRunning() {
    return window_running;
}

void GSDL_stop() {
    window_running = false;
}

#ifndef timersub
#define timersub(a, b, result) \
    /* https://gist.github.com/zeta709/6208314 */ \
    do { \
        (result)->tv_sec = (a)->tv_sec - (b)->tv_sec; \
        (result)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
        if ((result)->tv_usec < 0) { \
            --(result)->tv_sec; \
            (result)->tv_usec += 1000000; \
        } \
    } while (0)
#endif /* timersub */

/* returns the current system time */
struct timeval GSDL_getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv;
}

/* returns a time difference in ms */
int32_t GSDL_timeDifference(const struct timeval *time) {
    struct timeval end, diff;
    gettimeofday(&end, NULL);
    timersub(&end, time, &diff);
    return (int32_t)(((double)diff.tv_sec + (double)diff.tv_usec / 1000000.0) * 1000.0);
}

void GSDL_paintFramebuffer() {
    /* Updates the window surface with the contents of the framebuffer */
    if(gfxScaleFactor > 1) {  // draw framebuffer
        // blit image to 32 bit surface (we need a 32 bit surface for the scale to work nicely)
        SDL_BlitSurface(window_framebuffer, NULL, p32BPPSurface, NULL);
        // scale 32 bit surface onto destination surface
        SDL_BlitScaled(p32BPPSurface, NULL, scaled_fb, NULL);
        SDL_BlitSurface(scaled_fb, NULL, window_surface, NULL);
    } else {
        SDL_BlitSurface(window_framebuffer, NULL, window_surface, NULL);
    }
    SDL_UpdateWindowSurface(window);  // update window
}

int GSDL_showWindow() {
    FPS_DELAY = (uint8_t)(1000 / gameFps);
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Engine_logError("SDL_init - %s\n", SDL_GetError());
        exit(-1);
    }
    window = SDL_CreateWindow(
            "cmoria",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            screenSizeP.x * gfxScaleFactor, screenSizeP.y * gfxScaleFactor, 0);
    if(!window) {
        Engine_logError("SDL_CreateWindow -  %s\n", SDL_GetError());
        exit(-1);
    }
    window_surface = SDL_GetWindowSurface(window);
    if(!window_surface) {
        Engine_logError("SDL_GetWindowSurface - failed to get the surface from the window\n");
        SDL_DestroyWindow(window);
        window = NULL;
        exit(-1);
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    window_framebuffer = SDL_CreateRGBSurface(0, screenSizeP.x, screenSizeP.y, 32, 0, 0, 0, 0);
    /* create framebuffer targets for upscale */
    if(gfxScaleFactor > 1) {
        SDL_Rect src_dim, dst_dim;
        src_dim.x = 0;
        src_dim.y = 0;
        src_dim.w = window_framebuffer->w;
        src_dim.h = window_framebuffer->h;
        dst_dim.x = 0;
        dst_dim.y = 0;
        dst_dim.w = window_surface->w;
        dst_dim.h = window_surface->h;
        p32BPPSurface = SDL_CreateRGBSurface(
                window_framebuffer->flags, src_dim.w, src_dim.h, 32,
                window_framebuffer->format->Rmask, window_framebuffer->format->Gmask,
                window_framebuffer->format->Bmask, window_framebuffer->format->Amask);
        scaled_fb = SDL_CreateRGBSurface(
                p32BPPSurface->flags, dst_dim.w, dst_dim.h, p32BPPSurface->format->BitsPerPixel,
                p32BPPSurface->format->Rmask, p32BPPSurface->format->Gmask,
                p32BPPSurface->format->Bmask, p32BPPSurface->format->Amask);
    }
    /* paint the framebuffer for the first time */
    GSDL_paintFramebuffer();
    window_running = true;
    fps_start_time = frame_start_time = last_key_press = GSDL_getTime();
    return 0;
}

EngineInputEvent GSDL_getEvent() {
    /* Event handling */
    SDL_Event sdl_event;
    if(SDL_PollEvent(&sdl_event) > 0) {
        switch(sdl_event.type) {
            case SDL_QUIT:
                return KEY_QUIT;
        }
    }
    const uint8_t* keystates = SDL_GetKeyboardState(NULL);
    static EngineInputEvent last_event = NO_EVENT;
    EngineInputEvent event = NO_EVENT;
    if(keystates[SDL_SCANCODE_LEFT]) event = KEY_LEFT;
    else if(keystates[SDL_SCANCODE_RIGHT]) event = KEY_RIGHT;
    else if(keystates[SDL_SCANCODE_UP]) event = KEY_UP;
    else if(keystates[SDL_SCANCODE_DOWN]) event = KEY_DOWN;
    else if(keystates[SDL_SCANCODE_N]) event = KEY_B;
    else if(keystates[SDL_SCANCODE_M]) event = KEY_A;
    else if(keystates[SDL_SCANCODE_W]) event = KEY_UP;
    else if(keystates[SDL_SCANCODE_A]) event = KEY_LEFT;
    else if(keystates[SDL_SCANCODE_S]) event = KEY_DOWN;
    else if(keystates[SDL_SCANCODE_D]) event = KEY_RIGHT;
    if(event == last_event && event != NO_EVENT)
        if(GSDL_timeDifference(&last_key_press) < KEYBOARD_DELAY)
            return NO_EVENT; // delay is still active
    last_event = event;
    last_key_press = GSDL_getTime();
    return event;
}

bool GSDL_updateFrame() {
    // check if program is still supposed to be running
    if(!window_running)
        return false;
    // update window
    if(window_dirty) {
        // if(show_fps) GfxEngine_print(0, 0, "FPS %d", fps);
//        printf("REPAINT\n");
        GSDL_paintFramebuffer();
        window_dirty = false;
    }
    // delay to limit frame-rate
    int32_t td = GSDL_timeDifference(&frame_start_time);
    if(td < FPS_DELAY)
        SDL_Delay((unsigned int)(FPS_DELAY - td));
    frame_start_time = GSDL_getTime();
    return true;
}

void GSDL_close() {
    if(gfxScaleFactor > 1) {
        if (scaled_fb != NULL) { SDL_FreeSurface(scaled_fb); scaled_fb = NULL; }
        if (p32BPPSurface != NULL) { SDL_FreeSurface(p32BPPSurface); p32BPPSurface = NULL; }
    }
    if (window_framebuffer != NULL) { SDL_FreeSurface(window_framebuffer); window_framebuffer = NULL; }
    if (window_surface != NULL) { SDL_FreeSurface(window_surface); window_surface = NULL; }
    if (window != NULL) { SDL_DestroyWindow(window); window = NULL; }
    SDL_Quit();
}

void GSDL_drawRect(int x1, int y1, int width, int height) {
    int x2 = x1 + width - 1;
    int y2 = y1 + height - 1;
//    printf("%i,%i - %i,%i\n", x1, y1, x2, y2);
    Uint32* target_pixel;
    for (int x = x1; x < x2; x++) {
        target_pixel = (Uint32*)((Uint8*)window_framebuffer->pixels
                                                 + y1 * window_framebuffer->pitch
                                                 + x * window_framebuffer->format->BytesPerPixel);
        *target_pixel = 0xffffffff;
        target_pixel = (Uint32*)((Uint8*)window_framebuffer->pixels
                                 + y2 * window_framebuffer->pitch
                                 + x * window_framebuffer->format->BytesPerPixel);
        *target_pixel = 0xffffffff;
    }
    for (int y = y1; y <= y2; y++) {
        target_pixel = (Uint32*)((Uint8*)window_framebuffer->pixels
                                 + y * window_framebuffer->pitch
                                 + x1 * window_framebuffer->format->BytesPerPixel);
        *target_pixel = 0xffffffff;
        target_pixel = (Uint32*)((Uint8*)window_framebuffer->pixels
                                 + y * window_framebuffer->pitch
                                 + x2 * window_framebuffer->format->BytesPerPixel);
        *target_pixel = 0xffffffff;
    }
}

void GSDL_drawImage(const struct EngineBitmap* bitmap, int dstx, int dsty) {
    SDL_Rect src_dim, dst_dim;
    SDL_Surface* bmp = (SDL_Surface*)EngineBitmap_getBitmap(bitmap);
    src_dim.x = 0;
    src_dim.y = 0;
    src_dim.w = dst_dim.w = bmp->w;
    src_dim.h = dst_dim.h = bmp->h;
    dst_dim.x = dstx;
    dst_dim.y = dsty;
    SDL_BlitSurface(bmp, &src_dim, window_framebuffer, &dst_dim);
    window_dirty = true;
}

void GSDL_clearFrame() {
    SDL_FillRect(window_framebuffer, NULL, 0x000000);
}