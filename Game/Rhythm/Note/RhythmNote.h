#pragma once
#include "Game/Rhythm/Note/NoteBase.h"

enum class NotesType
{
	VerticalNote,
	JumpNote
};

class RhythmNote : public NoteBase
{
public:
	RhythmNote(GameObject* parent);
	~RhythmNote() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release();

	void OnCollision(GameObject* pTarget) override;

	void Setup(NotesType type);

	void SetHighLigheted(bool highlighted) {
		highlighted_ = highlighted;
	}

	bool IsHighlighted()const {
		return highlighted_;
	}

private:
	void CreateCollider();

private:
	int hNoteModel_;
	int hHighlightModel_;
	int hColliderModel_;
	NotesType beamType_;
	bool PlayerHit;
	bool showCollider_ = false;// 当たり判定の視覚化
	bool highlighted_ = false;

};