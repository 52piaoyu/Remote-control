#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.h"
#include"winsock2.h"
#pragma comment(lib,"ws2_32.lib")
#include<iostream>
#include"PublicList.h"
using namespace std;

/*
		���ݰ��ṹ
		1.����ͷ 6652���Ͽ����� 6653�����뽨������ 6654��ͬ������
		2.���ݰ�
		2.1����
		2.2�ļ�ͷ
		2.3�ļ�
*/
typedef struct transBag {
	int transBagHead;
	union {
		char num[13];
		char transBagOrder[1024];
		struct 
		{
			char name[1024];
			long length;
		}transBagFileHead;
		char transBagFileBody[2048];
	}transBagBody;

}transBag;


/*
ThreadforSeverRecv(LPVOID lParameter)
����˽�����Ϣ�õ��̺߳������������NULL
��������ת��
ȷ�����ݰ�����˳�򲻱�
*/
DWORD WINAPI ThreadforSeverRecv(LPVOID lParameter)
{
	
	SOCKET* c = (SOCKET*)(lParameter);
	SOCKET clientSocket = *c;
	SOCKET toSocket = 0;
	/*�ж����û��˻��ǿ��ƶˣ�0���û��ˣ�1�ǿ��ƶ�*/
	int flag = 0;
	while (1) 
	{
		transBag transBagRecv;
		int r = recv(clientSocket, (char*)& transBagRecv, sizeof(transBag), 0);
		if (r > 0)
		{
			switch (transBagRecv.transBagHead)
			{
			//case 1://ָ��
			//	break;
			//case 2://�ļ���Ϣ
			//	break;
			//case 3://�ļ�����
			//	break;
			case 4://�û���½
				/*
				1.���������û���
				*/
				cout << "�µ��û�����룺" << transBagRecv.transBagBody.num << endl;
				UserMsg userMsg;
				for(int i=0;i<13;i++)
					userMsg.num[i] = transBagRecv.transBagBody.num[i];
				userMsg.num[12] = '\0';
				userMsg.socketUser = clientSocket;
				userMsg.socketCtrl = 0;
				for (int i = 0; i < userList.size(); i++)
				{
					if (strcmp(userList[i].num, userMsg.num) == 0)
					{
						userList.erase(userList.begin() + i);
						break;
					}
				}
				userList.push_back(userMsg);
				break;
			case 5://���Ƶ�½
				/*
				1.���������
				*/
				cout << "�µĿ�������룺"<< transBagRecv.transBagBody.num << endl;
				CtrlMsg ctrlMsg;
				for (int i = 0; i < 13; i++)
					ctrlMsg.num[i] = transBagRecv.transBagBody.num[i];
				ctrlMsg.num[12] = '\0';
				ctrlMsg.socketCtrl = clientSocket;
				ctrlMsg.socketUser = 0;
				for (int i = 0; i < ctrlList.size(); i++)
				{
					if (strcmp(ctrlList[i].num, ctrlMsg.num) == 0)
					{
						ctrlList.erase(ctrlList.begin() + i);
						break;
					}
				}
				ctrlList.push_back(ctrlMsg);
				/*
				�����û���
				*/
				
				for (int i = 0; i < userList.size(); i++)
				{
					struct UseableNum
					{
						char num[13];
						bool useable;
					}useableNum;
					for (int m = 0; m < 13; m++)
					{
						useableNum.num[m] = userList[i].num[m];
					}
					if (userList[i].socketCtrl == 0)
						useableNum.useable = true;
					else
						useableNum.useable = false;

					send(clientSocket, (char*)&useableNum, sizeof(UseableNum), 0);
				}
				/*
				���ͽ�����
				*/
				send(clientSocket, "#############", 14, 0);
				flag = 1;
				break;
			case 6654://ͬ������
				for (int i = 0; i < userList.size(); i++)
				{
					if (userList[i].socketUser==clientSocket)
					{
						toSocket = userList[i].socketCtrl;
						break;
					}
				}
				if (toSocket)
				{
					
					transBag transBagSend;
					memset(&transBagSend, 0, sizeof(transBag));
					transBagSend.transBagHead = 6654;
					send(toSocket, (char*)& transBagSend, sizeof(transBag), 0);
				}
				break;
			case 6653:
				/*
				���ƶ˷�������
				1.���ƶ��������������Ϣ
				2.���������Ŀ��ƶˣ������û�socket�����¿�����,�����������û��ˣ�������ƶ�socket�������û���
				3.���������û��˷�����Ϣ
				4.�û��˷���ͬ�����ӻ�����ʧ����Ϣ
				5.�������ӣ�����ƶ˷�����Ϣ
				6.������ʧ�ܣ�����ƶ˷�����Ϣ
				*/
				toSocket = 0;
				for (int i = 0; i < userList.size(); i++)
				{
					if (strcmp(userList[i].num, transBagRecv.transBagBody.num)==0)
					{
						toSocket = userList[i].socketUser;
						break;
					}
				}
				if (toSocket)
				{
					
					for (int i = 0; i < ctrlList.size(); i++)
					{
						if (ctrlList[i].socketCtrl == clientSocket)
						{
							ctrlList[i].socketUser = toSocket;
							break;
						}
					}
					for (int i = 0; i < userList.size(); i++)
					{
						if (userList[i].socketUser == toSocket)
						{
							userList[i].socketCtrl = clientSocket;
							break;
						}
					}
					transBag transBagSend;
					memset(&transBagSend, 0, sizeof(transBag));
					transBagSend.transBagHead = 6653;
					send(toSocket, (char*)& transBagSend, sizeof(transBag), 0);
				}
				else
				{
					/*�û�˲������*/
					transBag transBagSend;
					memset(&transBagSend, 0, sizeof(transBag));
					transBagSend.transBagHead = 6652;
					send(clientSocket, (char*)& transBagSend, sizeof(transBag), 0);
				}
				break;
			case 2:
			{
#define LENGTH_FILE_BODY 2048
				if (toSocket)
				{
					send(toSocket, (char*)& transBagRecv, sizeof(transBag), 0);
				}
				char szBuf[LENGTH_FILE_BODY] = { 0 };
				int len = 0;
				long flen = transBagRecv.transBagBody.transBagFileHead.length;
				while (flen)
				{
					len = recv(clientSocket, szBuf, LENGTH_FILE_BODY, 0);
					if (len > 0)
					{
						if (toSocket)
						{
							send(toSocket, szBuf, len, 0);
							flen -= len;
						}
					}
					else if (len <= 0 && errno != EINTR)
						break;
				}
			}
				break;
			default:
				if (toSocket)
				{
					send(toSocket, (char*)& transBagRecv, sizeof(transBag), 0);
				}
			}
		}
		else if (r <= 0 && errno != EINTR)
		{
			/*
			�û��Ͽ�����
			1.���û���ɾ��
			2.֪ͨ����	���ƶ��жϵ�ǰ����������ڽ��е�����
			3.�˳�ѭ��
			���ƶϿ�����
			1.�ӿ�����ɾ��
			2.֪ͨ�û�	�û��жϵ�ǰ����������ڽ��е�����
			3.�û��������
			4.�˳�ѭ��
			*/

			if (!flag)
			{/*�û���*/
				
				for (int i = 0; i < userList.size(); i++)
				{
					if (userList[i].socketUser == clientSocket)
					{
						cout << "�û����ߣ�" << userList[i].num<< endl;
						if(!toSocket)
						toSocket = userList[i].socketCtrl;
						userList.erase(userList.begin() + i);
						break;
					}
				}

				if (toSocket)
				{
					transBag transBagSend;
					memset(&transBagSend, 0, sizeof(transBag));
					transBagSend.transBagHead = 6652;
					send(toSocket, (char*)& transBagSend, sizeof(transBag), 0);
					
				}

				closesocket(clientSocket);
				return 0;
			}
			else 
			{/*���ƶ�*/
				for (int i = 0; i < ctrlList.size(); i++)
				{
					if (ctrlList[i].socketUser == clientSocket)
					{
						cout << "���ƶ�����:" << ctrlList[i].num << endl;
						if (!toSocket)
							toSocket = ctrlList[i].socketUser;
						ctrlList.erase(ctrlList.begin() + i);
						break;
					}
				}

				if (toSocket)
				{
					transBag transBagSend;
					memset(&transBagSend, 0, sizeof(transBag));
					transBagSend.transBagHead = 6652;
					send(toSocket, (char*)& transBagSend, sizeof(transBag), 0);
					/*for (int i = 0; i < userList.size(); i++)
					{
						if (userList[i].socketUser == toSocket)
						{
							userList[i].socketUser = 0;
							break;
						}
					}*/
				}

				closesocket(clientSocket);
				return 0;
			}
			break;
		}
	}


	/*
	1.�ر�socket
	*/
	closesocket(clientSocket);
	return 0;
}

/*
start(int port),������ʼ���׽��֣�����port�˿�
*/
int SocketServer::start(int port) 
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return ERROR;
	SOCKET severSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (severSocket == INVALID_SOCKET)
		return ERROR;
	if (!port)
		return ERROR;
	SOCKADDR_IN severAddr;
	severAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	severAddr.sin_family = AF_INET;
	severAddr.sin_port = htons(port);

	if (-1 == bind(severSocket, (struct sockaddr*) & severAddr, sizeof(struct sockaddr)))
		return ERROR;

	if (-1 == listen(severSocket, 5))
		return ERROR;

	int len = sizeof(sockaddr);
	long long num = 0;
	while (1)
	{

		SOCKADDR_IN clientAddr;
		SOCKET clientSocket = accept(severSocket, (sockaddr*)& clientAddr, &len);
		/*
		�����߳�->
		1.��������->�ж�������Ϣ->��������
		2.��������->�жϷ���Ŀ��->��������
		3.��������->�ж�����״̬->��������
		�����߳�
		1.һ��ʱ��->����������Ϣ->д���ļ�
		*/
		
		cout << "������  ";
		printf("Accept client IP:[%s]   ", inet_ntoa(clientAddr.sin_addr));
		printf("port:[%d]", clientAddr.sin_port);
		cout << endl;
		HANDLE handle = CreateThread(NULL, 0, ThreadforSeverRecv, &clientSocket, 0, NULL);
		CloseHandle(handle);
	}
}