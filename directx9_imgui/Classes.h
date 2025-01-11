#pragma once
#include <cstdint>

class WeaponSlot
{
public:
    uint32_t Type; //0x0000
    uint32_t State; //0x0004
    uint32_t AmmoInClip; //0x0008
    uint32_t AmmoRemaining; //0x000C
    char pad_0010[12]; //0x0010

}; //Size: 0x001C

class ViewMatrix
{
public:
    float x1; //0x0000
    float x2; //0x0004
    float x3; //0x0008
    char pad_000C[4]; //0x000C
    float y1; //0x0010
    float y2; //0x0014
    float y3; //0x0018
    char pad_001C[4]; //0x001C
    float z1; //0x0020
    float z2; //0x0024
    float z3; //0x0028
    char pad_002C[4]; //0x002C
    float w1; //0x0030
    float w2; //0x0034
    float w3; //0x0038
    char pad_003C[4]; //0x003C

    inline float* m() {
		return new float[16] { x1, x2, x3, 0, y1, y2, y3, 0, z1, z2, z3, 0, w1, w2, w3, 1 };
    }
}; //Size: 0x0040

struct Vector3
{
	float x, y, z;
};

struct Vector2
{
	float x, y;
};

struct Matrix {
	float m[4][4];
};

class Camera
{
public:
	char pad_0000[768]; //0x0000
	Matrix vMatrix; //0x0304
	char pad_0344[12]; //0x0344
	Matrix vMatrix2; //0x034C
	char pad_038C[1324]; //0x038C
}; //Size: 0x08B8


class CPed
{
public:
    char pad_0000[20]; //0x0000
    class ViewMatrix* pViewMatrix; //0x0014
    char pad_0018[44]; //0x0018
    float SpeedVectorX; //0x0044
    float SpeedVectorY; //0x0048
    char pad_004C[116]; //0x004C
    void* CarNearest; //0x00C0
    char pad_00C4[936]; //0x00C4
    int8_t N0000037C; //0x046C
    char pad_046D[2]; //0x046D
    uint8_t CrouchState; //0x046F
    char pad_0470[208]; //0x0470
    float Health; //0x0540
    float MaxHealth; //0x0544
    char pad_0548[20]; //0x0548
    float Yaw;
    char pad_05482[64]; //0x0548
    class WeaponSlot WeaponSlot[13]; //0x05A0
    char pad_05A4[88]; //0x05A4
    CPed* CpedFired; //0x0764
    char pad_0768[92]; //0x0768

    void GetBonePosition(Vector3& outPosition, unsigned int boneId, bool updateSkinBones);
    inline Vector3 getCurrentPosition() {
        return { pViewMatrix->w1, pViewMatrix->w2, pViewMatrix->w3 };
    }
}; //Size: 0x07C4

enum ePedBones
{
    BONE_PELVIS1 = 1,
    BONE_PELVIS = 2,
    BONE_SPINE1 = 3,
    BONE_UPPERTORSO = 4,
    BONE_NECK = 5,
    BONE_HEAD2 = 6,
    BONE_HEAD1 = 7,
    BONE_HEAD = 8,
    BONE_RIGHTUPPERTORSO = 21,
    BONE_RIGHTSHOULDER = 22,
    BONE_RIGHTELBOW = 23,
    BONE_RIGHTWRIST = 24,
    BONE_RIGHTHAND = 25,
    BONE_RIGHTTHUMB = 26,
    BONE_LEFTUPPERTORSO = 31,
    BONE_LEFTSHOULDER = 32,
    BONE_LEFTELBOW = 33,
    BONE_LEFTWRIST = 34,
    BONE_LEFTHAND = 35,
    BONE_LEFTTHUMB = 36,
    BONE_LEFTHIP = 41,
    BONE_LEFTKNEE = 42,
    BONE_LEFTANKLE = 43,
    BONE_LEFTFOOT = 44,
    BONE_RIGHTHIP = 51,
    BONE_RIGHTKNEE = 52,
    BONE_RIGHTANKLE = 53,
    BONE_RIGHTFOOT = 54,
};

struct CPedPoolInfo {
    CPed* FirstCPed;
    uint8_t(*CPedsInUse)[140];
    uint32_t MaxCPed;
    uint32_t CurrentCPedNum;
};// Size: 0x10