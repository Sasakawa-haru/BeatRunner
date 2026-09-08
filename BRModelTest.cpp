#include "BRModelTest.h"

BRModelTest::BRModelTest(GameObject* parent)
    : GameObject(parent, "BRModelTest")
{
}

void BRModelTest::Initialize()
{
    const bool result =
        model_.Load( 
            "Models/Object_A.brm"
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
    model_.Draw(transform_);
}

void BRModelTest::Release()
{
    model_.Release();
}