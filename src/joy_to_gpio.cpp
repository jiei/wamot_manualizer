#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <wiringPi.h>

#define LOGICOOL_WIRED
//#define LOGICOOL_WIRELESS

#define GPIO16 16
#define GPIO20 20
#define GPIO21 21

#define DEBUG

/** joy_status
 *  0:stop
 *  1:forward
 *  2:backward
 *  3:right turn
 *  4:left turn
 */
int joy_status=0;


void JoyCallBack(const sensor_msgs::Joy &msg) {
#ifdef LOGICOOL_WIRED
	if(msg.axes[1]>0.5) joy_status=1;
	else if(msg.axes[1]<-0.5) joy_status=2;
	else if(msg.buttons[5]) joy_status=3;
	else if(msg.buttons[4]) joy_status=4;
	else joy_status=0;

	#ifdef DEBUG
		//ROS_INFO("joy_status = %d",joy_status);
	#endif
#endif

  ROS_INFO_STREAM_ONCE("HAVE JOY !!");
}

int main(int argc, char **argv) {
  ros::init(argc, argv, "joy_to_velocity");
  ros::NodeHandle n;
  ros::NodeHandle local_nh("~");

	char *sub_topic_name;
	//ROS_INFO("argc = %d",argc);
	if(argc==2) sub_topic_name=argv[1]; 
	else sub_topic_name="/joy";	//default

  ros::Subscriber joy_sub = n.subscribe(sub_topic_name, 1, JoyCallBack);
	ros::Rate loop_rate(10);

	if(wiringPiSetupGpio() == -1) {
  	return 1;
  }

	pinMode(GPIO16,OUTPUT);
	pinMode(GPIO20,OUTPUT);
	pinMode(GPIO21,OUTPUT);

	while (ros::ok()) {
    ros::spinOnce();
		switch(joy_status){
		case 0:
			digitalWrite(GPIO16,0);
			digitalWrite(GPIO20,0);
			digitalWrite(GPIO21,1);
			ROS_INFO("STOP");
			break;
		case 1:
			digitalWrite(GPIO16,1);
			digitalWrite(GPIO20,0);
			digitalWrite(GPIO21,0);
			ROS_INFO("FORWARD");
			break;
		case 2:
			digitalWrite(GPIO16,0);
			digitalWrite(GPIO20,1);
			digitalWrite(GPIO21,0);
			ROS_INFO("BACKWARD");
			break;
		case 3:
			digitalWrite(GPIO16,1);
			digitalWrite(GPIO20,1);
			digitalWrite(GPIO21,0);
			ROS_INFO("RIGHT TURN");
			break;
		case 4:
			digitalWrite(GPIO16,0);
			digitalWrite(GPIO20,0);
			digitalWrite(GPIO21,0);
			ROS_INFO("LEFT_TURN");
			break;
		}
    loop_rate.sleep();
  }

	return 0;
}
