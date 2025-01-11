#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>

namespace Logger {
   inline void log(const std::string& message) {
        
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm buf;
        localtime_s(&buf, &in_time_t);

        
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
        std::cout << "[" << std::put_time(&buf, "%H:%M:%S") << "] - ";

        
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << message << std::endl;

        
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}
