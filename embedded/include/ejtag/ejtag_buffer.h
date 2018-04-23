#include <stdint.h>

/**
 * This is a simple buffer management code
 *
 * Generally we try to keep things to zero copy
 */

class EjtagBuffer {
private:
	/**< Pointer to the buffer data it self */
	uint8_t *mBuffer;

	/**< Where in the buffer the data starts */
	size_t mDataStart;

	/**< End of valid data in the buffer */
	size_t mDataEnd;

	/**< How large is the buffer */
	size_t mLen;
public:

	/* Constructor for a buffer */
	EjtagBuffer(uint8_t *pBuf = NULL, size_t len = 0);

	/** Return how many data bytes are available in the buffer */
	size_t bytesAvail(void);

	/** Return how many data bytes we can still insert */
	size_t spaceAvail(void);

	/** Move the valid data bytes to the front of the buffer */
	void normalize(void);

	/** Get a read pointer into the buffer where next byte comes from */
	void *removePointer(void);

	/** Get pointer to where we would insert data into the buffer */
	void *insertPointer(void);

	/** Actually insert data into the buffer */
	void insertData(const uint8_t *pBuf, size_t nbytes);

	/** Remove data from the front of the buffer */
	void removeData(size_t nbytes);

	/** Find an End Of Line (CR/LF) within the valid data */
	int findEndOfLine(void);

	/** Clear the buffer */
	void reset(void);

	/** Skip over leading white space at start of data */
	void skipWhiteSpace(void);

	/** Get the length in bytes of the buffer memory */
	size_t getLength(void) { return mLen; }

	/** Insert a byte into the buffer memory at current insertion point */
	void insertByte(int v);

};

