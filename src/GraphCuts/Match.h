#ifndef MATCH_H
#define MATCH_H

#include "image.h"
class Energy;

/// Main class for Kolmogorov-Zabih algorithm
class Match {
  public:
    Match(GeneralImage left, GeneralImage right, bool color = false);
    ~Match();

    void SetDispRange(int dMin, int dMax);

    /// Parameters of algorithm.
    struct Parameters {
        enum { L1, L2 } dataCost; ///< Data term
        /// Data term must be multiplied by denominator.
        /// Equivalent to using lambda1/denom, lambda2/denom, K/denom
        int denominator;

        // Smoothness term
        int edgeThresh; ///< Intensity level diff for 'edge'
        int lambda1; ///< Smoothness cost not across edge
        int lambda2; ///< Smoothness cost across edge (should be <=lambda1)
        int K; ///< Penalty for inactive assignment

        int maxIter; ///< Maximum number of iterations
        bool bRandomizeEveryIteration; ///< Random alpha order at each iter

    };
    float GetK();
    void SetParameters(Parameters *params);
    void KZ2();

    void SaveXLeft(const char *fileName); ///< Save disp. map as float TIFF
    void SaveScaledXLeft(const char *fileName, bool flag); ///< Save colormapped
    RGBImage GetOutputImage();
  private:
    Coord imSizeL, imSizeR; ///< image dimensions
    int originalHeightL; ///< true left image height before possible crop
    GrayImage imLeft, imRight;          ///< original images (if gray)
    RGBImage imColorLeft, imColorRight; ///< original images (if color)
    GrayImage imLeftMin, imLeftMax;     ///< range of gray based on neighbors
    GrayImage imRightMin, imRightMax;   ///< range of gray based on neighbors
    RGBImage imColorLeftMin, imColorLeftMax; ///< For color images
    RGBImage imColorRightMin, imColorRightMax;
    int dispMin, dispMax; ///< range of disparities

    static const int OCCLUDED; ///< Special value of disparity meaning occlusion
    /// If (p,q) is an active assignment
    /// q == p + Coord(IMREF(d_left,  p), p.y)
    /// p == q + Coord(IMREF(d_right, q), q.y)
    IntImage  d_left, d_right;
    Parameters  params; ///< Set of parameters

    int E; ///< Current energy
    IntImage vars0; ///< Variables before alpha expansion
    IntImage varsA; ///< Variables after alpha expansion

    void run();
    void InitSubPixel();

    // Data penalty functions
    int  data_penalty_gray (Coord l, Coord r) const;
    int  data_penalty_color(Coord l, Coord r) const;

    // Smoothness penalty functions
    int  SmoothnessPenaltyGray (Coord p, Coord np, int d) const;
    int  SmoothnessPenaltyColor(Coord p, Coord np, int d) const;

    // Kolmogorov-Zabih algorithm
    int  data_occlusion_penalty(Coord l, Coord r) const;
    int  smoothness_penalty(Coord p, Coord np, int d) const;
    int  ComputeEnergy() const;
    bool ExpansionMove(int a);

    // Graph construction
    void build_nodes        (Energy &e, Coord p, int a);
    void build_smoothness   (Energy &e, Coord p, Coord np, int a);
    void build_uniqueness_LR(Energy &e, Coord p);
    void build_uniqueness_RL(Energy &e, Coord p, int a);
    void update_disparity(const Energy &e, int a);
};

#endif
