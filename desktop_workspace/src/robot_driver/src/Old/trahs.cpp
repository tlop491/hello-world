#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"
//Odometry required header files
#include "nav_msgs/Odometry.h"
#include <fstream>
#include "math.h"
#include <ros/rosconsole.h>
geometry_msgs::Twist velocityCommand; 



using namespace std;

//Store range values for min/max computation
float locationValues[512];






/*
The scan subscriber call back function
To understand the sensor_msgs::LaserScan object look at
http://docs.ros.org/api/sensor_msgs/html/msg/LaserScan.html
*/
void laserScanCallback(const sensor_msgs::LaserScan::ConstPtr& laserScanData)
{
std::ofstream myfile;
myfile.open ("location.txt");
for(int tmp = 0; tmp < 511; tmp++){
        locationValues[tmp] = 0.0f;
}



// Compute the number of data points
// max angle and min angle of the laser scanner divide by the increment angle of each data point
float rangeDataNum = 1 + (laserScanData->angle_max - laserScanData->angle_min)  / (laserScanData->angle_increment);

 
//move forward
velocityCommand.linear.x = 0.0;
velocityCommand.angular.z = 0.0;
//	ROS_INFO("Angle Max : [%f]" , laserScanData->angle_max );
//	ROS_INFO("Angle Min : [%f]" , laserScanData->angle_min );
        ROS_INFO("Angle Max - Degrees : [%f]" , (180/3.1415)*laserScanData->angle_max );
        ROS_INFO("Angle Min- Degreees : [%f]" , (180/3.1415)*laserScanData->angle_min );

	ROS_INFO("Range:[%f]" , rangeDataNum );
	ROS_INFO("Angle Inc : [%f]" ,(180/3.1415)*laserScanData->angle_increment);


	for(int i = 0; i < 512; i++){

	//Store location in array 
	locationValues[i]=laserScanData->ranges[i];
		
	if((i%50)==0) {
	ROS_INFO("range[%d] : [%f]" , i , laserScanData->ranges[i]);
	//ROS_INFO("intensities[%d} :[%f]", i , laserScanData->intensities[i]);
		}
	}
	
	myfile << "Array = [ ";	
	//Print location array 
	int small = 0; 
	for(int j=0; j<512; j++){
	//Loop through arrya 


	//Find Smallesyt = centre 
	//Smallest = locationValue[small]
 
	if(locationValues[small] < locationValues[j]) {
		small = small; 
	} else {
	small = j;
	} 
	
	//ROS_INFO("\n\n\n\n\n\n\nArray[%d] =[%f]" , j , locationValues[j] );
 

	 myfile <<  ( "[%f] ", locationValues[j]) << ( ", " ) ;
	}
	//Loop out and find edge
	int edgeA = 0; 
	int edgeB = 0; 
	int edgeC = 0; 
	float thres;
	float leftSide = 0.0; 
	float rightSide = 0.0;
	for(int k=0; k<256; k++){
	//right edge
	if((locationValues[small+k]-locationValues[small+(k-1)]) > thres ) {
	edgeA = (small+(k-1)); 
	leftSide = locationValues[small+(k-1)];
	//locationValue[small+k];
	}
	//Left Edge
	if((locationValues[small-(k+1)]-locationValues[small-(k)]) > thres)	{
	edgeB = (small-(k-2));
	rightSide = locationValues[small - k];
	}
	
	}
	
	//Loop from edge to centre and check if gradient changes therefore circle=true; 
	//Calculate 
	float leftCord = 0.0; 
	float  centreCord = 0.0; 
	float  rightCord = 0.0; 
	//leftCord = locationValues[edgeA];
	leftCord = leftSide; 
	
	centreCord = locationValues[small];
	//rightCord = locationValues[edgeB]; 
	rightCord = rightSide; 	

	float angle1 = 0; 
	angle1 = 0.35226 * (edgeA - small);
	float triAns = 0.0; 	
	//a^2 = b^2 + c^2 -2bcCosA
	triAns = sqrt( (leftCord*leftCord) + (centreCord*centreCord) -( (2*leftCord*centreCord) * cos(angle1)));
	
        float angle2 = 0;  
        angle1 = 0.35226 * (small - edgeB); 
        float triAns1 = 0.0;      
        //a^2 = b^2 + c^2 -2bcCosA 
        triAns1 = sqrt( (rightCord*rightCord) + (centreCord*centreCord) -( (2*rightCord*centreCord) * cos(angle2))); 


	ROS_INFO("Width [%f]", triAns*1.2);
	ROS_INFO("Length [%f]", triAns1*1.2); 
	myfile << "LCord: " << leftCord << "CCord: " << centreCord << "RCord :" << rightCord << "triAns : " << triAns << std::endl;
	ROS_INFO("Leftside [%f] RIght side [%f]", leftSide, rightSide);
	myfile << " ]"; 
	myfile.close();

	

}



/*Function that finds Maxima*/
/*
The odometer call back function
http://docs.ros.org/api/nav_msgs/html/msg/Odometry.html
*/
void chatterCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
	//Print the odom data structure
	//Message sequence
	ROS_INFO("Seq: [%d]", msg->header.seq);
	//Position
	ROS_INFO("Position-> x: [%f], y: [%f], z: [%f]", msg->pose.pose.position.x,msg->pose.pose.position.y, msg->pose.pose.position.z);
	//Orientation
	ROS_INFO("Orientation-> x: [%f], y: [%f], z: [%f], w: [%f]", msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
	//Velocity
	ROS_INFO("Vel-> Linear: [%f], Angular: [%f]", msg->twist.twist.linear.x,msg->twist.twist.angular.z);
		
}

int main (int argc, char **argv)
{	
	ros::init(argc, argv, "pioneer_laser_node");	// command line ROS arguments
	ros::NodeHandle my_handle;	// ROS comms access point

	ros::Publisher vel_pub_object = my_handle.advertise<geometry_msgs::Twist>("/RosAria/cmd_vel",1);

	/*
	subscribe to the scan topic and define a callback function to process the data
	the call back function is called each time a new data is received from the topic
	*/
	ros::Subscriber laser_sub_object = my_handle.subscribe("/scan", 1, laserScanCallback);

	
	ros::Rate loop_rate(10);// loop 10 Hz

	for (int j = 0; j < 3 ; ++j)
//	while(ros::ok()) // publish the velocity set in the call back
 
	{
		ros::spinOnce();
		loop_rate.sleep();

		// publish to the twist to the topic
		vel_pub_object.publish(velocityCommand);
	}

	
	return 0;
}
