#pragma once
#include<string>
class ClientSocket
{
public:
	ClientSocket();
	~ClientSocket();

	int start(int port, char* ip);
};
/*������*/
class Input
{
public:
	Input();
	~Input();

	static int start();
};


/*����ָ��ĵڶ�����*/
void GetSecondOrder(std::string orders, std::string &secondOrder);
/*��ʼ����������*/
void InitPublicMumber();