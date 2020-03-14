#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    std::fstream tutturu("server.conf");
    
    std::cout << "\033[33m==== KING SLAYER GAME LAUNCHER " <<
        "\033[36mv1.5 (stable)\033[33m ====\033[0m" << std::endl;
    std::cout << "Parsing 'server.conf'... ";
    
    int nports, from, nclients;
    std::string trash, value, hitler;
    
    std::getline(tutturu, trash, '=');
    std::getline(tutturu, value);
    hitler = value;
    
    std::getline(tutturu, trash, '=');
    std::getline(tutturu, value);
    nports = std::stoi(value);

    std::getline(tutturu, trash, '=');
    std::getline(tutturu, value);
    from = std::stoi(value);
    
    std::getline(tutturu, trash, '=');
    std::getline(tutturu, value);
    nclients = std::stoi(value);
    
    std::cout << "successful!" << std::endl;
    
    std::cout << "Enabling your audio device (may require a password)..."
        << std::endl;
    system("sudo modprobe pcspkr");
    std::cout << "Audio device enabled" << std::endl;
    
    std::cout << "Launching \033[35m" << nports
        << "\033[0m servers starting from port \033[35m" << from
        << "\033[0m." << std::endl;
    
    std::string cmd;
    
    std::cout << "Launching servers..." << std::endl;
    for (int i = 0; i < nports; ++i) {
        cmd += "x-terminal-emulator --geometry=80x10 -x  ./server ";
        if (i == nports - 1)
            cmd += "1 ";
        else
            cmd += "0 ";
        cmd += std::to_string(nports - i - 1) + " " +
            std::to_string(from + nports - i - 1) + " " +
            std::to_string(nports);
        if (i + 1 != nports)
            cmd += "\n";
    }
    system(cmd.c_str());
    std::cout << "Launching \033[35m" << nclients << "\033[0m clients..."
        << std::endl;
    for (int i = 0; i < nclients; ++i) {
        cmd = "x-terminal-emulator -x ./client " + std::to_string(from) + " "
            + std::to_string(nports) + " " + std::to_string(i);
        system(cmd.c_str());
    }
    std::cout << "\033[32mDone.\033[0m" << std::endl;
    return 0;
}
