#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "GLBLoader.h"
#include <tiny_gltf.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
    float position[3];
    float normal[3];
    float texCoord[2];
    float joints[4];
    float weights[4];
};

GLint GLBLoader::getUniformLocation(GLuint shaderId, const std::string& name) {
    std::string key = std::to_string(shaderId) + name;
    if (uniformCache.find(key) != uniformCache.end()) return uniformCache[key];
    GLint loc = glGetUniformLocation(shaderId, name.c_str());
    uniformCache[key] = loc;
    return loc;
}

template <typename T>
void getAttributeData(const tinygltf::Model& model, int accessorIdx, std::vector<T>& out) {
    if (accessorIdx < 0) return;
    const auto& acc = model.accessors[accessorIdx];
    const auto& view = model.bufferViews[acc.bufferView];
    const auto& buf = model.buffers[view.buffer];
    size_t stride = acc.ByteStride(view);
    const unsigned char* data = &buf.data[view.byteOffset + acc.byteOffset];
    for (size_t i = 0; i < acc.count; i++) {
        out.push_back(*reinterpret_cast<const T*>(data + i * stride));
    }
}

bool GLBLoader::load(const char* path) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    if (!loader.LoadBinaryFromFile(&model, &err, &warn, path)) return false;

    std::cout << "Model Info: " << path << " Meshes: " << model.meshes.size() << " Skins: " << model.skins.size() << " Anims: " << model.animations.size() << std::endl;

    for (auto& img : model.images) {
        GLuint texID; glGenTextures(1, &texID); glBindTexture(GL_TEXTURE_2D, texID);
        GLenum format = (img.component == 1) ? GL_RED : (img.component == 2) ? GL_RG : (img.component == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, format, img.pixel_type, &img.image[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
        textures.push_back(texID);
    }

    for (size_t i = 0; i < model.nodes.size(); i++) {
        auto& n = model.nodes[i];
        Node node; node.index = (int)i; node.name = n.name; node.meshIndex = n.mesh; node.skinIndex = n.skin;
        if (n.matrix.size() == 16) { node.useMatrix = true; node.matrix = glm::make_mat4(n.matrix.data()); }
        else {
            if (n.translation.size() == 3) node.translation = glm::make_vec3(n.translation.data());
            if (n.rotation.size() == 4) node.rotation = glm::make_quat(n.rotation.data());
            if (n.scale.size() == 3) node.scale = glm::make_vec3(n.scale.data());
        }
        node.children = n.children; nodes.push_back(node);
    }
    for (auto& node : nodes) { for (int child : node.children) nodes[child].parent = node.index; }
    for (size_t i = 0; i < nodes.size(); i++) { if (nodes[i].parent == -1) rootNodes.push_back((int)i); }

    for (auto& s : model.skins) {
        Skin skin; skin.name = s.name; skin.joints = s.joints;
        if (s.inverseBindMatrices >= 0) {
            auto& acc = model.accessors[s.inverseBindMatrices];
            auto& view = model.bufferViews[acc.bufferView];
            const float* m = reinterpret_cast<const float*>(&model.buffers[view.buffer].data[view.byteOffset + acc.byteOffset]);
            for (size_t i = 0; i < acc.count; i++) skin.inverseBindMatrices.push_back(glm::make_mat4(&m[i * 16]));
        }
        skins.push_back(skin);
        std::cout << "Skin: " << skin.name << " Joints: " << skin.joints.size() << std::endl;
    }

    for (auto& a : model.animations) {
        Animation anim; anim.name = a.name;
        for (auto& s : a.samplers) {
            AnimationSampler samp;
            samp.interpolation = (s.interpolation == "STEP") ? AnimationSampler::STEP : (s.interpolation == "CUBICSPLINE") ? AnimationSampler::CUBICSPLINE : AnimationSampler::LINEAR;
            auto& inAcc = model.accessors[s.input]; auto& inView = model.bufferViews[inAcc.bufferView];
            const float* inData = reinterpret_cast<const float*>(&model.buffers[inView.buffer].data[inView.byteOffset + inAcc.byteOffset]);
            for (size_t i = 0; i < inAcc.count; i++) { samp.inputs.push_back(inData[i]); if (inData[i] > anim.duration) anim.duration = inData[i]; }
            auto& outAcc = model.accessors[s.output]; auto& outView = model.bufferViews[outAcc.bufferView];
            const float* outData = reinterpret_cast<const float*>(&model.buffers[outView.buffer].data[outView.byteOffset + outAcc.byteOffset]);
            for (size_t i = 0; i < outAcc.count; i++) {
                if (outAcc.type == TINYGLTF_TYPE_VEC3) samp.outputs.push_back(glm::vec4(glm::make_vec3(&outData[i * 3]), 0.0f));
                else if (outAcc.type == TINYGLTF_TYPE_VEC4) samp.outputs.push_back(glm::make_vec4(&outData[i * 4]));
            }
            anim.samplers.push_back(samp);
        }
        for (auto& c : a.channels) {
            AnimationChannel chan; chan.samplerIndex = c.sampler; chan.nodeIndex = c.target_node;
            chan.path = (c.target_path == "translation") ? AnimationChannel::TRANSLATION : (c.target_path == "rotation") ? AnimationChannel::ROTATION : AnimationChannel::SCALE;
            anim.channels.push_back(chan);
        }
        animations.push_back(anim);
        std::cout << "Animation loaded: " << anim.name << " (duration: " << anim.duration << "s)" << std::endl;
    }

    for (auto& mesh : model.meshes) {
        for (auto& primitive : mesh.primitives) {
            Primitive p; std::vector<Vertex> vertices;
            if (primitive.attributes.find("POSITION") == primitive.attributes.end()) continue;
            
            const auto& posAcc = model.accessors[primitive.attributes["POSITION"]];
            const auto& posView = model.bufferViews[posAcc.bufferView];
            const unsigned char* posBuf = &model.buffers[posView.buffer].data[posView.byteOffset + posAcc.byteOffset];
            size_t posStride = posAcc.ByteStride(posView);

            const unsigned char* normBuf = nullptr; size_t normStride = 0;
            if (primitive.attributes.count("NORMAL")) {
                const auto& acc = model.accessors[primitive.attributes["NORMAL"]];
                const auto& view = model.bufferViews[acc.bufferView];
                normBuf = &model.buffers[view.buffer].data[view.byteOffset + acc.byteOffset];
                normStride = acc.ByteStride(view);
            }

            const unsigned char* uvBuf = nullptr; size_t uvStride = 0;
            if (primitive.attributes.count("TEXCOORD_0")) {
                const auto& acc = model.accessors[primitive.attributes["TEXCOORD_0"]];
                const auto& view = model.bufferViews[acc.bufferView];
                uvBuf = &model.buffers[view.buffer].data[view.byteOffset + acc.byteOffset];
                uvStride = acc.ByteStride(view);
            }

            const unsigned char* jointBuf = nullptr; size_t jointStride = 0; int jointType = 0;
            if (primitive.attributes.count("JOINTS_0")) {
                const auto& acc = model.accessors[primitive.attributes["JOINTS_0"]];
                const auto& view = model.bufferViews[acc.bufferView];
                jointBuf = &model.buffers[view.buffer].data[view.byteOffset + acc.byteOffset];
                jointStride = acc.ByteStride(view);
                jointType = acc.componentType;
            }

            const unsigned char* weightBuf = nullptr; size_t weightStride = 0; int weightType = 0;
            if (primitive.attributes.count("WEIGHTS_0")) {
                const auto& acc = model.accessors[primitive.attributes["WEIGHTS_0"]];
                const auto& view = model.bufferViews[acc.bufferView];
                weightBuf = &model.buffers[view.buffer].data[view.byteOffset + acc.byteOffset];
                weightStride = acc.ByteStride(view);
                weightType = acc.componentType;
            }

            for (size_t i = 0; i < posAcc.count; i++) {
                Vertex v;
                memcpy(v.position, posBuf + i * posStride, 12);
                if (normBuf) memcpy(v.normal, normBuf + i * normStride, 12); else memset(v.normal, 0, 12);
                if (uvBuf) memcpy(v.texCoord, uvBuf + i * uvStride, 8); else memset(v.texCoord, 0, 8);
                
                if (jointBuf) {
                    const unsigned char* jPtr = jointBuf + i * jointStride;
                    if (jointType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) { for (int k = 0; k < 4; k++) v.joints[k] = reinterpret_cast<const uint16_t*>(jPtr)[k]; }
                    else if (jointType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) { for (int k = 0; k < 4; k++) v.joints[k] = jPtr[k]; }
                } else memset(v.joints, 0, 16);

                if (weightBuf) {
                    const unsigned char* wPtr = weightBuf + i * weightStride;
                    if (weightType == TINYGLTF_COMPONENT_TYPE_FLOAT) { memcpy(v.weights, wPtr, 16); }
                    else if (weightType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) { for (int k = 0; k < 4; k++) v.weights[k] = reinterpret_cast<const uint16_t*>(wPtr)[k] / 65535.0f; }
                    else if (weightType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) { for (int k = 0; k < 4; k++) v.weights[k] = wPtr[k] / 255.0f; }
                } else { v.weights[0] = 1.0f; memset(&v.weights[1], 0, 12); }
                
                vertices.push_back(v);
            }

            const auto& idxAcc = model.accessors[primitive.indices];
            const auto& idxView = model.bufferViews[idxAcc.bufferView];
            p.indexCount = (int)idxAcc.count; p.indexType = idxAcc.componentType;
            glGenVertexArrays(1, &p.vao); glGenBuffers(1, &p.vbo); glGenBuffers(1, &p.ebo);
            glBindVertexArray(p.vao);
            glBindBuffer(GL_ARRAY_BUFFER, p.vbo); glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p.ebo); glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxView.byteLength, &model.buffers[idxView.buffer].data[idxView.byteOffset + idxAcc.byteOffset], GL_STATIC_DRAW);
            glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);
            glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);
            glEnableVertexAttribArray(3); glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)32);
            glEnableVertexAttribArray(4); glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)48);
            
            p.materialIndex = -1;
            if (primitive.material >= 0) {
                const auto& mat = model.materials[primitive.material];
                if (mat.pbrMetallicRoughness.baseColorTexture.index >= 0) {
                    p.materialIndex = model.textures[mat.pbrMetallicRoughness.baseColorTexture.index].source;
                }
            }
            primitives.push_back(p);
        }
    }
    return true;
}

void GLBLoader::draw(GLuint shaderId) {
    GLint diffuseLoc = getUniformLocation(shaderId, "diffuseTexture");
    GLint hasTexLoc = getUniformLocation(shaderId, "hasTexture");
    for (auto& p : primitives) {
        if (p.materialIndex >= 0 && (size_t)p.materialIndex < textures.size()) {
            glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, textures[p.materialIndex]);
            glUniform1i(diffuseLoc, 0); glUniform1i(hasTexLoc, 1);
        } else glUniform1i(hasTexLoc, 0);
        glBindVertexArray(p.vao); glDrawElements(GL_TRIANGLES, p.indexCount, p.indexType, 0);
    }
}
