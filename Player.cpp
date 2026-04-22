#include "Player.h"
#include "Lane.h"
#include"ScoreSystem.h"
#include "Engine/GameCsvReader.h"
#include "Engine/Time.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/SphereCollider.h"
#include "Engine/Camera.h"
#include "Engine/Audio.h"
#include "Engine/Image.h"
#include <cassert>
#include <cmath>

using namespace DirectX;

Player::Player(GameObject* parent)
    : GameObject(parent, "Player")
    , hEffect_(-1)
    , hPlayerModel_(-1)
    , hMoveSound_(-1)
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
        if (tag == "radius")
        {
            radius_ = Player->GetFloat(i, 1);
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
        lanePos.x + Lane::laneWidth / 2.0f,
        centerY,
        lanePos.z - 10.0f
    );

    // モデル
    hPlayerModel_ = Model::Load("Models/Player.fbx");
    assert(hPlayerModel_ >= 0);

    // SE
    hMoveSound_ = Audio::Load("Sound/move.wav");
    assert(hMoveSound_ >= 0);

    // 衝突エフェクト画像
    hEffect_ = Image::Load("Effect.png");
    assert(hEffect_ >= 0);

    // 当たり判定
    collider_ = new SphereCollider(XMFLOAT3(0.0f, 0.0f, 0.0f), radius_);
    AddCollider(collider_);
    // ジャンプ初期化
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
        transform_.position_.x -= 2.0f;
        Audio::Play(hMoveSound_);
    }
    if (Input::IsKeyDown(DIK_D))
    {
        transform_.position_.x += 2.0f;
        Audio::Play(hMoveSound_);
    }

    // --- ジャンプ開始 ---
    if (isJumping_ == false && Input::IsKeyDown(DIK_SPACE))
    {
        isJumping_ = true;
        jumpVelocity_ = jumpSpeed;
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
            p.y + 3.0f,
            p.z - 10.0f
        );

        Camera::SetPosition(camPos);
        Camera::SetTarget(p);
    }

}

void Player::Draw()
{
    Model::SetTransform(hPlayerModel_, transform_);
    Model::Draw(hPlayerModel_);

    // まずは中央表示で確認
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
    if (pTarget->GetObjectName() == "VerticalBeam" ||
        pTarget->GetObjectName() == "BesideBeam")
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