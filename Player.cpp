#include "Player.h"
#include "Lane.h"
#include"Engine/GameCsvReader.h"
#include "Engine/Time.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/SphereCollider.h"
#include "Engine/Camera.h"
#include <cassert>
#include<cmath>

using namespace DirectX;

Player::Player(GameObject* parent)
    : GameObject(parent, "Player")
    , hPlayerModel_(-1)
{
    GameCsvReader* Player = new GameCsvReader("Assets/Csv/PlayerState.csv");
    for (int i = 0; i < Player->GetLines(); i++) {
        std::string tag = Player->GetString(i, 0);
        if (tag == "jumpHeight") {
            jumpHeight = Player->GetFloat(i, 1);
        }
        if (tag == "upGravity") {
            upGravity = Player->GetFloat(i, 1);
        }
        if (tag == "downGravity") {
            downGravity = Player->GetFloat(i, 1);
        }
    }
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
    // 真ん中のレーンを初期位置に使う
    int centerIndex = Lane::laneCount / 2;   
    Lane* centerLane = lanes_[centerIndex];
    assert(centerLane);

    XMFLOAT3 lanePos = centerLane->GetCenterPosition();

    float laneTopY = lanePos.y;
    float centerY = laneTopY + radius_;

    transform_.position_ = XMFLOAT3(
        lanePos.x+Lane::laneWidth/2,
        centerY,
        lanePos.z - 10.0f
    );

    // モデル読み込み
    hPlayerModel_ = Model::Load("Models/Player.fbx");
    assert(hPlayerModel_ >= 0);

    // 当たり判定
    collider_ = new SphereCollider(transform_.position_, radius_);
    AddCollider(collider_);

    isJumping_ = false;
    jumpSpeed = std::sqrt(2.0f * jumpHeight * -upGravity);
}
void Player::Update()
{
    float dt = Time::DeltaTime();
    if (dt <= 0.0f || dt > 0.05f)  // 0〜0.05秒の範囲にクランプ（20fps相当）
    {
        dt = 1.0f / 60.0f;
    }    // --- 横移動 ---
    if (Input::IsKeyDown(DIK_A))
    {
        transform_.position_.x -= 2.0f;
    }
    if (Input::IsKeyDown(DIK_D))
    {
        transform_.position_.x += 2.0f;
    }

    // --- ジャンプ開始 ---
    if (isJumping_==false && Input::IsKeyDown(DIK_SPACE))
    {
        isJumping_ = true;
        jumpVelocity_ = jumpSpeed;
    }

    // --- 重力・縦移動 ---
    if (isJumping_)
    {
        float g = (jumpVelocity_ > 0.0f) ? upGravity : downGravity;
        jumpVelocity_ += g*dt;
        transform_.position_.y += jumpVelocity_*dt;
    }

    XMFLOAT3 rayStart(
        transform_.position_.x,
        transform_.position_.y + 100.0f,   
        transform_.position_.z
    );
    XMFLOAT3 rayDir(0.0f, -1.0f, 0.0f);   

    bool  anyHit = false;
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
        // レーン表面の高さ
        float hitY = rayStart.y + rayDir.y * bestDist; // dir.y=-1 なので rayStart.y - dist

        // プレイヤーの足の高さ（中心 - 半径）
        float footY = transform_.position_.y - radius_;

        // 足が地面より下に行っていたら「乗せる」
        if (footY <= hitY)
        {
            float diff = hitY - footY;
            transform_.position_.y += diff;

            // 落下完了
            isJumping_ = false;
            jumpVelocity_ = 0.0f;
        }
    }

    // --- カメラ追従 ---
    {
        XMFLOAT3 p = transform_.position_;

        XMFLOAT3 camPos(
            p.x,
            p.y + 1.0f,
            p.z - 5.0f
        );
        Camera::SetPosition(camPos);
        Camera::SetTarget(p);
    }

    // --- コライダーの中心を更新 ---
    if (collider_)
    {
        collider_->SetCenter(transform_.position_);
    }
}

void Player::Draw()
{
    Model::SetTransform(hPlayerModel_, transform_);
    Model::Draw(hPlayerModel_);
}

void Player::Release()
{
    // 必要なら collider_ の delete など
}
