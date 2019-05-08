#include "GlobalMatcher.h"
using namespace std;
using namespace cv;

int GlobalMatcher::run(Mat &left_view, Mat &right_view,
                       int dMin, int dMax, Mat &output) {
    //srand
    time_t seed = time(NULL);
    srand((unsigned int)seed);
    //convert image
    int xsize = left_view.cols, ysize = left_view.rows;
    GeneralImage im1 = (GeneralImage)imNew(IMAGE_GRAY, xsize, ysize);
    GeneralImage im2 = (GeneralImage)imNew(IMAGE_GRAY, xsize, ysize);
    const size_t size = xsize*ysize;
    for (size_t i = 0; i < size; i++) {
        imRef((GrayImage)im1, i, 0) = left_view.data[i];
        imRef((GrayImage)im2, i, 0) = right_view.data[i];
    }
    bool color = false;
    //set match
    Match m(im1, im2, color);
    m.SetDispRange(dMin, dMax);
    //set param
    //params.maxIter, params.edgeThresh, params.bRandomizeEveryIteration, params.dataCost;
    float K =-1, lambda = -1, lambda1 = -1, lambda2 = -1;
    Match::Parameters params = { // Default parameters
        Match::Parameters::L2, 1, // dataCost, denominator
        8, -1, -1, // edgeThresh, lambda1, lambda2 (smoothness cost)
        -1,        // K (occlusion cost)
        4, false   // maxIter, bRandomizeEveryIteration
    };
    fix_parameters(m, params, K, lambda, lambda1, lambda2);
    m.KZ2();
    //output
    m.SaveScaledXLeft("../output.png", false);
    output = imread("../output.png");

    imFree(im1);
    imFree(im2);
    return 0;
}

void GlobalMatcher::set_fractions(Match::Parameters &params, float K,
                                  float lambda1, float lambda2) {
    static const int MAX_DENOM = 1 << 4;
    float minError = std::numeric_limits<float>::max();
    for (int i = 1; i <= MAX_DENOM; i++) {
        float e = 0;
        int numK = 0, num1 = 0, num2 = 0;
        if (K > 0) {
            e += std::abs((numK = int(i * K + .5f)) / (i * K) - 1.0f);
        }
        if (lambda1 > 0) {
            e += std::abs((num1 = int(i * lambda1 + .5f)) / (i * lambda1) - 1.0f);
        }
        if (lambda2 > 0) {
            e += std::abs((num2 = int(i * lambda2 + .5f)) / (i * lambda2) - 1.0f);
        }
        if (e < minError) {
            minError = e;
            params.denominator = i;
            params.K = numK;
            params.lambda1 = num1;
            params.lambda2 = num2;
        }
    }
}

void GlobalMatcher::fix_parameters(Match &m, Match::Parameters &params,
                                   float &K, float &lambda, float &lambda1, float &lambda2) {
    if (K < 0) { // Automatic computation of K
        m.SetParameters(&params);
        K = m.GetK();
    }
    if (lambda < 0) { // Set lambda to K/5
        lambda = K / 5;
    }
    if (lambda1 < 0) {
        lambda1 = 3 * lambda;
    }
    if (lambda2 < 0) {
        lambda2 = lambda;
    }
    set_fractions(params, K, lambda1, lambda2);
    m.SetParameters(&params);
}
