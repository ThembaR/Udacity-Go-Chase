#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    if (!client.call(srv))
	ROS_ERROR("Failed to call service /ball_chaser/command_robot "); 
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    bool ball_found = false;
    int location  = -1; 
    ROS_INFO("Process Image callback function has been called");
 
    //Loop through each pixel in the image and check if there's a bright white one
    for (int i=0; i<img.height*img.step; i++)
     {
	
    	if ((img.data[i]== 255) && (img.data [i+1]== 255) &&(img.data[i+2])== 255)
          {
	    	ROS_INFO("The white ball has been found.");
		ball_found = true;
            	break;
	      }
 	    

		// Then, identify if this pixel falls in the left, mid, or right side of the image 
		float left_location= img.step/3;
		float right_location=2*img.step/3;
		//
		location =i%img.step;
		// Depending on the white ball position, call the drive_bot function and pass velocities to it
		if (ball_found==true) {
 			//ROS_INFO( "The rotot can see the white ball.");
			if (location < left_location)
				drive_robot(0.0,1.0);
			else if (location < right_location)
				drive_robot(0.0,-1.0);
			else //move forward if the white ball is neither left nor right
				drive_robot(1.0,0.0);

			  
		   }

		else // Request a stop when there's no white ball seen by the camera
		 drive_robot( 0.0, 0.0);
    }
       
   
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}

