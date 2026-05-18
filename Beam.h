#pragma once
#include "NoteBase.h"

enum class BeamType
{
	Vertical,
	Beside
};

class Beam : public NoteBase
{
public:
	Beam(GameObject* parent);
	~Beam() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release();

	void OnCollision(GameObject* pTarget) override;

	void Setup(BeamType type);

private:
	void CreateCollider();

private:
	int hBeamModel_;
	BeamType beamType_;
	bool PlayerHit;
};