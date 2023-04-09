#include "audio_volume.hpp"
#include <iostream>
#include <sstream>

void print_usage() {
    std::cout<<"Usage: test_set_volume <floating number>\n";
    std::cout<<"       A small test program that sets the volume of the default windows speakers.\n";
}

int main(int argc, char** argv) {
    if (argc != 2)
    {
        print_usage();
        return -1;
    }

    try {
        audio_volume speaker(audio_device_type::speaker);
        std::cout<<"Speaker current volume: " << static_cast<int>(100 * speaker.volume()) << "%\n";
        float new_volume = 0.0f;
        std::string input(argv[1]);
        std::istringstream stream(input);
        if(stream>>new_volume) {
            std::cout<<"Setting volume to: " << static_cast<int>(100 * new_volume) << "%\n";
            speaker.set_volume(new_volume);
        } else {
            std::cout<<"Failed to convert '"<<input<<"' to a floating point number!\n";
            print_usage();
            return -1;
        }
    }
    catch (const std::exception& e) {
        std::cout<<"Error: " << e.what() << "\n";
        return -1;
    }
    return 0;
}
