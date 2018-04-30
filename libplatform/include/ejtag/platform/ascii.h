#if !defined(EJTAG_PLATFOR_ASCII_H)
#define EJTAG_PLATFOR_ASCII_H

#define ASCII_NUL	0x000
#define ASCII_NULL	(ASCII_NUL) /* alternate spelling */
#define ASCII_CTRL_ATSIGN  ASCII_NULL

#define ASCII_SOH		   0x001
#define ASCII_CTRL_A       ASCII_SOH

#define ASCII_STX		   0x002
#define ASCII_CTRL_B	   ASCII_STX

#define ASCII_ETX		   0x003
#define ASCII_CTRL_C	   ASCII_ETX

#define ASCII_EOT		   0x004
#define ASCII_CTRL_D	   ASCII_EOT

#define ASCII_ENQ		   0x005
#define ASCII_CTRL_E	   ASCII_ENQ

#define ASCII_ACK		   0x006
#define ASCII_CTRL_F	   ASCII_ACK

#define ASCII_BEL		   0x007
#define ASCII_BELL		   ASCII_BEL
#define ASCII_CTRL_G	   ASCII_BEL

#define ASCII_BS		   0x008
#define ASCII_BACK_SPACE  ASCII_BS
#define ASCII_CTRL_H	  ASCII_BS

#define ASCII_HT		   0x009
#define ASCII_TAB		   ASCII_HT
#define ASCII_CTRL_I	   ASCII_HT

#define ASCII_LF		   0x00a
#define ACIII_LINEFEED	   ASCII_LF
#define ASCII_CTRL_J	   ASCII_LF

#define ASCII_VT		   0x00b
#define ASCII_VERT_TAB     ASCII_VT
#define ASCII_VERTICAL_TAB ASCII_VT
#define ASCII_CTRL_K	   ASCII_VT

#define ASCII_FF		   0x00c
#define ASCII_FORM_FEED    ASCII_FF
#define ASCII_CTRL_L       ASCII_FF

#define ASCII_CR		   0x00d
#define ASCII_CARRIAGE_RET ASCII_CR
#define ASCII_CARRIAGE_RETURN ASCII_CR
#define ASCII_CTRL_M       ASCII_CR

#define ASCII_SO           0x00e
#define ASCII_SHIFT_OUT    ASCII_SO
#define ASCII_CTRL_N       ASCII_SO

#define ASCII_SI           0x00f
#define ASCII_SHIFT_IN     ASCII_SI
#define ASCII_CTRL_O       ASCII_SI

#define ASCII_DLE          0x010
#define ASCII_CTRL_P       ASCII_DLE

#define ASCII_DC1          0x011
#define ASCII_XON          ASCII_DC1
#define ASCII_CTRL_Q       ASCII_DC1

#define ASCII_DC2          0x012
#define ASCII_CTRL_R       ASCII_DC2

#define ASCII_DC3          0x013
#define ASCII_XOFF         ASCII_DC3
#define ASCII_CTRL_S       ASCII_DC3

#define ASCII_DC4          0x014
#define ASCII_CTRL_T       ASCII_DC4

#define ASCII_NAK	       0x015
#define ASCII_CTRL_U       ASCII_NAK

#define ASCII_SYN		   0x016
#define ASCII_CTRL_V       ASCII_SYN

#define ASCII_ETB          0x017
#define ASCII_CTRL_W       ASCII_ETB

#define ASCII_CAN          0x018
#define ASCII_CTRL_X       ASCII_CAN

#define ASCII_EM           0x019
#define ASCII_CTRL_Y       ASCII_EM

#define ASCII_SUB          0x01a
#define ASCII_CTRL_Z       ASCII_SUB

#define ASCII_ESC          0x01b
#define ASCII_CTRL_LBRACKET ASCII_ESC

#define ASCII_FS           0x01c
#define ASCII_CTRL_BACKSLASH ASCII_FS

#define ASCII_GS           0x01d
#define ASCII_CTRL_RBRACKET ASCII_GS

#define ASCII_RS           0x01e
/* no real control key? */

#define ASCII_US           0x01f


#define ASCII_DEL          0x07f


#endif

