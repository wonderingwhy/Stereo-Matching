#include "LocalMatcher.h"

using namespace cv;

void LocalMatcher::LocalMatchingSAD(Mat &img1, Mat &img2, int window_size,
                                    int search_scope, Mat &disparity) {
    const int width = img1.cols;
    const int height = img1.rows;
    int N = window_size;

    for (int y = 0; y < height - N; ++y) {
        for (int x = search_scope; x < width - N; ++x) {
            int min_sum = 255 * N * N;
            int min_doff = 0;
            for (int doff = 0; doff < search_scope; ++doff) {
                int sum = 0;
                int avg1 = 0, avg2 = 0;
                for (int j = y; j < y + N; ++j) {
                    for (int i = x; i < x + N; ++i) {
                        avg1 += (int)img1.at<uchar>(j, i);
                        avg2 += (int)img2.at<uchar>(j, i - doff);
                    }
                }
                float ad = 1.0*avg2 / avg1;
                for (int j = y; j < y + N; ++j) {
                    for (int i = x; i < x + N; ++i) {
                        int dif = ad*(int)img1.at<uchar>(j, i) - (int)img2.at<uchar>(j, i - doff);
                        if (dif < 0) {
                            dif = -dif;
                        }
                        sum += dif;
                    }
                }
                if (sum < min_sum) {
                    min_sum = sum;
                    min_doff = doff;
                }
            }
            min_doff  = min_doff * 255 / search_scope;
            disparity.at<Vec3b>(y + N / 2, x + N / 2) = Vec3b(min_doff, min_doff, min_doff);
        }
    }
}


void LocalMatcher::LocalMatchingNCC(Mat &img1, Mat &img2, int window_size,
                                    int search_scope, Mat &disparity) {
    const int width = img1.cols;
    const int height = img1.rows;
    int N = window_size;
    int max_min_sum = 0;
    for (int y = 0; y < height - N; ++y) {
        for (int x = search_scope; x < width - N; ++x) {
            float max_ncc = -1;
            int max_doff = 0;
            for (int doff = 0; doff < search_scope; ++doff) {
                float avg1 = 0, avg2 = 0;
                for (int j = y; j < y + N; ++j) {
                    for (int i = x; i < x + N; ++i) {
                        avg1 += (int)img1.at<uchar>(j, i);
                        avg2 += (int)img2.at<uchar>(j, i - doff);
                    }
                }
                avg1 /= (N*N), avg2 /= (N*N);
                float m1 = 0, m2 = 0;
                for (int j = y; j < y + N; ++j) {
                    for (int i = x; i < x + N; ++i) {
                        m1 += ((int)img1.at<uchar>(j, i)-avg1)*((int)img1.at<uchar>(j, i) - avg1);
                        m2 += ((int)img2.at<uchar>(j, i - doff)-avg2)* ((int)img2.at<uchar>(j,
                                i - doff) - avg2);
                    }
                }
                m1 = sqrt(m1), m2 = sqrt(m2);
                float ncc = 0;
                for (int j = y; j < y + N; ++j) {
                    for (int i = x; i < x + N; ++i) {
                        float t1 = ((int)img1.at<uchar>(j, i) - avg1)/m1;
                        float t2 = ((int)img2.at<uchar>(j, i - doff) - avg2)/m2;
                        ncc += t1*t2;
                    }
                }

                if (ncc > max_ncc) {
                    max_ncc = ncc;
                    max_doff = doff;
                }
            }
            max_doff = max_doff * 255 / search_scope;
            disparity.at<Vec3b>(y + N / 2, x + N / 2) = Vec3b(max_doff, max_doff, max_doff);

        }
    }
}