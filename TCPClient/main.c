#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cv.h"
#include "highgui.h"

#define N 3
#define SERVER_IP "172.17.168.45"
#define SERVER_PORT 6000

typedef struct TAG_MSG_PACK
{
	int nImageType;
	int nLen;
	char data[];
}MSG_PACK;

int RecvedData[6] = {0,0,0,0,0,0};
int socket_fd = 0;

IplImage* Threshold(IplImage* imgThresh,IplImage* imgHSV)
{
	cvInRangeS(imgHSV, cvScalar(RecvedData[0],RecvedData[2],RecvedData[4],0), cvScalar(RecvedData[1],RecvedData[3],RecvedData[5],0), imgThresh);
	return imgThresh;
}

void *RcvData(void *fd)
{
	int byte, nType;
	unsigned char char_recv[100];
	int sock = *((int*)fd);
	while(1)
	{
		recv(sock, char_recv,sizeof(int),0);
		nType = (int)char_recv[0];
		printf("Recv data 1 : %d \n",nType);

		recv(sock, char_recv,sizeof(int),0);
		printf("Recv data 2 : %d \n",char_recv[0]);

		if(nType  == 0)
		{
			if((byte = recv(sock, char_recv, 100, 0)) == -1)
			{
				printf("recv error \r\n");
			}
			int i;
			for(i=0;i<6;i++)
			{
				RecvedData[i] = char_recv[i];
			}
			printf("recv data: %d %d %d %d %d %d\r\n", char_recv[0],char_recv[1],char_recv[2],char_recv[3],char_recv[4],char_recv[5]);
			}
		else if(nType  == 1)
        {
			printf("Recv Save Command\n");
		}
		else if(nType  == 2)
		{
			printf("Recv Load Command\n");
			char buf[6];
			buf[0] = 10;
			buf[1] = 80;
			buf[2] = 50;
			buf[3] = 90;
			buf[4] = 40;
			buf[5] = 90;
            int nRes = SendImage(socket_fd, buf,6, 3);
            printf("Send Data : %d\n",nRes);
		}
	}
}

int  SendImage(int sock, char *buf, int nLen, int imgeType)
{
	int nRes = 0;
	MSG_PACK *msgPack;
	char *pSZMsgPack = (char*)malloc(sizeof(MSG_PACK)+nLen);
	msgPack = (MSG_PACK *)pSZMsgPack;
	msgPack->nImageType = htonl(imgeType);
	msgPack->nLen = htonl(nLen);
	memcpy(pSZMsgPack+sizeof(MSG_PACK), buf, nLen);
	nRes = write(sock, pSZMsgPack, sizeof(MSG_PACK)+nLen);
	printf("Write imageBuf: %d\n",nRes);
	free(pSZMsgPack);
	if(nRes == nLen+sizeof(MSG_PACK))
	{
		return nLen+sizeof(MSG_PACK);
	}
	return 0;
}

int InitSocket()
{
	//Create socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	//Server information
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT); 
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	memset(&(server_addr.sin_zero), 0, 8);
	//Connect to server
	int res = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	printf("conncet res is %d\r\n", res);
	//Create a thread to receive Server message
	pthread_t thread;
	if(pthread_create(&thread, NULL, RcvData, &socket_fd)!=0)
	{
		printf("Thread create error \r\n");
	}
}

int main()
{
	InitSocket();

	time_t start,end;

	CvMemStorage *storage = cvCreateMemStorage(0);    
	CvSeq *contours[N], *tmp_cont, *contour;
	IplImage *frame, *imgHSV, *imgThresh;  
	CvCapture *capture; 
	int area, tmp_area, i, j, k,m;
			  
	imgHSV = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U,3);     
	imgThresh = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U,1); 

	capture = cvCaptureFromCAM(0);        
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,320);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,240);

	if(!capture){
		printf("Capture failure\n");
		return -1;
	}

	time(&start);
	int counter=0;
          
    while(1)
	{     	      
		frame = cvQueryFrame(capture);         
		if(!frame) break;  
						
		cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);  
		imgThresh = Threshold(imgThresh,imgHSV);

		SendImage(socket_fd, imgThresh->imageData, imgThresh->imageSize, 0);//Send grey image, type 0
		
//		cvShowImage("Result", imgThresh);
		
		cvFindContours(imgThresh, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0)); 
		 
		int cnt = 0; 
		int maxArea[N]={0}; 
		
		for(;contour != 0;contour = contour->h_next)   
		{    
		  
		  area = fabs(cvContourArea(contour, CV_WHOLE_SEQ,1 ));
		  
		  if(area<100 || area>50000)
		  {
			cvSeqRemove(contour,0);
			continue;
		  }          
		 
		  cnt++;
		  
		  for(i = N-1; i >= 0; --i)
		  {
			if(area > maxArea[i])
			{
			  maxArea[i] = area;
			  contours[i] = contour;
			  for(m = (i-1); m >= 0; --m)
			   {
				 if(maxArea[m] < maxArea[m+1])
				  {
					tmp_area = maxArea[m+1];
					tmp_cont = contours[m+1];
					maxArea[m+1] = maxArea[m];
					contours[m+1] = contours[m];
					maxArea[m] = tmp_area;
					contours[m] = tmp_cont;
				  }
				}
			  break;
			}
		  }          
		}                                          

		if(cnt != 0)
		{	
			CvRect rect = ((CvContour*)contours[0])->rect;
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 255, 0), 2, 8, 0);
		}	      
		
		time(&end);
		++counter;
		double sec=difftime(end,start);
		double fps=counter/sec;
		printf("FPS = %.2f\n\n",fps);                          
        
        cvConvertImage(frame,frame,CV_CVTIMG_SWAP_RB);   //Swap data of R and B channel for QImgae showing
		SendImage(socket_fd, frame->imageData, frame->imageSize, 1);//Send original RGB image,type 1
		
		cvShowImage("Original", frame);
		
		if ( (cvWaitKey(10) & 255) == 27 ) break; 
      }
      
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThresh); 
	cvReleaseMemStorage(&storage); 
	cvDestroyAllWindows() ;
	cvReleaseCapture(&capture);     

	return 0;
}
