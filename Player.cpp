#include "Player.h"
#include"Lane.h"
#include"Engine/Model.h"
#include"Engine/Input.h"
#include"Engine/SphereCollider.h"
#include"Engine/Camera.h"



Player::Player(GameObject* parent)
	:GameObject(parent,"Player"),hPlayerModel_(-1)
{
}

Player::~Player()
{
}

void Player::Initialize()
{
    Lane* lane = (Lane*)FindObject("Lane");
    assert(lane);

    XMFLOAT3 lanePos = lane->GetCenterPosition();
    float laneTopY = lanePos.y;            // モデルの原点が床ならそのまま
    float centerY = laneTopY + radius_;   // 足が床に乗る高さ

    transform_.position_ = XMFLOAT3(lanePos.x, centerY, lanePos.z - 20.0f);

    hPlayerModel_ = Model::Load("Models/Player.fbx");
    assert(hPlayerModel_ >= 0);

    collider_ = new SphereCollider(transform_.position_, radius_);
    AddCollider(collider_);

    isJumping_ = false;
    jumpVelocity_ = 0.0f;
}
void Player::Update()
{
    Lane* lane = (Lane*)FindObject("Lane");
    assert(lane);
    int hLaneModel = lane->GetLaneHandle();

    if (Input::IsKeyDown(DIK_A))
    {
        transform_.position_.x -= 2.0f;
    }
    if (Input::IsKeyDown(DIK_D))
    {
        transform_.position_.x += 2.0f;
    }

    // --- ジャンプ開始 ---
    if (!isJumping_ && Input::IsKeyDown(DIK_SPACE))
    {
        isJumping_ = true;
        jumpVelocity_ = jumpV0;
    }

    // --- 重力・縦移動 ---
    if (isJumping_)
    {
        jumpVelocity_ += gravity;
        transform_.position_.y += jumpVelocity_; 
        //if (Input::IsKeyDown(DIK_SPACE)) {
        //    transform_.position_.y=
        //}
    }

    // --- 足元の地面を RayCast で取得 ---
    {
        // プレイヤーのちょっと上から真下に向けてレイを飛ばす
        XMFLOAT3 start(transform_.position_.x,
            transform_.position_.y + 100.0f,  // かなり上から
            transform_.position_.z);
        XMFLOAT3 dir(0.0f, -1.0f, 0.0f);

        RayCastData rayData{};
        rayData.start = start;
        rayData.dir = dir;

        Model::RayCast(hLaneModel, &rayData);

        if (rayData.hit)
        {
            // レーン表面の高さ
            float hitY = start.y + dir.y * rayData.dist; // dir.y=-1なので start.y - dist

            // プレイヤーの足の高さ
            float footY = transform_.position_.y - radius_;

            // 足が地面より下に行っていたら「乗せる」
            if (footY <= hitY)
            {
                float diff = hitY - footY;
                transform_.position_.y += diff;   // 足をちょうど地面にくっつける

                // 落下完了
                isJumping_ = false;
                jumpVelocity_ = 0.0f;
            }
        }

        XMFLOAT3 p = transform_.position_;

        XMFLOAT3 camPos(
            p.x,
            p.y + 2.0f,
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
}

