#ifndef H_COORDINATE_H
#define H_COORDINATE_H

#ifdef __cplusplus
extern "C" {
#endif

struct HCoordinate {
    union {
        int x;
        int width;
    };
    union {
        int y;
        int height;
    };
};

#ifdef __cplusplus
}
#endif

#endif // H_COORDINATE_H