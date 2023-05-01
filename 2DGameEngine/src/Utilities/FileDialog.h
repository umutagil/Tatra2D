#pragma once

#include <Windows.h>
#include <string>
#include <assert.h>

namespace FileDialog
{
	const char* datafileFilter = "Lua Files (*.lua) |*.lua\0*.lua\0 Tilemap Files (*.map) | *.map\0*.map\0";
	const char* imagefileFilter = "PNG Files (*.png) | *.png\0*.png\0 Bitmap Files (*.bmp) |*.bmp\0*.bmp\0 JPEG Files (*.jpg) |*.jpg\0*.jpg\0";

	static std::string OpenFile(const char* filter = datafileFilter, HWND owner = nullptr)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = owner;
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn)) {
			return ofn.lpstrFile;
		}

		return std::string();
	}

	static std::string SaveFile(const char* filter = datafileFilter, HWND owner = nullptr)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = owner;
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = szFile;
		ofn.lpstrDefExt = "lua"; // If the user does not put the lua extension, add it 
		ofn.nMaxFile = sizeof(szFile);
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&ofn)) {
			return ofn.lpstrFile;
		}

		return std::string();
	}

	static std::string OpenImageFile(const char* filter = imagefileFilter, HWND owner = nullptr)
	{
		assert(filter == imagefileFilter);
		return OpenFile(filter, owner);
	}
}



