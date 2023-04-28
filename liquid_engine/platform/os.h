#if !defined(PLATFORM_HPP)
#define PLATFORM_HPP
/**
 * Description:  Operating System related functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"

/// Types of message boxes
enum MessageBoxType : u32 {
    MBTYPE_OK,
    MBTYPE_OKCANCEL,
    MBTYPE_RETRYCANCEL,
    MBTYPE_YESNO,
    MBTYPE_YESNOCANCEL,

    MBTYPE_COUNT
};

/// Message box icons
enum MessageBoxIcon : u32 {
    MBICON_INFORMATION,
    MBICON_WARNING,
    MBICON_ERROR,

    MBICON_COUNT
};

/// User selection from a message box or an error from creating message box.
enum MessageBoxResult : u32 {
    MBRESULT_OK,
    MBRESULT_CANCEL,
    MBRESULT_RETRY,
    MBRESULT_YES,
    MBRESULT_NO,

    MBRESULT_UNKNOWN_ERROR,

    MBRESULT_COUNT
};

/// Create a message box to report urgent information.
SM_API MessageBoxResult message_box(
    const char* window_title,
    const char* message,
    MessageBoxType type,
    MessageBoxIcon icon
);

#define MESSAGE_BOX_FATAL( title, message ) \
    message_box(\
        title,\
        message,\
        MBTYPE_OK,\
        MBICON_ERROR\
    )

/// Put the current thread to sleep for the specified amount of milliseconds.
SM_API void sleep( u32 ms );


#endif