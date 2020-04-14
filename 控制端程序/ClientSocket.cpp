#include "ClientSocket.h"
#include"GetMAC.h"
#include <winsock.h>
#include <fstream>
#include<iostream>
#include<io.h>
#include<direct.h>
#include<vector>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

/*ȫ�ֱ���*/
namespace PublicMumber {
	SOCKET socket;
	int if_break;//1���жϲ�����0������
	string path_download;
	string name_file;
	string path_now;
	string root_now;
	vector<string> list_num;
};



/*
		���ݰ��ṹ
		1.����ͷ 6652���Ͽ����� 6653�����뽨������ 6654��ͬ������ 6655:�ļ����ͳɹ� 6656:�����ļ� 6657:��ֹ����
				 6658:�ļ������� 6659:�ļ��������� 6660:�鿴Ŀ¼�Ƿ���� 6661������ 6662��������
				 6663:ɾ���ļ�/�ļ���  6664��ɾ���ɹ�  6665��ɾ��ʧ��

		2.���ݰ�
		2.1����
		2.2�ļ�ͷ
		2.3�ļ�
*/
typedef struct transBag {
	int transBagHead;
	union {
		char num[13];
		char transBagOrder[2048];
		struct
		{
			char name[1024];
			long length;
		}transBagFileHead;
		char transBagFileBody[2048];
	}transBagBody;

}transBag;



ClientSocket::ClientSocket()
{
}

ClientSocket::~ClientSocket()
{
}

/*��Ҫ����*/
int ClientSocket::start(int port, char* ip)

{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return ERROR;
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
		return ERROR;
	SOCKADDR_IN clientAddr;
	clientAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(port);
	
	if (connect(clientSocket, (struct  sockaddr*) & clientAddr, sizeof(clientAddr)) == INVALID_SOCKET)
	{
		cout << "�����쳣" << endl;
		return ERROR;
	}
		
	/*
	1.���ӳɹ�
	2.��ȡ������ʶ��
	3.���ͱ�����ʶ��
	*/
	std::string macAdress;
	if (!GetMAC::GetMacByGetAdaptersInfo(macAdress))
		return ERROR;

	transBag transBagSendLogin;
	memset(&transBagSendLogin, 0, sizeof(transBag));
	transBagSendLogin.transBagHead = 5;
	for (int i = 0; i < macAdress.size(); i++)
		transBagSendLogin.transBagBody.num[i] = macAdress[i];
	send(clientSocket, (char*)& transBagSendLogin, sizeof(transBag), 0);
	struct UseableNum
	{
		char num[13];
		bool useable;
	}useableNum;
	int number = 0;
	while (1)
	{
		int r = recv(clientSocket, (char*)& useableNum, sizeof(UseableNum), 0);
		if (r > 0)
		{
			if (strcmp(useableNum.num, "#############") == 0)
			{
				break;
			}
			cout << "[" << number << "] " << useableNum.num;
			if (useableNum.useable == true)
				cout << "      free" << endl;
			else
				cout << "    connected" << endl;
			PublicMumber::list_num.push_back(useableNum.num);
			number++;
		}
		else
		{
			cout << "�����������쳣" << endl;
			return 0;
		}
	}
	if (PublicMumber::list_num.size() == 0)
	{
		cout << "�޿����û�" << endl;
		closesocket(clientSocket);
		return 0;
	}
	{
		int numberChoose = 0;
		cin >> numberChoose;
		transBag transBagSendConnect;
		memset(&transBagSendConnect, 0, sizeof(transBag));
		transBagSendConnect.transBagHead = 6653;
		for (int i = 0; i < PublicMumber::list_num[numberChoose].size(); i++)
		{
			transBagSendConnect.transBagBody.num[i] = PublicMumber::list_num[numberChoose][i];
		}
		send(clientSocket, (char*)& transBagSendConnect, sizeof(transBag), 0);
		transBag transBagRecv;
		memset(&transBagRecv, 0, sizeof(transBag));
		int r = recv(clientSocket, (char*)& transBagRecv, sizeof(transBag), 0);
		if (r > 0)
		{
			switch (transBagRecv.transBagHead)
			{
			case 6652:
				cout << "����ʧ��" << endl;
				break;
			case 6654:
				cout << "���ӳɹ�" << endl;
				//cout << PublicMumber::root_now << PublicMumber::path_now << ">";
				break;
			}
		}
	}
	
	PublicMumber::socket = clientSocket;

	Input::start();
	while (1)
	{
		transBag transBagRecv;
		memset(&transBagRecv, 0, sizeof(transBag));
		transBag transBagSend;
		int r = recv(clientSocket, (char*)& transBagRecv, sizeof(transBag), 0);
		if (r > 0)
		{
			std::string result = "";
			memset(&transBagSend, 0, sizeof(transBag));

			switch (transBagRecv.transBagHead)
			{
			case 1://���
			{
				cout << transBagRecv.transBagBody.transBagOrder;
			}
				break;
			case 2://�ļ���Ϣ
			{
#define LENGTH_FILE_BODY 2048
				std::fstream out;//д���ļ�
				out.open(PublicMumber::path_download+"\\"+PublicMumber::name_file,
					std::fstream::out | std::fstream::binary | std::fstream::trunc);
				long flen = transBagRecv.transBagBody.transBagFileHead.length;
				int len = 0;
				int flag = 0;//�Ƿ����سɹ�
				PublicMumber::if_break = 0;
				cout << "�ļ���		" << PublicMumber::name_file << endl;
				cout << "��С��		" << flen << " B		" << endl;
				cout << "�����У�	";
				cout << "   ";
				long Flen = flen;
				char content[2048] = { 0 };
				while (flen)
				{
					len = recv(clientSocket, content, LENGTH_FILE_BODY, 0);
					if (len > 0)
					{
						out.write(content, len);
						flen -= len;
						if (flen == 0)flag = 1;
					}
					else if (len <= 0 && errno != EINTR)
						break;

					if ((int)(Flen - flen) * 100 / (int)Flen < 10 && (int)(Flen - flen) * 100 / (int)Flen >= 0)
						cout << "\b\b";
					else
					if ((int)(Flen - flen) * 100 / (int)Flen >= 10 && (int)(Flen - flen) * 100 / (int)Flen <= 100)
						cout << "\b\b\b";
					cout << (int)(Flen - flen) * 100 / (int)Flen << "%";
				}
				out.close();
				if (flag)
				{
					cout << "\n" << PublicMumber::name_file << " ���سɹ�" << endl;
				}
			}
				break;
			case 6652://�Ͽ�����
			{
				cout << "\n�Ͽ�����" << endl;
				closesocket(PublicMumber::socket);
				WSACleanup();
				return 0;
			}	
				break;
			case 6654://���ӳɹ�
			{
				cout << "���ӳɹ�" << endl;
			}
				break;
			case 6655://�ϴ��ɹ�
			{
				cout << "\n�ļ��ϴ��ɹ�" << endl;
			}
				break;
			case 6658://�ļ�����ʧ��
			{
				cout << "�ļ�����ʧ��" << endl;
			}
				break;
			case 6661:///·������
			{/*·������*/
				PublicMumber::root_now = "";
				PublicMumber::path_now = "";
				for (int i = 0; i < 2048; i++)
				{
					if (transBagRecv.transBagBody.transBagOrder[i] == '\0')break;
					if (i < 3)
						PublicMumber::root_now.push_back(transBagRecv.transBagBody.transBagOrder[i]);
					else
						PublicMumber::path_now.push_back(transBagRecv.transBagBody.transBagOrder[i]);
				}
				if(PublicMumber::root_now.size()==2)PublicMumber::root_now.push_back('\\');
			}
				break;
			case 6662://Ŀ¼������
			{
				cout << "Ŀ¼������" << endl;
			}
				break;
			case 6664://ɾ���ɹ�
			{
				cout << "ɾ���ɹ�" << endl;
			}
				break;
			case 6665://ɾ��ʧ��
			{
				cout << "ɾ��ʧ��" << endl;
			}
				break;
			default:
				break;
			}
			cout << PublicMumber::root_now << PublicMumber::path_now << ">";
		}
		else if (r <= 0 && errno != EINTR)
		{
			closesocket(PublicMumber::socket);
			WSACleanup();
			break;
		}
	}
	return 1;
}

/*Input��*/
Input::Input()
{
}
Input::~Input()
{
}

/*��Ҫ����*/
DWORD WINAPI InputThread(LPVOID lpParameter)
{
	SOCKET clientSocket = PublicMumber::socket;
	transBag transBagSend;
	transBag transBagRecv;
	memset(&transBagRecv, 0, sizeof(transBag));
	while (1)
	{
		/*cout << PublicMumber::root_now << PublicMumber::path_now << ">";*/
		char orders[2048];
		cin.getline(orders, 2048);
		string orders_s = orders;
		string orderFirst = "";
		for (int i = 0; i < 2048; i++)
		{
			if (orders[i] == ' ' || orders[i] == '\0')
			{
				break;
			}
			orderFirst.push_back(orders[i]);
		}
		if (orderFirst == "dir")
		{
			/*dir + path_now*/
			string sendOrders = "";
			if (orders_s.size() == orderFirst.size())
			{
				sendOrders += "dir ";
				sendOrders += PublicMumber::root_now;
				sendOrders += PublicMumber::path_now;
			}
			else
			{
				sendOrders = orders_s;
			}
			memset(&transBagSend, 0, sizeof(transBag));
			transBagSend.transBagHead = 1;
			int i = 0;
			for (; i < sendOrders.size(); i++)
			{
				transBagSend.transBagBody.transBagOrder[i] = sendOrders[i];
			}
			transBagSend.transBagBody.transBagOrder[i] = '\0';
			send(clientSocket, (char*)& transBagSend, sizeof(transBag), 0);
		}
		else if (orderFirst == "cd")//change dir
		{
			/*�鿴Ŀ¼��������ı䣬��֮����*/
			memset(&transBagSend, 0, sizeof(transBag));
			transBagSend.transBagHead = 6660;
			string path;
			string path_now_change;
			GetSecondOrder(orders, path);
			if (PublicMumber::path_now.size() != 0)
				path_now_change = PublicMumber::path_now + "\\" + path;
			else
				path_now_change = PublicMumber::path_now + path;
			string patn_all = PublicMumber::root_now + path_now_change;
			int i_pnc = 0;
			for (; i_pnc < patn_all.size(); i_pnc++)
			{
				transBagSend.transBagBody.transBagOrder[i_pnc] = patn_all[i_pnc];
			}
			transBagSend.transBagBody.transBagOrder[i_pnc] = '\0';
			send(clientSocket, (char*)& transBagSend, sizeof(transBag), 0);
			
		}
		else if (orderFirst == "cd..")
		{
			/*�鿴Ŀ¼�����һ���򲻱䣬��֮����һ��*/
			int end = PublicMumber::path_now.size();

			for (int i = PublicMumber::path_now.size() - 1; i >= 0; i--)
			{
				if (PublicMumber::path_now[i] == '\\' || i == 0)
				{
					end = i;
					break;
				}
			}
			string path_now_change = "";
			for (int i = 0; i < end; i++)
			{
				path_now_change.push_back(PublicMumber::path_now[i]);
			}
			PublicMumber::path_now = path_now_change;
			cout << PublicMumber::root_now << PublicMumber::path_now << ">";
		}
		else if (orders_s.size() == 2 && orders_s[1] == ':')
		{
			memset(&transBagSend, 0, sizeof(transBag));
			transBagSend.transBagHead = 6660;
			int i_root = 0;
			for (; i_root < orders_s.size(); i_root++)
			{
				transBagSend.transBagBody.transBagOrder[i_root] = orders_s[i_root];
			}
			transBagSend.transBagBody.transBagOrder[i_root] = '\0';
			send(clientSocket, (char*)& transBagSend, sizeof(transBag), 0);
			//recv(clientSocket, (char*)& transBagRecv, sizeof(transBag), 0);
			//if (transBagRecv.transBagHead == 6661)
			//{
			//	PublicMumber::path_now = orders_s + "\\";
			//}
			//else if (transBagRecv.transBagHead == 6662)
			//{
			//	/*ʲôҲ����*/
			//}
		}
		else if (orderFirst == "upload")
		{
			/*�ϴ��ļ�*/
			memset(&transBagSend, 0, sizeof(transBag));
			transBagSend.transBagHead = 2;
			string nameWithPath = PublicMumber::root_now + PublicMumber::path_now;
			string secondOrders;
			string name="";
			GetSecondOrder(orders, secondOrders);
			fstream fs(secondOrders, fstream::in | fstream::binary);
			if (!fs.is_open()) {
				cout << "�ļ�������" << endl;
				cout << PublicMumber::root_now << PublicMumber::path_now << ">";
				continue;
			}
			
			int last_line = 0;
			for (int i = secondOrders.size() - 1; i >= 0; i--)
			{
				if (secondOrders[i] == '\\')
				{
					last_line = i;
					break;
				}
			}
			for (int i = last_line; i < secondOrders.size(); i++)
			{
				nameWithPath.push_back(secondOrders[i]);
				if (i != last_line)
					name.push_back(secondOrders[i]);
			}
			fs.seekg(0, fstream::end);//������λ��Ϊ��׼��ƫ��
			long Flen = fs.tellg();//ȡ���ļ���С
			fs.seekg(0, fstream::beg);
			int i_name = 0;
			for (; i_name < nameWithPath.size(); i_name++)
			{
				transBagSend.transBagBody.transBagFileHead.name[i_name] = nameWithPath[i_name];
			}
			transBagSend.transBagBody.transBagFileHead.name[i_name] = '\0';
			transBagSend.transBagBody.transBagFileHead.length = Flen;
			int r = send(clientSocket, (char*)& transBagSend, sizeof(transBag), 0);
			if (r <= 0)
			{
				fs.close();
				continue;
			}
			cout << "�ļ���		" << name << endl;
			cout << "��С��		" << Flen << " B		" << endl;
			cout << "�ϴ��У�	";
			cout << "   ";
			long flen = 0;
#define LENGTH_FILE_BODY 2048
			char szBuf[LENGTH_FILE_BODY] = { 0 };
			while (!fs.eof())
			{
				fs.read(szBuf, LENGTH_FILE_BODY);
				int len = fs.gcount();
				flen += send(clientSocket, szBuf, len, 0);
				if ((int)flen * 100 / (int)Flen < 10 && (int)flen * 100 / (int)Flen >= 0)
					cout << "\b\b";
				else
					if ((int)flen * 100 / (int)Flen >= 10 && (int)flen * 100 / (int)Flen <= 100)
						cout << "\b\b\b";
				cout << (int)flen * 100 / (int)Flen << "%";
			}
		}
		else if (orderFirst == "download")
		{
			/*��������ָ�ȥ��download��ֱ�ӷ����ļ�����6656*/
			string sendOrders="";
			string secondOrders;
			GetSecondOrder(orders, secondOrders);
			sendOrders = PublicMumber::root_now + PublicMumber::path_now + "\\" + secondOrders;
			memset(&transBagSend, 0, sizeof(transBag));
			transBagSend.transBagHead = 6656;
			int i = 0;
			for (; i < sendOrders.size(); i++)
			{
				transBagSend.transBagBody.transBagOrder[i] = sendOrders[i];
			}
			transBagSend.transBagBody.transBagOrder[i] = '\0';
			send(clientSocket, (char*)& transBagSend, sizeof(transBag), 0);
			PublicMumber::name_file = secondOrders;
		}
		else if (orderFirst == "start")
		{
			/*���ļ�*/
			string sendOrders = "";
			string secondOrders;
			GetSecondOrder(orders, secondOrders);
			sendOrders = "start " + PublicMumber::root_now + PublicMumber::path_now + "\\" + secondOrders;
			memset(&transBagSend, 0, sizeof(transBag));
			transBagSend.transBagHead = 1;
			int i = 0;
			for (; i < sendOrders.size(); i++)
			{
				transBagSend.transBagBody.transBagOrder[i] = sendOrders[i];
			}
			transBagSend.transBagBody.transBagOrder[i] = '\0';
			send(clientSocket, (char*)& transBagSend, sizeof(transBag), 0);
		}
		else if (orderFirst == "rm")
		{
		memset(&transBagSend, 0, sizeof(transBag));
		transBagSend.transBagHead = 6663;
		string name;
		string path_all;
		GetSecondOrder(orders, name);
		path_all = PublicMumber::root_now+ PublicMumber::path_now + "\\" + name;
		int i_pnc = 0;
		for (; i_pnc < path_all.size(); i_pnc++)
		{
			transBagSend.transBagBody.transBagOrder[i_pnc] = path_all[i_pnc];
		}
		transBagSend.transBagBody.transBagOrder[i_pnc] = '\0';
		send(clientSocket, (char*)& transBagSend, sizeof(transBag), 0);
		}
		else if (orderFirst == "cdd")//change download dir
		{
			/*�ı����ص�ַ*/
			string secondOrders;
			int rt = 0;
			GetSecondOrder(orders, secondOrders);
			if (0 != _access(secondOrders.c_str(), 0))
			{
				// if this folder not exist, create a new one.
				rt = _mkdir(secondOrders.c_str());				// ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
			}
			if (rt == 0)
				PublicMumber::path_download = secondOrders;
			else
				cout << "δ�ܸı�����Ŀ¼��Ŀ¼���ڷǷ��ַ�" << endl;
		}
		else if (orderFirst == "break")
		{//�˳�����
			memset(&transBagSend, 0, sizeof(transBag));
			transBagSend.transBagHead = 6666;
			send(clientSocket, (char*)& transBagSend, sizeof(transBag), 0);
			cout << "�û����ѹر�" << endl;
		}
		else if (orderFirst == "clear")
		{
			system("cls");
			cout << PublicMumber::root_now << PublicMumber::path_now << ">";
		}
		else
		{
			/*����ָ�ֱ�ӷ��ͣ���ָ��continue*/
			string sendOrders =orders;
			if (sendOrders.size() == 0)
			{
				cout << PublicMumber::root_now << PublicMumber::path_now << ">";
				continue;
			}
			memset(&transBagSend, 0, sizeof(transBag));
			transBagSend.transBagHead = 1;
			int i = 0;
			for (; i < sendOrders.size(); i++)
			{
				transBagSend.transBagBody.transBagOrder[i] = sendOrders[i];
			}
			transBagSend.transBagBody.transBagOrder[i] = '\0';
			send(clientSocket, (char*)& transBagSend, sizeof(transBag), 0);


		}
	}
	return 0;
}

int Input::start()
{
	/*
	1.�����߳�
	2.����->���봦��
	2.1->����
	2.2->����ָ��  2.2.1->���  2.2.2cd  2.2.3 �ı�����Ŀ¼
	2.3->�ϴ��ļ�
	*/
	CreateThread(NULL, 0, InputThread, NULL, 0, NULL);
	return 0;
}

/*����ָ��ĵڶ�����*/
void GetSecondOrder(string orders,string &secondOrder)
{
	secondOrder = "";
	int i = 0;
	for (; i < orders.size(); i++)
	{
		if (orders[i] != ' ')break;
	}
	for ( ;i < orders.size(); i++)
	{
		if (orders[i] == ' ')break;
	}
	for (; i < orders.size(); i++)
	{
		if (orders[i] != ' ')break;
	}
	for (; i < orders.size(); i++)
	{
		secondOrder.push_back(orders[i]);
	}
	/*for (i = 0; i < SecondOrder.size(); i++)
	{
		secondOrder[i] = SecondOrder[i];
	}
	secondOrder[i] = '\0';*/
}

/*��ʼ����������*/
void InitPublicMumber()
{
	PublicMumber::socket=0;
	PublicMumber::if_break=0;//1���жϲ�����0������
	PublicMumber::path_download="C:\\download";
	PublicMumber::name_file="";
	PublicMumber::path_now = "abc";//"Users\\win10\\Desktop";
	PublicMumber::root_now="C:\\";
	PublicMumber::list_num;
	if (0 != _access(PublicMumber::path_download.c_str(), 0))
	{
		// if this folder not exist, create a new one.
		_mkdir(PublicMumber::path_download.c_str());				// ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
	}
}