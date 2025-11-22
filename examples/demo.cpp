#include <iostream>

#include "gemini/client.h"

int main() {
    std::setlocale(LC_ALL, "Turkish");
    
    GeminiCPP::Client client("AIzaSyCVd7tFJdtLW0kvK0E0TcIfk2IFRDJ4aBA");

    std::cout << "Gemini 2.5 flash test...\n\n\n\n\n\n";
    auto Response = client.generateContent("What are the technologies of the future?", GeminiCPP::Model::GEMINI_2_5_FLASH);

    std::cout << Response << std::endl;
    
    auto QuickResponse = client.generateContent("Answer quickly!", "gemini-2.5-flash-lite"); 

    std::cout << "\n\n\n\n\n\n" << QuickResponse;
    
    return 0;
}