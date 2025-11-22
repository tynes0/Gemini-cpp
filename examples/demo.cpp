#include <iostream>
#include "gemini/client.h"
#include "gemini/logger.h"

int main() {
    GeminiCPP::Log::init();
    GeminiCPP::Client client("AIzaSyCVd7tFJdtLW0kvK0E0TcIfk2IFRDJ4aBA");

    std::cout << "--- Fluent Builder Demo ---\n";

    auto response = client.request()
          .model(GeminiCPP::Model::GEMINI_2_5_FLASH)
          .systemInstruction("Sen profesyonel bir sanat elestirmenisin. Asiri detayli ve teknik konus.")
          .text("Bu resimdeki kompozisyonu elestir.")
          .image(R"(C:\Users\cihan\Desktop\portrait.jpg)") 
          .generate();

    std::cout << "Elestirmen:\n" << response << std::endl;

    return 0;
}