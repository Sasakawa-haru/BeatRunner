#include "camera.h"
#include"Time.h"
#include "Direct3D.h"

XMFLOAT3 _position;
XMFLOAT3 _target;
XMMATRIX _view;
XMMATRIX _proj;
XMMATRIX _billBoard;

float shakePower = 0.0f;
float shakeTime = 0.0f;

// 0.0 ～ 1.0 の乱数
static float Rand01()
{
	return (float)std::rand() / (float)RAND_MAX;
}

// -1.0 ～ 1.0 の乱数
static float RandRange11()
{
	return Rand01() * 2.0f - 1.0f;
}

//初期化（プロジェクション行列作成）
void Camera::Initialize()
{
	_position = XMFLOAT3(0, 3, -10);	//カメラの位置
	_target = XMFLOAT3( 0, 0, 0);	//カメラの焦点

	//プロジェクション行列
	_proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)Direct3D::screenWidth_ / (FLOAT)Direct3D::screenHeight_, 0.1f, 1000.0f);
	shakePower = 0.0f;
	shakeTime = 0.0f;
}

//更新（ビュー行列作成）
void Camera::Update()
{
	//画面揺らし
	XMFLOAT3 shakeOffset(0, 0, 0);
	if (shakeTime > 0)
	{
		shakeTime -= Time::DeltaTime();
		if (shakeTime < 0.0f)shakeTime = 0.0f;
		shakeOffset.x = RandRange11() * shakePower;
		shakeOffset.y = RandRange11() * shakePower;
		shakeOffset.z = 0.0f;
	}
	XMFLOAT3 camPos = XMFLOAT3(
		_position.x + shakeOffset.x, 
		_position.y + shakeOffset.y, 
		_position.z + shakeOffset.z
	);

	//ビュー行列
	_view = XMMatrixLookAtLH(XMVectorSet(_position.x, _position.y, _position.z, 0),
		XMVectorSet(_target.x, _target.y, _target.z, 0), XMVectorSet(0, 1, 0, 0));


	//ビルボード行列
	//（常にカメラの方を向くように回転させる行列。パーティクルでしか使わない）
	//http://marupeke296.com/DXG_No11_ComeOnBillboard.html
	_billBoard = XMMatrixLookAtLH(XMVectorSet(0, 0, 0, 0), XMLoadFloat3(&_target)- XMLoadFloat3(&_position), XMVectorSet(0, 1, 0, 0));
	_billBoard = XMMatrixInverse(nullptr, _billBoard);
}

//焦点を設定
void Camera::SetTarget(XMFLOAT3 target) { _target = target; }

void Camera::StartShake(float power, float TimeSec)
{
	shakePower = power;
	shakeTime = TimeSec;
}

//位置を設定
void Camera::SetPosition(XMFLOAT3 position) { _position = position; }

//焦点を取得
XMFLOAT3 Camera::GetTarget() { return _target; }

//位置を取得
XMFLOAT3 Camera::GetPosition() { return _position; }

//ビュー行列を取得
XMMATRIX Camera::GetViewMatrix() { return _view; }

//プロジェクション行列を取得
XMMATRIX Camera::GetProjectionMatrix() { return _proj; }

//ビルボード用回転行列を取得
XMMATRIX Camera::GetBillboardMatrix(){	return _billBoard; }
