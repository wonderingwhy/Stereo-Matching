#ifndef LOCAL_MATCHER_H_
#define LOCAL_MATCHER_H_

#include "opencv2/opencv.hpp"
class LocalMatcher {
  public:
    void LocalMatchingSAD(cv::Mat &img1, cv::Mat &img2, int window_size,
                          int search_scope, cv::Mat &disparity);

    void LocalMatchingNCC(cv::Mat &img1, cv::Mat &img2, int window_size,
                          int search_scope, cv::Mat &disparity);
};
#endif  // LOCAL_MATCHER_H_