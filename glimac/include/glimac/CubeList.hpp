#pragma once

#include <vector>
#include <Eigen/Dense>

#include "common.hpp"
#include "Cube.hpp"

namespace glimac {

class CubeList {

public:

    // Constructeur
    CubeList();
    ~CubeList();

    // Get size
    uint getSize(){
        return m_cubeList.size();
    };

    // Get vertex count
    GLsizei getVertexCount(int index){
        return m_cubeList[index].getVertexCount();
    };

    // Get data pointer
    const Vertex3DTexture* getDataPointer(int index) const{
        return m_cubeList[index].getDataPointer();
    };

    // Get ibo count
    GLsizei getIBOCount(int index) const{
        return m_cubeList[index].getIBOCount();
    };

    // Get ibo count
    const uint32_t* getIBOPointer(int index) const{
        return m_cubeList[index].getIBOPointer();
    };

    // Get texture index
    GLuint getTextureIndex(int index){
        return m_cubeList[index].getTextureIndex();
    };

    // Set texture
    void setTextureIndex(int index, GLuint textureIndex);

    // Set cube index
    void setCubeIndex(int index, GLuint cubeIndex);
    GLuint getCubeIndex(int index){
        return m_cubeList[index].getCubeIndex();
    };

    // Scale
    void setScale(GLuint cubeIndex, GLfloat x, GLfloat y, GLfloat z);
    glm::vec3 getScale(int index){
        return m_cubeList[index].getScale();
    };
    // Rotate
    void setRot(GLuint cubeIndex, GLfloat degrees, GLfloat x, GLfloat y, GLfloat z);
    glm::vec3 getRot(int index){
        return m_cubeList[index].getRot();
    };
    GLfloat getRotDeg(int index){
        return m_cubeList[index].getRotDeg();
    };
    // Translate
    void setTrans(GLuint cubeIndex, GLfloat x, GLfloat y, GLfloat z);
    glm::vec3 getTrans(int index){
        return m_cubeList[index].getTrans();
    };

    //Ajouter/supprimer cube
    void addOrigin();
    void addCube(Cube cube);
    void deleteCube(int index);

    // Translate
    void translateCube(int index, GLfloat tx, GLfloat ty, GLfloat tz);

    //bool compareCubes(const int index1, const int index2);
    void sortCubes();

    void printCubes();
    Eigen::VectorXd RBF(Eigen::MatrixXd points);
    double interpolatePoints(double x, double y, Eigen::MatrixXd points);

//private:
    std::vector<Cube> m_cubeList;
};

}