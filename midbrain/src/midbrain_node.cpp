#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/Vector3Stamped.h"
#include "geometry_msgs/TwistStamped.h"
#include "sensor_msgs/NavSatFix.h"
#include "nav_msgs/Path.h"
#include "geometry_msgs/PoseStamped.h"


double y;
double x;
double then;

double pos_x, pos_y;
double vel_x, vel_y;

nav_msgs::Path path_msg;

void magCallback(const geometry_msgs::Vector3Stamped::ConstPtr& msg)
{
	ROS_INFO("Imu Magnetic Orientation x: [%f], y: [%f], z: [%f]", msg->vector.x,msg->vector.y,msg->vector.z);

return;
}

void data_RawCallback(const sensor_msgs::Imu::ConstPtr& msg)
{
	const ros::Time& t = msg->header.stamp;

	double now = t.toSec();

	double dt = now - then;
	
	vel_x += msg->linear_acceleration.x * dt;
	vel_y += msg->linear_acceleration.y * dt;

	pos_x += vel_x * dt;
	pos_y += vel_y * dt;

	geometry_msgs::PoseStamped pose;
	
	pose.pose.position.x = pos_x;
	pose.pose.position.y = pos_y;
	pose.pose.position.z = 0.0;

	path_msg.poses.push_back(pose);

	then = now;
	ROS_INFO("Imu Seq: [%d]", msg->header.seq);
	ROS_INFO("Imu Angular_velocity x: [%f], y: [%f], z: [%f]", msg->angular_velocity.x,msg->angular_velocity.y,msg->angular_velocity.z);
	ROS_INFO("Imu Linear_Acceleration x: [%f], y: [%f], z: [%f]", msg->linear_acceleration.x,msg->linear_acceleration.y,msg->linear_acceleration.z);
	return;
}

void gpsFixCallback(const sensor_msgs::NavSatFix::ConstPtr& msg)
{
	ROS_INFO("GPS FIX STATUS: [%d], [%d]", msg->status.status, msg->status.service);
	ROS_INFO("Lat: [%f] Lon: [%f]", msg->latitude, msg->longitude);
	return;
}

void gpsVelCallback(const geometry_msgs::TwistStamped::ConstPtr& msg)
{
	ROS_INFO("Linear");

	return;
}

int main(int argc, char* argv[])
{
	x = 0;
	y = 0;
  //initialize the node. 
  ros::init(argc, argv, "midbrain");
  //node handle.
  ros::NodeHandle n;
  //subscribes to a node.
  then = ros::Time::now().toSec();
  path_msg.header.frame_id = "path";

  ros::Subscriber imuSub = n.subscribe("/imu/mag", 1000, magCallback);
  ros::Subscriber magSub = n.subscribe("/imu/data", 1000, data_RawCallback);
  ros::Subscriber gpsFixSub = n.subscribe("fix", 1000, gpsFixCallback);
  ros::Subscriber gpsVelSub = n.subscribe("vel", 1000, gpsVelCallback);
  //wait for callbacks

  ros::Publisher pathPub = n.advertise<nav_msgs::Path>("path", 1000, true);

  while(ros::ok()){
  	pathPub.publish(path_msg);
  	ros::spinOnce();
  }

	return 0;
}
