#pragma once
#include "Engine/GameObject.h"
class NoteBase :
    public GameObject
{
public:
    NoteBase(GameObject*parent,const std::string&name)
        :GameObject(parent,name)
    {}
    void SetLane(int lane) { lane_ = lane; }
    int GetLane()const { return lane_; }
    void SetHitTimeSec(float t) { hitTimeSec_ = t; }
    float GetHitTimeSec()const { return hitTimeSec_; }

private:
    int lane_ = 0;
    float hitTimeSec_ = 0.0f;
};

