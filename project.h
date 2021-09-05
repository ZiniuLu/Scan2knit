/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <imgui.h>

# define PROJECT_NAME Scan2knit
# define BEGIN_PROJECT_NAMESPACE namespace PROJECT_NAME {
# define END_PROJECT_NAMESPACE }

#define STRING(x) #x
#define TO_STRING(x) STRING(x)

BEGIN_PROJECT_NAMESPACE

struct GuiConsole
{
	char                  InputBuf[256];
	ImVector<char*>       Items;
	ImVector<const char*> Commands;
	ImVector<char*>       History;
	int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter       Filter;
	bool                  AutoScroll;
	bool                  ScrollToBottom;

	GuiConsole();
	~GuiConsole();

	// Portable helpers
	static int   Stricmp(const char* s1, const char* s2);
	static int   Strnicmp(const char* s1, const char* s2, int n);
	static char* Strdup(const char* s);
	static void  Strtrim(char* s);

	void         ClearLog();
	void         AddLog(const char* fmt, ...);
	void         Draw(const char* title, bool* p_open);
	void         ExecCommand(const char* command_line);

	static int   TextEditCallbackStub(ImGuiInputTextCallbackData* data);
	int          TextEditCallback(ImGuiInputTextCallbackData* data);
};

END_PROJECT_NAMESPACE

/// <summary>
/// print_to = 
/// 0...output to console
/// 1...output to gui	-- default
/// </summary>
extern size_t print_to;
/// <summary>
/// process_nr = 
/// 0...Reset	-- default
/// 1...Load Settings (|| isotropic Remeshing)
/// 2...Load Mesh File
/// 3...extract skeleton
/// 4...segmentation
/// 5...slicing
/// </summary>
extern size_t process_nr;
extern PROJECT_NAME::GuiConsole* gui_console;

void Print(const std::string& text);
void Print(const std::string& text, size_t print_to);