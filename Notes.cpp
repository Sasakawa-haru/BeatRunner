#include "Notes.h"
#include"VerticalBeam.h"
#include"BesideBeam.h"
#include"Engine/Model.h"
#include"Lane.h"


Notes::Notes(GameObject* parent)
	:GameObject(parent,"Notes")
{
}

Notes::~Notes()
{
}

void Notes::Initialize()
{
	Instantiate<VerticalBeam>;
	Instantiate<BesideBeam>;
}

void Notes::Update()
{
}

void Notes::Draw()
{
}

void Notes::Release()
{
}
