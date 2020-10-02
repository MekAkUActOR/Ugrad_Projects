
// AutorunsDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Autoruns.h"
#include "AutorunsDlg.h"
#include "afxdialogex.h"

#include "logon.h"
#include "autorun.h"
#include <iostream>
#include <wincrypt.h>
#include <wintrust.h>
#include <windows.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <io.h>
#include <tchar.h>
#include <atlbase.h>
#pragma warning(disable:4996)
#pragma comment(lib, "crypt32.lib")
using namespace std;

#define AUTORUN_BHO						0x00000001
#define AUTORUN_SHELL_EXECUTE_HOOK		0x00000002
#define AUTORUN_URL_SEARCH_HOOK			0x00000004
#define AUTORUN_USERINIT				0x00000008
#define AUTORUN_LOGON					0x00000010		
#define AUTORUN_NETWORK_PROVIDER		0x00000020
#define AUTORUN_APPINIT					0x00000040
#define AUTORUN_GINA					0x00000080
#define AUTORUN_SCREEN_SAVE				0x00000100
#define AUTORUN_KNOWNDLL				0x00000200
#define AUTORUN_IMAGE_HIJACK			0x00000400
#define AUTORUN_BOOT_EXECUTE			0x00000800
#define AUTORUN_SHELL					0x00001000
#define AUTORUN_STARTUP					0x00002000
#define AUTORUN_SERVICE					0x00004000
#define AUTORUN_DRIVER					0x00008000
#define AUTORUN_ALL						0xFFFFFFFF
#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)
/*
typedef struct _inline_run_t
{
	run_t data;
	snode_t node;
}inline_run_t;

typedef struct _logon_t
{
	slist_t run_list;
}logon_t;

*/

typedef struct {
	LPWSTR lpszProgramName;
	LPWSTR lpszPublisherLink;
	LPWSTR lpszMoreInfoLink;
} SPROG_PUBLISHERINFO, * PSPROG_PUBLISHERINFO;

BOOL GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo,
	PSPROG_PUBLISHERINFO Info);
BOOL GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME* st);
LPTSTR PrintCertificateInfo(PCCERT_CONTEXT pCertContext);
BOOL GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo,
	PCMSG_SIGNER_INFO* pCounterSignerInfo);


struct key_unit {
	int type = -1;
	char* Valuename = NULL;	
	string data;
};

struct father_unit {
	LPCTSTR repo;
	int num; 
	key_unit* sons;
};

struct grandfather_unit {
	father_unit* string;
	int num;
	bool* nempty;
};

void read_value(HKEY hkey, LPCTSTR cpp_data[], grandfather_unit* a);
void print_tree(grandfather_unit* a);
int read_from_startup(grandfather_unit* a);
//void getsubkey(stack<LPCTSTR> stk);
void Enum(LPCTSTR path, grandfather_unit* a);
LPCSTR add_tail(LPCSTR head, char* tail);
int read_from_task(grandfather_unit* a);
LPCTSTR getpublisher(string s);
wchar_t* multiByteToWideChar(const string& pKey);
string curve(string beforestr);
int cnt = 0;

/*
this part is for publisher query
*/
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/*
BOOL EnumStartup(AUTORUN_CALLBACK pfnCallback, LPVOID lpParam)
{
	CHKey hKey;
	DWORD dwResult = RegOpenKeyEx(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
		0, KEY_QUERY_VALUE, (PHKEY)&hKey);
	if (ERROR_SUCCESS != dwResult) {
		SetLastError(dwResult);
		return FALSE;
	}

	char Buffer[512] = { 0 };
	DWORD cbSize = sizeof(Buffer);
	dwResult = RegQueryValueEx(hKey.GetHandle(), "Startup", NULL, NULL, (PUCHAR)Buffer, &cbSize);
	if (ERROR_SUCCESS != dwResult) {
		SetLastError(dwResult);
		return FALSE;
	}

	char StartupDir[512] = { 0 };
	ExpandEnvironmentStrings(Buffer, StartupDir, sizeof(StartupDir) - 1);

	char Keyword[512] = { 0 };
	strncpy(Keyword, StartupDir, sizeof(Keyword) - 1);
	PathAddBackslash(Keyword);
	strncat(Keyword, "*.lnk", sizeof(Keyword) - strlen(Keyword) - 1);

	//枚举文件数量
	WIN32_FIND_DATA FindFileData;
	ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATA));
	HANDLE hFind = FindFirstFile(Keyword, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	do {
		if (strncmp(FindFileData.cFileName, "..", 2) == 0
			|| strncmp(FindFileData.cFileName, ".", 1) == 0)
			continue;

		//判断是否目录
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		//获取文件全路径
		AUTORUN_ITEM Item = { 0 };
		char LinkFile[512] = { 0 };
		strncpy(Item.Name, FindFileData.cFileName, sizeof(Item.Name) - 1);
		strncpy(LinkFile, StartupDir, sizeof(LinkFile) - 1);
		PathAddBackslash(LinkFile);
		strncat(LinkFile, FindFileData.cFileName, sizeof(LinkFile) - strlen(LinkFile) - 1);
		GetLinkInfo(NULL, LinkFile, Item.ImagePath, NULL);
		GetLongPathName(Item.ImagePath, Item.ImagePath, sizeof(Item.ImagePath) - 1);
		if (!pfnCallback(AUTORUN_STARTUP, &Item, lpParam)) {
			SetLastError(ERROR_CANCELLED);
			return FALSE;
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);

	return TRUE;
}
*/


int read_from_task(grandfather_unit* a)
{
	int num = 0;
	string inPath = "D:\\Tasks\\*";

	long handle;
	struct _finddata_t fileinfo;
	handle = _findfirst(inPath.c_str(), &fileinfo);
	if (handle == -1)
	{
		return -1;
	}
	do
	{
		if (strcmp(fileinfo.name, ".") != 0
			&& strcmp(fileinfo.name, "..") != 0
			&& (!(fileinfo.attrib & _A_SUBDIR))
			)
		{
			num++;
		}
		
	} while (!_findnext(handle, &fileinfo));
	if (num > 0)
	{

		handle = _findfirst(inPath.c_str(), &fileinfo);
		a->num = 1;
		a->string = new father_unit[1];
		a->nempty = new bool[1];
		a->nempty[0] = true;
		a->string[0].num = num;
		a->string[0].sons = new key_unit[num + 1];
		int mmax = num + 1;
		a->string[0].repo = "C:\\Windows\\System32\\Tasks";
		num = 0;
		do
		{
			if (num < mmax) {
				if (strcmp(fileinfo.name, ".") != 0
					&& strcmp(fileinfo.name, "..") != 0
					&& (!(fileinfo.attrib & _A_SUBDIR))
					)
				{

					a->string[0].sons[num].type = 1;
					int len = strlen(fileinfo.name);

					a->string[0].sons[num].Valuename = new char[len + 1];
					strcpy(a->string[0].sons[num].Valuename, fileinfo.name);

					a->string[0].sons[num].data = "Schduled Tasks";
					num++;
				}
			}
		} while (!_findnext(handle, &fileinfo));
	}

	_findclose(handle);
	if (num == 0) return 0;
	return 1;
}

void print_tree(grandfather_unit* a)
{
	int cnt = 0;
	for (int i = 0; i < a->num; i++)
	{
		if (!a->nempty[i])
		{
			continue;
		}
		if (a->string[i].num <= 0)
		{
			continue;
		}
		for (int j = 0; j < a->string[i].num; j++)
		{
			cnt++;
			if (a->string[i].sons[j].Valuename == " ")
			{
				continue;
			}
		}
	}
}

void read_value(HKEY hkey, LPCTSTR cpp_data[], grandfather_unit* a)
{
	HKEY cpp_key;
	int keynum = a->num;
	a->string = new father_unit[a->num];
	a->nempty = new bool[a->num];

	for (int i = 0; i < keynum; i++)
	{
		a->string[i].repo = cpp_data[i];
		if (RegOpenKeyEx(hkey, cpp_data[i], 0, KEY_READ | KEY_WOW64_64KEY, &cpp_key) == ERROR_SUCCESS)
		{
			a->nempty[i] = true;
			DWORD NameSize, NameCnt, NameMaxLen, Type;
			DWORD KeyCnt, KeyMaxLen, DataSize, MaxDataLen;
			if (ERROR_SUCCESS == RegQueryInfoKey(cpp_key, NULL, NULL, 0, &KeyCnt, &KeyMaxLen, NULL, &NameCnt, &NameMaxLen, &MaxDataLen, NULL, NULL))
			{
				a->string[i].num = NameCnt;
				a->string[i].sons = new key_unit[NameCnt];
				for (int j = 0; j < int(NameCnt); j++)    //枚举键值
				{
					DataSize = MaxDataLen + 1;
					NameSize = NameMaxLen + 1;
					char* szValueName = (char*)malloc(NameSize);
					LPBYTE szValueData = (LPBYTE)malloc(DataSize);
					RegEnumValue(cpp_key, j, szValueName, &NameSize, NULL, &Type, (LPBYTE)szValueData, &DataSize);//读取键值
					string beforestr = (char*)szValueData;
					beforestr = curve(beforestr);

					a->string[i].sons[j].type = Type;
					a->string[i].sons[j].Valuename = new char[NameSize];
					a->string[i].sons[j].Valuename = szValueName;
					a->string[i].sons[j].data = beforestr;
					if (NameSize == 0)
					{
						a->string[i].num--;
						j--;
						NameCnt--;
					}
				}
			}
			else {
				a->nempty[i] = false;
			}
		}
		else {
			a->nempty[i] = false;
		}
		RegCloseKey(cpp_key);//关闭句柄
	}
}

int read_from_startup(grandfather_unit* a)
{
	int num = 0;
	string inPath = "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\*";
	long handle;
	struct _finddata_t fileinfo;
	handle = _findfirst(inPath.c_str(), &fileinfo);
	if (handle == -1)
		return -1;
	do
	{
		if (strcmp(fileinfo.name, "desktop.ini") != 0
			&& strcmp(fileinfo.name, ".") != 0
			&& strcmp(fileinfo.name, "..") != 0
			)
		{
			num++;
		}
	} while (!_findnext(handle, &fileinfo));
	if (num > 0)
	{
		handle = _findfirst(inPath.c_str(), &fileinfo);
		a->num = 1;
		a->string = new father_unit[1];
		a->nempty = new bool[1];
		a->nempty[0] = true;
		a->string[0].num = num;
		a->string[0].sons = new key_unit[num + 1];
		int mmax = num + 1;
		a->string[0].repo = "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup";
		num = 0;
		do
		{
			if (num < mmax)
			{
				if (strcmp(fileinfo.name, "desktop.ini") != 0
					&& strcmp(fileinfo.name, ".") != 0
					&& strcmp(fileinfo.name, "..") != 0
					)
				{
					a->string[0].sons[num].type = 1;
					int len = strlen(fileinfo.name);
					a->string[0].sons[num].Valuename = new char[len + 1];
					strcpy(a->string[0].sons[num].Valuename, fileinfo.name);

					a->string[0].sons[num].data = "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\"
						+ (string)(fileinfo.name);
					num++;

				}
			}
		} while (!_findnext(handle, &fileinfo));
	}

	_findclose(handle);
	if (num == 0) return 0;
	return 1;
}


void Enum(LPCTSTR path, grandfather_unit* a)
{
	HKEY cpp_key;//母键
	DWORD  NameCnt, NameMaxLen;
	DWORD KeySize, KeyCnt, KeyMaxLen, MaxDatalen;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_READ | KEY_WOW64_64KEY, &cpp_key))
	{
		if (ERROR_SUCCESS == RegQueryInfoKey(cpp_key, NULL, NULL, 0, &KeyCnt, &KeyMaxLen, NULL, &NameCnt, &NameMaxLen, &MaxDatalen, NULL, NULL))
		{
			a->num = KeyCnt;
			a->nempty = new bool[a->num];
			a->string = new father_unit[a->num];

			for (DWORD i = 0; i < KeyCnt; i++)
			{

				KeySize = KeyMaxLen + 1;
				char* szKeyName = (char*)malloc(KeySize);
				RegEnumKeyEx(cpp_key, i, szKeyName, &KeySize, NULL, NULL, NULL, NULL);
				string str = string(path);
				str = str + "\\" + string(szKeyName);
				LPCTSTR new_path = str.c_str();
				a->string[i].repo = path;
				//查询到的子键
				HKEY key1;
				if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, new_path, 0, KEY_READ, &key1))
				{
					//cnt ++;
					DWORD dwValue, t_data, dwType = REG_DWORD, dwSize = MAX_PATH;
					LPBYTE sbValue = (LPBYTE)malloc(dwSize);
					char tmp[MAX_PATH];
					memcpy(tmp, sbValue, MAX_PATH);
					DWORD dwDataType = 0;
					RegQueryValueEx(key1, "Type", 0, &dwType, (LPBYTE)&t_data, &dwValue);
					if ((t_data == 16) || (t_data == 32))//which means is service
					{
						RegQueryValueEx(key1, "ImagePath", 0, &dwDataType, sbValue, &dwSize);
						if (!strncmp((char*)(sbValue), tmp, 5)) continue;
						string Imgp = (char*)(sbValue);
						if (Imgp.find("svchost.exe") != string::npos)
						{
							a->nempty[i] = true;
							a->string[i].num = 1;
							a->string[i].sons = new key_unit[1];
							a->string[i].sons[0].type = t_data;
							a->string[i].sons[0].Valuename = new char[KeySize];
							a->string[i].sons[0].Valuename = szKeyName;
							char* para = (char*)("Parameters");
							string ss = (string)(new_path);
							ss = ss + "\\" + string(para);
							LPCTSTR para_path = ss.c_str();

							HKEY key2;
							DWORD iType = REG_EXPAND_SZ;
							DWORD isize = MAX_PATH;
							LPBYTE iValue = (LPBYTE)malloc(isize);
							if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, para_path, 0, KEY_READ, &key2))
							{
								RegQueryValueEx(key2, "ServiceDll", 0, &iType, iValue, &isize);
								string beforestr = (char*)iValue;
								beforestr = curve(beforestr);
								a->string[i].sons[0].data = beforestr;

							}
							else
							{
								string beforestr = (char*)sbValue;
								beforestr = curve(beforestr);
								a->string[i].sons[0].data = beforestr;

							}
						}
						else
						{
							a->nempty[i] = true;
							a->string[i].num = 1;
							a->string[i].sons = new key_unit[1];
							a->string[i].sons[0].type = t_data;
							a->string[i].sons[0].data = (char*)sbValue;
							string beforestr = (char*)sbValue;
							beforestr = curve(beforestr);
							a->string[i].sons[0].data = beforestr;

							a->string[i].sons[0].Valuename = new char[KeySize];
							a->string[i].sons[0].Valuename = szKeyName;
						}
					}
					else if ((t_data == 1) || (t_data == 2))
					{
						RegQueryValueEx(key1, "ImagePath", 0, &dwDataType, sbValue, &dwSize);
						if (!strncmp((char*)(sbValue), tmp, 5)) continue;
						a->nempty[i] = true;
						a->string[i].num = 1;
						a->string[i].sons = new key_unit[1];
						a->string[i].sons[0].type = t_data;

						string beforestr = (char*)sbValue;
						beforestr = curve(beforestr);
						a->string[i].sons[0].data = beforestr;

						a->string[i].sons[0].Valuename = new char[KeySize];
						a->string[i].sons[0].Valuename = szKeyName;
					}

					else
					{
						a->nempty[i] = false;
					}

				}
				else
				{
					a->nempty[i] = false;
				}
			}
		}
		else {
			cout << "打开注册表失败" << endl;
		}
	}
}


LPCSTR add_tail(LPCSTR head, char* tail)
{
	std::string s = string(head);
	s = s + "\\" + string(tail);
	LPCTSTR r = s.c_str();
	return r;
}


LPCTSTR getpublisher(string s)
{
	LPCTSTR vain = "Null";
	char* dest1 = const_cast<char*>(s.c_str());
	int num = MultiByteToWideChar(0, 0, dest1, -1, NULL, 0);
	wchar_t* wide = new wchar_t[num];
	MultiByteToWideChar(0, 0, dest1, -1, wide, num);

	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fResult;
	DWORD dwEncoding, dwContentType, dwFormatType;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	PCMSG_SIGNER_INFO pCounterSignerInfo = NULL;
	DWORD dwSignerInfo;
	CERT_INFO CertInfo;
	SPROG_PUBLISHERINFO ProgPubInfo;
	SYSTEMTIME st;

	ZeroMemory(&ProgPubInfo, sizeof(ProgPubInfo));

	fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE,
		wide,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
		CERT_QUERY_FORMAT_FLAG_BINARY,
		0,
		&dwEncoding,
		&dwContentType,
		&dwFormatType,
		&hStore,
		&hMsg,
		NULL);
	if (!fResult)
	{
		return vain;
	}

	fResult = CryptMsgGetParam(hMsg,
		CMSG_SIGNER_INFO_PARAM,
		0,
		NULL,
		&dwSignerInfo);
	if (!fResult)
	{
		printf("cannot!");
		return vain;
	}

	pSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);
	if (!pSignerInfo)
	{
		printf("cannot!");
		return vain;
	}

	fResult = CryptMsgGetParam(hMsg,
		CMSG_SIGNER_INFO_PARAM,
		0,
		(PVOID)pSignerInfo,
		&dwSignerInfo);
	if (!fResult)
	{
		printf("cannot!");
		return vain;
	}

	if (GetProgAndPublisherInfo(pSignerInfo, &ProgPubInfo))
	{
		if (ProgPubInfo.lpszProgramName != NULL)
		{
			wprintf(L"Program Name : %s\n##",
				ProgPubInfo.lpszProgramName);
		}

		if (ProgPubInfo.lpszPublisherLink != NULL)
		{
			wprintf(L"Publisher Link : %s\n",
				ProgPubInfo.lpszPublisherLink);
		}

		if (ProgPubInfo.lpszMoreInfoLink != NULL)
		{
			wprintf(L"MoreInfo Link : %s\n",
				ProgPubInfo.lpszMoreInfoLink);
		}
	}

	_tprintf(_T("\n"));

	CertInfo.Issuer = pSignerInfo->Issuer;
	CertInfo.SerialNumber = pSignerInfo->SerialNumber;

	pCertContext = CertFindCertificateInStore(hStore,
		ENCODING,
		0,
		CERT_FIND_SUBJECT_CERT,
		(PVOID)&CertInfo,
		NULL);
	if (!pCertContext)
	{
		_tprintf(_T("CertFindCertificateInStore failed with %x\n"),
			GetLastError());
		printf("cannot!");
		return vain;
	}

	LPCTSTR szNmae1 = PrintCertificateInfo(pCertContext);

	return szNmae1;

	if (GetTimeStampSignerInfo(pSignerInfo, &pCounterSignerInfo))
	{
		CertInfo.Issuer = pCounterSignerInfo->Issuer;
		CertInfo.SerialNumber = pCounterSignerInfo->SerialNumber;

		pCertContext = CertFindCertificateInStore(hStore,
			ENCODING,
			0,
			CERT_FIND_SUBJECT_CERT,
			(PVOID)&CertInfo,
			NULL);
		if (!pCertContext)
		{
			_tprintf(_T("CertFindCertificateInStore failed with %x\n"),
				GetLastError());
			printf("cannot!");
			return vain;
		}
	}
	return 0;
}


LPTSTR PrintCertificateInfo(PCCERT_CONTEXT pCertContext)
{
	BOOL fReturn = FALSE;
	LPTSTR szName = NULL;
	DWORD dwData;

	__try
	{
		if (!(dwData = CertGetNameString(pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			CERT_NAME_ISSUER_FLAG,
			NULL,
			NULL,
			0)))
		{
			_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}

		szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
		if (!szName)
		{
			_tprintf(_T("Unable to allocate memory for issuer name.\n"));
			__leave;
		}

		if (!(CertGetNameString(pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			CERT_NAME_ISSUER_FLAG,
			NULL,
			szName,
			dwData)))
		{
			_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}

		_tprintf(_T("## \tIssuer Name: %s\n"), szName);
		LPTSTR szName1 = szName;
		LocalFree(szName);
		szName = NULL;
		return szName1;

		// Get Subject name size.
		if (!(dwData = CertGetNameString(pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			0,
			NULL,
			NULL,
			0)))
		{
			_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}

		// Allocate memory for subject name.
		szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
		if (!szName)
		{
			_tprintf(_T("Unable to allocate memory for subject name.\n"));
			__leave;
		}

		// Get subject name.
		if (!(CertGetNameString(pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			0,
			NULL,
			szName,
			dwData)))
		{
			_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}

		_tprintf(_T("## \tSubject Name: %s\n"), szName);

		fReturn = TRUE;
	}
	__finally
	{
		if (szName != NULL) LocalFree(szName);
	}

}

LPWSTR AllocateAndCopyWideString(LPCWSTR inputString)
{
	LPWSTR outputString = NULL;

	outputString = (LPWSTR)LocalAlloc(LPTR,
		(wcslen(inputString) + 1) * sizeof(WCHAR));
	if (outputString != NULL)
	{
		lstrcpyW(outputString, inputString);
	}
	return outputString;
}

BOOL GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo,
	PSPROG_PUBLISHERINFO Info)
{
	BOOL fReturn = FALSE;
	PSPC_SP_OPUS_INFO OpusInfo = NULL;
	DWORD dwData;
	BOOL fResult;

	__try
	{
		// Loop through authenticated attributes and find
		// SPC_SP_OPUS_INFO_OBJID OID.
		for (DWORD n = 0; n < pSignerInfo->AuthAttrs.cAttr; n++)
		{
			if (lstrcmpA(SPC_SP_OPUS_INFO_OBJID,
				pSignerInfo->AuthAttrs.rgAttr[n].pszObjId) == 0)
			{
				// Get Size of SPC_SP_OPUS_INFO structure.
				fResult = CryptDecodeObject(ENCODING,
					SPC_SP_OPUS_INFO_OBJID,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					NULL,
					&dwData);
				if (!fResult)
				{
					_tprintf(_T("CryptDecodeObject failed with %x\n"),
						GetLastError());
					__leave;
				}

				// Allocate memory for SPC_SP_OPUS_INFO structure.
				OpusInfo = (PSPC_SP_OPUS_INFO)LocalAlloc(LPTR, dwData);
				if (!OpusInfo)
				{
					_tprintf(_T("Unable to allocate memory for Publisher Info.\n"));
					__leave;
				}

				// Decode and get SPC_SP_OPUS_INFO structure.
				fResult = CryptDecodeObject(ENCODING,
					SPC_SP_OPUS_INFO_OBJID,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					OpusInfo,
					&dwData);
				if (!fResult)
				{
					_tprintf(_T("CryptDecodeObject failed with %x\n"),
						GetLastError());
					__leave;
				}

				// Fill in Program Name if present.
				if (OpusInfo->pwszProgramName)
				{
					Info->lpszProgramName =
						AllocateAndCopyWideString(OpusInfo->pwszProgramName);
				}
				else
					Info->lpszProgramName = NULL;

				// Fill in Publisher Information if present.
				if (OpusInfo->pPublisherInfo)
				{

					switch (OpusInfo->pPublisherInfo->dwLinkChoice)
					{
					case SPC_URL_LINK_CHOICE:
						Info->lpszPublisherLink =
							AllocateAndCopyWideString(OpusInfo->pPublisherInfo->pwszUrl);
						break;

					case SPC_FILE_LINK_CHOICE:
						Info->lpszPublisherLink =
							AllocateAndCopyWideString(OpusInfo->pPublisherInfo->pwszFile);
						break;

					default:
						Info->lpszPublisherLink = NULL;
						break;
					}
				}
				else
				{
					Info->lpszPublisherLink = NULL;
				}

				// Fill in More Info if present.
				if (OpusInfo->pMoreInfo)
				{
					switch (OpusInfo->pMoreInfo->dwLinkChoice)
					{
					case SPC_URL_LINK_CHOICE:
						Info->lpszMoreInfoLink =
							AllocateAndCopyWideString(OpusInfo->pMoreInfo->pwszUrl);
						break;

					case SPC_FILE_LINK_CHOICE:
						Info->lpszMoreInfoLink =
							AllocateAndCopyWideString(OpusInfo->pMoreInfo->pwszFile);
						break;

					default:
						Info->lpszMoreInfoLink = NULL;
						break;
					}
				}
				else
				{
					Info->lpszMoreInfoLink = NULL;
				}

				fReturn = TRUE;

				break; // Break from for loop.
			} // lstrcmp SPC_SP_OPUS_INFO_OBJID                 
		} // for 
	}
	__finally
	{
		if (OpusInfo != NULL) LocalFree(OpusInfo);
	}

	return fReturn;
}

BOOL GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME* st)
{
	BOOL fResult;
	FILETIME lft, ft;
	DWORD dwData;
	BOOL fReturn = FALSE;

	// Loop through authenticated attributes and find
	// szOID_RSA_signingTime OID.
	for (DWORD n = 0; n < pSignerInfo->AuthAttrs.cAttr; n++)
	{
		if (lstrcmpA(szOID_RSA_signingTime,
			pSignerInfo->AuthAttrs.rgAttr[n].pszObjId) == 0)
		{
			// Decode and get FILETIME structure.
			dwData = sizeof(ft);
			fResult = CryptDecodeObject(ENCODING,
				szOID_RSA_signingTime,
				pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
				pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData,
				0,
				(PVOID)&ft,
				&dwData);
			if (!fResult)
			{
				_tprintf(_T("CryptDecodeObject failed with %x\n"),
					GetLastError());
				break;
			}

			// Convert to local time.
			FileTimeToLocalFileTime(&ft, &lft);
			FileTimeToSystemTime(&lft, st);

			fReturn = TRUE;

			break; // Break from for loop.

		} //lstrcmp szOID_RSA_signingTime
	} // for 

	return fReturn;
}

BOOL GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo, PCMSG_SIGNER_INFO* pCounterSignerInfo)
{
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fReturn = FALSE;
	BOOL fResult;
	DWORD dwSize;

	__try
	{
		*pCounterSignerInfo = NULL;

		// Loop through unathenticated attributes for
		// szOID_RSA_counterSign OID.
		for (DWORD n = 0; n < pSignerInfo->UnauthAttrs.cAttr; n++)
		{
			if (lstrcmpA(pSignerInfo->UnauthAttrs.rgAttr[n].pszObjId,
				szOID_RSA_counterSign) == 0)
			{
				// Get size of CMSG_SIGNER_INFO structure.
				fResult = CryptDecodeObject(ENCODING,
					PKCS7_SIGNER_INFO,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					NULL,
					&dwSize);
				if (!fResult)
				{
					_tprintf(_T("CryptDecodeObject failed with %x\n"),
						GetLastError());
					__leave;
				}

				// Allocate memory for CMSG_SIGNER_INFO.
				*pCounterSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSize);
				if (!*pCounterSignerInfo)
				{
					_tprintf(_T("Unable to allocate memory for timestamp info.\n"));
					__leave;
				}

				// Decode and get CMSG_SIGNER_INFO structure
				// for timestamp certificate.
				fResult = CryptDecodeObject(ENCODING,
					PKCS7_SIGNER_INFO,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					(PVOID)*pCounterSignerInfo,
					&dwSize);
				if (!fResult)
				{
					_tprintf(_T("CryptDecodeObject failed with %x\n"),
						GetLastError());
					__leave;
				}

				fReturn = TRUE;

				break; // Break from for loop.
			}
		}
	}
	__finally
	{
		// Clean up.
		if (pCertContext != NULL) CertFreeCertificateContext(pCertContext);
	}

	return fReturn;
}

wchar_t* multiByteToWideChar(const string& pKey)
{
	char* pCStrKey = (char*)pKey.c_str();
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t* pWCStrKey = new wchar_t[pSize];
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}

string curve(string beforestr)
{
	int firstindex1 = -1, firstindex2 = -1, firstindex3 = -1, firstindex4 = -1, firstindex5 = -1, firstindex6 = -1, firstindex7 = -1;
	if ((firstindex1 = beforestr.find(" -")) != std::string::npos)
	{
		//cout << "1   ";
		beforestr = beforestr.substr(0, firstindex1);
		//cout << beforestr << endl;
	}
	if (beforestr[0] == '"')
	{
		//cout << "2";
		beforestr = beforestr.substr(1, beforestr.length());
		//cout << beforestr << endl;
	}
	if ((firstindex2 = beforestr.rfind("\"")) != std::string::npos)
	{
		//cout << "3";
		beforestr = beforestr.substr(0, firstindex2);
	}
	if (firstindex3 = beforestr.find("%SystemRoot%") != std::string::npos)
	{
		//cout << "4";
		beforestr = "C:\\Windows" + beforestr.substr(firstindex3 + 12, beforestr.length());
	}
	if (firstindex4 = beforestr.find("%Systemroot%") != std::string::npos)
	{
		//cout << "5";
		beforestr = "C:\\Windows" + beforestr.substr(firstindex4 + 12, beforestr.length());
	}
	if (firstindex5 = beforestr.find("%systemRoot%") != std::string::npos)
	{
		//cout << "6";
		beforestr = "C:\\Windows" + beforestr.substr(firstindex5 + 12, beforestr.length());
	}
	if (firstindex6 = beforestr.find("%systemroot%") != std::string::npos)
	{
		//cout << "7";
		beforestr = "C:\\Windows" + beforestr.substr(firstindex6 + 12, beforestr.length());
	}
	if (firstindex6 = beforestr.find("%windir%") != std::string::npos)
	{
		//cout << "7";
		beforestr = "C:\\Windows" + beforestr.substr(firstindex6 + 8, beforestr.length());
	}

	string wxw = beforestr.substr(0, 11);
	if (wxw == "\\SystemRoot")
	{
		//cout << "8";
		beforestr = "C:\\Windows" + beforestr;
	}
	string tss = beforestr.substr(0, 8);
	if (tss == "System32"||tss == "system32")
	{
		//cout << "9";
		beforestr = "C:\\Windows" + beforestr;
	}

	//cout << "curved:" << beforestr<<endl;
	return beforestr;
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAutorunsDlg 对话框



CAutorunsDlg::CAutorunsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AUTORUNS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutorunsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, m_PropertyGrid);
}

BEGIN_MESSAGE_MAP(CAutorunsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CAutorunsDlg 消息处理程序

BOOL CAutorunsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	HDITEM item;
	item.cxy = 120;
	item.mask = HDI_WIDTH;
	m_PropertyGrid.GetHeaderCtrl().SetItem(0, new HDITEM(item));

	//################

		LPCTSTR logon[] = {
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
			"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run",
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run",
			"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run",
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\RunOnce",
			"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Policies\\RunOnce",
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\RunOnceEx",
			"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Policies\\RunOnceEx",
		};

		LPCTSTR drivers[] = {
		"System\\CurrentControlSet\\Services"
		};


		grandfather_unit* a[11];
		//int num_of_gf= sizeof(a)/sizeof(grandfather_unit);
		int num_of_gf = sizeof(a) / sizeof(grandfather_unit*);
		for (int i = 0; i < num_of_gf; i++)
		{
			a[i] = new grandfather_unit; // 用来存的节点。
		}
		/**Local_Machine Logon**/
		a[0]->num = sizeof(logon) / sizeof(LPCTSTR);
		read_value(HKEY_LOCAL_MACHINE, logon, a[0]); //往里读 !

		/**Current User Logon**/
		a[1]->num = sizeof(logon) / sizeof(LPCTSTR);
		read_value(HKEY_CURRENT_USER, logon, a[1]);

		LPCTSTR dlls[] = { "System\\CurrentControlSet\\Control\\Session Manager\\KnownDlls" };
		a[7]->num = sizeof(dlls) / sizeof(LPCTSTR);
		read_value(HKEY_LOCAL_MACHINE, dlls, a[7]);

		LPCTSTR winsock[] = { "SYSTEM\\CurrentControlSet\\Services\\WinSock2" };
		a[8]->num = sizeof(winsock) / sizeof(LPCTSTR);
		read_value(HKEY_LOCAL_MACHINE, winsock, a[8]);

		LPCTSTR network[] = { "SYSTEM\\CurrentControlSet\\Control\\NetworkProvider\\ProviderOrder" };
		a[9]->num = sizeof(network) / sizeof(LPCTSTR);
		read_value(HKEY_LOCAL_MACHINE, network, a[9]);

		LPCTSTR winlogon[] = { "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Taskman",
			"SYSTEM\\Setup\\CmdLine",
			"System\\CurrentControlSet\\Control\\BootVerificationProgram\\ImagePath"
		};
		a[10]->num = sizeof(winlogon) / sizeof(LPCTSTR);
		read_value(HKEY_LOCAL_MACHINE, winlogon, a[10]);


		LPCTSTR service = "System\\CurrentControlSet\\Services";


	//################

		CMFCPropertyGridProperty* group1 = new CMFCPropertyGridProperty(_T("logon"));
		CMFCPropertyGridProperty* group2 = new CMFCPropertyGridProperty(_T("driver"));
		CMFCPropertyGridProperty* group3 = new CMFCPropertyGridProperty(_T("service"));
		CMFCPropertyGridProperty* group4 = new CMFCPropertyGridProperty(_T("scheduled tasks"));
		CMFCPropertyGridProperty* group5 = new CMFCPropertyGridProperty(_T("boot execution"));
		CMFCPropertyGridProperty* group6 = new CMFCPropertyGridProperty(_T("Image Hijacks"));
		CMFCPropertyGridProperty* group7 = new CMFCPropertyGridProperty(_T("known dlls"));
		CMFCPropertyGridProperty* group8 = new CMFCPropertyGridProperty(_T("winsocks"));
		CMFCPropertyGridProperty* group9 = new CMFCPropertyGridProperty(_T("network provider"));
		CMFCPropertyGridProperty* group10 = new CMFCPropertyGridProperty(_T("winlogon"));
		for (int i = 0; i < a[0]->num; i++)
		{
			if (!a[0]->nempty[i])
			{
				continue;
			}
			if (a[0]->string[i].num <= 0)
			{
				continue;
			}
			for (int j = 0; j < a[0]->string[i].num; j++)
			{
				if (a[0]->string[i].sons[j].Valuename == " ")
				{
					continue;
				}

				LPCTSTR t = a[0]->string[i].sons[j].data.c_str();

				CMFCPropertyGridProperty* tmp = new CMFCPropertyGridProperty(
					_T(a[0]->string[i].sons[j].Valuename),
					_T(t),
					_T(getpublisher(a[0]->string[i].sons[j].data)));
				group1->AddSubItem(tmp);
			}
		}
		for (int i = 0; i < a[1]->num; i++)
		{
			if (!a[1]->nempty[i])
			{
				continue;
			}
			if (a[1]->string[i].num <= 0)
			{
				continue;
			}
			for (int j = 0; j < a[1]->string[i].num; j++)
			{
if (a[1]->string[i].sons[j].Valuename == " ")
{
	continue;
}

LPCTSTR t = a[1]->string[i].sons[j].data.c_str();

CMFCPropertyGridProperty* tmp = new CMFCPropertyGridProperty(
	_T(a[1]->string[i].sons[j].Valuename),
	_T(t),
	_T(getpublisher(a[1]->string[i].sons[j].data)));
group1->AddSubItem(tmp);
			}
		}

		for (int i = 0; i < a[2]->num; i++)
		{
			if (!a[2]->nempty[i])
			{
				continue;
			}
			if (a[2]->string[i].num <= 0)
			{
				continue;
			}
			for (int j = 0; j < a[2]->string[i].num; j++)
			{
				if (a[2]->string[i].sons[j].Valuename == " ")
				{
					continue;
				}

				LPCTSTR t = a[2]->string[i].sons[j].data.c_str();

				CMFCPropertyGridProperty* tmp = new CMFCPropertyGridProperty(
					_T(a[2]->string[i].sons[j].Valuename),
					_T(t),
					_T(getpublisher(a[2]->string[i].sons[j].data)));
				group1->AddSubItem(tmp);
			}
		}
		Enum(service, a[3]);
		for (int i = 0; i < a[3]->num; i++)
		{
			if (!a[3]->nempty[i])
			{
				continue;
			}
			if (a[3]->string[i].num <= 0)
			{
				continue;
			}
			for (int j = 0; j < a[3]->string[i].num; j++)
			{
				if (a[3]->string[i].sons[j].Valuename == " ")
				{
					continue;
				}

				LPCTSTR t = a[3]->string[i].sons[j].data.c_str();

				CMFCPropertyGridProperty* tmp = new CMFCPropertyGridProperty(
					_T(a[3]->string[i].sons[j].Valuename),
					_T(t),
					_T(getpublisher(a[3]->string[i].sons[j].data)));
				if (a[3]->string[i].sons[j].type == 1 || a[3]->string[i].sons[j].type == 2)
					group2->AddSubItem(tmp);
				else
					group3->AddSubItem(tmp);
			}
		}
		read_from_task(a[4]);
		for (int i = 0; i < a[4]->num; i++)
		{
			if (!a[4]->nempty[i])
			{
				continue;
			}
			if (a[4]->string[i].num <= 0)
			{
				continue;
			}
			for (int j = 0; j < a[4]->string[i].num; j++)
			{
				if (a[4]->string[i].sons[j].Valuename == " ")
				{
					continue;
				}

				LPCTSTR t = a[4]->string[i].sons[j].data.c_str();

				CMFCPropertyGridProperty* tmp = new CMFCPropertyGridProperty(
					_T(a[4]->string[i].sons[j].Valuename),
					_T(t),
					_T(getpublisher(a[4]->string[i].sons[j].data)));
				group4->AddSubItem(tmp);
			}
		}
		HKEY cpq_key;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "System\\ControlSet001\\SessionManager", 0, KEY_READ | KEY_WOW64_64KEY, &cpq_key) == ERROR_SUCCESS)
		{
			a[5]->num = 1;
			LPCTSTR booot[] = { "System\\ControlSet001\\SessionManager" };
			read_value(HKEY_LOCAL_MACHINE, booot, a[5]);
		}
		for (int i = 0; i < a[5]->num; i++)
		{
			if (!a[5]->nempty[i])
			{
				continue;
			}
			if (a[5]->string[i].num <= 0)
			{
				continue;
			}
			for (int j = 0; j < a[5]->string[i].num; j++)
			{
				if (a[5]->string[i].sons[j].Valuename == " ")
				{
					continue;
				}

				LPCTSTR t = a[5]->string[i].sons[j].data.c_str();

				CMFCPropertyGridProperty* tmp = new CMFCPropertyGridProperty(
					_T(a[5]->string[i].sons[j].Valuename),
					_T(t),
					_T(getpublisher(a[5]->string[i].sons[j].data)));
				group5->AddSubItem(tmp);
			}
		}
		HKEY cpr_key;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options", 0, KEY_READ | KEY_WOW64_64KEY, &cpq_key) == ERROR_SUCCESS)
		{
			a[6]->num = 1;
			LPCTSTR hjk[] = { "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options" };
			read_value(HKEY_LOCAL_MACHINE, hjk, a[6]);
		}
		for (int i = 0; i < a[6]->num; i++)
		{
			if (!a[6]->nempty[i])
			{
				continue;
			}
			if (a[6]->string[i].num <= 0)
			{
				continue;
			}
			for (int j = 0; j < a[6]->string[i].num; j++)
			{
				if (a[6]->string[i].sons[j].Valuename == " ")
				{
					continue;
				}

				LPCTSTR t = a[6]->string[i].sons[j].data.c_str();

				CMFCPropertyGridProperty* tmp = new CMFCPropertyGridProperty(
					_T(a[6]->string[i].sons[j].Valuename),
					_T(t),
					_T(getpublisher(a[6]->string[i].sons[j].data)));
				group6->AddSubItem(tmp);
			}
		}
		for (int i = 0; i < a[7]->num; i++)
		{
			if (!a[7]->nempty[i])
			{
				continue;
			}
			if (a[7]->string[i].num <= 0)
			{
				continue;
			}
			for (int j = 0; j < a[7]->string[i].num; j++)
			{
				if (a[7]->string[i].sons[j].Valuename == " ")
				{
					continue;
				}

				LPCTSTR t = a[7]->string[i].sons[j].data.c_str();

				CMFCPropertyGridProperty* tmp = new CMFCPropertyGridProperty(
					_T(a[7]->string[i].sons[j].Valuename),
					_T(t),
					_T(getpublisher(a[7]->string[i].sons[j].data)));
				group7->AddSubItem(tmp);
			}
		}
		for (int i = 0; i < a[8]->num; i++)
		{
			if (!a[8]->nempty[i])
			{
				continue;
			}
			if (a[8]->string[i].num <= 0)
			{
				continue;
			}
			for (int j = 0; j < a[8]->string[i].num; j++)
			{
				if (a[8]->string[i].sons[j].Valuename == " ")
				{
					continue;
				}

				LPCTSTR t = a[8]->string[i].sons[j].data.c_str();

				CMFCPropertyGridProperty* tmp = new CMFCPropertyGridProperty(
					_T(a[8]->string[i].sons[j].Valuename),
					_T(t),
					_T(getpublisher(a[8]->string[i].sons[j].data)));
				group8->AddSubItem(tmp);
			}
		}
		
		for (int i = 0; i < a[9]->num; i++)
		{
			if (!a[9]->nempty[i])
			{
				continue;
			}
			if (a[9]->string[i].num <= 0)
			{
				continue;
			}
			for (int j = 0; j < a[9]->string[i].num; j++)
			{
				if (a[9]->string[i].sons[j].Valuename == " ")
				{
					continue;
				}

				LPCTSTR t = a[9]->string[i].sons[j].data.c_str();

				CMFCPropertyGridProperty* tmp = new CMFCPropertyGridProperty(
					_T(a[9]->string[i].sons[j].Valuename),
					_T(t),
					_T(getpublisher(a[9]->string[i].sons[j].data)));
				group9->AddSubItem(tmp);
			}
		}
		
		
	



	//group1->AddSubItem(pProp1);


	m_PropertyGrid.AddProperty(group1);
	m_PropertyGrid.AddProperty(group2);
	m_PropertyGrid.AddProperty(group3);
	m_PropertyGrid.AddProperty(group4);
	m_PropertyGrid.AddProperty(group5);
	m_PropertyGrid.AddProperty(group6);
	m_PropertyGrid.AddProperty(group7);
	m_PropertyGrid.AddProperty(group8);
	m_PropertyGrid.AddProperty(group9);
	m_PropertyGrid.AddProperty(group10);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAutorunsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutorunsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAutorunsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



