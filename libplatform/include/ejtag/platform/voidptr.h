#if !defined(EJTAG_PLATFORM_VOIDPTR_H)
#define EJTAG_PLATFORM_VOIDPTR_H 1

/**
 * \defgroup VoidPointerMath Void Pointer Math
 * @{
 */

/** Void Pointer Math
 *
 * Some compilers - treat void pointers as char pointers
 * and then there are others who think otherwise
 *
 * This is a set of macros that let us add & subtract a void pointer
 *
 *  CVOID is a constant void pointer
 *  VVOID is a volatile void pointer
 * CVVOID is a const volatile void pointer
 *   VOID is a normal   void pointer
 */

#define  _VP_ADD( RESULT_TYPE, CHAR_TYPE, PTR, AMOUNT )     \
      ((RESULT_TYPE *)(  ((CHAR_TYPE *)(PTR)) += (AMOUNT) ))

/**
 * Add an offset to a void pointer
 *
 * \param PTR - the pointer to add to
 * \param AMOUNT - the amount to add (can be negative)
 * 
 * \addtogroup VOID_POINTER_MATH
 */
#define VOID_PTR_ADD( PTR, AMOUNT )   \
    _VP_ADD( void, char, PTR, AMOUNT )


 /**
 * Add an offset to a const void pointer
 *
 * \param PTR - the pointer to add to
 * \param AMOUNT - the amount to add (can be negative)
 *
 * \addtogroup VOID_POINTER_MATH
 */
#define CVOID_PTR_ADD( PTR, AMOUNT )  \
    _VP_ADD( const void, const char, PTR, AMOUNT )


 /**
 * Add an offset to a volatile void pointer
 *
 * \param PTR - the pointer to add to
 * \param AMOUNT - the amount to add (can be negative)
 *
 * \addtogroup VOID_POINTER_MATH
 */
#define VVOID_PTR_ADD( PTR, AMOUNT )  \
    _VP_ADD( volatile void, volatile char, PTR, AMOUNT )


 /**
 * Add an offset to a cont volatile void pointer
 *
 * \param PTR - the pointer to add to
 * \param AMOUNT - the amount to add (can be negative)
 *
 * \addtogroup VOID_POINTER_MATH
 */
#define CVVOID_PTR_ADD( PTR, AMOUNT )  \
    _VP_ADD( const volatile void, const volatile char, PTR, AMOUNT )

/** @} */

#endif
