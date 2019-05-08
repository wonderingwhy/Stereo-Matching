#include "StereoRectifier.h"

#include <fstream>
#include <istream>
#include <streambuf>
#include <vector>

using namespace std;
using namespace cv;

void StereoRectifier::ReadParam(const string &filename, Mat &cam_matrix,
                                Mat *left_RT, Mat *right_RT) {
    ifstream ifs(filename);
    float fx = 0, fy = 0, cx = 0, cy = 0;
    char buffer[512];
    ifs.getline(buffer, 512);
    sscanf(buffer, "fx:%f", &fx);
    ifs.getline(buffer, 512);
    sscanf(buffer, "fy:%f", &fy);
    ifs.getline(buffer, 512);
    sscanf(buffer, "cx:%f", &cx);
    ifs.getline(buffer, 512);
    sscanf(buffer, "cy:%f", &cy);
    cam_matrix = (Mat1f(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);

    float mat[12] = { 0 };
    ifs.getline(buffer, 512);//left RT
    for (int i = 0; i < 12; ++i)
        ifs >> mat[i];
    (*left_RT) = (Mat1f(4, 4) << mat[0], mat[1], mat[2], mat[3],
                  mat[4], mat[5], mat[6], mat[7],
                  mat[8], mat[9], mat[10], mat[11],
                  0, 0, 0, 1);
    ifs.getline(buffer, 512), ifs.getline(buffer, 512); //right RT
    for (int i = 0; i < 12; ++i)
        ifs >> mat[i];
    (*right_RT) = (Mat1f(4, 4) << mat[0], mat[1], mat[2], mat[3],
                   mat[4], mat[5], mat[6], mat[7],
                   mat[8], mat[9], mat[10], mat[11],
                   0, 0, 0, 1);

    ifs.close();
}

void StereoRectifier::Rectify(const Mat &left_view, const Mat &right_view,
                              const Mat &cam_matrix, const Mat &dist_coeffs,
                              const Mat &left_RT, const Mat &right_RT,
                              Mat *view_rect_l, Mat *view_rect_r,
                              Rect *roi_l, Rect *roi_r) {
    // Compute RT
    Mat RT_r2l = right_RT * left_RT.inv();
    Mat R = (Mat1d(3, 3) <<
             RT_r2l.at<float>(0), RT_r2l.at<float>(1), RT_r2l.at<float>(2),
             RT_r2l.at<float>(4), RT_r2l.at<float>(5), RT_r2l.at<float>(6),
             RT_r2l.at<float>(8), RT_r2l.at<float>(9), RT_r2l.at<float>(10));
    Mat T = (Mat1d(3, 1) <<
             RT_r2l.at<float>(3), RT_r2l.at<float>(7), RT_r2l.at<float>(11));
    // Rectify
    Mat R1, R2, P1, P2, Q;
    Size size = left_view.size();
    stereoRectify(cam_matrix, dist_coeffs, cam_matrix, dist_coeffs, size, R, T,
                  R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, Size(), roi_l, roi_r);

    // Remap
    Mat map_matrix[2][2];
    initUndistortRectifyMap(cam_matrix, dist_coeffs, R1, P1, size,
                            CV_16SC2, map_matrix[0][0], map_matrix[0][1]);
    initUndistortRectifyMap(cam_matrix, dist_coeffs, R2, P2, size,
                            CV_16SC2, map_matrix[1][0], map_matrix[1][1]);

    remap(left_view, *view_rect_l, map_matrix[0][0], map_matrix[0][1],
          CV_INTER_LINEAR);
    remap(right_view, *view_rect_r, map_matrix[1][0], map_matrix[1][1],
          CV_INTER_LINEAR);
}

void StereoRectifier::StereoRectify(string calib_filename,
                                    Mat &view_l, Mat &view_r,
                                    Mat &show_two_image) {
    bool need_rectify = true;
    FILE *calib = fopen(calib_filename.c_str(), "r");
    if (calib == nullptr) {
        need_rectify = false;
    }

    Mat left_rect, right_rect;
    int width = view_l.cols;
    int height = view_l.rows;
    show_two_image.create(height, width * 2, CV_8UC1);
    left_rect = show_two_image(Rect(0, 0, width, height));
    right_rect = show_two_image(Rect(width, 0, width, height));

    if (need_rectify) {
        Mat cam_matrix;
        Mat dist_coeffs = Mat(1, 4, CV_32FC1, Scalar(0));
        Mat left_RT, right_RT;
        ReadParam(calib_filename, cam_matrix, &left_RT, &right_RT);

        Rect roi_l, roi_r;
        Rectify(view_l, view_r, cam_matrix, dist_coeffs,
                left_RT, right_RT, &left_rect, &right_rect, &roi_l, &roi_r);
        view_l = left_rect;
        view_r = right_rect;
    } else {
        view_l.copyTo(left_rect);
        view_r.copyTo(right_rect);
    }
}
