#ifndef _LOGON_HEADER_H_
#define _LOGON_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#define INVALID_LOGON  (int)0

typedef struct _run_t
{
	char name[256];
	char image_path[512];
}run_t;



#ifdef __cplusplus
};
#endif

#endif