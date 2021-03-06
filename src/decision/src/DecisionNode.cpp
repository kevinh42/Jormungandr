/*
 * Created By: Reid Oliveira
 * Created On: March 17, 2018
 * Description: Node responsible for making navigation decisions. Invokes a
 * subroutine for each logical state of
 * operation.
 */

#include "DecisionNode.h"
#include <std_msgs/String.h>

DecisionNode::DecisionNode(int argc, char** argv, std::string node_name) {
    ros::init(argc, argv, node_name);
    ros::NodeHandle nh;
    ros::NodeHandle private_nh("~");

    getConstants(nh);
    setupSubroutineMap();

    std::string state_topic = "/world_state_node/output";
    int refresh_rate        = 10;
    worldstate_subscriber_  = nh.subscribe(
    state_topic, refresh_rate, &DecisionNode::worldStateCallback, this);

    info_publisher_ = private_nh.advertise<std_msgs::String>("info", 1);
}

/**
 * Callback function when a message is received from the world state node.
 * @param StateMsg message containing the current state
 */
void DecisionNode::worldStateCallback(
const worldstate::StateMsg::ConstPtr& StateMsg) {
    state_t state = StateMsg->state;

    std::string s = std::to_string(state);

    if (subroutines_.find(state) == subroutines_.end()) {
        // We forgot to add a subroutine to the map. This is bad.

        ROS_ERROR_STREAM(state
                         << " was not found in the map of known subroutines");
        ros::shutdown();
    }

    Subroutine* newState = subroutines_[state];
    if (newState != running_) {
        running_->shutdown();
        running_ = newState;
        running_->startup();
    }

    std_msgs::String info;
    info.data = running_->getName();
    info_publisher_.publish(info);
}

/**
 * Sets up the map "subroutines_" such that each enumerated state is mapped to
 * its appropriate subroutine.
 * @param argc standard argc passed in from main, used for the ros::init of each
 * subroutine
 * @param argv standard argv passed in from main, used for the ros::init of each
 * subroutine
 */
void DecisionNode::setupSubroutineMap() {
    // Gate Subroutines
    subroutines_[worldstate::StateMsg::adjustingDepth] =
    new AdjustDepth(&constants_);
    subroutines_[worldstate::StateMsg::locatingGate] =
    new LocateGate(&constants_);
    subroutines_[worldstate::StateMsg::approachingGate] =
    new ApproachGate(&constants_);
    subroutines_[worldstate::StateMsg::aligningWithGate] =
    new LineUpWithGate(&constants_);
    subroutines_[worldstate::StateMsg::passingGate] =
    new GoThroughGate(&constants_);

    // Line Subroutines
    subroutines_[worldstate::StateMsg::findingLine] = new FindLine(&constants_);
    subroutines_[worldstate::StateMsg::adjustingToLine] =
    new AdjustToLine(&constants_);
    subroutines_[worldstate::StateMsg::followingLine] =
    new FollowLine(&constants_);

    running_ = subroutines_[worldstate::StateMsg::adjustingDepth];
    running_->startup();
}

void DecisionNode::getConstants(ros::NodeHandle nh) {
    XmlRpc::XmlRpcValue v;

    nh.getParam("/global_constants", v);

    for (auto value = v.begin(); value != v.end(); value++) {
        constants_[std::string((*value).first)] = (*value).second;
    }
}
