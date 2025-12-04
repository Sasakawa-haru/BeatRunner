#pragma once
#include "Engine/GameObject.h"
class BesideBeam :
    public GameObject
{
public:
    BesideBeam(GameObject* parent);
    ~BesideBeam()override;
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void Release()override;
private:
    int hBesideBeamModel_;

};

