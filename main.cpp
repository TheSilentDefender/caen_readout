#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "ClassDigitizer2Gen.h"

Digitizer2Gen *digi = new Digitizer2Gen();
std::atomic<bool> stopRequested(false);

void enableNonBlockingInput()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK); 
}

void resetTerminalSettings()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK); 
}

void checkForExit()
{
    while (!stopRequested)
    {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0) 
        {
            if (ch == 'q' || ch == 'Q')
            {
                stopRequested = true;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <url> [-s]" << std::endl;
        std::cerr << "  -s: Save settings to file" << std::endl;
        return 1;
    }

    const char *url = argv[1];

    digi->OpenDigitizer(url);
    digi->SetDataFormat(DataFormat::ALL);

    for (int i = 2; i < argc; ++i)
    {
        if (std::string(argv[i]) == "-s")
        {
            digi->SaveSettingsToFile();
            delete digi;
            return 0;
        }
    }

    digi->LoadSettingsFromFile();
    digi->OpenOutFile("test", "wb");
    std::cout << "Starting acquisition... Press 'q' to stop." << std::endl;
    digi->StartACQ();

    enableNonBlockingInput();

    std::thread exitThread(checkForExit);

    while (!stopRequested)
    {
        digi->ReadData();
        digi->SaveDataToFile();
    }

    exitThread.join(); 

    resetTerminalSettings();

    std::cout << "Stopping acquisition..." << std::endl;
    digi->StopACQ();
    digi->CloseDigitizer();

    delete digi;
    return 0;
}
