#include "RhythmNote.h"
#include "Game/Config/OptionData.h"
#include "Engine/Model.h"
#include "Engine/Time.h"
#include "Engine/BoxCollider.h"

namespace
{
	constexpr float kKillZ = -10.0f;

	// 当たり判定のZ補正
	constexpr float kColliderOffsetZ = 0.0f;

	// 当たり判定のZサイズ
	constexpr float kColliderSizeZ = 0.3f;

	//縦ノーツの判定
	constexpr float kVerticalColliderSizeX = 1.0f;
	constexpr float kVerticalColliderSizeY = 20.0f;
	//横ノーツの判定
	constexpr float kBesideColliderSizeX = 10.0f;
	constexpr float kBesideColliderSizeY = 0.5f;
}

RhythmNote::RhythmNote(GameObject* parent)
	: NoteBase(parent, "RhythmNote")
	, hNoteModel_(-1)
	, hColliderModel_(-1)
	, beamType_(NotesType::VerticalNote)
	, PlayerHit(false)
{
}

RhythmNote::~RhythmNote()
{
}

void RhythmNote::Initialize()
{
	hNoteModel_ = -1;
	beamType_ = NotesType::VerticalNote;
	PlayerHit = false;

	hColliderModel_ = Model::Load("DebugCollision/BoxCollider.fbx");
	assert(hColliderModel_ >= 0);

	Enter();
	Visible();
}

void RhythmNote::Setup(NotesType type)
{
	beamType_ = type;

	if (beamType_ == NotesType::VerticalNote)
	{
		hNoteModel_ = Model::Load("Models/VerticalBeam.fbx");
	}
	else
	{
		hNoteModel_ = Model::Load("Models/BesideBeam.fbx");
	}

	CreateCollider();
}

void RhythmNote::CreateCollider()
{
	if (beamType_ == NotesType::VerticalNote)
	{
		BoxCollider* collision = new BoxCollider(
			XMFLOAT3(
				0.0f,
				0.0f,
				kColliderOffsetZ
			),
			XMFLOAT3(
				kVerticalColliderSizeX,
				kVerticalColliderSizeY,
				kColliderSizeZ
			)
		);

		AddCollider(collision);
	}
	else
	{
		BoxCollider* collision = new BoxCollider(
			XMFLOAT3(
				0.0f,
				0.0f,
				kColliderOffsetZ
			),
			XMFLOAT3(
				kBesideColliderSizeX,
				kBesideColliderSizeY,
				kColliderSizeZ
			)
		);

		AddCollider(collision);
	}
}
void RhythmNote::Update()
{
	float dt = Time::DeltaTime();

	transform_.position_.z -= GetActualNotesSpeed() * dt;

	if (transform_.position_.z < kKillZ)
	{
		KillMe();
	}
}

void RhythmNote::Draw()
{
	if (hNoteModel_ < 0) {
		return;
	}
	Model::SetTransform(hNoteModel_, transform_);
	Model::Draw(hNoteModel_);

	if (showCollider_ && hColliderModel_ >= 0)
	{
		Transform colTf = transform_;

		colTf.position_.z += kColliderOffsetZ;

		if (beamType_ == NotesType::VerticalNote)
		{
			colTf.scale_ = XMFLOAT3(
				kVerticalColliderSizeX,
				kVerticalColliderSizeY,
				kColliderSizeZ
			);
		}
		else
		{
			colTf.scale_ = XMFLOAT3(
				kBesideColliderSizeX,
				kBesideColliderSizeY,
				kColliderSizeZ
			);
		}

		Model::SetTransform(hColliderModel_, colTf);
		Model::Draw(hColliderModel_);
	}
}

void RhythmNote::Release()
{
}

void RhythmNote::OnCollision(GameObject* pTarget)
{
	PlayerHit = true;
}

