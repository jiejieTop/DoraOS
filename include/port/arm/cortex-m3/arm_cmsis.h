#ifndef _ARM_CMSIS_H_
#define _ARM_CMSIS_H_

#ifndef   __ASM
  #define __ASM                     __asm
#endif
#ifndef   __INLINE
  #define __INLINE                  __inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE           static __inline
#endif
#ifndef   __NO_RETURN
  #define __NO_RETURN               __declspec(noreturn)
#endif
#ifndef   __USED
  #define __USED                    __attribute__((used))
#endif
#ifndef   __WEAK
  #define __WEAK                    __attribute__((weak))
#endif
#ifndef   __UNALIGNED_UINT32
  #define __UNALIGNED_UINT32(x)     (*((__packed uint32_t *)(x)))
#endif
#ifndef   __ALIGNED
  #define __ALIGNED(x)              __attribute__((aligned(x)))
#endif
#ifndef   __PACKED
  #define __PACKED                  __attribute__((packed))
#endif
#ifndef   __PACKED_STRUCT
  #define __PACKED_STRUCT           __packed struct
#endif


/**
 * Get IPSR Register 
 * Returns the content of the IPSR Register.
 * return IPSR Register value
 */
__STATIC_INLINE dos_uint32 __get_IPSR(void)
{
  register dos_uint32 __regIPSR          __ASM("ipsr");
  return(__regIPSR);
}


/**
 * Get APSR Register
 * Returns the content of the APSR Register.
 * return APSR Register value
 */
__STATIC_INLINE dos_uint32 __get_APSR(void)
{
  register dos_uint32 __regAPSR          __ASM("apsr");
  return(__regAPSR);
}


/**
 * Get xPSR Register
 * Returns the content of the xPSR Register.
 * return xPSR Register value
 */
__STATIC_INLINE dos_uint32 __get_xPSR(void)
{
  register dos_uint32 __regXPSR          __ASM("xpsr");
  return(__regXPSR);
}


/**
 * Get Process Stack Pointer
 * Returns the current value of the Process Stack Pointer (PSP).
 * return PSP Register value
 */
__STATIC_INLINE dos_uint32 __get_PSP(void)
{
  register dos_uint32 __regProcessStackPointer  __ASM("psp");
  return(__regProcessStackPointer);
}


/**
 * Set Process Stack Pointer
 * Assigns the given value to the Process Stack Pointer (PSP).
 * @param [in]  topOfProcStack  Process Stack Pointer value to set
 */
__STATIC_INLINE void __set_PSP(dos_uint32 topOfProcStack)
{
  register dos_uint32 __regProcessStackPointer  __ASM("psp");
  __regProcessStackPointer = topOfProcStack;
}


/**
 * Get Main Stack Pointer
 * Returns the current value of the Main Stack Pointer (MSP).
 * return MSP Register value
 */
__STATIC_INLINE dos_uint32 __get_MSP(void)
{
  register dos_uint32 __regMainStackPointer     __ASM("msp");
  return(__regMainStackPointer);
}


/**
 * Set Main Stack Pointer
 * Assigns the given value to the Main Stack Pointer (MSP).
 * @param [in] topOfMainStack  Main Stack Pointer value to set
 */
__STATIC_INLINE void __set_MSP(dos_uint32 topOfMainStack)
{
  register dos_uint32 __regMainStackPointer     __ASM("msp");
  __regMainStackPointer = topOfMainStack;
}


#endif  /**_ARM_CMSIS_H_ */

