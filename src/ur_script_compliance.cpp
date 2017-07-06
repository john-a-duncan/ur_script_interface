
#include <ur_script_compliance.h>


// Constructor for compliance of one arm
ur_script_compliance::ur_script_compliance (std::string ur_rostopic) :
  node_()
{
  ur_topic_pub_ = node_.advertise<std_msgs::String>(ur_rostopic, 1);
  ros::Duration(0.5).sleep();
}


// Destructor
ur_script_compliance::~ur_script_compliance(void)
{
  end_force_mode_();
}


int ur_script_compliance::enable_force_mode_( std::vector<float> task_frame, std::vector<int> selection_vector, std::vector<int> target_wrench, int type, std::vector<float> limits )
{

  // URScript force_mode command:
  sprintf(cmd_, "force_mode(p[%1.5f, %1.5f, %1.5f, %1.5f, %1.5f, %1.5f]," 
    "[%d, %d, %d, %d, %d, %d],"
    "[%d, %d, %d, %d, %d, %d],"
    "%d,"
    "[%1.5f, %1.5f, %1.5f, %1.5f, %1.5f, %1.5f])\n", 
    task_frame.at(0), task_frame.at(1), task_frame.at(2), task_frame.at(3), task_frame.at(4), task_frame.at(5),  //task frame. Note the leading 'p' for 'pose'
    selection_vector.at(0), selection_vector.at(1), selection_vector.at(2), selection_vector.at(3), selection_vector.at(4), selection_vector.at(5),  // selection vector -- does each axis act like compliance or admittance?
    target_wrench.at(0), target_wrench.at(1), target_wrench.at(2), target_wrench.at(3), target_wrench.at(4), target_wrench.at(5),  // target wrench
    type, // type of force_frame transform
    limits.at(0), limits.at(1), limits.at(2), limits.at(3), limits.at(4), limits.at(5)  // Displacement limits
  ); 

  publish_command_();

  return 0;
}


int ur_script_compliance::end_force_mode_()
{
  sprintf(cmd_, "end_force_mode()\n");
  publish_command_();

  return 0;
}


// Angles in rad
int ur_script_compliance::move_to_joints_(std::vector<float> joints)
{
	sprintf(cmd_, "movej([%1.5f, %1.5f, %1.5f, %1.5f, %1.5f, %1.5f], a=0.1, v=0.1)\n", joints.at(0), joints.at(1), joints.at(2), joints.at(3), joints.at(4), joints.at(5) );
    publish_command_();

	return 0;
}


// This speed is in the UR "Base" frame.
// xyz vector component of 1 => 1 cm/s in that direction
int ur_script_compliance::linear_speed_(std::vector<float> speed)
{
  sprintf(cmd_, "speedl([%1.5f, %1.5f, %1.5f, %1.5f, %1.5f, %1.5f], a=0.1, v=0.01)\n", speed.at(0), speed.at(1), speed.at(2), speed.at(3), speed.at(4), speed.at(5) );

  while( ros::ok() )
  {
    publish_command_();
    ros::Duration( 10*control_period_ ).sleep();
  }

  return 0;
}


int ur_script_compliance::move_to_pose_(std::vector<float> pose)
{
  sprintf(cmd_, "movel(p[%1.5f, %1.5f, %1.5f, %1.5f, %1.5f, %1.5f], a=0.1, v=0.01)\n", pose.at(0), pose.at(1), pose.at(2), pose.at(3), pose.at(4), pose.at(5) );
  publish_command_();

  return 0;
}


int ur_script_compliance::publish_command_()
{
  ur_script_string_.data = cmd_;  // Convert from char array to ROS string
  //ROS_INFO_STREAM( ur_script_string_.data );
  ur_topic_pub_.publish( ur_script_string_ );
  ros::Duration(0.1).sleep();

  return 0;
}


// Set a digital output. Useful for testing the connection.
// Check the pendant's IO monitor to see if it worked.
int ur_script_compliance::set_digital_output_()
{
    sprintf(cmd_, "set_digital_out(2,True)\n");
    publish_command_();

    return 0;
}