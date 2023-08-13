#if !defined(LEPKG_TERM_COLOR_HPP)
#define LEPKG_TERM_COLOR_HPP
// * Description:  Terminal color codes
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 11, 2023

#define TERM_COLOR_BLACK   "\033[1;30m" // BLACK
#define TERM_COLOR_RED     "\033[1;31m" // RED
#define TERM_COLOR_GREEN   "\033[1;32m" // GREEN
#define TERM_COLOR_YELLOW  "\033[1;33m" // YELLOW
#define TERM_COLOR_MAGENTA "\033[1;35m" // MAGENTA
#define TERM_COLOR_CYAN    "\033[1;36m" // CYAN
#define TERM_COLOR_WHITE   "\033[1;37m" // WHITE
#define TERM_COLOR_BLUE    "\033[1;34m" // BLUE
#define TERM_COLOR_RESET   "\033[1;0m"  // RESET
#define TERM_COLOR_DEFAULT "\033[1;39m" // DEFAULT

#define TC_BLACK( string )   TERM_COLOR_BLACK string TERM_COLOR_RESET
#define TC_RED( string )     TERM_COLOR_RED string TERM_COLOR_RESET
#define TC_GREEN( string )   TERM_COLOR_GREEN string TERM_COLOR_RESET
#define TC_YELLOW( string )  TERM_COLOR_YELLOW string TERM_COLOR_RESET
#define TC_MAGENTA( string ) TERM_COLOR_MAGENTA string TERM_COLOR_RESET
#define TC_CYAN( string )    TERM_COLOR_CYAN string TERM_COLOR_RESET
#define TC_WHITE( string )   TERM_COLOR_WHITE string TERM_COLOR_RESET
#define TC_BLUE( string )    TERM_COLOR_BLUE string TERM_COLOR_RESET

#endif // header guard
