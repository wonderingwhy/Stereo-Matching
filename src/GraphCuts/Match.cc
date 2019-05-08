#include "Match.h"
#include <algorithm>
#include <limits>
#include <iostream>

/// Not a number, only for setting a variable.
static const float NaN = sqrt(-1.0f);

/// Test if \a x is \em not NaN
inline bool is_number(float x) {
    return (x == x);
}

const int Match::OCCLUDED = std::numeric_limits<int>::max();

/// Constructor
Match::Match(GeneralImage left, GeneralImage right, bool color) {
    originalHeightL = imGetYSize(left);
    int height = std::min(imGetYSize(left), imGetYSize(right));
    imSizeL = Coord(imGetXSize(left), height);
    imSizeR = Coord(imGetXSize(right), height);

    if (!color) {
        imColorLeft = imColorRight = 0;
        imColorLeftMin = imColorRightMin = 0;
        imColorLeftMax = imColorRightMax = 0;

        imLeft  = (GrayImage)left;
        imRight = (GrayImage)right;
        imLeftMin = imLeftMax = imRightMin = imRightMax = 0;
    } else {
        imLeft = imRight = 0;
        imLeftMin = imRightMin = 0;
        imLeftMax = imRightMax = 0;

        imColorLeft = (RGBImage)left;
        imColorRight = (RGBImage)right;

        imColorLeftMin = imColorLeftMax = 0;
        imColorRightMin = imColorRightMax = 0;
    }

    dispMin = dispMax = 0;

    d_left  = (IntImage)imNew(IMAGE_INT, imSizeL);
    d_right = (IntImage)imNew(IMAGE_INT, imSizeR);

    vars0 = (IntImage)imNew(IMAGE_INT, imSizeL);
    varsA = (IntImage)imNew(IMAGE_INT, imSizeL);
    if (!d_left || !d_right || !vars0 || !varsA) {
        std::cerr << "Not enough memory!" << std::endl;
        exit(1);
    }
}

/// Destructor
Match::~Match() {
    imFree(imLeftMin);
    imFree(imLeftMax);
    imFree(imRightMin);
    imFree(imRightMax);
    imFree(imColorLeftMin);
    imFree(imColorLeftMax);
    imFree(imColorRightMin);
    imFree(imColorRightMax);

    imFree(d_left);
    imFree(d_right);

    imFree(vars0);
    imFree(varsA);
}

/// Save disparity map as float TIFF image
void Match::SaveXLeft(const char *fileName) {
    Coord outSize(imSizeL.x, originalHeightL);
    FloatImage out = (FloatImage)imNew(IMAGE_FLOAT, outSize);

    RectIterator end = rectEnd(outSize);
    for (RectIterator p = rectBegin(outSize); p != end; ++p) {
        IMREF(out, *p) = NaN;
    }

    end = rectEnd(imSizeL);
    for (RectIterator p = rectBegin(imSizeL); p != end; ++p) {
        int d = IMREF(d_left, *p);
        IMREF(out, *p) = (d == OCCLUDED ? NaN : static_cast<float>(d));
    }

    imSave(out, fileName);
    imFree(out);
}

/// Save scaled disparity map as 8-bit color image (gray between 64 and 255).
/// flag: lowest disparity should appear darkest (true) or brightest (false).
void Match::SaveScaledXLeft(const char *fileName, bool flag) {
    Coord outSize(imSizeL.x, originalHeightL);
    RGBImage im = (RGBImage)imNew(IMAGE_RGB, outSize);

    RectIterator end = rectEnd(outSize);
    for (RectIterator p = rectBegin(outSize); p != end; ++p) {
        IMREF(im, *p).c[0] = 0;
        IMREF(im, *p).c[1] = IMREF(im, *p).c[2] = 255;
    }

    const int dispSize = dispMax - dispMin + 1;

    end = rectEnd(imSizeL);
    for (RectIterator p = rectBegin(imSizeL); p != end; ++p) {
        int d = IMREF(d_left, *p), c;
        if (d == OCCLUDED) {
            IMREF(im, *p).c[0] = 0;
            IMREF(im, *p).c[1] = IMREF(im, *p).c[2] = 255;
        } else {
            if (dispSize == 0) {
                c = 255;
            } else if (flag) {
                c = 255 - (255 - 64) * (dispMax - d) / dispSize;
            } else {
                c = 255 - (255 - 64) * (d - dispMin) / dispSize;
            }
            IMREF(im, *p).c[0] = IMREF(im, *p).c[1] = IMREF(im, *p).c[2] = c;
        }
    }

    imSave(im, fileName);
    imFree(im);
}

/// Save scaled disparity map as 8-bit color image (gray between 64 and 255).
/// flag: lowest disparity should appear darkest (true) or brightest (false).
RGBImage  Match::GetOutputImage() {
    Coord outSize(imSizeL.x, originalHeightL);
    RGBImage im = (RGBImage)imNew(IMAGE_GRAY, outSize);

    RectIterator end = rectEnd(outSize);
    for (RectIterator p = rectBegin(outSize); p != end; ++p) {
        IMREF(im, *p).c[0] = 0;
        IMREF(im, *p).c[1] = IMREF(im, *p).c[2] = 255;
    }

    const int dispSize = dispMax - dispMin + 1;

    end = rectEnd(imSizeL);
    for (RectIterator p = rectBegin(imSizeL); p != end; ++p) {
        int d = IMREF(d_left, *p), c;
        if (d == OCCLUDED) {
            IMREF(im, *p).c[0] = 0;
            IMREF(im, *p).c[1] = IMREF(im, *p).c[2] = 255;
        } else {
            if (dispSize == 0) {
                c = 255;
            } else if (false) {
                c = 255 - (255 - 64) * (dispMax - d) / dispSize;
            } else {
                c = 255 - (255 - 64) * (d - dispMin) / dispSize;
            }
            IMREF(im, *p).c[0] = IMREF(im, *p).c[1] = IMREF(im, *p).c[2] = c;
        }
    }
    return im;
}

/// Specify disparity range
void Match::SetDispRange(int dMin, int dMax) {
    dispMin = dMin;
    dispMax = dMax;
    if (! (dispMin <= dispMax) ) {
        std::cerr << "Error: wrong disparity range!\n" << std::endl;
        exit(1);
    }
    RectIterator end = rectEnd(imSizeL);
    for (RectIterator p = rectBegin(imSizeL); p != end; ++p) {
        IMREF(d_left, *p) = OCCLUDED;
    }
    end = rectEnd(imSizeR);
    for (RectIterator q = rectBegin(imSizeR); q != end; ++q) {
        IMREF(d_right, *q) = OCCLUDED;
    }
}

/// Heuristic for selecting parameter 'K'
/// Details are described in Kolmogorov's thesis
float Match::GetK() {
    int i = dispMax - dispMin + 1;
    int k = (i + 2) / 4; // around 0.25 times the number of disparities
    if (k < 3) {
        k = 3;
    }

    int *array = new int[k];
    std::fill_n(array, k, 0);
    int sum = 0, num = 0;

    int xmin = std::max(0, -dispMin); // 0<=x,x+dispMin
    int xmax = std::min(imSizeL.x, imSizeR.x - dispMax); // x<wl,x+dispMax<wr
    Coord p;
    for (p.y = 0; p.y < imSizeL.y && p.y < imSizeR.y; p.y++)
        for (p.x = xmin; p.x < xmax; p.x++) {
            // compute k'th smallest value among data_penalty(p, p+d) for all d
            for (int i = 0, d = dispMin; d <= dispMax; d++) {
                int delta = (imLeft ?
                             data_penalty_gray(p, p + d) :
                             data_penalty_color(p, p + d));
                if (i < k) {
                    array[i++] = delta;
                } else for (i = 0; i < k; i++)
                        if (delta < array[i]) {
                            std::swap(delta, array[i]);
                        }
            }
            sum += *std::max_element(array, array + k);
            num++;
        }

    delete[] array;
    if (num == 0) {
        std::cerr << "GetK: Not enough samples!" << std::endl;
        exit(1);
    }
    if (sum == 0) {
        std::cerr << "GetK failed: K is 0!" << std::endl;
        exit(1);
    }

    float K = ((float)sum) / num;
    std::cout << "Computing statistics: K(data_penalty noise) =" << K << std::endl;
    return K;
}

/// Upper bound for intensity level difference when computing data cost
static int CUTOFF = 30;

/// Distance from v to interval [min,max]
inline int dist_interval(int v, int min, int max) {
    if (v < min) {
        return (min - v);
    }
    if (v > max) {
        return (v - max);
    }
    return 0;
}

/// Birchfield-Tomasi gray distance between pixels p and q
int Match::data_penalty_gray(Coord p, Coord q) const {
    int Ip = IMREF(imLeft, p), Iq = IMREF(imRight, q);
    int IpMin = IMREF(imLeftMin, p), IqMin = IMREF(imRightMin, q);
    int IpMax = IMREF(imLeftMax, p), IqMax = IMREF(imRightMax, q);

    int dp = dist_interval(Ip, IqMin, IqMax);
    int dq = dist_interval(Iq, IpMin, IpMax);
    int d = std::min(dp, dq);
    if (d > CUTOFF) {
        d = CUTOFF;
    }
    if (params.dataCost == Parameters::L2) {
        d = d * d;
    }

    return d;
}

/// Birchfield-Tomasi color distance between pixels p and q
int Match::data_penalty_color(Coord p, Coord q) const {
    int dSum = 0;
    // Loop over the 3 channels
    for (int i = 0; i < 3; i++) {
        int Ip = IMREF(imColorLeft, p).c[i];
        int Iq = IMREF(imColorRight, q).c[i];
        int IpMin = IMREF(imColorLeftMin, p).c[i];
        int IqMin = IMREF(imColorRightMin, q).c[i];
        int IpMax = IMREF(imColorLeftMax, p).c[i];
        int IqMax = IMREF(imColorRightMax, q).c[i];

        int dp = dist_interval(Ip, IqMin, IqMax);
        int dq = dist_interval(Iq, IpMin, IpMax);
        int d = std::min(dp, dq);
        if (d > CUTOFF) {
            d = CUTOFF;
        }
        if (params.dataCost == Parameters::L2) {
            d = d * d;
        }
        dSum += d;
    }
    return dSum / 3;
}

/************************************************************/
/******************* Preprocessing for Birchfield-Tomasi ****/
/************************************************************/

static void SubPixel(GrayImage Im, GrayImage ImMin, GrayImage ImMax) {
    Coord p;
    int I, I1, I2, I3, I4, IMin, IMax;
    int xmax = imGetXSize(ImMin), ymax = imGetYSize(ImMin);

    for (p.y = 0; p.y < ymax; p.y++)
        for (p.x = 0; p.x < xmax; p.x++) {
            I = IMin = IMax = imRef(Im, p.x, p.y);
            I1 = (p.x > 0 ? (imRef(Im, p.x - 1, p.y) + I) / 2 : I);
            I2 = (p.x + 1 < xmax ? (imRef(Im, p.x + 1, p.y) + I) / 2 : I);
            I3 = (p.y > 0 ? (imRef(Im, p.x, p.y - 1) + I) / 2 : I);
            I4 = (p.y + 1 < ymax ? (imRef(Im, p.x, p.y + 1) + I) / 2 : I);

            if (IMin > I1) {
                IMin = I1;
            }
            if (IMin > I2) {
                IMin = I2;
            }
            if (IMin > I3) {
                IMin = I3;
            }
            if (IMin > I4) {
                IMin = I4;
            }
            if (IMax < I1) {
                IMax = I1;
            }
            if (IMax < I2) {
                IMax = I2;
            }
            if (IMax < I3) {
                IMax = I3;
            }
            if (IMax < I4) {
                IMax = I4;
            }

            imRef(ImMin, p.x, p.y) = IMin;
            imRef(ImMax, p.x, p.y) = IMax;
        }
}

static void SubPixelColor(RGBImage Im, RGBImage ImMin, RGBImage ImMax) {
    int I, I1, I2, I3, I4, IMin, IMax;

    Coord p;
    int xmax = imGetXSize(ImMin), ymax = imGetYSize(ImMin);
    for (p.y = 0; p.y < ymax; p.y++)
        for (p.x = 0; p.x < xmax; p.x++)
            for (int i = 0; i < 3; i++) { // Loop over channels
                I = IMin = IMax = imRef(Im, p.x, p.y).c[i];
                I1 = (p.x > 0 ? (imRef(Im, p.x - 1, p.y).c[i] + I) / 2 : I);
                I2 = (p.x + 1 < xmax ? (imRef(Im, p.x + 1, p.y).c[i] + I) / 2 : I);
                I3 = (p.y > 0 ? (imRef(Im, p.x, p.y - 1).c[i] + I) / 2 : I);
                I4 = (p.y + 1 < ymax ? (imRef(Im, p.x, p.y + 1).c[i] + I) / 2 : I);

                if (IMin > I1) {
                    IMin = I1;
                }
                if (IMin > I2) {
                    IMin = I2;
                }
                if (IMin > I3) {
                    IMin = I3;
                }
                if (IMin > I4) {
                    IMin = I4;
                }
                if (IMax < I1) {
                    IMax = I1;
                }
                if (IMax < I2) {
                    IMax = I2;
                }
                if (IMax < I3) {
                    IMax = I3;
                }
                if (IMax < I4) {
                    IMax = I4;
                }

                imRef(ImMin, p.x, p.y).c[i] = IMin;
                imRef(ImMax, p.x, p.y).c[i] = IMax;
            }
}

void Match::InitSubPixel() {
    if (imLeft && !imLeftMin) {
        imLeftMin = (GrayImage)imNew(IMAGE_GRAY, imSizeL);
        imLeftMax = (GrayImage)imNew(IMAGE_GRAY, imSizeL);
        imRightMin = (GrayImage)imNew(IMAGE_GRAY, imSizeR);
        imRightMax = (GrayImage)imNew(IMAGE_GRAY, imSizeR);

        SubPixel(imLeft, imLeftMin, imLeftMax);
        SubPixel(imRight, imRightMin, imRightMax);
    }
    if (imColorLeft && !imColorLeftMin) {
        imColorLeftMin = (RGBImage)imNew(IMAGE_RGB, imSizeL);
        imColorLeftMax = (RGBImage)imNew(IMAGE_RGB, imSizeL);
        imColorRightMin = (RGBImage)imNew(IMAGE_RGB, imSizeR);
        imColorRightMax = (RGBImage)imNew(IMAGE_RGB, imSizeR);

        SubPixelColor(imColorLeft, imColorLeftMin, imColorLeftMax);
        SubPixelColor(imColorRight, imColorRightMin, imColorRightMax);
    }
}

int Match::SmoothnessPenaltyGray(Coord p1, Coord p2, int disp) const {
    // |I1(p1)-I1(p2)| and |I2(p1+disp)-I2(p2+disp)|
    int dl = IMREF(imLeft, p1) - IMREF(imLeft, p2);
    int dr = IMREF(imRight, p1 + disp) - IMREF(imRight, p2 + disp);
    if (dl < 0) {
        dl = -dl;
    }
    if (dr < 0) {
        dr = -dr;
    }
    return (dl < params.edgeThresh && dr < params.edgeThresh) ?
           params.lambda1 : params.lambda2;
}

int Match::SmoothnessPenaltyColor(Coord p1, Coord p2, int disp) const {
    int d, dMax = 0; // Max inf norm in RGB space of (p1,p2) and (p1+disp,p2+disp)
    for (int i = 0; i < 3; i++) {
        d = IMREF(imColorLeft, p1).c[i] - IMREF(imColorLeft, p2).c[i];
        if (d < 0) {
            d = -d;
        }
        if (dMax < d) {
            dMax = d;
        }
        d = IMREF(imColorRight, p1 + disp).c[i] - IMREF(imColorRight, p2 + disp).c[i];
        if (d < 0) {
            d = -d;
        }
        if (dMax < d) {
            dMax = d;
        }
    }
    return (dMax < params.edgeThresh) ? params.lambda1 : params.lambda2;
}

void Match::SetParameters(Parameters *_params) {
    params = *_params;
    InitSubPixel();
}