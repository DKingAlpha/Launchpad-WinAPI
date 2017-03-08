#include <windows.h>  
#include <stdio.h>
#include "lpd.h"

#pragma comment(lib, "winmm.lib")

// Channel: 0-7
// Color:   0-127
// DevID:   EnumMidiDevices and choose INPUT/OUPUT
// text length < 120

// THIS PROGRAM ONLY WORKS ON x86


int main(void)
{



    Launchpad lpd;
    
    char midif[20] = "";
    char midifilename[40] = "";

    while (true)
    {
        Sleep(2000);
        lpd.ClearPad();
        scanf("%s", midif);
        sprintf(midifilename, "%s.mid", midif);
        lpd.PlayMidiFile(midifilename);
        
    }

    return 0;
}
