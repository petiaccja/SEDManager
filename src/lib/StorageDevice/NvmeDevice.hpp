#pragma once


#ifdef _WIN32
#include "Windows/NvmeDevice.hpp"
#elif defined(__linux__)
#include "Linux/NvmeDevice.hpp"
#else
static_assert(false, "Storage devices are not implemented for this platform.");
#endif
