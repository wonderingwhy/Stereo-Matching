#include "StereoRectify.h"
#include "LocalMatcher.h"

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <string>
#include <sstream>

using namespace cv;
using namespace std;

Mat show_two_image;
int width, height;
enum match_method { SAD, NCC, GRAPH_CUT };

void StereoRectification(string calib_filename, Mat &view_l, Mat &view_r) {
    bool need_rectify = true;
    FILE *file = fopen(calib_filename.c_str(), "r");
    if (file == nullptr) {
        need_rectify = false;
    }

    Mat left_rect, right_rect;
    show_two_image.create(height, width * 2, CV_8UC1);
    left_rect = show_two_image(Rect(0, 0, width, height));
    right_rect = show_two_image(Rect(width, 0, width, height));

    if (need_rectify) {
        Mat cam_matrix;
        Mat dist_coeffs = Mat(1, 4, CV_32FC1, Scalar(0));
        Mat left_RT, right_RT;
        stereo::ReadParam(calib_filename, cam_matrix, &left_RT, &right_RT);

        Rect roi_l, roi_r;
        stereo::Rectify(view_l, view_r, cam_matrix, dist_coeffs,
                        left_RT, right_RT, &left_rect, &right_rect, &roi_l, &roi_r);
        view_l = left_rect;
        view_r = right_rect;
    } else {
        view_l.copyTo(left_rect);
        view_r.copyTo(right_rect);
    }
}

int _main(int argc, char *argv[]) {
    //input
    match_method m_method = SAD;
    string calib_filename, output_filename;
    string filename_left_view, filename_right_view;

    output_filename = "../dis.png";
    filename_left_view = "../data/1/left.jpg";
    filename_right_view = "../data/1/right.jpg";
    calib_filename = "../data/1/para.txt";

    // Read image
    Mat left_view, right_view;
    left_view = imread(filename_left_view, CV_LOAD_IMAGE_GRAYSCALE);
    right_view = imread(filename_right_view, CV_LOAD_IMAGE_GRAYSCALE);

    width = left_view.size().width;
    height = left_view.size().height;

    // Stereo rectification
    StereoRectification(calib_filename, left_view, right_view);
    //disp
    Mat disparity(left_view.rows, left_view.cols, CV_8UC1, Scalar(0));
    int max_disparity;

    time_t start_time, end_time;
    start_time = clock();
    if (m_method == SAD) {
        max_disparity = 64;
        cout << "Running SAD Match" << endl;
        LocalMatchingSAD(left_view, right_view, 16, 64, disparity);
    } else  if (m_method == NCC) {
        max_disparity = 64;
        cout << "Running NCC Match" << endl;
        LocalMatchingNCC(left_view, right_view, 16, 64, disparity);
    } else if (m_method == GRAPH_CUT) {

    }
    end_time = clock();
    Mat disp_view;
    disparity.convertTo(disp_view, CV_8U, 255 / (max_disparity * 1.0));
    double total_time = double(end_time - start_time) / CLOCKS_PER_SEC * 1000;
    cout << "processing time: " << total_time << "ms" << endl;
    cout << "Image size: [" << disp_view.cols << ", " << disp_view.rows << "]";

    namedWindow("Disparity");
    moveWindow("Disparity", 0, height);
    imshow("Disparity", disp_view);

    for (int row = 0; row <show_two_image.rows; row += 32)
        line(show_two_image, Point(0, row), Point(show_two_image.cols, row),
             Scalar(0, 0, 255));
    namedWindow("Stereo");
    moveWindow("Stereo", 0, 0);
    imshow("Stereo", show_two_image);

    waitKey(0);

    destroyAllWindows();
    if (!output_filename.empty())
        imwrite(output_filename, disp_view);

    return 0;
}
