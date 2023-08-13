// * Description:  LEPkg File Format
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 12, 2023
#include "defines.h"
#include "format.h"
#include <stdio.h>

internal void print_outline_0_1() {
    println( "LE Package Version 0.1 file format outline" );
    println( "Tightly packed (no padding), little endian" );

    println( "Header (24 bytes):" );
    println( "Offset  0 | Size 4 | Identifier ( uint32 = 658458948 )" );
    println( "Offset  4 | Size 4 | Version ( (uint16 (major version) | uint16 (minor version)) or uint32 )" );
    println( "Offset  8 | Size 4 | Header Size ( uint32 )" );
    println( "Offset 12 | Size 4 | Asset Count ( uin32 )" );
    println( "Offset 20 | Size 8 | File Size ( uint64 )" );

    println( "" );

    println( "Asset (41 bytes):" );
    println( "Offset 0 | Size 1  | Asset Type ( enum, uint8 )" );
    println( "Offset 1 | Size 40 | Assets Union" );

    println( "" );

    println( "Asset Metadata:" );
    
    println( "" );

    println( "Asset Texture (20 bytes):" );
    println( "Offset  0 | Size 4 | Dimensions Width ( int32 )" );
    println( "Offset  4 | Size 4 | Dimensions Height ( int32 )" );
    println( "Offset  8 | Size 8 | Buffer Offset ( uint64 )" );
    println( "Offset 16 | Size 4 | Texture Format ( enum, uint32 )" );

    println( "" );

    println( "Asset Font:" );
    println( "Offset  0 | Size 4 | Point Size ( float )" );
    println( "Offset  4 | Size 4 | Texture ( AssetID ( uint32 ) )" );
    println( "Offset  8 | Size 4 | Metrics Count ( uint32 )" );
    println( "Offset 16 | Size 8 | Metrics Offset ( uint64 )" );

    println( "" );

    println( "Asset Audio:" );

    println( "" );

    println( "Asset Model 3D (34 bytes):" );
    println( "Offset  0 | Size 1 | Vertex Type ( enum, uint8 )" );
    println( "Offset  1 | Size 1 | Index Type ( enum, uint8 )" );
    println( "Offset  2 | Size 8 | Vertex Count ( uint64 )" );
    println( "Offset 10 | Size 8 | Index Count ( uint64 )" );
    println( "Offset 18 | Size 8 | Vertices Offset ( uint64 )" );
    println( "Offset 26 | Size 8 | Indices Offset ( uint64 )" );

    println( "" );

    println( "Asset Shader (17 bytes):" );
    println( "Offset 0 | Size 1 | Shader Type ( enum, uint8 )" );
    println( "Offset 1 | Size 8 | Buffer Offset ( uint64 )" );
    println( "Offset 9 | Size 8 | Buffer Size ( uint64 )" );
}

void print_outline( u16 major, u16 minor ) {
    switch( major ) {
        case 0: switch( minor ) {
            case 1:
                print_outline_0_1();
                break;
            default: break;
        } break;
        default: break;
    }
}

