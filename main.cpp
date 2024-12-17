#include <iostream>
#include <string>
#include "ClassDigitizer2Gen.h"

Digitizer2Gen * digi = new Digitizer2Gen();


int main(int argc, char *argv[]) {
    // Check if a URL argument is provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <url> [-s]" << std::endl;
        std::cerr << "  -s: Save settings to file" << std::endl;
        return 1;
    }

    // Get the URL from the first argument
    const char *url = argv[1];

    // Initialize the digitizer with the URL
    digi->OpenDigitizer(url);
    digi->SetDataFormat(DataFormat::ALL);

    // Process additional command-line options
    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "-s") {
            digi->SaveSettingsToFile();
            delete digi;
            return 0;
        }
    }

    digi->LoadSettingsFromFile();
    digi->OpenOutFile("test", "wb");
    std::cout << "Starting acquisition..." << std::endl;
    digi->StartACQ();
    
    digi->ReadData();
    digi->SaveDataToFile();

    std::cout << "Stopping acquisition..." << std::endl;
    digi->StopACQ();
    digi->CloseDigitizer();

    delete digi;
    return 0;
}
