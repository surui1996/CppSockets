#include "WPILib.h"


//Connection Includes
//#include <sys/types.h>

#include <sockLib.h> // socket(), bind(), accept(), read(), write()	
#include <stdio.h> // printf
#include <netinet/in.h> //sockaddr_in struct
#include <string.h> //memset + bzero
#include <unistd.h> //read() and write()
static bool start;
int SocketServer(...)
{
	int n;
	int sockfd, cli_len, clientfd;
	char buffer[1024];
	char msg[6];
	strcpy(msg, "start");
	struct sockaddr_in serv_addr, cli_addr;
	static const int PORT_NUM = 4590;
	//CTOR
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("socket file descriptor:%d\n", sockfd);
	
	//reset
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT_NUM);

	bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	listen(sockfd, 5);
	cli_len = sizeof(cli_addr);
	clientfd = accept(sockfd, (struct sockaddr *) &cli_addr, (int*)&cli_len);
	//printf("new socket file descriptor:%d\n", sockfd);
	//printf("client address:%d\n", ntohl(cli_addr.sin_addr.s_addr));
	//printf("client port:%d\n", ntohs(cli_addr.sin_port));

	while(1)	
	{
		n = read(clientfd, buffer, sizeof(buffer));
		if(n <= 0)
		{
			printf("omg where is the connection #swag");
			break;
		}
		//buffer[n] = '\0';
		if(buffer[0] == '0')
			start = true;
		//printf("%d\n", n);
		//printf("msg: %s\n", buffer);
		
		
		write(clientfd, buffer, n);
	}
	return 0;
}

class RobotDemo : public SimpleRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick

	int (*f)(...);
	Task task;	
	
public:
	RobotDemo(void):
		myRobot(1, 2, 3, 4),	// these must be initialized in the same order
		stick(1),
		f(&SocketServer),
		task("Server", f)
		
	{
		myRobot.SetExpiration(0.1);
		start = false;
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
		while (IsOperatorControl())
		{	
			if(start)
				myRobot.ArcadeDrive(stick);
			Wait(0.005);

		}
	}



	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

