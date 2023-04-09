/**
* SPDX-License-Identifier: 0BSD
*/

#ifndef AUDIO_VOLUME_HPP_INCLUDED
#define AUDIO_VOLUME_HPP_INCLUDED

#include <endpointvolume.h>
#include <exception>

/** Defines the audio device controlled with the class audio_volume */
enum class audio_device_type {
    speaker,
    microphone
};

/** Controls the default audio device on windows. Allows the user to 
 * mute and unmute the device and to get and set the volume level.
 */
class audio_volume {
public:
    /** Opens the specified device. 
     * The windows COM API is initialized as COINIT_APARTMENTTHREADED.
     * Throws a runtime_error if anything goes wrong.
     **/
    explicit audio_volume(audio_device_type t);

    ~audio_volume();

    /** Checks the type of the device. */
    audio_device_type device_type() const noexcept {
        return type_;
    }

    /** Gets the mute status. Returns true if muted. */
    bool is_muted() const;
    
    /** Gets the volume level of the device.
     * Valid range: [0.0, 1.0]
     */
    float volume() const;

    /** Mutes the device if called with true as argument, otherwise
     * the device is unmuted.
     */
    void mute(bool v);

    /** Sets the volume level of the device.
     * Valid range: [0.0, 1.0]
     */
    void set_volume(float v);

private:
    IAudioEndpointVolume* audio_endpoint_{nullptr};
    audio_device_type type_;

    void check_and_report_init_error(HRESULT res, void* ptr, const char* msg);
};

#endif /* AUDIO_VOLUME_HPP_INCLUDED */
