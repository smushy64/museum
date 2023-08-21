// * Description:  Liquid Engine Resource Packager
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 19, 2023
#include <defines.h>
#include <core/ldstring.h>
#include "corec.inl"

int main( int argc, char** argv ) {
    for( int i = 0; i < argc; ++i ) {
        println( "{cc}", argv[i] );
    }
    return 0;
}

