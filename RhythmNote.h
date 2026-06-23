#pragma once
#include "NoteBase.h"

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

private:
	void CreateCollider();

private:
	int hNoteModel_;
	int hColliderModel_;
	NotesType beamType_;
	bool PlayerHit;
	bool showCollider_ = false;// デバッグ用に当たり判定を表示するかどうか

};