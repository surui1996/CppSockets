#include "WPILib.h"


//Connection Includes
//#include <sys/types.h>

#include <sockLib.h> // socket(), bind(), accept(), read(), write()	
#include <stdio.h> // printf
#include <netinet/in.h> //sockaddr_in struct
#include <string.h> //memset + bzero
#include <unistd.h> //read() and write()
static bool start;
static bool joystickPressed;
static bool detectedObject;
int SocketServer(...)
{
	int n;
	int sockfd, cli_len, clientfd;
	char buffer[1024];
	char msg[6];
	strcpy(msg, "start");
	struct sockaddr_in serv_addr, cli_addr;
	static const int PORT_NUM = 4590;
	//Socket init
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	//reset + address init
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT_NUM);
	
	bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	listen(sockfd, 5);
	cli_len = sizeof(cli_addr);
	clientfd = accept(sockfd, (struct sockaddr *) &cli_addr, (int*)&cli_len);

	while(1)	
	{
		n = read(clientfd, buffer, sizeof(buffer));
		if(n <= 0)
		{
			printf("omg where is the connection #swag");
			break;
		}
		if(buffer[0] == '0')
			start = true;
		else if(buffer[0] == '4')
		{
			detectedObject = true;
		}
		
		
		write(clientfd, buffer, n);
	}
	return 0;
}

#define SHOOTER_MOTOR1_PORT 5
#define SHOOTER_MOTOR2_PORT 6
#define CAMERA_LED_PORT 2

class RobotDemo : public SimpleRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
  	int (*f)(...);
	Task task;
	Jaguar motor1, motor2;
	Relay cameraLed;
public:
	RobotDemo(void):
		myRobot(1, 2, 3, 4),	// these must be initialized in the same order
		stick(1),
		f(&SocketServer),
		task("Server", f),
		motor1(SHOOTER_MOTOR1_PORT),
		motor2(SHOOTER_MOTOR2_PORT),
		cameraLed(CAMERA_LED_PORT, Relay::kForwardOnly)
		
	{
		myRobot.SetExpiration(0.1);
		start = false;
		joystickPressed = false;
		detectedObject = false;
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		myRobot.SetSafetyEnabled(false);
		myRobot.Drive(-0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		myRobot.SetSafetyEnabled(false);
		task.Start();
		cameraLed.Set(Relay::kForward);
		while (IsOperatorControl())
		{	
			if(start)
				myRobot.ArcadeDrive(stick);
			if(stick.GetRawButton(1))
				joystickPressed = true;
			if(detectedObject)
				motor1.Set(0.2f);
				motor2.Set(0.2f);
			Wait(0.005);

		}
	}
	
	void Disabled()
	{
		cameraLed.Set(Relay::kOff);
	}
	



	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

