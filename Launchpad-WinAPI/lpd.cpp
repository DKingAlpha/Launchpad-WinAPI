#include <windows.h>  
#include <stdio.h>  
#include "./Library/midifile/MidiFile.h"
#include "lpd.h"



Launchpad::Launchpad(int p_in, int p_out)
{
    port_in = p_in; port_out = p_out;
    if (midiInOpen(&hin, port_in, (DWORD)(void*)MidiInProc, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
    {
        printf("Failed to open Launchpad pro input\n");
        return;
    }

    if (midiOutOpen(&hout, port_out, (DWORD)(void*)MidiOutProc, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
    {
        printf("Failed to open Launchpad pro output\n");
        return;
    }

    midiInStart(hin);

}

Launchpad::Launchpad()
{
    UINT nMidiDeviceNum;
    MIDIINCAPS capsin;
    MIDIOUTCAPS capsout;
    int p_in = 1, p_out = 2;
    bool lpdpro = false;

    nMidiDeviceNum = midiInGetNumDevs();
    if (nMidiDeviceNum == 0)
    {
        printf("Launchpad Not Plugged in");
        return;
    }
    for (int i = 0; i < nMidiDeviceNum; ++i)
    {
        midiInGetDevCaps(i, &capsin, sizeof(MIDIINCAPS));
        if (wcscmp(capsin.szPname, L"Launchpad Pro") >= 0) lpdpro = true;   // live mode
        if (lpdpro && (wcscmp(capsin.szPname, L"MIDIIN") >= 0))
        {
            p_in = i;          // programmer layout
            break;
        }
    }

    nMidiDeviceNum = midiOutGetNumDevs();
    if (nMidiDeviceNum == 0) {
        printf("Launchpad Not Plugged in");
        return;
    }
    for (int i = 0; i < nMidiDeviceNum; ++i) {
        if (wcscmp(capsout.szPname, L"Launchpad Pro") >= 0) lpdpro = true;  // live mode
        if (lpdpro && (wcscmp(capsin.szPname, L"MIDIOUT") >= 0))
        {
            p_out = i;          // programmer layout
            break;
        }
    }

    port_in = p_in; port_out = p_out;
    if (midiInOpen(&hin, port_in, (DWORD)(void*)MidiInProc, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
    {
        printf("Failed to open Launchpad pro input\n");
        return;
    }

    if (midiOutOpen(&hout, port_out, (DWORD)(void*)MidiOutProc, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
    {
        printf("Failed to open Launchpad pro output\n");
        return;
    }

    midiInStart(hin);


    BYTE mode[2] = { 0x21,1 };      //standalone mode
    SendSysExCmdBytes(mode, 2);
    mode[0] = 0x2c; mode[1] = 3;    //programmer layout
    SendSysExCmdBytes(mode, 2);

}

Launchpad::~Launchpad()
{
    midiInStop(hin);
    midiInClose(hin);
    midiOutReset(hout);
    midiOutClose(hout);
}


void Launchpad::EnumMidiDevices()
{
    UINT nMidiDeviceNum;
    MIDIINCAPS capsin;
    MIDIOUTCAPS capsout;

    printf("=== MidiDevices\n");

    printf(" === Input\n");

    nMidiDeviceNum = midiInGetNumDevs();
    if (nMidiDeviceNum == 0) {
        printf("No Midi Input Device Connected");
        return;
    }

    for (unsigned int i = 0; i < nMidiDeviceNum; ++i) {
        midiInGetDevCaps(i, &capsin, sizeof(MIDIINCAPS));
        wprintf(L"\t%d : Device Name = %s\n", i, capsin.szPname);
    }
    printf("\n === Output\n");

    nMidiDeviceNum = midiOutGetNumDevs();
    if (nMidiDeviceNum == 0) {
        printf("No Midi Output Device Connected");
        return;
    }

    for (unsigned int i = 0; i < nMidiDeviceNum; ++i) {
        midiOutGetDevCaps(i, &capsout, sizeof(MIDIINCAPS));
        wprintf(L"\t%d : Port Name : %s\n", i, capsout.szPname);
    }
    printf("\n=====\n");

}

// basic methods

MMRESULT Launchpad::SetColor(int row, int col, int color)
{
    BYTE key = 0; BYTE keytype = 0; bool keyflag = false;
    if (row >= 1 && row <= 8)
    {
        if (col >= 0 && col <= 9)
        {
            key = 10 * row + col;
            keytype = 0x90 + channel;
            keyflag = true;
        }
    }
    if (row == 0 || row == 9)
    {
        if (col >= 1 && col <= 8)
        {
            key = 10 * row + col;
            keytype = 0xB0 + channel;
            keyflag = true;
        }
    }

    if (keyflag && color>0 && color < 127)
    {
        DWORD msg = keytype + (key << 8) + (color << 16);
        return midiOutShortMsg(hout, msg);
    }
    else
    {
        return -1;
    }

}

MMRESULT Launchpad::FlashColor(int row, int col, int newcolor)
{
    BYTE key = 0; bool keyflag = false;
    if (row >= 1 && row <= 8)
    {
        if (col >= 0 && col <= 9)
        {
            key = 10 * row + col;
            keyflag = true;
        }
    }
    if (row == 0 || row == 9)
    {
        if (col >= 1 && col <= 8)
        {
            key = 10 * row + col;
            keyflag = true;
        }
    }

    if (keyflag && newcolor>0 && newcolor < 127)
    {
        BYTE cmd[3] = { 0x23, key, newcolor };
        return SendSysExCmdBytes(cmd, 3);
    }
    else
    {
        return -1;
    }
}

MMRESULT Launchpad::PulseColor(int row, int col, int color)
{
    BYTE key = 0; bool keyflag = false;
    if (row >= 1 && row <= 8)
    {
        if (col >= 0 && col <= 9)
        {
            key = 10 * row + col;
            keyflag = true;
        }
    }
    if (row == 0 || row == 9)
    {
        if (col >= 1 && col <= 8)
        {
            key = 10 * row + col;
            keyflag = true;
        }
    }

    if (keyflag && color>0 && color < 127)
    {
        BYTE cmd[3] = { 0x28, key, color };
        return SendSysExCmdBytes(cmd, 3);
    }
    else
    {
        return -1;
    }
}

MMRESULT Launchpad::ShowText(LPSTR text, int color, bool loop)
{
    if (color < 0 || color > 127) return -1;
    BYTE cmd[128] = { 20,color,loop ? 1 : 0 };
    memcpy(cmd + 3, text, strlen(text));
    return SendSysExCmdBytes(cmd, 3 + strlen(text));
}

MMRESULT Launchpad::SetColumnColor(int col, int color)
{
    MMRESULT res = MMSYSERR_NOERROR;
    if (col >= 0 && col <= 9 && color >= 0 && color <= 127)
    {
        for (int i = 0; i <= 9; i++)
        {
            if (res = SetColor(i, col, color) != MMSYSERR_NOERROR) return res;
        }
        return MMSYSERR_NOERROR;
    }
    else
    {
        return -1;
    }
}

MMRESULT Launchpad::SetRowColor(int row, int color)
{
    MMRESULT res = MMSYSERR_NOERROR;
    if (row >= 0 && row <= 9 && color >= 0 && color <= 127)
    {
        for (int i = 0; i <= 9; i++)
        {
            if (res = SetColor(row, i, color) != MMSYSERR_NOERROR) return res;
        }
        return MMSYSERR_NOERROR;
    }
    else
    {
        return -1;
    }
}

MMRESULT Launchpad::StopShowText()
{
    BYTE cmd[1] = { 20 };
    return SendSysExCmdBytes(cmd, 1);
}

MMRESULT Launchpad::SetPadColor(int color)
{
    if (color < 0 || color>127)return -1;
    BYTE cmd[2] = { 14,color };
    return SendSysExCmdBytes(cmd, 2);
}

MMRESULT Launchpad::ClearPad()
{

    BYTE cmd[2] = { 14, 0 };
    return StopShowText() || SendSysExCmdBytes(cmd, 2);
}

void Launchpad::PlayMidiFile(const char *file) {
    MidiFile midifile;
    MidiEvent *mev;
    int keytype, key, note, color, deltatick;
    midifile.read(file);
    midifile.joinTracks();
    int BPM = midifile.getTicksPerQuarterNote();
    for (int event = 0; event < midifile[0].size(); event++)
    {
        mev = &midifile[0][event];
        keytype = (int)(*mev)[0];
        note = (int)(*mev)[1];
        key = note_to_button(note);
        color = (int)(*mev)[2];
        if (event == 0) deltatick = mev->tick;
        else deltatick = mev->tick - midifile[0][event - 1].tick;
        if (keytype == 0x80 || keytype == 0x90)
        {
            if (keytype == 0x80) color = 0;
            if (key % 10 > 0 && key % 10 < 9 && key / 10 > 0 && key / 10 < 9)
                keytype = 0x90;
            else
                keytype = 0xB0;
            if (deltatick > 0) Sleep(60000 / (BPM * deltatick));
            keytype += channel;
            DWORD msg = keytype + (key << 8) + (color << 16);
            midiOutShortMsg(hout, msg);
        }


    }
}


// expert methods

MMRESULT Launchpad::SendSysExCmdBytes(LPBYTE cmd, int cmdlen)
{
    int length = 6;
    char msgdata[128] = { 0xF0, 0x00, 0x20, 0x29, 0x02, 0x10 };
    memcpy(msgdata + length, cmd, cmdlen);
    length += cmdlen;
    msgdata[length] = 247;
    length++;
    LPMIDIHDR msg = new MIDIHDR;
    msg->dwFlags = 0; msg->dwBufferLength = length; msg->lpData = msgdata;
    midiOutPrepareHeader(hout, msg, sizeof(MIDIHDR));
    return midiOutLongMsg(hout, msg, sizeof(MIDIHDR));
}

MMRESULT Launchpad::SendCmdBytes(LPBYTE cmd, int cmdlen)
{
    LPMIDIHDR msg = new MIDIHDR;
    msg->dwFlags = 0; msg->dwBufferLength = cmdlen; msg->lpData = (LPSTR)cmd;
    midiOutPrepareHeader(hout, msg, sizeof(MIDIHDR));
    return midiOutLongMsg(hout, msg, sizeof(MIDIHDR));
}

MMRESULT Launchpad::SendDWORD(DWORD cmd)
{
    return midiOutShortMsg(hout, cmd);
}


int Launchpad::note_to_button(int note) {
    if (note < note_button_offset || note >(note_button_size + note_button_offset)) return 0;
    return note_button_map[note - note_button_offset];
}
int Launchpad::button_to_note(int button) {
    for (int i = 0; i < note_button_size + note_button_offset; i++)
        if (note_button_map[i] == button) return i + note_button_offset;
    return 0;
}

void CALLBACK Launchpad::MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance,
    DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    int lpdkey, lpdkeyvel, lpdkeytype;
    switch (wMsg) {
    case MIM_OPEN:
        printf("Midi input device Opened\n");
        break;
    case MIM_CLOSE:
        printf("Midi input device Closed\n");
        break;
    case MIM_DATA:
        lpdkeytype = (dwParam1 & 0x000000FF);
        lpdkey = ((dwParam1 & 0x0000FF00) >> 8);
        lpdkeyvel = ((dwParam1 & 0x00FF0000) >> 16);
        printf("Midi input device Data:0x%08X  Key:%d , Keytype:%s , Vel:%d\n", dwParam1, lpdkey, lpdkeytype == 0x90 ? "Key" : "Button", lpdkeyvel);
        break;
    default:
        break;
    }
    return;
}

void CALLBACK Launchpad::MidiOutProc(HMIDIIN hMidiOut, UINT wMsg, DWORD_PTR dwInstance,
    DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    switch (wMsg) {
    case MOM_OPEN:
        printf("Midi output device Opened\n");
        break;
    case MOM_CLOSE:
        printf("Midi output device Closed\n");
        break;
    case MOM_DONE:
        //printf("Midi output device Done\n");
        break;
    default:
        break;
    }
    return;
}
