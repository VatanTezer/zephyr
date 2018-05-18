#include <fstream>
#include <iostream>

#include <Eigen/Core>
#include <mav_msgs/conversions.h>
#include <mav_msgs/default_topics.h>
#include <ros/ros.h>
#include <trajectory_msgs/MultiDOFJointTrajectory.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/LaserScan.h>

// Globals
nav_msgs::Odometry current_odom;
sensor_msgs::LaserScan current_scan;
const float DEG_2_RAD = M_PI / 180.0;
const int DEBUG = 0;

void odom_callback(const nav_msgs::Odometry::ConstPtr& msg)
{
	current_odom.header.seq = msg->header.seq;
	current_odom.header.frame_id = "world";
	current_odom.child_frame_id = "firefly/base_link";
	current_odom.pose.pose.position.x = msg->pose.pose.position.x;
	current_odom.pose.pose.position.y = msg->pose.pose.position.y;
	current_odom.pose.pose.position.z = msg->pose.pose.position.z;
	current_odom.pose.pose.orientation.x = msg->pose.pose.orientation.x;
	current_odom.pose.pose.orientation.y = msg->pose.pose.orientation.y;
	current_odom.pose.pose.orientation.z = msg->pose.pose.orientation.z;
	current_odom.pose.pose.orientation.w = msg->pose.pose.orientation.w;
	current_odom.twist.twist.linear.x = msg->twist.twist.linear.x;
	current_odom.twist.twist.linear.y = msg->twist.twist.linear.y;
	current_odom.twist.twist.linear.z = msg->twist.twist.linear.z;
	current_odom.twist.twist.angular.x = msg->twist.twist.angular.x;
	current_odom.twist.twist.angular.y = msg->twist.twist.angular.y;
	current_odom.twist.twist.angular.z = msg->twist.twist.angular.z;
	
	if(DEBUG)
	{
		ROS_INFO("Seq: [%d]", current_odom.header.seq);
		ROS_INFO("Position-> x: [%f], y: [%f], z: [%f]", current_odom.pose.pose.position.x,current_odom.pose.pose.position.y, current_odom.pose.pose.position.z);
		ROS_INFO("Orientation-> x: [%f], y: [%f], z: [%f], w: [%f]", current_odom.pose.pose.orientation.x, current_odom.pose.pose.orientation.y, current_odom.pose.pose.orientation.z, current_odom.pose.pose.orientation.w);
		ROS_INFO("Vel-> Linear: [%f], Angular: [%f]", current_odom.twist.twist.linear.x,current_odom.twist.twist.angular.z);
	}

}

void laser_callback(const sensor_msgs::LaserScan::ConstPtr& msg)
{

	double min_angle = msg->angle_min;
	double max_angle = msg->angle_max;
	double increment = msg->angle_increment;
	int size = msg->ranges.size();
	int start = size/2 + min_angle/increment;
	int end = size/2 + max_angle/increment;

	current_scan.scan_time = msg->scan_time;
	current_scan.angle_increment = increment;
	current_scan.time_increment = msg->time_increment;
	current_scan.range_min = msg->range_min;
	current_scan.range_max = msg->range_max;
	current_scan.ranges.resize(end-start);
	int count = 0;

	for (int i = start; i <= end; i++)
	{
		current_scan.ranges[count] = msg->ranges[i];
		count++;
		if(DEBUG)
		{
			ROS_INFO("Ranges: [%f]", current_scan.ranges[i]);	
		}
	}
}


int main(int argc, char** argv) 
{
	// Init Sequence
	ros::init(argc, argv, "waypoint_generator");
	ros::NodeHandle nh;
	ros::Rate loop_rate(200);

	// Subscriber(s)
	ros::Subscriber odom_sub = nh.subscribe("/firefly/odom_fused", 1000, odom_callback);
	ros::Subscriber laser_sub = nh.subscribe("/firefly/scan", 1000, laser_callback);

	// Publisher(s)
	ros::Publisher odom_pub;
	if(DEBUG)
	{
		odom_pub = nh.advertise<nav_msgs::Odometry>("/firefly/test_odom", 10);
	}
	
	/*
	ros::Publisher trajectory_pub =
      nh.advertise<trajectory_msgs::MultiDOFJointTrajectory>(
      mav_msgs::default_topics::COMMAND_TRAJECTORY, 10);
    */

	ROS_INFO("Started waypoint_generator.");

	while(ros::ok())
	{

		if(DEBUG)
		{
			current_odom.header.stamp = ros::Time::now();
			odom_pub.publish(current_odom);
		}

		// trajectory_msgs::MultiDOFJointTrajectory trajectory_msg;
		// trajectory_msg.header.stamp = ros::Time::now();

		// Position calculation comes here
		/*
		Eigen::Vector3d desired_position(std::stof(args.at(1)), std::stof(args.at(2)),
	                                   std::stof(args.at(3)));

		double desired_yaw = std::stof(args.at(4)) * DEG_2_RAD;

		mav_msgs::msgMultiDofJointTrajectoryFromPositionYaw(desired_position,
	      desired_yaw, &trajectory_msg);
		*/

		// Wait for some time to create the ros publisher.
		// ros::Duration(delay).sleep();

		ros::spinOnce();
		loop_rate.sleep();
	}
}