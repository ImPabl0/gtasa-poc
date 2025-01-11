#include "Classes.h"

void CPed::GetBonePosition(Vector3& outPosition, unsigned int boneId, bool updateSkinBones)
{
	((void(__thiscall*)(CPed*, Vector3&, unsigned int, bool))0x5E4280)(this, outPosition, boneId, updateSkinBones);
}
