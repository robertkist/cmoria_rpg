#include "engine_macbitmap.h"

extern GrafPtr doubleBufferPort;

uint8_t* loadFile(const char* fname) {
    OSErr err;
    int16_t fileRef = 0;
    int32_t fileSize = 0;
    int32_t bytesRead = 0;
    uint8_t *buffer;
    // set volume
    if ((err = SetVol(0L, 0))) {
        return NULL;
    }
    // create a "pascal string" file name
    char fn[1024];
    sprintf(fn, "0%s", fname);

    fn[0] = strlen(fn) - 1;
    // open file
    if ((err = FSOpen((ConstStringPtr)fn, 0, &fileRef))) {
        return NULL;
    }
    // get file size
    if ((err = GetEOF(fileRef, &fileSize))) {
        FSClose(fileRef);
        return NULL;
    }
    // rewind to start of file
    if ((err = SetFPos (fileRef, fsFromStart, 0L))) {
        FSClose(fileRef);
        return NULL;
    }
    // read file
    buffer = (uint8_t*)calloc(fileSize, sizeof (uint8_t));
    if (buffer == NULL) {
        free(buffer);
        FSClose(fileRef);
        return NULL;
    }
    bytesRead = fileSize; // 'bytesRead' contains how many bytes we want FSRead to read - i.e. we want to
    // read 'fileSize' bytes. The return value, how many byes have actually
    // been read will also be in 'bytesRead'.
    if ((err = FSRead(fileRef, &bytesRead, buffer))) {
        free(buffer);
        FSClose(fileRef);
        return NULL;
    }
    // close file
    FSClose(fileRef);
    return buffer;
}

struct MacBitmap* MacBitmap_new(const char* fname) {
    struct MacBitmap* self = (struct MacBitmap*)calloc(sizeof(struct MacBitmap), 1);
    if (self == nil) {
        SysBeep(20);
        return NULL;
    }
    unsigned char* buffer = loadFile(fname);
    if (buffer == nil) {
        SysBeep(20);
        return NULL;
    }

    int16_t width = (buffer[0] << 8) | buffer[1];
    int16_t height = (buffer[2] << 8) | buffer[3];
    bool transparency = buffer[4];
    int h = width - 1 + 16;

    size_t rowbytes = (h - h % 16) / 8;
    size_t size = rowbytes * height;

    self->bitmap = (BitMap*)calloc(sizeof(BitMap), 1);
    if (self->bitmap == nil) {
        SysBeep(20);
        return NULL;
    }
    self->bitmap->rowBytes = rowbytes;
    self->bitmap->baseAddr = (char*)calloc(size, 1);
    memcpy(self->bitmap->baseAddr, buffer + 5, size);
    self->bitmap->bounds.left = 0;
    self->bitmap->bounds.top = 0;
    self->bitmap->bounds.right = width;
    self->bitmap->bounds.bottom = height;

    self->transparency = transparency;

    if (transparency) {
        self->mask = (BitMap *) calloc(sizeof(BitMap), 1);
        if (self->mask == nil) {
            SysBeep(20);
            return NULL;
        }
        self->mask->rowBytes = rowbytes;
        self->mask->baseAddr = (char *) calloc(size, 1);
        memcpy(self->mask->baseAddr, buffer + size + 5, size);
        self->mask->bounds.left = 0;
        self->mask->bounds.top = 0;
        self->mask->bounds.right = width;
        self->mask->bounds.bottom = height;
    }
    free(buffer);
    return self;
}

void MacBitmap_delete(struct MacBitmap* self) {
    if (self) {
        if (self->bitmap) {
            free(self->bitmap->baseAddr);  // free bitmap buffer
            free(self->bitmap);  // free bitmap structure
        }
        if (self->mask) {
            free(self->mask->baseAddr);  // free bitmap buffer
            free(self->mask);  // free bitmap structure
        }
        free(self);          // free MacBitmap structure
    }
}

void MacBitmap_draw(struct MacBitmap* self, int ofsx, int ofsy) {
    // copy pip to buffer
    Rect r;
    SetRect(&r,
            ofsx,
            ofsy,
            self->bitmap->bounds.right + ofsx,
            self->bitmap->bounds.bottom + ofsy);

    // TODO does if(self->mask) work? do we need an explicit bool transparent flag???
    if (self->transparency) {
        CopyMask(self->bitmap,
                 self->mask,
                 &doubleBufferPort->portBits,
                 &self->bitmap->bounds,
                 &self->mask->bounds,
                 &r);
    } else {
        CopyBits(self->bitmap,  // source
                 &doubleBufferPort->portBits,  // dest
                 &self->bitmap->bounds,  // source rect
                 &r,  // dest rect
                 srcCopy,
                 NULL);
    }
}