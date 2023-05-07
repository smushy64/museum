#if !defined(VK_DEFINES_HPP)
#define VK_DEFINES_HPP
/**
 * Description:  Vulkan specific defines
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "defines.h"
#include <vulkan/vulkan.h>
#include "core/logging.h"

#if defined(LD_LOGGING)
    #define VK_LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE VK ] " format,\
            ##__VA_ARGS__\
        )
    #define VK_LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO VK ] " format,\
            ##__VA_ARGS__\
        )
    #define VK_LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG VK] " format,\
            ##__VA_ARGS__\
        )
    #define VK_LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN VK ] " format,\
            ##__VA_ARGS__\
        )
    #define VK_LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR VK] " format,\
            ##__VA_ARGS__\
        )

    #define VK_LOG_NOTE_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE VK  | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define VK_LOG_INFO_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO VK  | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define VK_LOG_DEBUG_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG VK | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
    #define VK_LOG_WARN_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN VK  | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define VK_LOG_ERROR_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR VK | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
#else
    #define VK_LOG_NOTE( format, ... )
    #define VK_LOG_INFO( format, ... )
    #define VK_LOG_DEBUG( format, ... )
    #define VK_LOG_WARN( format, ... )
    #define VK_LOG_ERROR( format, ... )
    #define VK_LOG_NOTE_TRACE( format, ... )
    #define VK_LOG_INFO_TRACE( format, ... )
    #define VK_LOG_DEBUG_TRACE( format, ... )
    #define VK_LOG_WARN_TRACE( format, ... )
    #define VK_LOG_ERROR_TRACE( format, ... )
#endif

struct VkContext {
    VkInstance             instance;
    VkAllocationCallbacks* allocator;
};

#endif // header guard