#pragma once
#include<vector>
#include"winsock2.h"
/*
�����û���Ϳ�����
*/
typedef struct UserMsg {
	char num[13];
	SOCKET socketUser;
	SOCKET socketCtrl;
}UserMsg;

typedef struct CtrlMsg {
	char num[13];
	SOCKET socketCtrl;
	SOCKET socketUser;
}CtrlMsg;

extern std::vector<UserMsg>userList;
extern std::vector<CtrlMsg>ctrlList;
