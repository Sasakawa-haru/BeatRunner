#pragma once
#include "Engine/GameObject.h"
#include <DirectXMath.h>
#include <vector>
#include <string>

class Lane;
class SphereCollider;

class Player : public GameObject
{
public:
    Player(GameObject* parent);
    ~Player();

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    void OnCollision(GameObject* pTarget)override;

private:
    int   hPlayerModel_;
    int   hColliderModel_;
    int   hMoveSound_;
    int   hEffect_;
    float radius_ ;   // プレイヤーの足元判定用半径
    float jumpVelocity_;
    bool  isJumping_ = false;
    bool showCollider_ = true;

    float jumpHeight;
    float upGravity;
    float downGravity;
    float diveGravity;
    float jumpSpeed;

    SphereCollider* collider_ = nullptr;
    std::vector<Lane*>   lanes_;      // 全レーンへのポインタ

    int PlayerHP = 100;
    bool Hit = false;
    int effectTimer_ = 0;
    Transform effectTransform_;

    DirectX::XMFLOAT3 WorldToScreen(const DirectX::XMFLOAT3& worldPos);
};
