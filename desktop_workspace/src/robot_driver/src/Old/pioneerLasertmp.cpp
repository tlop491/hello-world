#include "ros/ros.h"
#include "geometry_msgs/Twist.h"

int main (int argc, char **argv)
{
	// command line ROS arguments
	// pioneer laser node is the node name
	ros::init(argc, argv, "pioneer_laser_node");

	ros::NodeHandle my_handle; // ROS comms access point

	/*
	Recall that in part 2 of the lab we use the geometry_msgs::Twist message type
	to publish to the /RosAria/cmd_vel topic in order to control the robot.
	Further information on the message type http://docs.ros.org/api/geometry_msgs/html/msg/Twist.html
	I.e., the topic /RosAria/cmd_vel is subscribed by the RosAria node for controlling the movement of the robot.
	*/	

	/*
	We will create a publisher which sends Twist messages to the "/RosAria/cmd_vel" topic to control the robot.
	We use the advertise function to register our publisher with the master core (roscore).	
	*/
	ros::Publisher mypub_object = my_handle.advertise<geometry_msgs::Twist>("/RosAria/cmd_vel",1);
	

	ros::Rate loop_rate(10);	// our loop rate is loop 10 Hz
	geometry_msgs::Twist mypub_msg;

	//for (int j = 0; j < 50; ++j) publish the same velocity command 50 times
	while(ros::ok())
	{
		loop_rate.sleep();	//sleep at the rate of the loop rate (i.e., 100ms for 10Hz)

		//set out x velocity to be 0.05 ms-1
		mypub_msg.linear.x = 0.05;

		mypub_object.publish(mypub_msg); // publish to the twist to the topic
	}
	mypub_msg.linear.x = 0.0;	 // stop the robot by commanding 0 velocity
	mypub_object.publish(mypub_msg); // publish to the twist to the topic
	return 0;
}
