#pragma once

#include "Engine/GameObject.h"
#include "Engine/Model.h"

#include <vector>
#include <array>

class SideObject : public GameObject
{
public:
    SideObject(GameObject* parent);
    ~SideObject();

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

private:
    struct Building
    {
        int modelHandle = -1;
        Transform transform;
        float speed = 10.0f;
    };

    void SetupSidePositions();
    void CreateBuilding(float x, float z, float scaleX, float scaleY, float scaleZ);
    void ResetBuilding(Building& building);
    int GetRandomBuildingModel() const;

    std::vector<Building> buildings_;

    float scrollSpeed_ = 10.0f;

    float startZ_ = 80.0f;
    float endZ_ = -20.0f;
    float intervalZ_ = 12.0f;
	float sideOffset_ = 8.0f;

    float leftX_ = -8.0f;
    float rightX_ = 8.0f;

    int buildingCountPerSide_ = 8;

    // 3種類のビルモデル
    std::array<int, 3> hBuildingModels_ =
    {
        -1, -1, -1
    };
};