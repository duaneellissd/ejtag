
#include <stdint.h>


class EjtagPlatform {
public:
	EjtagPlatform(EjtagInstance *pInstance) { mInstance = pInstance; }
	static uint32_t msecs_now(void);
	EjtagInstance *mInstance;
	EjtagBuffer rxBuffer;
	EjtagBuffer txBuffer;

	void txFlush(void);
	bool txIsBusy(void);
	void ioInit(void);
	void ioDisconnect(void);
	bool ioIsCoconnected();
	int  ioConnect(int msecs_timeout);
	int  rxPoll(void);

	int echoEnable(int enabled);
};


