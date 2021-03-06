/*
 * Created By: Cameron Newton
 * Created On: February 27th, 2018
 */

#ifndef PROJECT_POLE_H
#define PROJECT_POLE_H

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Pole {
  public:
    Pole();

    /**
     * Object representing a detected Pole, horizontal or vertical
     *
     * @param side1 1 side of the pole
     *
     * @param side2 the other side of the pole
     *
     * @param m, b      y = m * x ^ -b, Function interpolated from data
     *
     *                  y = distance from pole
     *
     *                  x = pixel width of pole
     */
    Pole(cv::Vec4i side1, cv::Vec4i side2);

    int getVertMid();

    int getHorMid();

    int getVertWidth();

    int getHorWidth();

    cv::Vec4i getMiddleLine();

  private:
    cv::Vec4i side1_, side2_;
};

#endif // PROJECT_POLE_H
