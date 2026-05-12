#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../include/GLBLoader.h"

#include "../external/tinygltf/tiny_gltf.h"

#include <iostream>

bool GLBLoader::load(const char* path) {

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;

    std::string err;
    std::string warn;

    bool ok =
        loader.LoadBinaryFromFile(
            &model,
            &err,
            &warn,
            path
        );

    if (!ok) {
        std::cout << "GLB load failed\n";
        return false;
    }

    auto& mesh = model.meshes[0];
    auto& primitive = mesh.primitives[0];

    auto& posAccessor =
        model.accessors[primitive.attributes["POSITION"]];

    auto& posView =
        model.bufferViews[posAccessor.bufferView];

    auto& posBuffer =
        model.buffers[posView.buffer];

    auto& indexAccessor =
        model.accessors[primitive.indices];

    auto& indexView =
        model.bufferViews[indexAccessor.bufferView];

    auto& indexBuffer =
        model.buffers[indexView.buffer];

    unsigned char* vertexData =
        posBuffer.data.data()
        + posView.byteOffset
        + posAccessor.byteOffset;

    unsigned char* indexData =
        indexBuffer.data.data()
        + indexView.byteOffset
        + indexAccessor.byteOffset;

    glGenVertexArrays(1, &vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        posView.byteLength,
        vertexData,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indexView.byteLength,
        indexData,
        GL_STATIC_DRAW
    );

    int stride = posAccessor.ByteStride(posView);

    if (stride == 0)
        stride = 3 * sizeof(float);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)0
    );

    glEnableVertexAttribArray(0);

    indexCount = indexAccessor.count;

    switch (indexAccessor.componentType) {

        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            indexType = GL_UNSIGNED_SHORT;
            break;

        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            indexType = GL_UNSIGNED_INT;
            break;
    }

    return true;
}

void GLBLoader::draw() {

    glBindVertexArray(vao);

    glDrawElements(
        GL_TRIANGLES,
        indexCount,
        indexType,
        0
    );
}