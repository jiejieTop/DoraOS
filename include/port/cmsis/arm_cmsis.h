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


///**
//  \brief   Get Control Register
//  \details Returns the content of the Control Register.
//  \return               Control Register value
// */
//__STATIC_INLINE uint32_t __get_CONTROL(void)
//{
//  register uint32_t __regControl         __ASM("control");
//  return(__regControl);
//}


///**
//  \brief   Set Control Register
//  \details Writes the given value to the Control Register.
//  \param [in]    control  Control Register value to set
// */
//__STATIC_INLINE void __set_CONTROL(uint32_t control)
//{
//  register uint32_t __regControl         __ASM("control");
//  __regControl = control;
//}


/**
  \brief   Get IPSR Register
  \details Returns the content of the IPSR Register.
  \return               IPSR Register value
 */
__STATIC_INLINE uint32_t __get_IPSR(void)
{
  register uint32_t __regIPSR          __ASM("ipsr");
  return(__regIPSR);
}


/**
  \brief   Get APSR Register
  \details Returns the content of the APSR Register.
  \return               APSR Register value
 */
__STATIC_INLINE uint32_t __get_APSR(void)
{
  register uint32_t __regAPSR          __ASM("apsr");
  return(__regAPSR);
}


/**
  \brief   Get xPSR Register
  \details Returns the content of the xPSR Register.
  \return               xPSR Register value
 */
__STATIC_INLINE uint32_t __get_xPSR(void)
{
  register uint32_t __regXPSR          __ASM("xpsr");
  return(__regXPSR);
}


/**
  \brief   Get Process Stack Pointer
  \details Returns the current value of the Process Stack Pointer (PSP).
  \return               PSP Register value
 */
__STATIC_INLINE uint32_t __get_PSP(void)
{
  register uint32_t __regProcessStackPointer  __ASM("psp");
  return(__regProcessStackPointer);
}


/**
  \brief   Set Process Stack Pointer
  \details Assigns the given value to the Process Stack Pointer (PSP).
  \param [in]    topOfProcStack  Process Stack Pointer value to set
 */
__STATIC_INLINE void __set_PSP(uint32_t topOfProcStack)
{
  register uint32_t __regProcessStackPointer  __ASM("psp");
  __regProcessStackPointer = topOfProcStack;
}


/**
  \brief   Get Main Stack Pointer
  \details Returns the current value of the Main Stack Pointer (MSP).
  \return               MSP Register value
 */
__STATIC_INLINE uint32_t __get_MSP(void)
{
  register uint32_t __regMainStackPointer     __ASM("msp");
  return(__regMainStackPointer);
}


/**
  \brief   Set Main Stack Pointer
  \details Assigns the given value to the Main Stack Pointer (MSP).
  \param [in]    topOfMainStack  Main Stack Pointer value to set
 */
__STATIC_INLINE void __set_MSP(uint32_t topOfMainStack)
{
  register uint32_t __regMainStackPointer     __ASM("msp");
  __regMainStackPointer = topOfMainStack;
}


///**
//  \brief   Get Priority Mask
//  \details Returns the current state of the priority mask bit from the Priority Mask Register.
//  \return               Priority Mask value
// */
//__STATIC_INLINE uint32_t __get_PRIMASK(void)
//{
//  register uint32_t __regPriMask         __ASM("primask");
//  return(__regPriMask);
//}


///**
//  \brief   Set Priority Mask
//  \details Assigns the given value to the Priority Mask Register.
//  \param [in]    priMask  Priority Mask
// */
//__STATIC_INLINE void __set_PRIMASK(uint32_t priMask)
//{
//  register uint32_t __regPriMask         __ASM("primask");
//  __regPriMask = (priMask);
//}


#endif  /**_ARM_CMSIS_H_ */

