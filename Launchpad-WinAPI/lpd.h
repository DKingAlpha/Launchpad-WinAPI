#include <windows.h>  


// data from https://github.com/vinyldarkscratch/lightpad
const int note_button_map[] = {
    91, 92, 93, 94, 95, 96, 97, 98, 11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34, 41, 42, 43, 44,
    51, 52, 53, 54, 61, 62, 63, 64, 71, 72, 73, 74, 81, 82, 83, 84, 15, 16, 17, 18, 25, 26, 27, 28,
    35, 36, 37, 38, 45, 46, 47, 48, 55, 56, 57, 58, 65, 66, 67, 68, 75, 76, 77, 78, 85, 86, 87, 88,
    89, 79, 69, 59, 49, 39, 29, 19, 80, 70, 60, 50, 40, 30, 20, 10, 1, 2, 3, 4, 5, 6, 7, 8
};
const int note_button_offset = 28;
const int note_button_size = sizeof(note_button_map) / sizeof(int);




class Launchpad
{
    public:
        int channel = 0 ;    // 0-7

        Launchpad(int p_in, int p_out);
        Launchpad();
        ~Launchpad();

        static void EnumMidiDevices();

        // basic methods

        MMRESULT SetColor(int row, int col, int color);
        MMRESULT FlashColor(int row, int col, int newcolor);
        MMRESULT PulseColor(int row, int col, int color);
        MMRESULT ShowText(LPSTR text, int color, bool loop);
        MMRESULT SetColumnColor(int col, int color);
        MMRESULT SetRowColor(int row, int color);
        MMRESULT StopShowText();
        MMRESULT SetPadColor(int color);
        MMRESULT ClearPad();
        void PlayMidiFile(const char *file);


        // expert methods

        MMRESULT SendSysExCmdBytes(LPBYTE cmd, int cmdlen);

        MMRESULT SendCmdBytes(LPBYTE cmd, int cmdlen);

        MMRESULT SendDWORD(DWORD cmd);
        



    private:
        HMIDIIN hin; 
        HMIDIOUT hout;
        int port_in = 1;
        int port_out = 2;
       
        static int note_to_button(int note);
        static int button_to_note(int button);

        static void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance,
            DWORD_PTR dwParam1, DWORD_PTR dwParam2);
        static void CALLBACK MidiOutProc(HMIDIIN hMidiOut, UINT wMsg, DWORD_PTR dwInstance,
            DWORD_PTR dwParam1, DWORD_PTR dwParam2);
};

