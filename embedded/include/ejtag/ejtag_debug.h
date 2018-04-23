
#include <stdio.h>
#include <stdint.h>


class EjtagDebug {
private:
	bool need_now;
	void write_check(const char *fmt, size_t n);
	EjtagInstance *mInstance;
public:
	EjtagDebug(EjtagInstance *pInstance = NULL);
	void setInstance(EjtagInstance *pInstance);
	enum DbgArea {
		Generic = 0,
		Errors = 1,
		IoBuffers = 2,
	};
	static const char *area2name(enum DbgArea area);
	void now(void);
	int printf(EjtagDebug::DbgArea area, const char *fmt, ...);
	int vprintf(EjtagDebug::DbgArea area, const char *fmt, va_list ap);

	int  kbhit(void);
	int  getkey(void);
	void write(const char *buf, int n);
	void putc_raw(int c);
	void putc(int c);
	void puts_no_nl(const char *s);
	void puts(const char *s);


	void bar(void);
	void hyphen(void);
	void space(void);
	void colon_space(void);
	void colon(void);
	void newline(void);
	
	void hex8(uint32_t value);
	void hex16(uint32_t value);
	void hex32(uint32_t value);
	void hex64(uint64_t value);
	void integer(int integer);

	void str_hex8(const char *str, uint32_t value);
	void str_hex16(const char *str, uint32_t value);
	void str_hex32(const char *str, uint32_t value);
	void str_hex64(const char *str, uint64_t value);

	void assertFails(const char *filename, int lineno, const char *msg);
};

extern EjtagDebug dbg;
