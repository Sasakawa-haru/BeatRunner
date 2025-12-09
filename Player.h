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

private:
    int   hPlayerModel_;
    float radius_ = 3.0f;   // プレイヤーの足元判定用半径
    float jumpVelocity_;
    bool  isJumping_ = false;

    float jumpHeight;
    float upGravity;
    float downGravity;
    float jumpSpeed;

    SphereCollider* collider_ = nullptr;
    std::vector<Lane*>   lanes_;      // 全レーンへのポインタ
};
