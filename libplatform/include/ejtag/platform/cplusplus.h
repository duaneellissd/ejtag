#if !defined(EJTAG_PLATFORM_CPLUSPLUS_H)
#define EJTAG_PLATFORM_CPLUSPLUS_H
/*
 * Common macros to handle C and C++ extern 
 */

#if !defined(EXTERN)
#define EXTERN extern
#endif

#if defined(__cplusplus)
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END   }
#define EXTERN_C  extern "C"
#else
#define EXTERN_C_BEGIN 
#define EXTERN_C_END 
#define EXTERN_C  extern 
#endif

#endif /* EJTAG_PLATFORM_CPLUSPLUS_H */