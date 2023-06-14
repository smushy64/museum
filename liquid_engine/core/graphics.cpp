/**
 * Description:  Graphics Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#include "graphics.h"
#include "renderer/renderer.h"

void upload_mesh_list( struct RenderOrder* order, Mesh* mesh, u32 mesh_count ) {
    order->mesh_count = mesh_count;
    order->meshes     = mesh;
}
