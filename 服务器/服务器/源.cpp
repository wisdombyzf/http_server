#define  _WINSOCK_DEPRECATED_NO_WARNINGS  
#include <winsock2.h>   
#include<iostream>  
#include<string>  
#include<thread>  
#include<fstream>
#include<sstream>

#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll    

#define BUF_SIZE 10086  
//ֱ����std��������bug  
using std::cout;
using std::endl;
using std::thread;
using std::string;
using std::stringstream;			//�ַ����������㴦���ַ���
using std::ifstream;				//�ļ����������ļ�����

									//��Ϊsocketʵ������int�������ʿ�ֱ�ӷ���int  
int make_socket(int port)
{
	//��ʼ��dll  
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//�����׽���    
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);

	//���׽���    
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //ÿ���ֽڶ���0���    
	sockAddr.sin_family = PF_INET;  //ʹ��IPv4��ַ    
									//sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //����IP��ַ   
	sockAddr.sin_addr.s_addr = INADDR_ANY;          //������ֱ��Ĭ�ϱ���ip  
	sockAddr.sin_port = htons(port);  //�˿�    
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	//�������״̬    
	listen(servSock, 20);
	return servSock;
}

void handle_accept(int socket_feed)
{
	cout << "�߳�Ϊ:" << std::this_thread::get_id() << std::endl;
	char buf[1024] = { '\0' };
	string cmd;
	string filename;
	recv(socket_feed, buf, sizeof(buf), 0);
	stringstream sstream;//2
	cout << buf;
	sstream << buf;
	sstream >> cmd;
	sstream >> filename;
	cout << cmd << " " << filename << endl;
	//string head = "HTTP/1.0 200 OK\r\nContent - type:text/html\r\n\r\n";
	//send(socket_fd, head.c_str(), strlen(head.c_str()), 0);
	//send(socket_fd, buf, strlen(buf), 0);
	if (cmd == "GET")
	{
		ifstream file;
		filename = filename.substr(1, filename.length() - 1);
		file.open(filename, ifstream::binary);
		string head = "HTTP/1.0 200 OK\r\nContent - type:text/plain\r\n\r\n";
		//����ļ��Ƿ����
		if (!file)
		{
			cout << "fail" << endl;
			closesocket(socket_feed);
			return;
		}
		if (filename.find(".html") != string::npos || filename.find(".htm") != string::npos)
		{
			head = "HTTP/1.0 200 OK\r\nContent - type:text/html\r\n\r\n";
		}
		if (filename.find(".png") != string::npos)
		{
			head = "HTTP/1.0 200 OK\r\nContent - type:image/png\r\n\r\n";
		}
		if (filename.find(".jpg") != string::npos)
		{
			head = "HTTP/1.0 200 OK\r\nContent - type:image/jpg\r\n\r\n";
		}
		send(socket_feed, head.c_str(), strlen(head.c_str()), 0);
		//�����ļ�
		while (!file.eof())
		{
			char buf[1024] = { '\0' };
			memset(buf, 0, sizeof(buf));
			file.read(buf, sizeof(buf) - 1);
			int n = file.gcount();
			send(socket_feed, buf, n, 0);
		}
		file.close();
	}
	closesocket(socket_feed);
}

int main() {
	int servSock = make_socket(10086);
	while (true)
	{
		int socket_feed = accept(servSock, nullptr, nullptr);
		thread t(handle_accept, socket_feed);
		t.detach();         //�����߳�  
	}

	//��ֹ DLL ��ʹ��    
	WSACleanup();

	return 0;
}