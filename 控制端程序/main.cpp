#include<iostream>
#include"ClientSocket.h"
using namespace std;

int main()
{
	InitPublicMumber();
	ClientSocket ClientSocket1;
	char ip[16] = "10.10.10.10";//����Ϊ������IP
	ClientSocket1.start(8990, ip);
	return 0;
}