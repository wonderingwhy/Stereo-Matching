#ifndef STEREO_RECTIFIER_H_
#define STEREO_RECTIFIER_H_

#include <string>
#include "opencv2/opencv.hpp"

class StereoRectifier {
  public:
    void StereoRectify(std::string calib_filename, cv::Mat &view_l, cv::Mat &view_r,
                       cv::Mat &show_two_image);
  private:
    void ReadParam(const std::string &filename, cv::Mat &intrinsics,
                   cv::Mat *RT_left, cv::Mat *RT_right);

    void Rectify(const cv::Mat &view_left, const cv::Mat &view_right,
                 const cv::Mat &intrinsics, const cv::Mat &dist_coeffs,
                 const cv::Mat &RT_left, const cv::Mat &RT_right,
                 cv::Mat *view_rect_left, cv::Mat *view_rect_right,
                 cv::Rect *roi_left = nullptr, cv::Rect *roi_right = nullptr);


};

#endif  // STEREO_RECTIFIER_H_
