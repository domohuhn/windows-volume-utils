#include "audio_volume.hpp"
#include <stdio.h>

int main() {
    try {
        audio_volume speaker(audio_device_type::speaker);
        printf("Speaker current volume: %d %%\n", static_cast<int>(100 * speaker.volume()));
        auto is_muted = speaker.is_muted();
        printf("Speaker is muted: %d\n", is_muted);
        printf("Toggling mute now!\n");
        speaker.mute(!is_muted);
        printf("Speaker is muted: %d\n", speaker.is_muted());
    }
    catch (const std::exception& e) {
        printf("Error: %s\n", e.what());
        return -1;
    }
    return 0;
}
