/*
 * Created By: Logan Fillo
 * Created On: October 19 2019
 * Description: Multi thruster controller
 */

#include <controllers/multi_thruster_controller.h>
#include <pluginlib/class_list_macros.hpp>

namespace thruster_controllers {

    MultiThrusterController::MultiThrusterController() {}

    MultiThrusterController::~MultiThrusterController() {}

    bool MultiThrusterController::init(hardware_interface::ThrusterCommandInterface *robot, 
        ros::NodeHandle &root_nh, ros::NodeHandle &controller_nh)
    {   
        if (!controller_nh.getParam("names", thruster_names_))
        {
            ROS_ERROR("Couldn't retrieve thruster name param");
            return false;
        }

        if (!controller_nh.getParam("num_thrusters", num_thrusters_))
        {
            ROS_ERROR("Couldn't retrieve number of thrusters param");
            return false;
        }

        if (thruster_names_.size() != num_thrusters_)
        {
            ROS_ERROR("Num thrusters must match size of thrusters names");
            return false;
        }

        for (int i = 0; i < thruster_names_.size() ; i++){
            thruster_handles_[thruster_names_[i]] = robot->getHandle(thruster_names_[i]);
        }

        if (!pid_controller_.init(ros::NodeHandle(controller_nh, "pid")))
        {
            ROS_ERROR("Could not init PID controller");
            return false;
        }

        std::map<std::string,std::string> topics;  
        if (!controller_nh.getParam("topics", topics))
        {
            ROS_ERROR("Couldn't retrieve thruster controller topics");
            return false;
        }

        decision_sub_ = root_nh.subscribe<nav_msgs::Odometry>(
            topics["decision"], msg_queue_, 
            &MultiThrusterController::decisionCB, this);
        imu_sub_ = root_nh.subscribe<sensor_msgs::Imu>(
            topics["imu"], msg_queue_, 
            &MultiThrusterController::imuCB, this);
        depth_sub_ = root_nh.subscribe<std_msgs::Float64>(
            topics["depth"], msg_queue_, 
            &MultiThrusterController::depthCB, this);

        ROS_INFO("Multi Thruster Controller Initialized");
        return true;
    }

    void MultiThrusterController::update(const ros::Time& time, const ros::Duration& period) 
    {   
        imu_data_struct_ = *(imu_data_.readFromRT());
        decision_cmd_struct_ = *(decision_cmd_.readFromRT());
        depth_sensor_data_struct_ = *(depth_sensor_data_.readFromRT());
        // TODO: Thruster update algorithm
        // ROS_INFO("Multi Thruster Controller Updated");
    }

    void MultiThrusterController::starting(const ros::Time& time) 
    {
        // TODO: Initialize structs and val
        imu_data_.initRT(imu_data_struct_);
        decision_cmd_.initRT(decision_cmd_struct_);
        depth_sensor_data_.initRT(depth_sensor_data_struct_);

        pid_controller_.reset();
    }

    void MultiThrusterController::stopping(const ros::Time& time) 
    {
        decision_sub_.shutdown();
        imu_sub_.shutdown();
        depth_sub_.shutdown();
    }

    void MultiThrusterController::decisionCB(const nav_msgs::Odometry::ConstPtr& msg) 
    {   
        // ROS_INFO("Got decision message");
        DecisionCmd cmd;
        // TODO: fill cmd with msg
        decision_cmd_struct_ = cmd;
        decision_cmd_.writeFromNonRT(decision_cmd_struct_);
    }

    void MultiThrusterController::imuCB(const sensor_msgs::Imu::ConstPtr& msg)
    {
        // ROS_INFO("Got imu message");
        ImuData data;
        // TODO: fill data with msg
        imu_data_struct_ = data;
        imu_data_.writeFromNonRT(imu_data_struct_);
    }

    void MultiThrusterController::depthCB(const std_msgs::Float64::ConstPtr& msg) 
    {
        // ROS_INFO("Got depth message");
        DepthSensorData data;
        // TODO: fill data with msg
        depth_sensor_data_struct_ = data;
        depth_sensor_data_.writeFromNonRT(depth_sensor_data_struct_);
    }

} // namespace thruster_controllers

PLUGINLIB_EXPORT_CLASS(thruster_controllers::MultiThrusterController, controller_interface::ControllerBase)

