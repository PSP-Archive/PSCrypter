/*PSCRYPTER v2.0*/

#include <pspkernel.h>
#include <pspdebug.h>
#include <psppower.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <string.h>
#include <pspsdk.h>
#include <psputility.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>

#include <sys/unistd.h>

PSP_MODULE_INFO("PSCRYPTER", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(-1024);

#define MAX_BUFFER_SIZE 1024*1024*5+1024*512
#define MAX_ORIGIN_FILE_SIZE 1024*1024*4

#define printf	pspDebugScreenPrintf

void myexit() {
	printf("Returning to XMB...");
	sceKernelDelayThreadCB(3000000);
	sceKernelExitGame();
}


#include "main_crypter.h"
#include "unpack-pbp.h"
#include "pack-pbp.h"

#include "fix-realocations.h"


int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();
	return 0;
}

int CallbackThread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
	return 0;
}

int SetupCallbacks(void)
{
    int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0)
		sceKernelStartThread(thid, 0, 0);
    return thid;
}

char *filename_list[10] = {
"name","EBOOT_signed.PBP", "param.sfo", "icon0.png", "icon1.pmf", "pic0.png", "pic1.png", "snd0.at3", "data.psp", "data.psar"
};

int main(void)
{
	int err = 0;
	scePowerSetClockFrequency(333, 333, 166);
	pspDebugScreenInit();
	
	printf("####  PSCRYPTER v2.0 by Carlosgs  ####\nEncrypt EBOOT.PBP files directly from the PSP\nUses the code made by 'bbtgp' and parts of the PSPSDK\nv2.0 added realocation fixing by JJS\nTHANK YOU!\n\n");
	
	
	sceIoMkdir("./sign/", 0777);
	
	chdir("./sign/");
	
	sceIoRemove("EBOOT_signed.PBP");
	sceIoRemove("param.sfo");
	sceIoRemove("icon0.png");
	sceIoRemove("icon1.pmf");
	sceIoRemove("pic0.png");
	sceIoRemove("pic1.png");
	sceIoRemove("snd0.at3");
	sceIoRemove("data.psp");
	sceIoRemove("data_unsigned.psp");
	sceIoRemove("data.psar");
	
	printf("Unpacking EBOOT file...\n");
	
	err = main_unpack_pbp();
	if(err != 0)
	{
		printf("Error while unpacking: %d\n",err);
		myexit();
	}
	
	printf("\nFixing PRX realocations...\n");
	
	err = main_fix_realocations();
	if(err != 0)
	{
		printf("Error while fixing realocations: %d\n",err);
		myexit();
	}
	
	printf("\nCrypting PRX file...\n");
	
	err = main_crypter();
	if(err != 0)
	{
		printf("Error while crypting: %d\n",err);
		myexit();
	}
	
	
	printf("\nPacking new EBOOT file...\n");
	
	err = main_pack_pbp(10, filename_list);
	if(err != 0)
	{
		printf("Error while packing: %d\n",err);
		myexit();
	}
	
	
	printf("\n\nFinished! Hope it works!\n");
	
	myexit();
	return 0;
}
