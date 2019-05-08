#ifndef GLOBALMATCHER_H
#define GLOBALMATCHER_H



#include <limits>
#include <iostream>
#include <ctime>
#include "match.h"
#include "opencv2/opencv.hpp"

class GlobalMatcher {
  public:
    int run(cv::Mat &left_view, cv::Mat &right_view, int dMin, int dMax,
            cv::Mat &output);
  private:
    /// Store in \a params fractions approximating the last 3 parameters.
    ///
    /// They have the same denominator (up to \c MAX_DENOM), chosen so that the sum
    /// of relative errors is minimized.
    void set_fractions(Match::Parameters &params, float K, float lambda1,
                       float lambda2);

    /// Make sure parameters K, lambda1 and lambda2 are non-negative.
    ///
    /// - K may be computed automatically and lambda set to K/5.
    /// - lambda1=3*lambda, lambda2=lambda
    /// As the graph requires integer weights, use fractions and common denominator.
    void fix_parameters(Match &m, Match::Parameters &params,
                        float &K, float &lambda, float &lambda1, float &lambda2);

};

#endif // GLOBALMATCHER_H