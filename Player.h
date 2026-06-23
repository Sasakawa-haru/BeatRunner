#pragma once
#include "Engine/GameObject.h"
#include <DirectXMath.h>
#include <vector>
#include <string>

class Lane;
class BoxCollider;

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

    int GetHP()const { return PlayerHP; }
    int GetLaneIndex()const { return PlayerPosition; }
    bool IsRhythmActionTriggered()const { return rhythmActionTriggered_; }

    void StartClearPerformance();
    bool UpdateClearPerformance(float dt);

private:
    int   hPlayerModel_;
    int   hColliderModel_;
    int   hMoveSound_;
    int   hEffect_;

    float radius_ ;   // プレイヤーの足元判定用半径
    float jumpVelocity_;
    bool  isJumping_ = false;
    bool showCollider_ = true;
    bool rhythmActionTriggered_ = false;

    float jumpHeight;
    float upGravity;
    float downGravity;
    float diveGravity;
    float jumpSpeed;
    int PlayerHP;

    float cameraHeight;
    float cameraBackDistance;
    float cameraTargetHeight;
    float cameraTargetForward;

    BoxCollider* collider_ = nullptr;
    std::vector<Lane*>   lanes_;      // 全レーンへのポインタ

    int PlayerPosition;
    bool Hit = false;
    int effectTimer_ = 0;
    Transform effectTransform_;

    DirectX::XMFLOAT3 WorldToScreen(const DirectX::XMFLOAT3& worldPos);

    bool isClearPerformance_ = false;
    float clearTimer_ = 0.0f;
    //クリア演出調整用
    float clearPerformanceTime_ = 2.0f;
    float clearRotateSpeed_ = 6.0f;
    float clearForwardSpeed_ = 4.0f;
    float clearUpSpeed_ = 0.5f;
};
