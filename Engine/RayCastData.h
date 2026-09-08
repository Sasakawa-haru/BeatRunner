#pragma once

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

struct RayCastData
{
    XMFLOAT3 start;   // レイ発射位置
    XMFLOAT3 dir;     // レイ方向
    float dist;       // 衝突点までの距離
    BOOL hit;         // 当たったか
    XMFLOAT3 normal;  // 法線

    RayCastData()
        : start(0.0f, 0.0f, 0.0f)
        , dir(0.0f, 0.0f, 0.0f)
        , dist(99999.0f)
        , hit(FALSE)
        , normal(0.0f, 0.0f, 0.0f)
    {
    }
};