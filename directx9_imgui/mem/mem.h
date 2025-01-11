#pragma once
#include <cstdint>
template<typename T>
inline T* ptr_add(T* cped, uint32_t n) noexcept {
	char* ptr = (char*)cped;
	ptr += n;
	return (T*)ptr;
};
