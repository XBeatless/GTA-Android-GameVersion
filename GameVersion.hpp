/*
* MIT License
*
* Copyright (c) 2023 BEATLESS
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

// create by XMDS on 2023-10-01
// A separate header file for the game version, so that it can be used independently in other projects.

#ifndef __XBEATLESS_GTA_ANDROID_GAME_VERSION_HPP__
#define __XBEATLESS_GTA_ANDROID_GAME_VERSION_HPP__

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <android/log.h>

// need GLossHook: https://github.com/XMDS/GlossHook
#include "Gloss.h"

#ifndef XB_TAG
#define XB_TAG "XB_GAME"
#define XB_LOGE(text) __android_log_write(ANDROID_LOG_ERROR, XB_TAG, text)
#endif // !XB_TAG

namespace XB
{
	enum GAME_ID : uint8_t
	{
		GTA3,
		GTAVC,
		GTASA,
		GTALCS,
		GTACTW,
		MAX_GAME = 5
	};

	enum GAME_VER : uint8_t
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

	inline const char* GetGameProcessName()
	{
		char name[255];
		memset(name, 0, sizeof(name));
		FILE* cmd_file = fopen("/proc/self/cmdline", "r");
		if (cmd_file) {
			if (fgets(name, sizeof(name) - 1, cmd_file))
				return name;
		}
		return NULL;
	}

	inline GAME_ID GetGameId()
	{
		const char* p_name = GetGameProcessName();
		if (p_name) {
			for (uint8_t i = GAME_ID::GTA3; i <= GTACTW; i++) {
				if (strcasestr(p_name, game_name[i])) {
					return (GAME_ID)i;
				}
			}
		}

		// if the process name(package name) is modified, try to load lib
		for (uint8_t i = GAME_ID::GTA3; i <= GTACTW; i++) {
			auto lib = GlossOpen(game_lib[i]);
			if (lib) {
				GlossClose(lib, false);
				return (GAME_ID)i;
			}
		}

		// if the process name(package name) and lib name are modified, try to load lib
		// TODO: add more lib check
		
		XB_LOGE("GetGameId error, Unknown Game.");
		return GAME_ID::MAX_GAME;
	}
	
	inline const char* GetGameName()
	{
		auto id = GetGameId();
		return id == GAME_ID::MAX_GAME ? NULL : game_name[id];
	}
	
	inline const char* GetGameLibName()
	{
		auto id = GetGameId();
		return id == GAME_ID::MAX_GAME ? NULL : game_lib[id];
	}

	inline GAME_VER GetGameVersionId()
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
		XB_LOGE("GetGameVersionId error, Unknown Game version.");
		return GAME_VER::MAX_VER;
	}
	
	inline const char* GetGameVersion()
	{
		auto id = GetGameVersionId();
		return id == GAME_VER::MAX_VER ? NULL : game_version[id];
	}
}

#endif // !__XBEATLESS_GTA_ANDROID_GAME_VERSION_HPP__

