#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"
#include <string>
//gmapping required header files
#include "nav_msgs/OccupancyGrid.h"
#include "tf/transform_listener.h"
//Odometry required header files
#include "nav_msgs/Odometry.h"
#include <fstream>
#include "math.h"
#include <ros/console.h>

using namespace std;

//Store range values for min/max computation
float locationValues[512];
int edgeLocations[512];
int edges[512];
int flag = 1; 
geometry_msgs::Twist velocityCommand; 
/*
The scan subscriber call back function
To understand the sensor_msgs::LaserScan object look at
http://docs.ros.org/api/sensor_msgs/html/msg/LaserScan.html
*/
void laserScanCallback(const sensor_msgs::LaserScan::ConstPtr& laserScanData)
{
std::ofstream myfile;
std::ofstream myfile1;
myfile.open ("location.txt");
myfile1.open("edge.txt");
for(int tmp = 0; tmp < 511; tmp++){
        locationValues[tmp] = 0.0f;
}



// Compute the number of data points
// max angle and min angle of the laser scanner divide by the increment angle of each data point
float rangeDataNum = 1 + (laserScanData->angle_max - laserScanData->angle_min)  / (laserScanData->angle_increment);


 
//move forward
velocityCommand.linear.x = 0.5;
velocityCommand.angular.z = 0.0;
	//ROS_INFO("Angle Max : [%f]" , laserScanData->angle_max );
	//ROS_INFO("Angle Min : [%f]" , laserScanData->angle_min );
        //ROS_INFO("Angle Max - Degrees : [%f]" , (180/3.1415)*laserScanData->angle_max );
        //ROS_INFO("Angle Min- Degreees : [%f]" , (180/3.1415)*laserScanData->angle_min );

	//ROS_INFO("Range:[%f]" , rangeDataNum );
	//ROS_INFO("Angle Inc : [%f]" ,(180/3.1415)*laserScanData->angle_increment);
	

	for(int i = 0; i < 512; i++){
	
	//Spin 360
	if(flag == 1)
	{
	velocityCommand.linear.z = 0; 
	velocityCommand.angular.z = 0.1;
	}
	//Store location in array 
	locationValues[i]=laserScanData->ranges[i];
	if( laserScanData->ranges[i] < 0.5 )  // If there is an obiect within 0.5m

	{

		velocityCommand.linear.x = 0;   // stop forward movement

		velocityCommand.angular.z = 0.1; // turn left
		flag = 0;
		break;

	}
	}


	int small = 0; 
	int right =0; 
	int left = 0; 
	float tmp1 =0.0; 
	float tmp2 =0.0;
	float edgeTresh = 3.0;
	for(int j=0; j<510; j++){
	if(locationValues[small] < locationValues[j]) {
		small = small; 
	} else {
	small = j;
	} 
	

	//Loop through array 
	tmp1 =((-1*locationValues[j]) + 1*locationValues[j+2] );
	tmp2 =((-1*locationValues[j+1]) +( 1*locationValues[j+3]) );

	if (abs(tmp1-tmp2) >  edgeTresh){
		//Pring 1 
		myfile1 << " 1, ";
		edgeLocations[j]=1; 
	} else {
		//print0 
		myfile1 << " 0, ";
		edgeLocations[j]=0;
	}
	}



	
	for(int k = 0; k < 512; k++){

	//After every 101 is an edge  

	//If k-3 = 1 k-2=0 and k-1 = 1 then k is edge 
	if(edgeLocations[k-3] == 1 && edgeLocations[k-2] == 0 && edgeLocations[k-1] == 0){
	edges[k-2] = 1;
		if( k >= small){
			left = k-3;
		} else {
			right = k-2;
		} 
	} else{
	edges[k-2] = 8;
	}

	//Edges at k-2 and k-3
	//ROS_INFO("Edges :[%d][%f][%d][%d][%f][%f][%f][%d][%d]   ",k, locationValues[k], edgeLocations[k], edges[k], locationValues[small], locationValues[left], locationValues[right], left, right);

	}

	//x-y cordinate right 
	float anglr = 0.0061359*right;
	float xcorR = locationValues[right]*cos(anglr);
	float ycorR = locationValues[right]*sin(anglr);
	//ROS_INFO("Right X Cor: [%f], Y Cor: [%f]", xcorR, ycorR);

	//x-y cordinate left 
	float angll = 0.0061359*left;
	float xcorL = locationValues[left]*cos(angll);
	float ycorL = locationValues[left]*sin(angll);
	//ROS_INFO("Left X Cor: [%f], Y Cor: [%f]", xcorL, ycorL);


	//x-y cordinate small 
	float anglm = 0.0061359*small;
	float xcorM = locationValues[small]*cos(anglm);
		float ycorM = locationValues[small]*sin(anglm);
	//ROS_INFO("Small X Cor: [%f], Y Cor: [%f]", xcorM, ycorM);

	
	//MR Line 
	float grad = ((ycorR-ycorM)/(xcorR-xcorM));	
	
	//Checking if 3 points are on a circle
	int testPnt = int( 0.5*(small+right));
	//ROS_INFO("test x: [%f], [%f]", locationValues[testPnt]*sin(0.0061359*testPnt), locationValues[testPnt]*cos(0.0061359*testPnt));	
	//ROS_INFO("if : [%f]", ((locationValues[testPnt])*sin(0.0061359*testPnt)) -( (grad*(locationValues[testPnt])*cos(0.0061359*testPnt) )+ locationValues[small]*sin(anglm)));
	if(abs(((locationValues[testPnt])*sin(0.0061359*testPnt)) -( (grad*(locationValues[testPnt])*cos(0.0061359*testPnt) )+ locationValues[small]*sin(anglm))) >= 0.005 )	{
	//Issa Circle
	float cirA = (0.0061359*(small-right));
	float cirAdj = locationValues[right];
	float cirRad = cirAdj * tan(cirA);
	//ROS_INFO("Circle Radius: [%f]:", cirRad);
	};	

	//Find length assuming its has right angles 
	//right side	
	float angle1 =  0.0061359*(small-right);
	float sqA = locationValues[small]*locationValues[small];
	float sqB = locationValues[right]*locationValues[right];
	float twoobc = 2*locationValues[small]*locationValues[right] * cos(angle1);
	float calcTri = sqrt(sqA + sqB - twoobc);
	
	//Left Side
	float angle2 =  0.0061359*(left-small);
	float sqC = locationValues[small]*locationValues[small];
	float sqD = locationValues[left]*locationValues[left];
	float twobc = 2*locationValues[small]*locationValues[left] * cos(angle2);
	float calcTri2 = sqrt(sqC + sqD - twobc);
	//Check object length calculations
	//ROS_INFO("Side Length:[%f][%f][%f][%f][%f] ", sqA, sqB, twoobc ,angle1, calcTri2);
	myfile.close();
	myfile1.close(); 

}

int grid_x; 
int grid_y;
float map_o_x = 0;
float map_o_y = 0;
float map_r = 1;

/*
The OccupancyGrid is a 2-D grid map, in which 
each cell represents the probability of an object.
http://docs.ros.org/api/nav_msgs/html/msg/OccupancyGrid.html
*/
void mapCallback(const nav_msgs::OccupancyGrid::ConstPtr &msg) { 
	//get the origin of the map frame
	map_o_x = msg->info.origin.position.x;
	map_o_y = msg->info.origin.position.y;
	//get the resolution of each cell in the OccupancyGrid
	map_r = msg->info.resolution;
	
	std::string line;
	std::string output;
	
	//the occupancy grid is a 1D array representation of a 2-D grid map
	//compute the robot position in the 1D array
	int r_index = grid_x + grid_y * msg->info.width;
	
	int printEnable = 0;
	//go through the entire grid
	for (int i = 0; i < msg->info.width*msg->info.height; i++) {
		
		if (r_index == i) {
			//if the cell is the cell of the robot
			//print R
			line += "R";
			printEnable = 1;
		}else{
			//if the cell is unknown (-1) or empty (0) 
			//print a space
			if (msg->data[i] == -1) {
				line += " ";
				printEnable = 1; 
			}else if (msg->data[i] == 0) {
				line += "0";
				printEnable = 1;
			}else{
				//if the cell may contain an object
				//print *
				line += "*";
				printEnable = 1;
			}
		}
		
		//at the start of each row
		if (i % msg->info.width == 0) {
			//add to the output if there is something interesting on this row
			if (printEnable == 1) {
				output = line + "\n" + output;
			}
			line = "";
			printEnable = 0;
		}
	}
	//print output
	printf("%s\n",output.c_str());
	
	//other things about the map that you can print
	/* 
	ROS_INFO("");
	ROS_INFO("X: [%d]", grid_x);
	ROS_INFO("Y: [%d]", grid_y);
	ROS_INFO("width: [%d]", msg->info.width);
	ROS_INFO("height: [%d]", msg->info.height);
	ROS_INFO("resolution: [%d]", msg->info.resolution);
	ROS_INFO("Map Orientation-> x: [%f], y: [%f], z: [%f], w: [%f]", msg->info.origin.orientation.x, msg->info.origin.orientation.y, msg->info.origin.orientation.z, msg->info.origin.orientation.w);
	*/
}

/*
The odometer call back function
http://docs.ros.org/api/nav_msgs/html/msg/Odometry.html
*/
void chatterCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
	//Print the odom data structure
	//Message sequence
	//ROS_INFO("Seq: [%d]", msg->header.seq);
	//Position
	//ROS_INFO("Position-> x: [%f], y: [%f], z: [%f]", msg->pose.pose.position.x,msg->pose.pose.position.y, msg->pose.pose.position.z);
	//Orientation
	//ROS_INFO("Orientation-> x: [%f], y: [%f], z: [%f], w: [%f]", msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
	//Velocity
	//ROS_INFO("Vel-> Linear: [%f], Angular: [%f]", msg->twist.twist.linear.x,msg->twist.twist.angular.z);
		
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

	/*
	subscribe to the map created by gmapping
	*/
	ros::Subscriber sub_map = my_handle.subscribe("map", 1, mapCallback);
	ros::Rate loop_rate(10);// loop 10 Hz

	tf::TransformListener listener;
	//for(int j=0; j<3; ++j)
	while(ros::ok()) // publish the velocity set in the call back
	{
		ros::spinOnce();
		loop_rate.sleep();
		
		tf::StampedTransform transform;
		try{
			//transform the coordinate frame of the robot to that of the map
			//(x,y) index of the 2D Grid
			listener.lookupTransform("map", "base_link",ros::Time(0), transform);
			grid_x = (unsigned int)((transform.getOrigin().x() - map_o_x) / map_r);
			grid_y = (unsigned int)((transform.getOrigin().y() - map_o_y) / map_r);
		}
		catch (tf::TransformException ex){
			ROS_ERROR("%s\n",ex.what());
			ros::Duration(1.0).sleep();
		}
		
		// publish to the twist to the topic
		vel_pub_object.publish(velocityCommand);
	}

	return 0;
}
