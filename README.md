# TCP_Vision
TCP Server and Client for Vision Tuning System, TCP/IP socket. 
TCPServer is running on Linux/Windows QT 5+ Platform, it can receive live-video(image) from client and send tuning parameters.
TCPClient is running on Linux machine (I use it in Raspberry Pi and ODROID board), it can send live-video(image) to server and receive tunning parameters.
For TCPClient, I use OpenCV to capture image(IplImage) and do image processing.
