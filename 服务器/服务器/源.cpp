#define  _WINSOCK_DEPRECATED_NO_WARNINGS  
#include <winsock2.h>   
#include<iostream>  
#include<string>  
#include<thread>  
#include<fstream>
#include<sstream>

#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll    

#define BUF_SIZE 10086  
//直接用std好像会出现bug  
using std::cout;
using std::endl;
using std::thread;
using std::string;
using std::stringstream;			//字符串流，方便处理字符串
using std::ifstream;				//文件流，进行文件操作

									//因为socket实际上是int变量，故可直接返回int  
int make_socket(int port)
{
	//初始化dll  
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//创建套接字    
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);

	//绑定套接字    
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充    
	sockAddr.sin_family = PF_INET;  //使用IPv4地址    
									//sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //回送IP地址   
	sockAddr.sin_addr.s_addr = INADDR_ANY;          //服务器直接默认本地ip  
	sockAddr.sin_port = htons(port);  //端口    
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	//进入监听状态    
	listen(servSock, 20);
	return servSock;
}

void handle_accept(int socket_feed)
{
	cout << "线程为:" << std::this_thread::get_id() << std::endl;
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
		//检查文件是否存在
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
		//传输文件
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
		t.detach();         //分离线程  
	}

	//终止 DLL 的使用    
	WSACleanup();

	return 0;
}