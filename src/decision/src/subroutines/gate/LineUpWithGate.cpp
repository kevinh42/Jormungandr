/*
 * Created By: Reid Oliveira
 * Created On: March 24, 2018
 * Description: Subroutine that tries to position the sub in front and
 * orthogonal with the gate, ready to go through.
 */

#include "LineUpWithGate.h"
#include "constants.h"

std::vector<ros::Subscriber>
LineUpWithGate::getSubscriptions(ros::NodeHandle nh) {
    std::vector<ros::Subscriber> subs;
    subs.push_back(
    nh.subscribe("gate_location", 10, &LineUpWithGate::decisionCallback, this));
    return subs;
}

void LineUpWithGate::decisionCallback(
const gate_detect::GateDetectMsg::ConstPtr& msg) {
    // logic: given the location of the poles, try to put ourselves centred in
    // front

    // send the message
    nav_msgs::Odometry command;

    double x_linear  = 0.0;
    double y_linear  = 0.0;
    double z_linear  = 0.0;
    double x_angular = 0.0;
    double y_angular = 0.0;
    double z_angular = 0.0;

    // If we're at an acceptable distance away from the top pole, adjust for
    // clearance
    if (msg->detectedTopPole &&
        abs((int) (msg->distanceTopPole -
                   (*constants_)["TARGET_TOP_POLE_DISTANCE"])) <
        (*constants_)["ERROR_TOLERANCE_TOP_POLE_DISTANCE"]) {
        float top_pole_clearance =
        (float) sin(msg->angleTopPole) * msg->distanceTopPole;
        if ((top_pole_clearance - (*constants_)["TARGET_TOP_POLE_CLEARANCE"]) >
            (*constants_)["ERROR_TOLERANCE_TOP_POLE_CLEARANCE"]) {
            command.pose.pose.position.z =
            (*constants_)["TARGET_TOP_POLE_CLEARANCE"] - top_pole_clearance;
            publishCommand(command);
            return;
        } else if ((top_pole_clearance -
                    (*constants_)["TARGET_TOP_POLE_CLEARANCE"]) <
                   (*constants_)["ERROR_TOLERANCE_TOP_POLE_CLEARANCE"]) {
            command.pose.pose.position.z =
            (*constants_)["TARGET_TOP_POLE_CLEARANCE"] - top_pole_clearance;
            publishCommand(command);
            return;
        }
    }

    // Make sure we're pointing at the middle of the gate
    if ((msg->angleRightPole + msg->angleLeftPole) >
        (*constants_)["ERROR_TOLERANCE_SIDE_POLES_ANGLE"]) {
        command.twist.twist.angular.z = RIGHT;
    } else if ((msg->angleRightPole + msg->angleLeftPole) <
               (*constants_)["ERROR_TOLERANCE_SIDE_POLES_ANGLE"]) {
        command.twist.twist.angular.z = LEFT;
    }

    // Get within a good passing distance for gate
    float averageDistanceToGate =
    (msg->distanceLeftPole + msg->distanceRightPole + msg->distanceTopPole) /
    (msg->detectedLeftPole + msg->detectedRightPole + msg->detectedTopPole);
    if ((averageDistanceToGate - (*constants_)["TARGET_SIDE_POLES_DISTANCE"]) <
        -(*constants_)["ERROR_TOLERANCE_SIDE_POLES_DISTANCE"]) {
        command.twist.twist.linear.x = BACKWARD;
    } else if ((averageDistanceToGate -
                (*constants_)["TARGET_SIDE_POLES_DISTANCE"]) >
               (*constants_)["ERROR_TOLERANCE_SIDE_POLES_DISTANCE"]) {
        command.twist.twist.linear.x = FORWARD;
    }

    // Position ourselves laterally in front of the gate
    if ((msg->distanceRightPole - msg->distanceLeftPole) >
        (*constants_)["ERROR_TOLERANCE_SIDE_POLES_DISTANCE"]) {
        command.twist.twist.linear.y = LEFT;
    } else if ((msg->distanceRightPole - msg->distanceLeftPole) <
               -(*constants_)["ERROR_TOLERANCE_SIDE_POLES_DISTANCE"]) {
        command.twist.twist.linear.y = RIGHT;
    }

    publishCommand(command);
}
