#include <GL/glew.h>
#include "mesh.h"

#ifndef GLOBJECT_H
#define GLOBJECT_H

struct GLObject
{
    GLuint vbo[3], vao;
    unsigned int numVertices;
};

void globject_new(const struct Mesh* mesh, struct GLObject* glo);
void globject_free(struct GLObject* glo);

#endif
