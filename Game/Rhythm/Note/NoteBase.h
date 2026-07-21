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
    void SetGroupId(int id) { groupId_ = id; }
    int GetGroupId()const { return groupId_; }

private:
    int groupId_ = -1;
    int lane_ = 0;
    float hitTimeSec_ = 0.0f;
};

