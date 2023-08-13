// * Description:  Package validation implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 13, 2023
#include "validate.h"
#include "term_color.h"
#include <stdio.h>

b32 is_package_valid( const char* path ) {
    // TODO(alicia): actually validate package
    b32 is_valid = true;

    if( is_valid ) {
        println(
            "Package \"%s\" is a valid Liquid Engine Package.",
            path
        );
    } else {
        printerrln(
            "Package \"%s\" is an "
            TC_RED( "invalid " )
            "Liquid Engine Package.",
            path
        );
    }
    return is_valid;
}

