#pragma once

#include "Engine/GameObject.h"
#include "Engine/BRModel.h"

class BRModelTest : public GameObject
{
public:
    BRModelTest(GameObject* parent);

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

private:
    BRModel model_;
};