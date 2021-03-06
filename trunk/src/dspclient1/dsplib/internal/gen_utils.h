/** ============================================================================
 *  @file   gen_utils.h
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Platform independent common library function interface.
 *
 *  @ver    1.64
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


#if !defined (GEN_H)
#define GEN_H

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   GEN_Initialize
 *
 *  @desc   Initializes the GEN module's private state.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *
 *  @enter  None
 *
 *  @leave  Subcomponent is initialized.
 *
 *  @see    GEN_Finalize
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Initialize (Void) ;


/** ============================================================================
 *  @func   GEN_Finalize
 *
 *  @desc   Discontinues usage of the GEN module.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *
 *  @enter  Subcomponent must be initialized.
 *
 *  @leave  None
 *
 *  @see    GEN_Initialize
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Finalize (Void) ;


/** ============================================================================
 *  @func   GEN_NumToAscii
 *
 *  @desc   Converts a 1 or 2 digit number to a 2 digit string.
 *
 *  @arg    number
 *              Number to convert.
 *  @arg    strNumber
 *              Buffer to store converted string.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *
 *  @enter  Subcomponent must be initialized.
 *          The number to convert must be between 0 and 99, both numbers
 *          included.
 *          The buffer to store output string must be valid.
 *
 *  @leave  The buffer to store output string is valid.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_NumToAscii (IN Uint32 number, OUT Char8 * strNumber) ;


/** ============================================================================
 *  @func   GEN_Strcmp
 *
 *  @desc   Compares 2 ASCII strings.  Works the same way as stdio's strcmp.
 *
 *  @arg    string1
 *              First string for comparison.
 *  @arg    string2
 *              Second string for comparison.
 *  @arg    cmpResult
 *              Result of comparision (zero = equal, non-zero otherwise).
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *
 *  @enter  Subcomponent must be initialized.
 *          The buffer to store first string must be valid.
 *          The buffer to store second string must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Strcmp (IN  CONST Char8 * string1,
            IN  CONST Char8 * string2,
            OUT       Int32 * cmpResult) ;


/** ============================================================================
 *  @func   GEN_Strcpyn
 *
 *  @desc   Safe strcpy function.
 *
 *  @arg    destination
 *              destination buffer.
 *  @arg    source
 *              Source buffer.
 *  @arg    maxNum
 *              Number of characters to copy.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *
 *  @enter  Subcomponent must be initialized.
 *          The destination buffer must be valid.
 *          The source buffer must be valid.
 *          The number of characters to copy must be more than zero.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Strcpyn (OUT Char8 * destination,
             IN  Char8 * source,
             IN  Int32   maxNum) ;


/** ============================================================================
 *  @func   GEN_Strlen
 *
 *  @desc   Determines the length of a null terminated ASCI string.
 *
 *  @arg    strSrc
 *              Pointer to string.
 *  @arg    length
 *              Out parameter to hold the length of string.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *
 *  @enter  Subcomponent must be initialized.
 *          The pointer to the string buffer must be valid.
 *          The pointer to the length field must be valid.
 *
 *  @leave  The pointer to the length field is valid.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Strlen (IN CONST Char8 * strSrc, OUT Uint32 * length) ;


/** ============================================================================
 *  @func   GEN_WcharToAnsi
 *
 *  @desc   Converts a wide char string to an ansi string.
 *
 *  @arg    destination
 *              Destination buffer.
 *  @arg    source
 *              Source buffer.
 *  @arg    numChars
 *              Number of characters (Wide chars) to be converted/copied.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *
 *  @enter  Subcomponent must be initialized.
 *          The destination buffer must be valid.
 *          The source buffer must be valid.
 *          The number of characters to be converted/copied must be greater
 *          than 0.
 *
 *  @leave  The destination buffer is valid.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_WcharToAnsi (OUT Char8  * destination,
                 IN  Char16 * source,
                 IN  Int32    numChars) ;


/** ============================================================================
 *  @func   GEN_AnsiToWchar
 *
 *  @desc   Converts an ANSI string to a wide char string.
 *
 *  @arg    destination
 *              Destination buffer.
 *  @arg    source
 *              Source buffer.
 *  @arg    numChars
 *              Number of characters (Ansi) to be converted/copied.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *
 *  @enter  Subcomponent must be initialized.
 *          The destination buffer must be valid.
 *          The source buffer must be valid.
 *          The number of characters to be converted/copied must be greater
 *          than 0.
 *
 *  @leave  The destination buffer is valid.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_AnsiToWchar (OUT Char16 * destination,
                 IN  Char8 *  source,
                 IN  Int32    numChars) ;


/** ============================================================================
 *  @func   GEN_Wstrlen
 *
 *  @desc   Determines the length of a null terminated wide character string.
 *
 *  @arg    strSrc
 *              pointer to string.
 *  @arg    length
 *              Length of string.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *
 *  @enter  Subcomponent must be initialized.
 *          The pointer to the string buffer must be valid.
 *          The pointer to length of buffer must be valid.
 *
 *  @leave  The pointer to length of buffer is valid.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Wstrlen (IN Char16 * strSrc, IN Uint32 * length) ;


/** ============================================================================
 *  @func   GEN_Strcatn
 *
 *  @desc   Safe strcat function.
 *
 *  @arg    destination
 *              destination buffer.
 *  @arg    source
 *              Source buffer.
 *  @arg    maxNum
 *              Maximum length of the destination buffer after concatenation.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *
 *  @enter  The destination buffer must be valid.
 *          The source buffer must be valid.
 *          The number of characters to copy must be more than zero.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Strcatn (OUT Char8 * destination,
             IN  Char8 * source,
             IN  Int32   maxNum) ;

/** ============================================================================
 *  @func   GEN_Strcmpn
 *
 *  @desc   Safe strcmp function.
 *
 *  @arg    str1
 *              Source string1.
 *  @arg    str2
 *              Source string2.
 *  @arg    numChars
 *               Number of characters.
 *  @arg    cmpResult
 *              Result of comparison (zero = equal, non-zero otherwise).
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *
 *  @enter  The string buffer must be valid.
 *          The number of characters to copy must be more than zero.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Strcmpn (IN  Char8 * str1,
             IN  Char8 * str2,
             IN  Int32   numChars,
             OUT Int32 * cmpResult);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (GEN_H) */

