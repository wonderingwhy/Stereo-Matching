#ifndef IMAGE_H
#define IMAGE_H

#include <stdlib.h>

typedef enum {
    IMAGE_GRAY,
    IMAGE_RGB,
    IMAGE_INT,
    IMAGE_FLOAT
} ImageType;

typedef struct ImageHeader_st {
    ImageType type;
    int data_size;
    int xsize, ysize;
} ImageHeader;

typedef struct GeneralImage_t {
    void *data;
} *GeneralImage;

typedef struct GrayImage_t  {
    unsigned char                *data;
} *GrayImage;
typedef struct RGBImage_t   {
    struct {
        unsigned char c[3];
    } *data;
} *RGBImage;
typedef struct IntImage_t   {
    int                          *data;
} *IntImage;
typedef struct FloatImage_t {
    float                        *data;
} *FloatImage;

#define imHeader(im) ((ImageHeader*) ( ((char*)(im)) - sizeof(ImageHeader) ))

#define imRef(im, x, y) ( ((im)+(y))->data[x] )
#define imGetXSize(im) (imHeader(im)->xsize)
#define imGetYSize(im) (imHeader(im)->ysize)

void *imNew(ImageType type, int xsize, int ysize);
inline void imFree(void *im) {
    if (im) {
        free(GeneralImage(im)->data);
        free(imHeader(im));
    }
}
void *imLoad(ImageType type, const char *filename);
int imSave(void *im, const char *filename);

/// Pixel coordinates with basic operations.
struct Coord {
    int x, y;

    Coord() {}
    Coord(int a, int b) {
        x = a;
        y = b;
    }

    Coord operator+ (Coord a) const {
        return Coord(x + a.x, y + a.y);
    }
    Coord operator+ (int a)   const {
        return Coord(x + a, y);
    }
    Coord operator- (int a)   const {
        return Coord(x - a, y);
    }
    bool  operator< (Coord a) const {
        return (x <  a.x) && (y <  a.y);
    }
    bool  operator<=(Coord a) const {
        return (x <= a.x) && (y <= a.y);
    }
    bool  operator!=(Coord a) const {
        return (x != a.x) || (y != a.y);
    }
};
/// Value of image im at pixel p
#define IMREF(im, p) (imRef((im), (p).x, (p).y))

/// Overload with parameter of type Coord
inline void *imNew(ImageType type, Coord size) {
    return imNew(type, size.x, size.y);
}

/// Is p inside rectangle r?
inline bool inRect(Coord p, Coord r) {
    return (Coord(0, 0) <= p && p < r);
}

/// Rectangle iterator
class RectIterator {
    Coord p; ///< Current point
    int w; ///< Width of rectangle
public:
    RectIterator(Coord rect): p(0, 0), w(rect.x) {}
    const Coord &operator*() const {
        return p;
    }
    bool operator!=(const RectIterator &it) const {
        return (p != it.p);
    }
    RectIterator &operator++() {
        if (++p.x == w) {
            p.x = 0;
            ++p.y;
        }
        return *this;
    }

    friend RectIterator rectBegin(Coord rect);
    friend RectIterator rectEnd(Coord rect);
};

inline RectIterator rectBegin(Coord rect) {
    return RectIterator(rect);
}
inline RectIterator rectEnd(Coord rect) {
    RectIterator it(rect);
    it.p.y = rect.y;
    return it;
}

/// Is the color image actually gray?
inline bool isGray(RGBImage im) {
    const int xsize = imGetXSize(im), ysize = imGetYSize(im);
    for (int y = 0; y < ysize; y++)
        for (int x = 0; x < xsize; x++)
            if (imRef(im, x, y).c[0] != imRef(im, x, y).c[1] ||
                    imRef(im, x, y).c[0] != imRef(im, x, y).c[2]) {
                return false;
            }
    return true;
}

/// Convert to gray level a color image (extract red channel)
inline void convert_gray(GeneralImage &im) {
    const int xsize = imGetXSize(im), ysize = imGetYSize(im);
    GrayImage g = (GrayImage)imNew(IMAGE_GRAY, xsize, ysize);
    for (int y = 0; y < ysize; y++)
        for (int x = 0; x < xsize; x++) {
            imRef(g, x, y) = imRef((RGBImage)im, x, y).c[0];
        }
    imFree(im);
    im = (GeneralImage)g;
}

#endif
