#include "BRModelTest.h"

BRModelTest::BRModelTest(GameObject* parent)
    : GameObject(parent, "BRModelTest")
{
}

void BRModelTest::Initialize()
{
    const bool result =
        model_.Load(
            "Assets/Models/Object_A.brm"
        );

    if (!result)
    {
        OutputDebugStringA(
            "Object_A.brm Load Failed\n"
        );
    }
}

void BRModelTest::Update()
{
}

void BRModelTest::Draw()
{
    model_.Draw();
}

void BRModelTest::Release()
{
    model_.Release();
}