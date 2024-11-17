#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
const std::string EXPECTED_ID = "arduino_id";

void showNotification(const std::string &message) {
    std::string command = "notify-send 'Arduino Notification' '" + message + "'" + " 2> /dev/null";
    system(command.c_str());
}

std::string executeCommand(const std::string &command) {
    std::string result;
    char buffer[128];
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("Error! popen() failed!");
    }
    try {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

int main() {
    std::string myTTY = executeCommand("ls /dev/tty* | grep USB");
    std::string COM_PORT = myTTY;
    COM_PORT.erase(std::remove(COM_PORT.begin(), COM_PORT.end(), '\n'), COM_PORT.cend());

    std::cout << "Listening on: " << COM_PORT << std::endl;

    std::ifstream serialInput(COM_PORT);
    std::ofstream serialOutput(COM_PORT);

    if (!serialInput.is_open() || !serialOutput.is_open()) {
        std::cerr << "Error opening " << COM_PORT << std::endl;
        return 1;
    }

    std::cout << "Waiting..." << std::endl;
    std::string receivedID;

    while (true) {
        if (std::getline(serialInput, receivedID)) {
            if (!receivedID.empty()) {
                receivedID.erase(receivedID.find_last_not_of("\r\n") + 1);
                std::cout << "Recieved:" << "\"" << receivedID << "\"" << std::endl;
                if (receivedID == EXPECTED_ID) {
                    std::cout << "ID Approved!" << std::endl;
                    showNotification("Unique ID Approved");
                    serialOutput << "STOP\n" << std::flush;
                }
                if (receivedID == "STOPPING")
                    break;
            }
        }
    }
    std::cout << "END OF LINE" << std::endl;
    return 0;
}
