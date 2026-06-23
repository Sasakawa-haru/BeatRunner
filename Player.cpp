#include "Player.h"
#include "Lane.h"
#include"ScoreSystem.h"
#include"OptionData.h"
#include "Engine/GameCsvReader.h"
#include "Engine/Time.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/BoxCollider.h"
#include "Engine/Camera.h"
#include "Engine/Audio.h"
#include "Engine/Image.h"
#include"RhythmLayout.h"
#include <cassert>
#include <cmath>

using namespace DirectX;

Player::Player(GameObject* parent)
    : GameObject(parent, "Player")
    , hEffect_(-1)
    , hPlayerModel_(-1)
    ,hColliderModel_(-1)
    , hMoveSound_(-1)
    ,PlayerPosition(3)
{
    GameCsvReader* Player = new GameCsvReader("Csv/PlayerState.csv");
    for (int i = 0; i < Player->GetLines(); i++)
    {
        std::string tag = Player->GetString(i, 0);

        if (tag == "jumpHeight")
        {
            jumpHeight = Player->GetFloat(i, 1);
        }
        if (tag == "upGravity")
        {
            upGravity = Player->GetFloat(i, 1);
        }
        if (tag == "downGravity")
        {
            downGravity = Player->GetFloat(i, 1);
        }
        if (tag == "diveGravity")
        {
            diveGravity = Player->GetFloat(i, 1);
        }
        if (tag == "radius")
        {
            radius_ = Player->GetFloat(i, 1);
        }
        if (tag == "PlayerHP")
        {
            PlayerHP = Player->GetInt(i, 1);
        }
        if (tag == "cameraHeight")
        {
			cameraHeight = Player->GetFloat(i, 1);
        }
        if (tag == "cameraBackDistance")
        {
            cameraBackDistance = Player->GetFloat(i, 1);
        }
        if (tag == "cameraTargetHeight")
        {
            cameraTargetHeight = Player->GetFloat(i, 1);
        }
        if (tag == "cameraTargetForward")
        {
            cameraTargetForward = Player->GetFloat(i, 1);
        }
    }

    delete Player;
    Player = nullptr;
}

Player::~Player()
{
}

void Player::Initialize()
{
    lanes_.clear();
    lanes_.resize(Lane::laneCount);

    for (int i = 0; i < Lane::laneCount; ++i)
    {
        std::string lname = "lane" + std::to_string(i + 1);
        Lane* lane = Lane::FindByName(lname);
        assert(lane);
        lanes_[i] = lane;
    }

    // 真ん中レーンに配置
    int centerIndex = Lane::laneCount / 2;
    Lane* centerLane = lanes_[centerIndex];
    assert(centerLane);

    XMFLOAT3 lanePos = centerLane->GetCenterPosition();

    float laneTopY = lanePos.y;
    float centerY = laneTopY + radius_;

    transform_.position_ = XMFLOAT3(
        centerLane->GetLaneCenterX(),
        centerY,
        RhythmLayout::PlayerZ
    );

    // モデル
    hPlayerModel_ = Model::Load("Models/Player.fbx");
    assert(hPlayerModel_ >= 0);
    hColliderModel_ = Model::Load("DebugCollision/BoxCollider.fbx");

    // SE
    hMoveSound_ = Audio::Load("Sound/SE/move.wav");
    assert(hMoveSound_ >= 0);
    Audio::SetVolume(hMoveSound_, gOptionData.SeVolume * gOptionData.MasterVolume);

    // 衝突エフェクト画像
    hEffect_ = Image::Load("Effect.png");
    assert(hEffect_ >= 0);

    // 当たり判定
    collider_ = new BoxCollider(
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(1.2f, 2.0f, 1.2f)
    );
    AddCollider(collider_);    // ジャンプ初期化
    isJumping_ = false;
    jumpVelocity_ = 0.0f;
    jumpSpeed = std::sqrt(2.0f * jumpHeight * -upGravity);

    // エフェクト初期化
    effectTimer_ = 0;
    effectTransform_ = Transform();
    effectTransform_.position_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    effectTransform_.scale_ = XMFLOAT3(0.2f, 0.2f, 1.0f);
}

void Player::Update()
{
    rhythmActionTriggered_ = false;

    if (isClearPerformance_)
    { 
        return;
    }

    if (effectTimer_ > 0)
    {
        effectTimer_--;
    }

    float dt = Time::DeltaTime();
    if (dt <= 0.0f || dt > 0.05f)
    {
        dt = 1.0f / 60.0f;
    }

    // --- 横移動 ---
    if (Input::IsKeyDown(DIK_A))
    {
        if (PlayerPosition > 1)
        {
            PlayerPosition--;

            Lane* lane = lanes_[PlayerPosition - 1];
            if (lane)
            {
                transform_.position_.x = lane->GetLaneCenterX();
            }

            Audio::Play(hMoveSound_);
            rhythmActionTriggered_ = true;
        }
    }

    if (Input::IsKeyDown(DIK_D))
    {
        if (PlayerPosition < 5)
        {
            PlayerPosition++;

            Lane* lane = lanes_[PlayerPosition - 1];
            if (lane)
            {
                transform_.position_.x = lane->GetLaneCenterX();
            }

            Audio::Play(hMoveSound_);
            rhythmActionTriggered_ = true;
        }
    }
    // --- ジャンプ開始 ---
    if (isJumping_ == false && Input::IsKeyDown(DIK_SPACE))
    {
        isJumping_ = true;
        jumpVelocity_ = jumpSpeed;
        rhythmActionTriggered_ = true;
    }

    // --- 重力・縦移動 ---
    if (isJumping_)
    {
        float g = (jumpVelocity_ > 0.0f) ? upGravity : downGravity;
        jumpVelocity_ += g * dt;
        transform_.position_.y += jumpVelocity_ * dt;
        
    }

    // --- 地面との接地判定 ---
    XMFLOAT3 rayStart(
        transform_.position_.x,
        transform_.position_.y + 100.0f,
        transform_.position_.z
    );
    XMFLOAT3 rayDir(0.0f, -1.0f, 0.0f);

    bool anyHit = false;
    float bestDist = 0.0f;

    for (Lane* lane : lanes_)
    {
        if (!lane) continue;

        int hLaneModel = lane->GetLaneHandle();

        RayCastData tmp{};
        tmp.start = rayStart;
        tmp.dir = rayDir;

        Model::RayCast(hLaneModel, &tmp);

        if (tmp.hit)
        {
            if (!anyHit || tmp.dist < bestDist)
            {
                anyHit = true;
                bestDist = tmp.dist;
            }
        }
    }

    if (anyHit)
    {
        float hitY = rayStart.y + rayDir.y * bestDist;
        float footY = transform_.position_.y - radius_;

        if (jumpVelocity_ <= 0.0f && footY <= hitY)
        {
            float diff = hitY - footY;
            transform_.position_.y += diff;

            isJumping_ = false;
            jumpVelocity_ = 0.0f;
        }
    }


    // --- カメラ追従 ---
    {
        XMFLOAT3 p = transform_.position_;

        XMFLOAT3 camPos(
            p.x,
            p.y + cameraHeight,
            p.z - cameraBackDistance
        );

        XMFLOAT3 camTarget(
            p.x,
            p.y+cameraTargetHeight,
            p.z+cameraTargetForward
        );

        Camera::SetPosition(camPos);
        Camera::SetTarget(camTarget);
    }

}

void Player::Draw()
{
    Model::SetTransform(hPlayerModel_, transform_);
    Model::Draw(hPlayerModel_);

    if (showCollider_)
    {
        Transform colTf = transform_;

        // BoxCollider の size と同じ値にする
        colTf.scale_ = XMFLOAT3(1.2f, 2.0f, 1.2f);

        Model::SetTransform(hColliderModel_, colTf);
        Model::Draw(hColliderModel_);
    }

    if (effectTimer_ > 0)
    {
        Image::SetTransform(hEffect_, effectTransform_);
        Image::Draw(hEffect_);
    }
}
void Player::Release()
{
}

void Player::OnCollision(GameObject* pTarget)
{
    if (pTarget->GetObjectName()=="RhythmNote")
    {
        Hit = true;
        PlayerHP -= 10;


        auto* score = (ScoreSystem*)FindObject("ScoreSystem");
        if (score)
        {
            score->OnMissCollision();
        }

        pTarget->KillMe();
    }
}

void Player::StartClearPerformance()
{
    isClearPerformance_ = true;
    clearTimer_ = 0.0f;
}

bool Player::UpdateClearPerformance(float dt)
{
    if (!isClearPerformance_)
    {
        return false;
    }
    clearTimer_ += dt;
    // 回転
    transform_.rotate_.y += clearRotateSpeed_ * dt;
    // 前進
    transform_.position_.z += clearForwardSpeed_ * dt;
    // 上昇
    transform_.position_.y += clearUpSpeed_ * dt;

    XMFLOAT3 p = transform_.position_;

    XMFLOAT3 camPos(
        p.x,
        p.y + 2.5f,
        p.z - 7.0f
    );

    XMFLOAT3 camTarget(
        p.x,
        p.y + 1.0f,
        p.z
    );

    Camera::SetPosition(camPos);
    Camera::SetTarget(camTarget);

    return clearTimer_ >= clearPerformanceTime_;
}