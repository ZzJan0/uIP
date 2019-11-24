#ifndef	_UIP_APPCALL_H
#define _UIP_APPCALL_H

#include "uipopt.h"
#include "psock.h"

#define UIP_APPCALL uIP_AppCall

/* �û�������Ҫ������ӵĳ�Ա��������¼һЩ��Ϣ */
struct tcp_appstate
{
	uint8_t status;
	uint8_t *ptxdat;
	uint16_t txlen;
};

typedef struct tcp_appstate uip_tcp_appstate_t;

void uIP_AppCall(void);

#endif
