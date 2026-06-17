#include "SideObject.h"
#include"Lane.h"
#include "Engine/Time.h"
#include<cstdlib>

SideObject::SideObject(GameObject* parent)
	:GameObject(parent, "SideObject")
{
}

SideObject::~SideObject()
{
}

void SideObject::Initialize()
{
	hBuildingModels_[0] = Model::Load("Models/Object_A.fbx");
	hBuildingModels_[1] = Model::Load("Models/Object_B.fbx");
	hBuildingModels_[2] = Model::Load("Models/Object_C.fbx");

	buildings_.clear();

    for (int i = 0; i < buildingCountPerSide_; ++i)
    {
        const float z = startZ_ - intervalZ_ * i;

        const float heightScaleA = 2.0f + static_cast<float>(i % 3);
        const float heightScaleB = 2.5f + static_cast<float>((i + 1) % 3);

        // 左側
        CreateBuilding(leftX_, z, 2.0f, heightScaleA, 2.0f);

        // 右側
        CreateBuilding(rightX_, z + intervalZ_ * 0.5f, 2.0f, heightScaleB, 2.0f);
    }
}

void SideObject::Update()
{
    const float deltaTime = Time::DeltaTime();

    for (Building& building : buildings_)
    {
        building.transform.position_.z -= building.speed * deltaTime;

        if (building.transform.position_.z <= endZ_)
        {
            ResetBuilding(building);
        }
    }
}

void SideObject::Draw()
{
    for (Building& building : buildings_)
    {
        Model::SetTransform(building.modelHandle, building.transform);
        Model::Draw(building.modelHandle);
    }
}

void SideObject::Release()
{
    buildings_.clear();

    for (int& modelHandle : hBuildingModels_)
    {
        if (modelHandle != -1)
        {
            Model::Release(modelHandle);
            modelHandle = -1;
        }
    }
}

void SideObject::SetupSidePositions()
{
    Lane* leftLane = Lane::FindByName("lane1");

    const std::string rightLaneName = "Lane" + std::to_string(Lane::laneCount);
	Lane* rightLane = Lane::FindByName(rightLaneName);
    if (leftLane != nullptr) {
		leftX_ = leftLane->GetLaneCenterX()-sideOffset_;
    }
    if (rightLane != nullptr) {
		rightX_ = rightLane->GetLaneCenterX()+sideOffset_;
    }
}

void SideObject::CreateBuilding(float x, float z, float scaleX, float scaleY, float scaleZ)
{
    Building building;

    building.modelHandle = GetRandomBuildingModel();

    building.transform.position_ = XMFLOAT3(x, 0.0f, z);
    building.transform.rotate_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    building.transform.scale_ = XMFLOAT3(scaleX, scaleY, scaleZ);

    building.speed = scrollSpeed_;

    buildings_.push_back(building);
}

void SideObject::ResetBuilding(Building& building)
{
    building.transform.position_.z = startZ_;

    // 手前から奥に戻るタイミングでモデルもランダム変更
    building.modelHandle = GetRandomBuildingModel();

    // 高さも少しランダムに変える
    const int randomHeight = std::rand() % 4;
    building.transform.scale_.y = 2.0f + static_cast<float>(randomHeight);

    if (building.transform.position_.x < 0.0f)
    {
        building.transform.position_.x = leftX_;
    }
    else
    {
        building.transform.position_.x = rightX_;
    }
}

int SideObject::GetRandomBuildingModel() const
{
    const int index = std::rand() % static_cast<int>(hBuildingModels_.size());
    return hBuildingModels_[index];
}