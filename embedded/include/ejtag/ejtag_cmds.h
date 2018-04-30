#if !defined(EJTAG_CMDS_H)
#define EJTAG_CMDS_H 1

#include <stdint.h>


/*
 * This builds upon the Xilinx virtual cable protocol.
 * defined here:  https://github.com/Xilinx/XilinxVirtualCable
 *
 * V1 of the Xilinx virtual cable supports these commands:
 *
 * getinfo
 * settck
 * shift
 *
 * by default, this module operates in "xilinx mode"
 * but will try to guess the protocol being used based upon
 * various strings that the host might send.
 *
 * Command:  setmode - sets the command recognition mode.
 *
 *   option:  getinfo	Must occur early in the process, selects XILINX mode
 *   option:  setmode: xilinx\n - limits commands to xilinx mode only.
 *   option:  setmode: ascii\n - changes the command syntax in this way.
 *   option:  $vEjtag: some command #xx  - enables GDB protocol mode.
 *
 * ASCII mode is the prefered method.
 * In ascii mode, the command and response generally have the same format.
 *
 * In ascii mode, the format looks alot like an HTTP or EMAIL header
 * sequence in the following way:
 *
 * Commands are lines of ascii text terminated with a CR, LF, or CR/LF
 * The CR, LF, CR/LF is known as an EOL termination.
 * It is strongly recommend to terminate with both CR/LF but it is not required
 *
 *    Rational: It is easier to debug simple ascii text.
 *
 * Most lines should be less then 80 characters
 * Any line of text longer then 100 bytes may not be ignored and/rejected.
 *   Rational: Commands are easier to log this way.
 *   Embedded targets with limited memory have limited buffer space.
 *
 * Blank lines are ignored and reset the command parser.
 *   Rational: As a human you can connect and hit ENTER a few times
 *   then start typing commands to debug things.
 *
 * The transport is assumed to perfect, and without error
 * all retransmission, timeouts, etc), are handled by the transport layer
 *   Rational: TCP/IP socket stream or USB is assumed
 *
 * The command must start in column 0
 *   Rational: See line continuation below.
 * The command should contain only ASCII characters A-Za-z0-9, and Hyphen.
 * The command must not contain white space, or any other characters.
 * The command is a string terminated with a colon.
 *
 * After the colon:
 *   Option 1 - A terminal EOL sequence
 *   Option 2 - a space character followed by data, and an EOL sequence
 *
 * Binary data if present, should be base64 encoded.
 * Strings should be UTF8 encoded.
 *
 * Line continuation (handling large data transfers)
 *   First base64 encode the data, then break the data
 *   into multiple lines of ascii text, indent each
 *   line of text by using space character in column 0.
 */

namespace Ejtag {

}
#define CMD_MAX_LINE  128

class CmdInfo;

/* all incomming commands are processed via table look up */
struct cmd_table_entry {
	/* the command to match */
	const char *cmd;

	/* for use by the command handler */
	intptr_t cookie;


	/* the command handler */
	void (*cmd_callback)(EjtagCmdInfo *pInfo);
};

class Foo;

class Bar {
public:
	Bar(Foo *p) { mFoo = p; }

	Foo *mFoo;


};


class EjtagInstance;

#define CMD_MEM_SIZE 256
class EjtagCmdInfo {
public:
	EjtagCmdInfo( EjtagInstance *pInstance );
	EjtagInstance *mInstance;
	const struct cmd_table_entry *mCmdEntry;
	enum CmdState {
		Disconnect = 0,
		Start = 1,
		Data = 2,
		Base64 =3,
		Binary = 4,
		End = 100
	};
	CmdState      mState;
	/* memory for the command to use */
	uint64_t mCmdMemory[(CMD_MEM_SIZE / sizeof(uint64_t))];
	uint32_t    mRxExpected;
	EjtagBuffer mRxBuffer;
	uint8_t     decode_buffer[128];
	int mLastBytesAvail;
	void reset(void);

	void expectBinaryData(uint32_t nBytes);
	void expectBase64Data(uint32_t nBytes);
	void expectData(void);
};

#define CMD_STATE_START      0
#define CMD_STATE_DATA       1
#define CMD_STATE_BASE64     2
#define CMD_STATE_END        -1
#define CMD_STATE_DISCONNECT -2


/**
 * Array (in flashrom) of supported commands
 * Terminated with 'handler=NULL'
 */
extern const cmd_table_entry cmd_table[];

class EjtagCommandProcessor {
public:
	EjtagInstance *mInstance;
	EjtagCommandProcessor(EjtagInstance *p);
	enum Mode {
		unknown = 0,
		xilinx = 1,
		ascii  = 2,
		gdb    = 3,
	};
	Mode mMode;

	EjtagCmdInfo mCurCmd;
	void disconnect(void);
	void finishCmd(void);
	void parseCmd(void);
private:
	void parseCmd_unknown(void);
	void parseCmd_xilinx(void);
	void parseCmd_gdb(void);
	void parseCmd_ascii();

};


#endif
