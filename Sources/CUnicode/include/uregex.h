/*
**********************************************************************
*   Copyright (C) 2004-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ifndef UREGEX_H
#define UREGEX_H

#include <stddef.h>
#include <stdint.h>
#include "uerror.h"
#include "utext.h"

#ifndef CF_OPTIONS
#define CF_OPTIONS(_type, _name) enum _name : _type _name; enum _name : _type
#endif

#pragma clang assume_nonnull begin

/**
  * Structure representing a compiled regular expression, plus the results
  *    of a match operation.
  * @stable ICU 3.0
  */
typedef struct _URegularExpression {} URegularExpression;

/**
 * Constants for Regular Expression Match Modes.
 * @stable ICU 2.4
 */
typedef CF_OPTIONS(unsigned, URegexpFlag) {
    /**  Enable case insensitive matching.  @stable ICU 2.4 */
    UREGEX_CASE_INSENSITIVE U_SWIFT_NAME(caseInsensitive) = 2,

    /**  Allow white space and comments within patterns  @stable ICU 2.4 */
    UREGEX_COMMENTS U_SWIFT_NAME(allowCommentsAndWhitespace) = 4,

    /**  If set, '.' matches line terminators,  otherwise '.' matching stops at line end.
      *  @stable ICU 2.4 */
    UREGEX_DOTALL U_SWIFT_NAME(dotMatchesLineSeparators) = 32,
    
    /**  If set, treat the entire pattern as a literal string.  
      *  Metacharacters or escape sequences in the input sequence will be given 
      *  no special meaning. 
      *
      *  The flag UREGEX_CASE_INSENSITIVE retains its impact
      *  on matching when used in conjunction with this flag.
      *  The other flags become superfluous.
      *
      * @stable ICU 4.0
      */
    UREGEX_LITERAL U_SWIFT_NAME(ignoreMetacharacters) = 16,

    /**   Control behavior of "$" and "^"
      *    If set, recognize line terminators within string,
      *    otherwise, match only at start and end of input string.
      *   @stable ICU 2.4 */
    UREGEX_MULTILINE U_SWIFT_NAME(anchorsMatchLines) = 8,
    
    /**   Unix-only line endings.
      *   When this mode is enabled, only \\u000a is recognized as a line ending
      *    in the behavior of ., ^, and $.
      *   @stable ICU 4.0
      */
    UREGEX_UNIX_LINES U_SWIFT_NAME(useUnixLineSeparators) = 1,

    /**  Unicode word boundaries.
      *     If set, \b uses the Unicode TR 29 definition of word boundaries.
      *     Warning: Unicode word boundaries are quite different from
      *     traditional regular expression word boundaries.  See
      *     http://unicode.org/reports/tr29/#Word_Boundaries
      *     @stable ICU 2.8
      */
    UREGEX_UWORD U_SWIFT_NAME(useUnicodeWordBoundaries) = 256,

     /**  Error on Unrecognized backslash escapes.
       *     If set, fail with an error on patterns that contain
       *     backslash-escaped ASCII letters without a known special
       *     meaning.  If this flag is not set, these
       *     escaped letters represent themselves.
       *     @stable ICU 4.0
       */
     UREGEX_ERROR_ON_UNKNOWN_ESCAPES U_SWIFT_NAME(failOnUnknownEscapes) = 512
}  U_SWIFT_NAME(URegularExpression.Options);

/**
  *  Open (compile) an ICU regular expression.  Compiles the regular expression in
  *  string form into an internal representation using the specified match mode flags.
  *  The resulting regular expression handle can then be used to perform various
  *   matching operations.
  * 
  *
  * @param pattern        The Regular Expression pattern to be compiled. 
  * @param patternLength  The length of the pattern, or -1 if the pattern is
  *                       NUL terminated.
  * @param flags          Flags that alter the default matching behavior for
  *                       the regular expression, UREGEX_CASE_INSENSITIVE, for
  *                       example.  For default behavior, set this parameter to zero.
  *                       See <code>enum URegexpFlag</code>.  All desired flags
  *                       are bitwise-ORed together.
  * @param pe             Receives the position (line and column numbers) of any syntax
  *                       error within the source regular expression string.  If this
  *                       information is not wanted, pass NULL for this parameter.
  * @param status         Receives error detected by this function.
  * @stable ICU 3.0
  *
  */
extern U_SWIFT_NAME(URegularExpression.open(patternString:length:options:errorDetails:status:))
URegularExpression *_Nullable
uregex_open( const  uint16_t       *_Nullable pattern,
                    int32_t        patternLength,
                    URegexpFlag    flags,
                    UParseError    *_Nullable pe,
                    UErrorCode     *status);

/**
  *  Open (compile) an ICU regular expression.  Compiles the regular expression in
  *  string form into an internal representation using the specified match mode flags.
  *  The resulting regular expression handle can then be used to perform various
  *   matching operations.
  *  <p>
  *  The contents of the pattern UText will be extracted and saved. Ownership of the
  *   UText struct itself remains with the caller. This is to match the behavior of
  *   uregex_open().
  *
  * @param pattern        The Regular Expression pattern to be compiled. 
  * @param flags          Flags that alter the default matching behavior for
  *                       the regular expression, UREGEX_CASE_INSENSITIVE, for
  *                       example.  For default behavior, set this parameter to zero.
  *                       See <code>enum URegexpFlag</code>.  All desired flags
  *                       are bitwise-ORed together.
  * @param pe             Receives the position (line and column numbers) of any syntax
  *                       error within the source regular expression string.  If this
  *                       information is not wanted, pass NULL for this parameter.
  * @param status         Receives error detected by this function.
  *
  * @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.open(pattern:options:errorDetails:status:))
URegularExpression *_Nullable
uregex_openUText(UText          *_Nullable pattern,
                 URegexpFlag    flags,
                 UParseError    *_Nullable pe,
                 UErrorCode     *status);

/**
  *  Open (compile) an ICU regular expression.  The resulting regular expression
  *   handle can then be used to perform various matching operations.
  *  <p>
  *   This function is the same as uregex_open, except that the pattern
  *   is supplied as an 8 bit char * string in the default code page.
  *
  * @param pattern        The Regular Expression pattern to be compiled, 
  *                       NUL terminated.  
  * @param flags          Flags that alter the default matching behavior for
  *                       the regular expression, UREGEX_CASE_INSENSITIVE, for
  *                       example.  For default behavior, set this parameter to zero.
  *                       See <code>enum URegexpFlag</code>.  All desired flags
  *                       are bitwise-ORed together.
  * @param pe             Receives the position (line and column numbers) of any syntax
  *                       error within the source regular expression string.  If this
  *                       information is not wanted, pass NULL for this parameter.
  * @param status         Receives errors detected by this function.
  * @return               The URegularExpression object representing the compiled
  *                       pattern.
  *
  * @stable ICU 3.0
  */
extern U_SWIFT_NAME(URegularExpression.open(cString:options:errorDetails:status:))
URegularExpression *_Nullable
uregex_openC( const uint8_t        *_Nullable pattern,
                    URegexpFlag    flags,
                    UParseError    *_Nullable pe,
                    UErrorCode     *status);

/**
  *  Close the regular expression, recovering all resources (memory) it
  *   was holding.
  *
  * @param regexp   The regular expression to be closed.
  * @stable ICU 3.0
  */
extern U_SWIFT_NAME(URegularExpression.close(self:))
void
uregex_close(URegularExpression *regexp);

/**
 * Make a copy of a compiled regular expression.  Cloning a regular
 * expression is faster than opening a second instance from the source
 * form of the expression, and requires less memory.
 * <p>
 * Note that the current input string and the position of any matched text
 *  within it are not cloned; only the pattern itself and the
 *  match mode flags are copied.
 * <p>
 * Cloning can be particularly useful to threaded applications that perform
 * multiple match operations in parallel.  Each concurrent RE
 * operation requires its own instance of a URegularExpression.
 *
 * @param regexp   The compiled regular expression to be cloned.
 * @param status   Receives indication of any errors encountered
 * @return the cloned copy of the compiled regular expression.
 * @stable ICU 3.0
 */
extern U_SWIFT_NAME(URegularExpression.clone(self:status:))
URegularExpression *_Nullable
uregex_clone(const URegularExpression *regexp, UErrorCode *status);

/**
 *  Returns a pointer to the source form of the pattern for this regular expression.
 *  This function will work even if the pattern was originally specified as a UText.
 *
 * @param regexp     The compiled regular expression.
 * @param patLength  This output parameter will be set to the length of the
 *                   pattern string.  A NULL pointer may be used here if the
 *                   pattern length is not needed, as would be the case if
 *                   the pattern is known in advance to be a NUL terminated
 *                   string.
 * @param status     Receives errors detected by this function.
 * @return a pointer to the pattern string.  The storage for the string is
 *                   owned by the regular expression object, and must not be
 *                   altered or deleted by the application.  The returned string
 *                   will remain valid until the regular expression is closed.
 * @stable ICU 3.0
 */
extern U_SWIFT_NAME(URegularExpression.getPatternPointer(self:length:status:))
const uint16_t *
uregex_pattern(const URegularExpression *regexp,
                     int32_t            *patLength,
                     UErrorCode         *status);

/**
 *  Returns the source text of the pattern for this regular expression.
 *  This function will work even if the pattern was originally specified as a UChar string.
 *
 * @param regexp     The compiled regular expression.
 * @param status     Receives errors detected by this function.
 * @return the pattern text.  The storage for the text is owned by the regular expression
 *                   object, and must not be altered or deleted.
 *
 * @stable ICU 4.6
 */
extern U_SWIFT_NAME(URegularExpression.getPattern(self:status:))
UText *
uregex_patternUText(const URegularExpression *regexp,
                          UErrorCode         *status);

/**
  * Get the match mode flags that were specified when compiling this regular expression.
  * @param status   Receives errors detected by this function.
  * @param regexp   The compiled regular expression.
  * @return         The match mode flags
  * @see URegexpFlag
  * @stable ICU 3.0
  */
extern U_SWIFT_NAME(URegularExpression.getOptions(self:status:))
URegexpFlag
uregex_flags(const  URegularExpression   *regexp,
                    UErrorCode           *status);


/**
  *  Set the subject text string upon which the regular expression will look for matches.
  *  This function may be called any number of times, allowing the regular
  *  expression pattern to be applied to different strings.
  *  <p>
  *  Regular expression matching operations work directly on the application's
  *  string data.  No copy is made.  The subject string data must not be
  *  altered after calling this function until after all regular expression
  *  operations involving this string data are completed.  
  *  <p>
  *  Zero length strings are permitted.  In this case, no subsequent match
  *  operation will dereference the text string pointer.
  *
  * @param regexp     The compiled regular expression.
  * @param text       The subject text string.
  * @param textLength The length of the subject text, or -1 if the string
  *                   is NUL terminated.
  * @param status     Receives errors detected by this function.
  * @stable ICU 3.0
  */
extern U_SWIFT_NAME(URegularExpression.setTextString(self:_:length:status:))
void
uregex_setText(URegularExpression *regexp,
               const uint16_t     *text,
               int32_t             textLength,
               UErrorCode         *status);

/**
  *  Set the subject text string upon which the regular expression will look for matches.
  *  This function may be called any number of times, allowing the regular
  *  expression pattern to be applied to different strings.
  *  <p>
  *  Regular expression matching operations work directly on the application's
  *  string data; only a shallow clone is made.  The subject string data must not be
  *  altered after calling this function until after all regular expression
  *  operations involving this string data are completed.  
  *
  * @param regexp     The compiled regular expression.
  * @param text       The subject text string.
  * @param status     Receives errors detected by this function.
  *
  * @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.setText(self:_:status:))
void
uregex_setUText(URegularExpression *regexp,
                UText              *text,
                UErrorCode         *status);

/**
  *  Get the subject text that is currently associated with this 
  *   regular expression object.  If the input was supplied using uregex_setText(),
  *   that pointer will be returned.  Otherwise, the characters in the input will
  *   be extracted to a buffer and returned.  In either case, ownership remains
  *   with the regular expression object.
  *
  *  This function will work even if the input was originally specified as a UText.
  *
  * @param regexp      The compiled regular expression.
  * @param textLength  The length of the string is returned in this output parameter. 
  *                    A NULL pointer may be used here if the
  *                    text length is not needed, as would be the case if
  *                    the text is known in advance to be a NUL terminated
  *                    string.
  * @param status      Receives errors detected by this function.
  * @return            Pointer to the subject text string currently associated with
  *                    this regular expression.
  * @stable ICU 3.0
  */
extern U_SWIFT_NAME(URegularExpression.getTextString(self:length:status:))
const uint16_t *
uregex_getText(URegularExpression *const regexp,
               int32_t            *textLength,
               UErrorCode         *status);

/**
  *  Get the subject text that is currently associated with this 
  *   regular expression object.
  *
  *  This function will work even if the input was originally specified as a UChar string.
  *
  * @param regexp      The compiled regular expression.
  * @param dest        A mutable UText in which to store the current input.
  *                    If NULL, a new UText will be created as an immutable shallow clone
  *                    of the actual input string.
  * @param status      Receives errors detected by this function.
  * @return            The subject text currently associated with this regular expression.
  *                    If a pre-allocated UText was provided, it will always be used and returned.
  *
  * @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.getText(self:_:status:))
UText *
uregex_getUText(URegularExpression *const regexp,
                UText              *dest,
                UErrorCode         *status);

/**
  *  Set the subject text string upon which the regular expression is looking for matches
  *  without changing any other aspect of the matching state.
  *  The new and previous text strings must have the same content.
  *
  *  This function is intended for use in environments where ICU is operating on 
  *  strings that may move around in memory.  It provides a mechanism for notifying
  *  ICU that the string has been relocated, and providing a new UText to access the
  *  string in its new position.
  *
  *  Note that the regular expression implementation never copies the underlying text
  *  of a string being matched, but always operates directly on the original text 
  *  provided by the user. Refreshing simply drops the references to the old text 
  *  and replaces them with references to the new.
  *
  *  Caution:  this function is normally used only by very specialized
  *            system-level code.   One example use case is with garbage collection 
  *            that moves the text in memory. 
  *
  * @param regexp     The compiled regular expression.
  * @param text       The new (moved) text string.
  * @param status     Receives errors detected by this function.
  *
  * @stable ICU 4.8
  */
extern U_SWIFT_NAME(URegularExpression.refreshText(self:with:status:))
void
uregex_refreshUText(URegularExpression *regexp,
                    UText              *text,
                    UErrorCode         *status);

/**
  *   64bit version of uregex_matches.
  *   Attempts to match the input string against the pattern.
  *   To succeed, the match must extend to the end of the string,
  *   or cover the complete match region.
  *
  *   If startIndex >= zero the match operation starts at the specified
  *   index and must extend to the end of the input string.  Any region
  *   that has been specified is reset.
  *
  *   If startIndex == -1 the match must cover the input region, or the entire
  *   input string if no region has been set.  This directly corresponds to
  *   Matcher.matches() in Java
  *
  *    @param  regexp      The compiled regular expression.
  *    @param  startIndex  The input string (native) index at which to begin matching, or -1
  *                        to match the input Region.
  *    @param  status      Receives errors detected by this function.
  *    @return             TRUE if there is a match
  *   @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.matches(self:atIndex:status:))
int8_t
uregex_matches64(URegularExpression *regexp,
                 int64_t            startIndex,
                 UErrorCode        *status);

/**
  *   64bit version of uregex_lookingAt.
  *   Attempts to match the input string, starting from the specified index, against the pattern.
  *   The match may be of any length, and is not required to extend to the end
  *   of the input string.  Contrast with uregex_matches().
  *
  *   <p>If startIndex is >= 0 any input region that was set for this
  *   URegularExpression is reset before the operation begins.
  *
  *   <p>If the specified starting index == -1 the match begins at the start of the input 
  *   region, or at the start of the full string if no region has been specified.
  *   This corresponds directly with Matcher.lookingAt() in Java.
  *
  *   <p>If the match succeeds then more information can be obtained via the
  *    <code>uregexp_start()</code>, <code>uregexp_end()</code>,
  *    and <code>uregex_group()</code> functions.</p>
  *
  *    @param   regexp      The compiled regular expression.
  *    @param   startIndex  The input string (native) index at which to begin matching, or
  *                         -1 to match the Input Region
  *    @param   status      A reference to a UErrorCode to receive any errors.
  *    @return  TRUE if there is a match.
  *    @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.isLooking(self:atIndex:status:))
int8_t
uregex_lookingAt64(URegularExpression *regexp,
                   int64_t             startIndex,
                   UErrorCode         *status);

/**
  *   64bit version of uregex_find.
  *   Find the first matching substring of the input string that matches the pattern.
  *   If startIndex is >= zero the search for a match begins at the specified index,
  *          and any match region is reset.  This corresponds directly with
  *          Matcher.find(startIndex) in Java.
  *
  *   If startIndex == -1 the search begins at the start of the input region,
  *           or at the start of the full string if no region has been specified.
  *
  *   If a match is found, <code>uregex_start(), uregex_end()</code>, and
  *   <code>uregex_group()</code> will provide more information regarding the match.
  *
  *   @param   regexp      The compiled regular expression.
  *   @param   startIndex  The position (native) in the input string to begin the search, or
  *                        -1 to search within the Input Region.
  *   @param   status      A reference to a UErrorCode to receive any errors.
  *   @return              TRUE if a match is found.
  *   @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.findFirstMatch(self:startingAtIndex:status:))
int8_t
uregex_find64(URegularExpression *regexp,
              int64_t             startIndex, 
              UErrorCode         *status);

/**
  *  Find the next pattern match in the input string.  Begin searching 
  *  the input at the location following the end of he previous match, 
  *  or at the start of the string (or region) if there is no 
  *  previous match.  If a match is found, <code>uregex_start(), uregex_end()</code>, and
  *  <code>uregex_group()</code> will provide more information regarding the match.
  *
  *  @param   regexp      The compiled regular expression.
  *  @param   status      A reference to a UErrorCode to receive any errors.
  *  @return              TRUE if a match is found.
  *  @see uregex_reset
  *  @stable ICU 3.0
  */
extern U_SWIFT_NAME(URegularExpression.findNext(self:status:))
int8_t
uregex_findNext(URegularExpression *regexp,
                UErrorCode         *status);

/**
  *   Get the number of capturing groups in this regular expression's pattern.
  *   @param   regexp      The compiled regular expression.
  *   @param   status      A reference to a UErrorCode to receive any errors.
  *   @return the number of capture groups
  *   @stable ICU 3.0
  */
extern U_SWIFT_NAME(URegularExpression.numberOfCaptureGroups(self:status:))
int32_t
uregex_groupCount(URegularExpression *regexp,
                  UErrorCode         *status);

/**
  * Get the group number corresponding to a named capture group.
  * The returned number can be used with any function that access
  * capture groups by number.
  *
  * The function returns an error status if the specified name does not
  * appear in the pattern.
  *
  * @param  regexp      The compiled regular expression.
  * @param  groupName   The capture group name.
  * @param  nameLength  The length of the name, or -1 if the name is a
  *                     nul-terminated string.
  * @param  status      A pointer to a UErrorCode to receive any errors.
  *
  * @stable ICU 55
  */
extern U_SWIFT_NAME(URegularExpression.groupNumber(self:forName:length:status:))
int32_t
uregex_groupNumberFromName(URegularExpression *regexp,
                           const uint16_t     *groupName,
                           int32_t             nameLength,
                           UErrorCode          *status);


/**
  * Get the group number corresponding to a named capture group.
  * The returned number can be used with any function that access
  * capture groups by number.
  *
  * The function returns an error status if the specified name does not
  * appear in the pattern.
  *
  * @param  regexp      The compiled regular expression.
  * @param  groupName   The capture group name,
  *                     platform invariant characters only.
  * @param  nameLength  The length of the name, or -1 if the name is
  *                     nul-terminated.
  * @param  status      A pointer to a UErrorCode to receive any errors.
  *
  * @stable ICU 55
  */
extern U_SWIFT_NAME(URegularExpression.groupNumber(self:forCName:length:status:))
int32_t
uregex_groupNumberFromCName(URegularExpression *regexp,
                            const char         *groupName,
                            int32_t             nameLength,
                            UErrorCode          *status);

/** Extract the string for the specified matching expression or subexpression.
  * Group #0 is the complete string of matched text.
  * Group #1 is the text matched by the first set of capturing parentheses.
  *
  *   @param   regexp       The compiled regular expression.
  *   @param   groupNum     The capture group to extract.  Group 0 is the complete
  *                         match.  The value of this parameter must be
  *                         less than or equal to the number of capture groups in
  *                         the pattern.
  *   @param   dest         Buffer to receive the matching string data
  *   @param   destCapacity Capacity of the dest buffer.
  *   @param   status       A reference to a UErrorCode to receive any errors.
  *   @return               Length of matching data,
  *                         or -1 if no applicable match.
  *   @stable ICU 3.0
  */
extern U_SWIFT_NAME(URegularExpression.matchString(self:forGroupAtIndex:_:capacity:status:))
int32_t
uregex_group(URegularExpression *regexp,
             int32_t             groupNum,
             uint16_t           *dest,
             int32_t             destCapacity,
             UErrorCode          *status);

/** Returns a shallow immutable clone of the entire input string with the current index set
  *   to the beginning of the requested capture group.  The capture group length is also
  *   returned via groupLength.
  * Group #0 is the complete string of matched text.
  * Group #1 is the text matched by the first set of capturing parentheses.
  *
  *   @param   regexp       The compiled regular expression.
  *   @param   groupNum     The capture group to extract.  Group 0 is the complete
  *                         match.  The value of this parameter must be
  *                         less than or equal to the number of capture groups in
  *                         the pattern.
  *   @param   dest         A mutable UText in which to store the current input.
  *                         If NULL, a new UText will be created as an immutable shallow clone
  *                         of the entire input string.
  *   @param   groupLength  The group length of the desired capture group. Output parameter.
  *   @param   status       A reference to a UErrorCode to receive any errors.
  *   @return               The subject text currently associated with this regular expression.
  *                         If a pre-allocated UText was provided, it will always be used and returned.

  *
  *   @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.match(self:forGroupAtIndex:_:groupLength:status:))
UText *
uregex_groupUText(URegularExpression *regexp,
                  int32_t             groupNum,
                  UText              *dest,
                  int64_t            *groupLength,
                  UErrorCode         *status);

/**
  *   64bit version of uregex_start.
  *   Returns the index in the input string of the start of the text matched by the
  *   specified capture group during the previous match operation.  Return -1 if
  *   the capture group was not part of the last match.
  *   Group #0 refers to the complete range of matched text.
  *   Group #1 refers to the text matched by the first set of capturing parentheses.
  *
  *    @param   regexp      The compiled regular expression.
  *    @param   groupNum    The capture group number
  *    @param   status      A reference to a UErrorCode to receive any errors.
  *    @return              the starting (native) position in the input of the text matched 
  *                         by the specified group.
  *   @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.startIndex(self:forGroupAtIndex:status:))
int64_t
uregex_start64(URegularExpression *regexp,
               int32_t             groupNum,
               UErrorCode          *status);

/**
  *   64bit version of uregex_end.
  *   Returns the index in the input string of the position following the end
  *   of the text matched by the specified capture group.
  *   Return -1 if the capture group was not part of the last match.
  *   Group #0 refers to the complete range of matched text.
  *   Group #1 refers to the text matched by the first set of capturing parentheses.
  *
  *    @param   regexp      The compiled regular expression.
  *    @param   groupNum    The capture group number
  *    @param   status      A reference to a UErrorCode to receive any errors.
  *    @return              the (native) index of the position following the last matched character.
  *   @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.endIndex(self:forGroupAtIndex:status:))
int64_t
uregex_end64(URegularExpression *regexp,
             int32_t               groupNum,
             UErrorCode           *status);

/**
  *  64bit version of uregex_reset.
  *  Reset any saved state from the previous match.  Has the effect of
  *  causing uregex_findNext to begin at the specified index, and causing
  *  uregex_start(), uregex_end() and uregex_group() to return an error 
  *  indicating that there is no match information available.  Clears any
  *  match region that may have been set.
  *
  *    @param   regexp      The compiled regular expression.
  *    @param   index       The position (native) in the text at which a
  *                         uregex_findNext() should begin searching.
  *    @param   status      A reference to a UErrorCode to receive any errors.
  *    @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.reset(self:toStartIndex:status:))
void
uregex_reset64(URegularExpression  *regexp,
               int64_t               index,
               UErrorCode            *status);

/**
  * 64bit version of uregex_setRegion.
  * Sets the limits of the matching region for this URegularExpression.
  * The region is the part of the input string that will be considered when matching.
  * Invoking this method resets any saved state from the previous match, 
  * then sets the region to start at the index specified by the start parameter
  * and end at the index specified by the end parameter.
  *
  * Depending on the transparency and anchoring being used (see useTransparentBounds
  * and useAnchoringBounds), certain constructs such as anchors may behave differently
  * at or around the boundaries of the region
  *
  * The function will fail if start is greater than limit, or if either index
  *  is less than zero or greater than the length of the string being matched.
  *
  * @param regexp The compiled regular expression.
  * @param regionStart  The (native) index to begin searches at.
  * @param regionLimit  The (native) index to end searches at (exclusive).
  * @param status A pointer to a UErrorCode to receive any errors.
  * @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.setRegion(self:start:end:status:))
void
uregex_setRegion64(URegularExpression *regexp,
                 int64_t               regionStart,
                 int64_t               regionLimit,
                 UErrorCode           *status);

/**
  *  Set the matching region and the starting index for subsequent matches
  *  in a single operation.
  *  This is useful because the usual function for setting the starting
  *  index, urgex_reset(), also resets any region limits.
  *
  * @param regexp The compiled regular expression.
  * @param regionStart  The (native) index to begin searches at.
  * @param regionLimit  The (native) index to end searches at (exclusive).
  * @param startIndex   The index in the input text at which the next 
  *                     match operation should begin.
  * @param status A pointer to a UErrorCode to receive any errors.
  * @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.setRegion(self:start:end:startIndex:status:))
void
uregex_setRegionAndStart(URegularExpression *regexp,
                 int64_t               regionStart,
                 int64_t               regionLimit,
                 int64_t               startIndex,
                 UErrorCode           *status);

/**
  * 64bit version of uregex_regionStart.
  * Reports the start index of the matching region. Any matches found are limited to
  * to the region bounded by regionStart (inclusive) and regionEnd (exclusive).
  *
  * @param regexp The compiled regular expression.
  * @param status A pointer to a UErrorCode to receive any errors.
  * @return The starting (native) index of this matcher's region.
  * @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.regionStart(self:status:))
int64_t
uregex_regionStart64(const  URegularExpression   *regexp,
                            UErrorCode           *status);

/**
  * 64bit version of uregex_regionEnd.
  * Reports the end index (exclusive) of the matching region for this URegularExpression.
  * Any matches found are limited to to the region bounded by regionStart (inclusive)
  * and regionEnd (exclusive).
  *
  * @param regexp The compiled regular expression.
  * @param status A pointer to a UErrorCode to receive any errors.
  * @return The ending point (native) of this matcher's region.
  * @stable ICU 4.6
  */
extern U_SWIFT_NAME(URegularExpression.regionEnd(self:status:))
int64_t
uregex_regionEnd64(const  URegularExpression   *regexp,
                          UErrorCode           *status);

/**
  * Queries the transparency of region bounds for this URegularExpression.
  * See useTransparentBounds for a description of transparent and opaque bounds.
  * By default, matching boundaries are opaque.
  *
  * @param regexp The compiled regular expression.
  * @param status A pointer to a UErrorCode to receive any errors.
  * @return TRUE if this matcher is using opaque bounds, false if it is not.
  * @stable ICU 4.0
  */
extern U_SWIFT_NAME(URegularExpression.usesTransparentBounds(self:status:))
int8_t
uregex_hasTransparentBounds(const  URegularExpression   *regexp,
                                   UErrorCode           *status);


/**
  * Sets the transparency of region bounds for this URegularExpression.
  * Invoking this function with an argument of TRUE will set matches to use transparent bounds.
  * If the boolean argument is FALSE, then opaque bounds will be used.
  *
  * Using transparent bounds, the boundaries of the matching region are transparent
  * to lookahead, lookbehind, and boundary matching constructs. Those constructs can
  * see text beyond the boundaries of the region while checking for a match.
  *
  * With opaque bounds, no text outside of the matching region is visible to lookahead,
  * lookbehind, and boundary matching constructs.
  *
  * By default, opaque bounds are used.
  *
  * @param   regexp The compiled regular expression.
  * @param   b      TRUE for transparent bounds; FALSE for opaque bounds
  * @param   status A pointer to a UErrorCode to receive any errors.
  * @stable ICU 4.0
  **/
extern U_SWIFT_NAME(URegularExpression.setUsesTransparentBounds(self:_:status:))
void
uregex_useTransparentBounds(URegularExpression   *regexp, 
                            int8_t              b,
                            UErrorCode           *status);


/**
  * Return true if this URegularExpression is using anchoring bounds.
  * By default, anchoring region bounds are used.
  *
  * @param  regexp The compiled regular expression.
  * @param  status A pointer to a UErrorCode to receive any errors.
  * @return TRUE if this matcher is using anchoring bounds.
  * @stable ICU 4.0
  */
extern U_SWIFT_NAME(URegularExpression.usesAnchoringBounds(self:status:))
int8_t
uregex_hasAnchoringBounds(const  URegularExpression   *regexp,
                                 UErrorCode           *status);


/**
  * Set whether this URegularExpression is using Anchoring Bounds for its region.
  * With anchoring bounds, pattern anchors such as ^ and $ will match at the start
  * and end of the region.  Without Anchoring Bounds, anchors will only match at
  * the positions they would in the complete text.
  *
  * Anchoring Bounds are the default for regions.
  *
  * @param regexp The compiled regular expression.
  * @param b      TRUE if to enable anchoring bounds; FALSE to disable them.
  * @param status A pointer to a UErrorCode to receive any errors.
  * @stable ICU 4.0
  */
extern U_SWIFT_NAME(URegularExpression.setUsesAnchoringBounds(self:_:status:))
void
uregex_useAnchoringBounds(URegularExpression   *regexp,
                          int8_t                b,
                          UErrorCode           *status);

/**
  * Return TRUE if the most recent matching operation touched the
  *  end of the text being processed.  In this case, additional input text could
  *  change the results of that match.
  *
  *  @param regexp The compiled regular expression.
  *  @param status A pointer to a UErrorCode to receive any errors.
  *  @return  TRUE if the most recent match hit the end of input
  *  @stable ICU 4.0
  */
extern U_SWIFT_NAME(URegularExpression.hasHitEnd(self:status:))
int8_t
uregex_hitEnd(const  URegularExpression   *regexp,
                     UErrorCode           *status);

/**
  * Return TRUE the most recent match succeeded and additional input could cause
  * it to fail. If this function returns false and a match was found, then more input
  * might change the match but the match won't be lost. If a match was not found,
  * then requireEnd has no meaning.
  *
  * @param regexp The compiled regular expression.
  * @param status A pointer to a UErrorCode to receive any errors.
  * @return TRUE  if more input could cause the most recent match to no longer match.
  * @stable ICU 4.0
  */
extern U_SWIFT_NAME(URegularExpression.requireEnd(self:status:))
int8_t
uregex_requireEnd(const  URegularExpression   *regexp,
                         UErrorCode           *status);

/**
 * Function pointer for a regular expression matching callback function.
 * When set, a callback function will be called periodically during matching
 * operations.  If the call back function returns FALSE, the matching
 * operation will be terminated early.
 *
 * Note:  the callback function must not call other functions on this
 *        URegularExpression.
 *
 * @param context  context pointer.  The callback function will be invoked
 *                 with the context specified at the time that
 *                 uregex_setMatchCallback() is called.
 * @param steps    the accumulated processing time, in match steps, 
 *                 for this matching operation.
 * @return         TRUE to continue the matching operation.
 *                 FALSE to terminate the matching operation.
 * @stable ICU 4.0
 */
U_SWIFT_NAME(URegularExpression.MatchCallback)
typedef int8_t URegexMatchCallback (
                   const void *_Nullable context,
                   int32_t     steps);

/**
 * Set a callback function for this URegularExpression.
 * During matching operations the function will be called periodically,
 * giving the application the opportunity to terminate a long-running
 * match.
 *
 * @param   regexp      The compiled regular expression.
 * @param   callback    A pointer to the user-supplied callback function.
 * @param   context     User context pointer.  The value supplied at the
 *                      time the callback function is set will be saved
 *                      and passed to the callback each time that it is called.
 * @param   status      A reference to a UErrorCode to receive any errors.
 * @stable ICU 4.0
 */
extern U_SWIFT_NAME(URegularExpression.setMatchCallback(self:_:context:status:))
void
uregex_setMatchCallback(URegularExpression      *regexp,
                        URegexMatchCallback     *_Nullable callback,
                        const void              *_Nullable context,
                        UErrorCode              *status);


/**
 *  Get the callback function for this URegularExpression.
 *
 * @param   regexp      The compiled regular expression.
 * @param   callback    Out parameter, receives a pointer to the user-supplied 
 *                      callback function.
 * @param   context     Out parameter, receives the user context pointer that
 *                      was set when uregex_setMatchCallback() was called.
 * @param   status      A reference to a UErrorCode to receive any errors.
 * @stable ICU 4.0
 */
extern U_SWIFT_NAME(URegularExpression.getMatchCallback(self:_:context:status:))
void
uregex_getMatchCallback(const URegularExpression    *regexp,
                        URegexMatchCallback        *_Nonnull *_Nullable callback,
                        const void                 *_Nonnull *_Nullable context,
                        UErrorCode                  *status);

/**
 * Function pointer for a regular expression find callback function.
 * 
 * When set, a callback function will be called during a find operation
 * and for operations that depend on find, such as findNext, split and some replace
 * operations like replaceFirst.
 * The callback will usually be called after each attempt at a match, but this is not a
 * guarantee that the callback will be invoked at each character.  For finds where the
 * match engine is invoked at each character, this may be close to true, but less likely
 * for more optimized loops where the pattern is known to only start, and the match
 * engine invoked, at certain characters.
 * When invoked, this callback will specify the index at which a match operation is about
 * to be attempted, giving the application the opportunity to terminate a long-running
 * find operation.
 * 
 * If the call back function returns FALSE, the find operation will be terminated early.
 *
 * Note:  the callback function must not call other functions on this
 *        URegularExpression
 *
 * @param context  context pointer.  The callback function will be invoked
 *                 with the context specified at the time that
 *                 uregex_setFindProgressCallback() is called.
 * @param matchIndex  the next index at which a match attempt will be attempted for this
 *                 find operation.  If this callback interrupts the search, this is the
 *                 index at which a find/findNext operation may be re-initiated.
 * @return         TRUE to continue the matching operation.
 *                 FALSE to terminate the matching operation.
 * @stable ICU 4.6
 */
U_SWIFT_NAME(URegularExpression.FindProgressCallback)
typedef int8_t URegexFindProgressCallback (
                   const void *_Nullable context,
                   int64_t     matchIndex);


/**
 *  Set the find progress callback function for this URegularExpression.
 *
 * @param   regexp      The compiled regular expression.
 * @param   callback    A pointer to the user-supplied callback function.
 * @param   context     User context pointer.  The value supplied at the
 *                      time the callback function is set will be saved
 *                      and passed to the callback each time that it is called.
 * @param   status      A reference to a UErrorCode to receive any errors.
 * @stable ICU 4.6
 */
extern U_SWIFT_NAME(URegularExpression.setFindProgressCallback(self:_:context:status:))
void
uregex_setFindProgressCallback(URegularExpression              *regexp,
                                URegexFindProgressCallback      *_Nullable callback,
                                const void                      *_Nullable context,
                                UErrorCode                      *status);

/**
 *  Get the find progress callback function for this URegularExpression.
 *
 * @param   regexp      The compiled regular expression.
 * @param   callback    Out parameter, receives a pointer to the user-supplied 
 *                      callback function.
 * @param   context     Out parameter, receives the user context pointer that
 *                      was set when uregex_setFindProgressCallback() was called.
 * @param   status      A reference to a UErrorCode to receive any errors.
 * @stable ICU 4.6
 */
extern U_SWIFT_NAME(URegularExpression.getFindProgressCallback(self:_:context:status:))
void
uregex_getFindProgressCallback(const URegularExpression          *regexp,
                                URegexFindProgressCallback        *_Nonnull *_Nullable callback,
                                const void                        *_Nonnull *_Nullable context,
                                UErrorCode                        *status);

#pragma clang assume_nonnull end

#endif   /*  UREGEX_H  */
