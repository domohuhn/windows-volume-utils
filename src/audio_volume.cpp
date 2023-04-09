/*
* SPDX-License-Identifier: 0BSD
*/

#include "audio_volume.hpp"

#include <windows.h>
#include <mmdeviceapi.h>
#include <stdexcept>


bool audio_volume::is_muted() const
{
    int current = 0;
    auto result = audio_endpoint_->GetMute(&current);
    if(result != S_OK) {
        throw std::runtime_error("Failed to get mute status");
    }
    return current!=0;
}
    

float audio_volume::volume() const
{
    float current = 0.0f;
    auto result = audio_endpoint_->GetMasterVolumeLevelScalar(&current);
    if(result != S_OK) {
        throw std::runtime_error("Failed to read volume level");
    }
    return current;
}


void audio_volume::mute(bool v)
{
    if (is_muted() == v) {
        return;
    }
    auto result = audio_endpoint_->SetMute(v,nullptr);
    if(result != S_OK) {
        throw std::runtime_error("Failed to set mute status");
    }
}

void audio_volume::set_volume(float v)
{
    if ( 0.0f<=v && v<=1.0f) {
        auto result = audio_endpoint_->SetMasterVolumeLevelScalar(v,nullptr);
        if(result != S_OK) {
            throw std::runtime_error("Failed to set the volume level");
        }
    }
    else {
        throw std::domain_error("Volume must be in range [0, 1]");
    }
}

static EDataFlow get_windows_device_id(audio_device_type t) {
    return t==audio_device_type::speaker ? eRender : eCapture;
}

audio_volume::audio_volume(audio_device_type t) : type_{t}
{
    const auto coinit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (coinit != S_OK && coinit != S_FALSE) {
        throw std::runtime_error("Failed to initialize the COM library");
    }
    
    IMMDeviceEnumerator *device_enumerator = nullptr;
    const auto ret_enum = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&device_enumerator);
    check_and_report_init_error(ret_enum, device_enumerator, "Failed to create the device enumerator");

    IMMDevice *default_device = nullptr;
    const auto id = get_windows_device_id(type_);
    const auto ret_dev = device_enumerator->GetDefaultAudioEndpoint(id, eConsole, &default_device);
    device_enumerator->Release();
    check_and_report_init_error(ret_dev, default_device, "Failed to get the default device");

    auto ret_endpoint = default_device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID *)&audio_endpoint_);
    check_and_report_init_error(ret_endpoint, audio_endpoint_, "Failed to activate the endpoint");
    default_device->Release();
}

audio_volume::~audio_volume() {
    audio_endpoint_->Release();
    CoUninitialize();
}

void audio_volume::check_and_report_init_error(HRESULT res, void* ptr, const char* msg)
{
    if (res == S_OK && ptr!=nullptr) {
        return;
    }
    CoUninitialize();
    throw std::runtime_error(msg);
}
