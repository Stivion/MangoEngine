#include "WindowsFileDialog.h"

#include <sstream>

#include <Windows.h>
#include <ShObjIdl.h>

bool Mango::WindowsFileDialog::Open(std::wstring* path) const
{
	IFileDialog* fileDialog = nullptr;
	HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&fileDialog));
	if (!SUCCEEDED(result))
	{
		CoUninitialize();
		return false;
	}

	result = fileDialog->Show(nullptr);
	if (!SUCCEEDED(result))
	{
		fileDialog->Release();
		CoUninitialize();
		return false;
	}

	IShellItem* item = nullptr;
	result = fileDialog->GetResult(&item);
	if (!SUCCEEDED(result))
	{
		fileDialog->Release();
		CoUninitialize();
		return false;
	}

	PWSTR filePath;
	result = item->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
	if (!SUCCEEDED(result))
	{
		item->Release();
		fileDialog->Release();
		CoUninitialize();
		return false;
	}

	std::wstringstream stream;
	stream << filePath;
	
	CoTaskMemFree(filePath);
	item->Release();
	fileDialog->Release();
	CoUninitialize();

	std::wstring finalFilePath = stream.str();
	path->resize(finalFilePath.length());
	finalFilePath.copy(path->data(), finalFilePath.length());
	return true;
}

bool Mango::WindowsFileDialog::Save(std::wstring* path) const
{
	IFileDialog* fileDialog = nullptr;
	HRESULT result = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&fileDialog));
	if (!SUCCEEDED(result))
	{
		CoUninitialize();
		return false;
	}

	result = fileDialog->Show(nullptr);
	if (!SUCCEEDED(result))
	{
		fileDialog->Release();
		CoUninitialize();
		return false;
	}

	IShellItem* item = nullptr;
	result = fileDialog->GetResult(&item);
	if (!SUCCEEDED(result))
	{
		fileDialog->Release();
		CoUninitialize();
		return false;
	}

	PWSTR filePath;
	result = item->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
	if (!SUCCEEDED(result))
	{
		item->Release();
		fileDialog->Release();
		CoUninitialize();
		return false;
	}

	std::wstringstream stream;
	stream << filePath;

	CoTaskMemFree(filePath);
	item->Release();
	fileDialog->Release();
	CoUninitialize();

	std::wstring finalFilePath = stream.str();
	path->resize(finalFilePath.length());
	finalFilePath.copy(path->data(), finalFilePath.length());
	return true;
}
