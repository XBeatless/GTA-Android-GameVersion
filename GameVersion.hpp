/*
MIT License

Copyright (c) 2023 BEATLESS

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// create by XMDS on 2023-10-01
// A separate header file for the game version, so that it can be used independently in other projects.

#ifndef __XBEATLESS_GTA_ANDROID_GAME_VERSION_HPP__
#define __XBEATLESS_GTA_ANDROID_GAME_VERSION_HPP__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <android/log.h>
#include <dlfcn.h>

// need GLossHook: https://github.com/XMDS/GlossHook
#include "Gloss.h"

namespace XB
{
	enum GAME_ID : unsigned char
	{
		GTA3,
		GTAVC,
		GTASA,
		GTALCS,
		GTACTW,
		MAX_GAME = 5
	};

	enum GAME_VER : unsigned char
	{
		// 32
		III_1_8,
		VC_1_09,
		SA_2_00,
		LCS_2_4,
		CTW_1_04,
		// hign ver
		III_1_9,
		VC_1_12,
		SA_2_10,
		// 64
		III_1_9_64,
		VC_1_12_64,
		SA_2_10_64,
		MAX_VER = 11
	};

	static const char* game_name[] = { "GTA3", "GTAVC", "GTASA", "GTALCS", "GTACTW" };
	static const char* game_version[] = { "1.8", "1.09", "2.00", "2.4", "1.04", "1.9", "1.12", "2.10", "1.9", "1.12", "2.10" };
	static const char* game_lib[] = { "libR1.so", "libGTAVC.so", "libGTASA.so", "LibGTALcs.so", "LibCTW.so" };

	static const char* GetGameProcessName()
	{
		static char name[255];
		memset(name, 0, sizeof(name));
		FILE* cmd_file = fopen("/proc/self/cmdline", "r");
		if (cmd_file) {
			if (fgets(name, sizeof(name), cmd_file))
				return name;
		}
		return NULL;
	}

	static GAME_ID GetGameId()
	{
		const char* p_name = GetGameProcessName();
		if (p_name) {
			if (strcasestr(p_name, game_name[GAME_ID::GTA3])) {
				return GAME_ID::GTA3;
			}
			else if (strcasestr(p_name, game_name[GAME_ID::GTAVC])) {
				return GAME_ID::GTAVC;
			}
			else if (strcasestr(p_name, game_name[GAME_ID::GTASA])) {
				return GAME_ID::GTASA;
			}
			else if (strcasestr(p_name, game_name[GAME_ID::GTALCS])) {
				return GAME_ID::GTALCS;
			}
			else if (strcasestr(p_name, game_name[GAME_ID::GTACTW])) {
				return GAME_ID::GTACTW;
			}
		}
		// if not found, try to load lib
		// RTLD_NOLOAD: Don't load the library.
		// This can be used to test if the library is already resident (dlopen() returns NULL if it is not, or the library's handle if it is resident).
		for (uint8_t i = GAME_ID::GTA3; i <= GTACTW; i++) {
			if (dlopen(game_lib[i], RTLD_NOLOAD)) {
				return (GAME_ID)i;
			}
		}
		// if not found, try to load lib again
		// RTLD_LAZY: Perform lazy binding. Only resolve symbols as the code that references them is executed.
		for (uint8_t i = GAME_ID::GTA3; i <= GTACTW; i++) {
			if (dlopen(game_lib[i], RTLD_LAZY)) {
				return (GAME_ID)i;
			}
		}
		__android_log_write(ANDROID_LOG_ERROR, "GAME", "GetGameId fail, Unknown Game.");
		return GAME_ID::MAX_GAME;
	}

	static inline const char* GetGameName()
	{
		auto id = GetGameId();
		return id == GAME_ID::MAX_GAME ? NULL : game_name[id];
	}

	static inline const char* GetGameLibName()
	{
		auto id = GetGameId();
		return id == GAME_ID::MAX_GAME ? NULL : game_lib[id];
	}

	static GAME_VER GetGameVersionId()
	{
		const char* lib_name = GetGameLibName();
		if (lib_name) {
			uintptr_t plt_addr = GlossGetLibSection(lib_name, ".plt", NULL);
			uintptr_t re_addr = plt_addr - GlossGetLibBase(lib_name, -1);
			if (re_addr && re_addr != plt_addr) {
				switch (re_addr)
				{
				case 0x00189D44:
					return GAME_VER::SA_2_00;
				case 0x000AFBA0:
					return GAME_VER::VC_1_09;
				case 0x000970B8:
					return GAME_VER::III_1_8;
				case 0x00109784:
					return GAME_VER::LCS_2_4;
				case 0x002987F8:
					return GAME_VER::CTW_1_04;
				case 0x00189CCC:
					return GAME_VER::SA_2_10;
				case 0x000BAE98:
					return GAME_VER::VC_1_12;
				case 0x00097140:
					return GAME_VER::III_1_9;
				case 0x0000000000218E90:
					return GAME_VER::SA_2_10_64;
				case 0x00000000000FFD90:
					return GAME_VER::VC_1_12_64;
				case 0x00000000000CE100:
					return GAME_VER::III_1_9_64;
				default:
					break;
				}
			}
		}
		__android_log_write(ANDROID_LOG_ERROR, "GAME", "GetGameVersion fail, Unknown Game version.");
		return GAME_VER::MAX_VER;
	}

	static inline const char* GetGameVersion()
	{
		auto id = GetGameVersionId();
		return id == GAME_VER::MAX_VER ? NULL : game_version[id];
	}
}

#endif // !__XBEATLESS_GTA_ANDROID_GAME_VERSION_HPP__

