#!/bin/bash



catkin_make
[ $? -eq 0 ] && {


echo "Made" 
source devel/setup.bash
echo "set up "

roslaunch src/robot_driver/launch/launchSimulationSLAM.launch



 } || { 
echo "Fail"
}

#END
