#ifndef INSTANCED_GAME_OBJECT_H
#define INSTANCED_GAME_OBJECT_H

#include "GameObject.h"
#include "Model.h"

class InstancedGameObject : public GameObject {
private:
    unsigned int instanceMatrixBuffer = 0;

    void prepareInstanceMatrixBuffer();
    void updateInstanceMatrixBuffer();

public:
    InstancedGameObject();
    InstancedGameObject(Model* model, Shader* shader, std::vector<Transform*> transforms);
    ~InstancedGameObject() = default;

    //void draw(Transform parent, glm::mat4 projection, glm::mat4 view, bool dirty) override;
    //void update();

    std::vector<Transform*> instanceTransforms;

};


#endif // !INSTANCED_GAME_OBJECT_H
