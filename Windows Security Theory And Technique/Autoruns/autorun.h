#ifdef  __AUTORUN_H_LHW_202061__
#define __AUTORUN_H_LHW_202061__

#ifdef __cplusplus
extern "C" {
#endif

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

typedef uint32_t	AUTORUN_MASK;

//自启动项
typedef struct _autoitem_t
{
	AUTORUN_MASK type;
	char Name[512];					//名称
	char ImagePath[512];			//文件路径
	char Paramter[512];				//参数
}autoitem_t;

int autorun_query(AUTORUN_MASK mask);

autoitem_t* autorun_first(int fd);

autoitem_t* autorun_next(autoitem_t* iter);

int autorun_close(int fd);

//自启动项回调函数
typedef BOOL ( *AUTORUN_CALLBACK )( AUTORUN_MASK Mask, PAUTORUN_ITEM pItem, LPVOID lpParam );

//枚举相关的自启动项
BOOL	EnumAutorun( AUTORUN_MASK Mask,  AUTORUN_CALLBACK pfnCallback,  LPVOID lpParam );

//获取自启动项的名称
LPCTSTR GetAutorunName( AUTORUN_MASK Mask );

// 下面实现的是非正常加载驱动程序
// 注意：使用本函数前，需要提升权限
// sysPath是全路径，包括文件名称
error_t	RegLoadDriver(
    char *sysName, 
    char *sysPath, 
    OUT HANDLE *lpHandle, 
		OUT dword_t *dwErrorCode );

error_t	RegUnloadDriver( char *sysName, char *sysPath, OUT DWORD *dwErrorCode );

#ifdef __cplusplus
}
#endif

#endif			//__AUTORUN_H_LHW_202061__