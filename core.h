
// Copyright (c) 2024-2026 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.

#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

#ifndef CORE_H
#define CORE_H

#ifdef API_SHARED_LIB
#ifdef _WIN32
#ifdef LIB_EXPORT_API
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API __attribute__((visibility("default")))
#endif
#else
#define LIB_API
#endif

#ifdef __cplusplus
extern "C" {
#endif
	LIB_API int GetCoreVersion();
	LIB_API const char* GetCoreBuild();

#ifdef __cplusplus
}
#endif

#endif