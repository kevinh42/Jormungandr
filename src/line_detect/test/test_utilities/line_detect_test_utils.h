/*
 * Created By: Cameron Newton
 * Created On: June 16th, 2018
 * Description: Utilities to aid in debugging LineDetector module
 */

#include "LineDetector.h"

#ifndef PROJECT_TESTUTILS_H
#define PROJECT_TESTUTILS_H

class TestUtils {
  public:
    static void drawLineToFollow(cv::Mat mat_in, LinesToFollow linesToFollow);
};

#endif // PROJECT_TESTUTILS_H
