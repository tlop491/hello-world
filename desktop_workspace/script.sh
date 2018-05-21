#!/bin/bash

rm ~/.ros/location.txt
rm ~/.ros/edge.txt

echo "Array  = [ " >> ~/.ros/location.txt 

echo " " >> ~/Desktop/desktop_workspace/src/robot_driver/srci/pioneerLaser.cpp
catkin_make
[ $? -eq 0 ] && {


echo "Made" 
source devel/setup.bash
echo "set up "

roslaunch src/robot_driver/launch/launchSimulationSLAM.launch



 } || { 
echo "Fail"
}

cat ~/.ros/location.txt
#END
