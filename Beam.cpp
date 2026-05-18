#include "Beam.h"

#include "Engine/Model.h"
#include "Engine/Time.h"
#include "Engine/BoxCollider.h"

namespace
{
	constexpr float kBeamSpeed = 20.0f;
	constexpr float kKillZ = -10.0f;
}

Beam::Beam(GameObject* parent)
	: NoteBase(parent, "Beam")
	, hBeamModel_(-1)
	, beamType_(BeamType::Vertical)
	, PlayerHit(false)
{
}

Beam::~Beam()
{
}

void Beam::Initialize()
{
	// 初期状態は縦ビームにしておく
	Setup(BeamType::Vertical);

	Enter();
	Visible();
}

void Beam::Setup(BeamType type)
{
	beamType_ = type;

	if (beamType_ == BeamType::Vertical)
	{
		hBeamModel_ = Model::Load("Models/VerticalBeam.fbx");
	}
	else
	{
		hBeamModel_ = Model::Load("Models/BesideBeam.fbx");
	}

	CreateCollider();
}

void Beam::CreateCollider()
{
	if (beamType_ == BeamType::Vertical)
	{
		BoxCollider* collision = new BoxCollider(
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.0f, 20.0f, 1.0f)
		);
		AddCollider(collision);
	}
	else
	{
		BoxCollider* collision = new BoxCollider(
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(3.0f, 1.0f, 1.0f)
		);
		AddCollider(collision);
	}
}

void Beam::Update()
{
	float dt = Time::DeltaTime();

	transform_.position_.z -= kBeamSpeed * dt;

	if (transform_.position_.z < kKillZ)
	{
		KillMe();
	}
}

void Beam::Draw()
{
	Model::SetTransform(hBeamModel_, transform_);
	Model::Draw(hBeamModel_);
}

void Beam::Release()
{
}

void Beam::OnCollision(GameObject* pTarget)
{
	PlayerHit = true;
}