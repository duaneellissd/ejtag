#include "ejtag/platform/config.h"

/**
 * Simple Circular FIFO type buffer management class.
 *
 * Support zero-copy and direct access to the buffer.
 *
 * Note this is not a circular buffer, it is array based.
 * periodically you must either normalize the data
 * or consume all of the data.
 *
 * The normal case is to consume all of the data.
 *
 * On a embedded USB device, the idea & intent is to
 * point the mBuffer element at the endpoint rx buffer
 * 
 * Thus, often the data is fully consumed and the
 * buffer will 'self-normalize'
 */

namespace Ejtag {

	class Fifo {
	private:
		/**< Pointer to the buffer data it self */
		uint8_t * mBuffer;

		/**< Where in the buffer the data starts */
		size_t mRd;

		/**< End of valid data in the buffer */
		size_t mWr;

		/**< How large is the buffer */
		size_t mLen;

		/**< How many elements are in the buffer */
	public:

		/* Constructor for a buffer */
		Fifo(uint8_t *pBuf = NULL, size_t len = 0);

		/** Return how many data items are available in the buffer */
		size_t availableSpace(void);

		/** IRQ protected version of availableSpace() */
		size_t safeAvailableSpace(void);

		/** Return how many data bytes we can still insert */
		size_t availableData(void);

		/** IRQ protected version of availableSpace() */
		size_t safeAvailableData(void);


		/** Move the valid data bytes to the front of the buffer */
		void normalize(void);

		/** IRQ protected version of normalize*) */
		void safeNormalize(void);

		/** Get direct access to the data present in the buffer thus allowing DMA access
		 * @param ppRd - where to store the read pointer
		 * @param n    - where to store the count of available bytes
		 */
		int dmaRemovePointer(uint8_t **ppRd, int *n);

		/** Call this after performing DMA operations to insert into the buffer
		 *
		 * @param count - how many bytes where transfered
		 *
		 * This call can be skipped if zero bytes where transferred.
		 */
		void dmaRemoveComplete(int count);

		/** Get direct access to empty data buffer area allowing DMA access
		 * @param - ppWr - where to write data into the buffer.
		 * @param - n - how much space is available to write data to.
		 */
		int dmaInsertPointer(uint8_t **ppWr, int *n);

		/** Complete a DMA insertion process
		 *
		 * @param count - how many bytes where transfered
		 *
		 * This call can be skipped if zero bytes where transferred.
		 */
		void dmaInsertComplete(int count);

		/**
		 * Insert data into the buffer
		 *
		 * @param pData - pointer to data to insert.
		 * @param count - count of data to insert
		 *
		 * @return - number of bytes actually transferred
		 */
		int insertData(const uint8_t *pData, size_t count);

		/** IRQ safe version of insertData() */
		int safeInsertData(const uint8_t *pData, size_t cnt);

		/** Remove data from the buffer
		 *
		 * @param pData - where to put the data that is removed.
		 * @param count   - how many bytes to remove
		 *
		 * @return number of bytes actually transferred.
		 */
		int removeData(uint8_t *pData, size_t count);

		/** IRQ Safe version of removeData*) */
		int safeRemoveData(uint8_t *pData, size_t nbytes);

		/**
		 * Clear the buffer dropping all data
		 */
		void reset(void);

		/** safe version of reset() */
		void safeReset(void);
		/* -- parser help functions */

		/**
		 * Search forward for an EOL sequence
		 *
		 * @return 0..N (positive) if found, location where the EOL is located
		 * @return EOF if not found
		 */
		int findEndOfLine(void);

		/** Skip over leading white space at start of data */
		void skipWhiteSpace(void);

		/** Insert a single byte into the buffer */
		void insertByte(int value);

		/* Remove a single byte from buffer
		 *
		 * @returns 0x00 to 0xff if data is ready
		 * @returns EOF if no data available.
		 */
		int  removeByte(void);

		/**
		 * Reverse a removeByte() operation.
		 *
		 * @returns 0..0xff (the byte) if successful.
		 * @returns EOF if not successful (buffer full)
		 */
		int  ungetByte(int value);

	};


}