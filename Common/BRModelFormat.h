#pragma once

#include <cstdint>


// ============================================
// BRM Version
// ============================================

constexpr uint32_t BRM_VERSION = 2;


// 1頂点に影響できる最大Bone数
constexpr uint32_t BRM_MAX_BONE_INFLUENCE = 4;


// Bone名最大長
constexpr uint32_t BRM_BONE_NAME_LENGTH = 64;


// ============================================
// Header
// ============================================

struct BRModelHeader
{
    // "BRM1"
    char magic[4];

    // ファイル形式Version
    uint32_t version;

    // 頂点数
    uint32_t vertexCount;

    // Index数
    uint32_t indexCount;

    // Bone数
    uint32_t boneCount;

    // Animation Frame数
    uint32_t frameCount;
};


// ============================================
// Vertex
// ============================================

struct BRVertex
{
    // Position
    float px;
    float py;
    float pz;

    // Normal
    float nx;
    float ny;
    float nz;

    // UV
    float u;
    float v;

    // Bone Index
    int32_t boneIndex[BRM_MAX_BONE_INFLUENCE];

    // Bone Weight
    float boneWeight[BRM_MAX_BONE_INFLUENCE];
};


// ============================================
// Bone
// ============================================

struct BRBone
{
    // Bone名
    char name[BRM_BONE_NAME_LENGTH];

    // 親Bone
    // 親がいない場合 -1
    int32_t parentIndex;

    // Bind Pose / Offset Matrix
    float bindPose[16];
};


// ============================================
// Animation
// 1Frame × 1Bone分
// ============================================

struct BRBoneTransform
{
    float matrix[16];
};