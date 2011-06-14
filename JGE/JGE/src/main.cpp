//-------------------------------------------------------------------------------------
//
// JGE is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h> 
#include <psppower.h>
#include <pspsdk.h> 
#include <pspaudiocodec.h> 
#include <pspaudio.h> 
#include <pspaudiolib.h>
#include <pspmpeg.h>
#include <malloc.h>
#include <string.h>


//#include <mikmod.h>

#include "../../JGE/include/JGE.h"
#include "../../JGE/include/JApp.h"
#include "../../JGE/include/JGameLauncher.h"
#include "../../JGE/include/JRenderer.h"

#ifndef JGEApp_Title
#define JGEApp_Title "JGE++"
#endif


#ifdef DEVHOOK
	
	PSP_MODULE_INFO(JGEApp_Title, 0, 1, 1);
	PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
	PSP_HEAP_SIZE_KB(-256);

#else

	PSP_MODULE_INFO(JGEApp_Title, 0x1000, 1, 1);
	PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#endif


int mikModThreadID = -1;
bool done = false;

JApp *game = NULL;
JGE *engine = NULL;

string exceptionDebugString;

//------------------------------------------------------------------------------------------------
// Exit callback
int exit_callback(int arg1, int arg2, void *common)
{
	if (engine != NULL)
		engine->End();

	//sceKernelExitGame();
	sceKernelDelayThread(100);

	return 0;
}

//------------------------------------------------------------------------------------------------
// Power Callback
int power_callback(int unknown, int pwrflags, void *common)
{
	if ((pwrflags & (PSP_POWER_CB_POWER_SWITCH | PSP_POWER_CB_STANDBY)) > 0) 
	{
		// suspending
		if (engine != NULL)
			engine->Pause();

		//sceKernelDelayThread(1000000);
	}
	else if ((pwrflags & PSP_POWER_CB_RESUME_COMPLETE) > 0)	
	{
		sceKernelDelayThread(1500000);
		// resume complete
		if (engine != NULL)
			engine->Resume();
	}

//     /* check for power switch and suspending as one is manual and the other automatic */
//     if (pwrflags & PSP_POWER_CB_POWER_SWITCH || pwrflags & PSP_POWER_CB_SUSPENDING) 
// 	{
// 		// suspending
// 		if (engine != NULL)
// 			engine->Pause();
//     } 
// 	else if (pwrflags & PSP_POWER_CB_RESUMING) 
// 	{
// 		// resuming from suspend mode
// 
//     } 
// 	else if (pwrflags & PSP_POWER_CB_RESUME_COMPLETE) 
// 	{
// 		// resume complete
// 		if (engine != NULL)
// 			engine->Resume();
//     } 
// 	else if (pwrflags & PSP_POWER_CB_STANDBY)
// 	{
// 		// entering standby mode
// 		if (engine != NULL)
// 			engine->Pause();
//     } 
// 	else 
// 	{
// 		// unhandled power event
//     }
//     sceDisplayWaitVblankStart();

	return 0;
}

//------------------------------------------------------------------------------------------------
// Callback thread
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

#ifdef DEVHOOK
	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
#endif
	cbid = sceKernelCreateCallback("Power Callback", power_callback, NULL);
    scePowerRegisterCallback(0, cbid);

	sceKernelSleepThreadCB();

	return 0;
}

//------------------------------------------------------------------------------------------------
// static int AudioChannelThread(SceSize args, void *argp)
// {
// 	while (!done)
// 	{
// 		MikMod_Update();
// 		// We have to sleep here to allow other threads a chance to process.
// 		// with no sleep this thread will take over when the output is disabled via MikMod_DisableOutput()
// 		// co-operative threading sucks bigtime...
// 		sceKernelDelayThread(1);
// 	}
// 	return (0);
// }

//------------------------------------------------------------------------------------------------
// Sets up the callback thread and returns its thread id
int SetupCallbacks(void)
{
	int thid = 0;

    thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0) 
	{
		sceKernelStartThread(thid, 0, 0);
    }

    return thid;
} 



#ifdef DEVHOOK
//code by sakya, crazyc, samuraiX
//http://forums.ps2dev.org/viewtopic.php?t=9591
PspDebugRegBlock exception_regs;

extern int _ftext;

static const char *codeTxt[32] =
{
	"Interrupt", "TLB modification", "TLB load/inst fetch", "TLB store",
	"Address load/inst fetch", "Address store", "Bus error (instr)",
	"Bus error (data)", "Syscall", "Breakpoint", "Reserved instruction",
	"Coprocessor unusable", "Arithmetic overflow", "Unknown 14",
	"Unknown 15", "Unknown 16", "Unknown 17", "Unknown 18", "Unknown 19",
	"Unknown 20", "Unknown 21", "Unknown 22", "Unknown 23", "Unknown 24",
	"Unknown 25", "Unknown 26", "Unknown 27", "Unknown 28", "Unknown 29",
	"Unknown 31"
};

static const unsigned char regName[32][5] =
{
	"zr", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};

u32 _ramAvailableLineareMax(void)
{
	u32 size, sizeblock;
	u8 *ram;

	// Init variables
	size = 0;
	sizeblock = (1024 * 1024);

#ifdef PSP
	int disableInterrupts = pspSdkDisableInterrupts();
#endif

	// Check loop
	while (sizeblock)
	{
		// Increment size
		size += sizeblock;

		// Allocate ram
		ram = (u8 *) malloc(size);

		// Check allocate
		if (!(ram))
		{
			// Restore old size
			size -= sizeblock;

			// Size block / 2
			sizeblock >>= 1;
		}
		else
			free(ram);
	}

#ifdef PSP
	pspSdkEnableInterrupts(disableInterrupts);
#endif

	return size;
}

u32 _ramAvailable(void)
{
	u8 **ram, **temp;
	u32 size, count, x;

	// Init variables
	ram = NULL;
	size = 0;
	count = 0;

#ifdef PSP
	int disableInterrupts = pspSdkDisableInterrupts();
#endif

	// Check loop
	for (;;)
	{
		// Check size entries
		if (!(count % 10))
		{
			// Allocate more entries if needed
			temp = (u8**) realloc(ram, sizeof(u8 *) * (count + 10));
			if (!(temp)) break;

			// Update entries and size (size contains also size of entries)
			ram = temp;
			size += (sizeof(u8 *) * 10);
		}

		// Find max lineare size available
		x = _ramAvailableLineareMax();
		if (!(x)) break;

		// Allocate ram
		ram[count] = (u8 *) malloc(x);
		if (!(ram[count])) break;

		// Update variables
		size += x;
		count++;
	}

	// Free ram
	if (ram)
	{
		for (x = 0; x < count; x++)
			free(ram[x]);
		free(ram);
	}

#ifdef PSP
	pspSdkEnableInterrupts(disableInterrupts);
#endif
	return size;
}

void ExceptionHandler(PspDebugRegBlock * regs)
{
	int i;
	SceCtrlData pad;

	pspDebugScreenInit();
	pspDebugScreenSetBackColor(0x00FF0000);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenClear();
	pspDebugScreenPrintf("Your PSP has just crashed!\n");
	pspDebugScreenPrintf("Exception details:\n\n");

	pspDebugScreenPrintf("Exception - %s\n", codeTxt[(regs->cause >> 2) & 31]);
	pspDebugScreenPrintf("EPC       - %08X / %s.text + %08X\n", (int)regs->epc, module_info.modname, (unsigned int)(regs->epc-(int)&_ftext));
	pspDebugScreenPrintf("Cause     - %08X\n", (int)regs->cause);
	pspDebugScreenPrintf("Status    - %08X\n", (int)regs->status);
	pspDebugScreenPrintf("BadVAddr  - %08X\n", (int)regs->badvaddr);
	for (i = 0; i < 32; i += 4) pspDebugScreenPrintf("%s:%08X %s:%08X %s:%08X %s:%08X\n", regName[i], (int)regs->r[i], regName[i+1], (int)regs->r[i+1], regName[i+2], (int)regs->r[i+2], regName[i+3], (int)regs->r[i+3]);

	pspDebugScreenPrintf("\nRam free : %d o\n", (int)_ramAvailable());
	pspDebugScreenPrintf("\n%s\n", exceptionDebugString.c_str());

	sceKernelDelayThread(1000000);
	pspDebugScreenPrintf("\n\nPress X to dump information on file exception.log and quit");
	pspDebugScreenPrintf("\nPress O to quit");

	for (;;)
	{
		sceCtrlReadBufferPositive(&pad, 1);
		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			FILE *log = fopen("exception.log", "w");
			if (log != NULL)
			{
				char testo[512];
				sprintf(testo, "Exception details:\n\n");
				fwrite(testo, 1, strlen(testo), log);
				sprintf(testo, "Exception - %s\n", codeTxt[(regs->cause >> 2) & 31]);
				fwrite(testo, 1, strlen(testo), log);
				sprintf(testo, "EPC       - %08X / %s.text + %08X\n", (int)regs->epc, module_info.modname, (unsigned int)(regs->epc-(int)&_ftext));
				fwrite(testo, 1, strlen(testo), log);
				sprintf(testo, "Cause     - %08X\n", (int)regs->cause);
				fwrite(testo, 1, strlen(testo), log);
				sprintf(testo, "Status    - %08X\n", (int)regs->status);
				fwrite(testo, 1, strlen(testo), log);
				sprintf(testo, "BadVAddr  - %08X\n", (int)regs->badvaddr);
				fwrite(testo, 1, strlen(testo), log);
				for (i = 0; i < 32; i += 4)
				{
					sprintf(testo, "%s:%08X %s:%08X %s:%08X %s:%08X\n", regName[i], (int)regs->r[i], regName[i+1], (int)regs->r[i+1], regName[i+2], (int)regs->r[i+2], regName[i+3], (int)regs->r[i+3]);
					fwrite(testo, 1, strlen(testo), log);
				}
				fclose(log);
			}
			break;
		}
		else if (pad.Buttons & PSP_CTRL_CIRCLE)
			break;
		sceKernelDelayThread(100000);
	}
	sceKernelExitGame();
}

void initExceptionHandler()
{
	SceKernelLMOption option;
	int args[2], fd, modid;

	memset(&option, 0, sizeof(option));
	option.size = sizeof(option);
	option.mpidtext = PSP_MEMORY_PARTITION_KERNEL;
	option.mpiddata = PSP_MEMORY_PARTITION_KERNEL;
	option.position = 0;
	option.access = 1;

	if ((modid = sceKernelLoadModule("exception.prx", 0, &option)) >= 0)
	{
		args[0] = (int)ExceptionHandler;
		args[1] = (int)&exception_regs;
		sceKernelStartModule(modid, 8, args, &fd, NULL);
	}
}

void EndExceptionHandler()
{
	SceKernelLMOption option;
	int args[2], fd, modid;

	memset(&option, 0, sizeof(option));
	option.size = sizeof(option);
	option.mpidtext = PSP_MEMORY_PARTITION_KERNEL;
	option.mpiddata = PSP_MEMORY_PARTITION_KERNEL;
	option.position = 0;
	option.access = 1;

	if ((modid = sceKernelLoadModule("exception.prx", 0, &option)) >= 0)
	{
		args[0] = (int)ExceptionHandler;
		args[1] = (int)&exception_regs;
		sceKernelStartModule(modid, 8, args, &fd, NULL);
	}
}
#else
//------------------------------------------------------------------------------------------------
// Custom exception handler
void MyExceptionHandler(PspDebugRegBlock *regs)
{
	pspDebugScreenInit();

	pspDebugScreenSetBackColor(0x00FF0000);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenClear();

	pspDebugScreenPrintf("I regret to inform you your psp has just crashed\n");
	pspDebugScreenPrintf("Please contact Sony technical support for further information\n\n");
	pspDebugScreenPrintf("Exception Details:\n");
	pspDebugDumpException(regs);
	pspDebugScreenPrintf("\nBlame the 3rd party software, it cannot possibly be our fault!\n");

	sceKernelExitGame();
}

//------------------------------------------------------------------------------------------------
// Sort of hack to install exception handler under USER THREAD
__attribute__((constructor)) void handlerInit()
{
	pspKernelSetKernelPC();

	pspSdkInstallNoDeviceCheckPatch();
	pspSdkInstallNoPlainModuleCheckPatch();
	pspSdkInstallKernelLoadModulePatch();

	pspDebugInstallErrorHandler(MyExceptionHandler);
}

#endif

//------------------------------------------------------------------------------------------------
// The main loop
int main(int argc, char* argv[])
{ 
	//scePowerSetClockFrequency(333, 333, 166); 

	SetupCallbacks(); 

#ifdef DEVHOOK
	initExceptionHandler();
#endif

//	pspDebugScreenInit();

// 	if ((mikModThreadID = sceKernelCreateThread("MikMod" , AudioChannelThread, 0x12,0x10000,0,0)) > 0)
// 	{
// 		sceKernelStartThread(mikModThreadID, 0 , 0);
// 	}

	engine = NULL;

	//engine->Init(0);
//
	JGameLauncher* launcher = new JGameLauncher();

	u32 flags = launcher->GetInitFlags();
	if ((flags&JINIT_FLAG_ENABLE3D)!=0)
		JRenderer::Set3DFlag(true);

	engine = JGE::GetInstance();

	std::string path = argv[0];
	int sizeDel = strlen("eboot.pbp");
	path.erase(path.size()-sizeDel, sizeDel);
	engine->SetAppDir(path);

	game = launcher->GetGameApp();
	game->Create();


	engine->SetApp(game);
	engine->Run();
	
	//pspDebugScreenSetXY(10,10);
	//pspDebugScreenPrintf("Hello world!");

	game->Destroy();
	delete game;
	game = NULL;

	engine->SetApp(NULL);

	done = true;
	
//	if (mikModThreadID > 0)
//	{
//		////SceUInt timeout = 100000;
//		////sceKernelWaitThreadEnd(mikModThreadID, &timeout);
//		// not 100% sure if this is necessary after a clean exit, but just to make sure any resources are freed:
//		sceKernelDeleteThread(mikModThreadID);
//	}


// 	JFileSystem::Destroy();
// 	JParticleSystem::Destroy();
// 	JMotionSystem::Destroy();

//	launcher->CleanUp();
	delete launcher;

	JGE::Destroy();
	engine = NULL;


//	sceKernelSleepThread(); 

	sceKernelExitGame();
	
	return 0; 
} 
