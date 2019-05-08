#include <iostream>
#include <string>
#include <sstream>
#include "StereoRectifier.h"
#include "LocalMatcher.h"
#include "GlobalMatcher.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


enum match_method { SAD = 1, NCC = 2, GRAPH_CUT =3};



int main(int argc, char *argv[]) {

    //input
    match_method m_method;
    string calib_filename, output_filename;
    string filename_left_view, filename_right_view;

    if (0) {
        //test
        m_method = SAD;
        output_filename = "../dis11.png";
        filename_left_view = "../../data/teddy/imL.png";
        filename_right_view = "../../data/teddy/imR.png";
    } else {
        //interactive
        cout << "输入左图路径" << endl;
        cin >> filename_left_view;
        cout << "输入右图路径" << endl;
        cin >> filename_right_view;
        cout << "输出视差图路径" << endl;
        cin >> output_filename;
        cout << "输入相机内参路径（输入0表示跳过矫正）" << endl;
        cin >> calib_filename;
        cout << "输入数字选择匹配算法（1：SAD 2：NCC 3：GC）" << endl;
        int t_method;
        cin >> t_method, m_method = match_method(t_method);
    }
    // Read image
    Mat left_view, right_view;
    left_view = imread(filename_left_view, CV_LOAD_IMAGE_GRAYSCALE);
    right_view = imread(filename_right_view, CV_LOAD_IMAGE_GRAYSCALE);
    if (left_view.empty() || right_view.empty()) {
        cout << "fail to open" << endl;
        return -1;
    }
    int width = left_view.size().width;
    int height = left_view.size().height;

    // Stereo rectify
    Mat stereo_image;
    StereoRectifier sr;
    sr.StereoRectify(calib_filename, left_view, right_view, stereo_image);
    // Stereo match
    int max_disparity = width / 8;
    int window_size = (max_disparity / 12) * 2 + 1;
    Mat disparity(left_view.rows, left_view.cols, CV_8UC3, Scalar(0, 0, 0));

    time_t start_time, end_time;
    start_time = clock();
    if (m_method == SAD) {
        LocalMatcher lm;
        cout << "Running SAD Match" << endl;
        lm.LocalMatchingSAD(left_view, right_view, window_size,
                            max_disparity, disparity);
    } else  if (m_method == NCC) {
        LocalMatcher lm;
        cout << "Running NCC Match" << endl;
        lm.LocalMatchingNCC(left_view, right_view, window_size,
                            max_disparity, disparity);
    } else if (m_method == GRAPH_CUT) {
        GlobalMatcher gm;
        gm.run(left_view, right_view, -max_disparity, 0, disparity);
    }
    end_time = clock();

    double total_time = double(end_time - start_time) / CLOCKS_PER_SEC * 1000;
    cout << "processing time: " << total_time << "ms" << endl;
    cout << "Image size: [" << disparity.cols << ", " << disparity.rows << "]";

    namedWindow("Disparity");
    moveWindow("Disparity", 0, height);
    imshow("Disparity", disparity);

    for (int row = 0; row <stereo_image.rows; row += 32)
        line(stereo_image, Point(0, row), Point(stereo_image.cols, row),
             Scalar(0, 0, 255));
    namedWindow("Stereo");
    moveWindow("Stereo", 0, 0);
    imshow("Stereo", stereo_image);

    waitKey(0);

    destroyAllWindows();
    if (!output_filename.empty())
        imwrite(output_filename, disparity);

    return 0;
}