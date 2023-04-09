#include "cheat/gui.h"
#include "imgui/imgui.h"
#include "audio_volume.hpp"
#include "keyboard_listener.hpp"
#include "windows.h"
#include <stdexcept>

#include <thread>

void render_speaker(audio_volume &speaker)
{
	const int volume_speaker = static_cast<int>(speaker.volume() * 100);
	const bool speaker_muted = speaker.is_muted();
	static int volume_speaker_ui = volume_speaker;
	static bool speaker_muted_ui = speaker_muted;

	ImGui::Text("Speaker");
	ImGui::SliderInt("Volume", &volume_speaker_ui, 0, 100);
	ImGui::Checkbox("Mute", &speaker_muted_ui);

	if (volume_speaker != volume_speaker_ui)
	{
		speaker.set_volume(volume_speaker_ui * 0.01F);
	}
	if (speaker_muted != speaker_muted_ui)
	{
		speaker.mute(speaker_muted_ui);
	}
}

void render_microphone(audio_volume &microphone, const bool ptt)
{
	const int volume_microphone = static_cast<int>(microphone.volume() * 100);
	bool microphone_muted = microphone.is_muted();
	static int volume_microphone_ui = volume_microphone;
	static bool microphone_muted_ui = microphone_muted;

	ImGui::Text("Microphone");
	ImGui::SliderInt("Volume##Mic", &volume_microphone_ui, 0, 100);
	ImGui::BeginDisabled(ptt);
	ImGui::Checkbox("Mute##Mic", ptt ? &microphone_muted : &microphone_muted_ui);
	ImGui::EndDisabled();

	if (volume_microphone != volume_microphone_ui)
	{
		microphone.set_volume(volume_microphone_ui * 0.01F);
	}
	if (!ptt && microphone_muted != microphone_muted_ui)
	{
		microphone.mute(microphone_muted_ui);
	}
}


int __stdcall wWinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	PWSTR arguments,
	int commandShow)
{
	gui::CreateHWindow("Volume Utils");
	gui::CreateDevice();
	gui::CreateImGui();

	keyboard_listener listener;
	audio_volume speaker(audio_device_type::speaker);
	audio_volume microphone(audio_device_type::microphone);

	bool push_to_talk = false;
	bool mute_state_before = false;
	bool mic_muted = false;
	using namespace std::chrono;
	auto last_mic_change = steady_clock::now();
	auto start = steady_clock::now();
	size_t frame_count = 0;

	while (gui::isRunning)
	{
		++frame_count;
		gui::BeginRender();

		ImGui::SetNextWindowPos({0, 0});
		ImGui::SetNextWindowSize({gui::WIDTH, gui::HEIGHT});
		ImGui::Begin(
			"Volume Utils",
			&gui::isRunning,
			ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoMove);

		render_speaker(speaker);
		render_microphone(microphone, push_to_talk);

		if (push_to_talk)
		{
			ImGui::Text("Hold Left CTRL to unmute your microphone");
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
		}
		auto now = steady_clock::now();
		if (ImGui::Button(push_to_talk ? "Disable Push-To-Talk" : "Enable Push-To-Talk"))
		{
			push_to_talk = !push_to_talk;
			if(push_to_talk && !listener.active()) {
				mute_state_before = microphone.is_muted();
				listener.listen();
				mic_muted = true;
				microphone.mute(mic_muted);
			}
			else if(listener.active()) {
				listener.stop();
				mic_muted = false;
				microphone.mute(mute_state_before);
			}
		}
		if (push_to_talk && mic_muted == listener.ctrl_pressed() && now - last_mic_change > seconds(1)) {
			steady_clock::time_point last_mic_change = steady_clock::now();
			mic_muted = !mic_muted;
			microphone.mute(mic_muted);
		}

		ImGui::Text("Keypress count: %d", listener.event_count());
		ImGui::Text("CTRL: %d", listener.ctrl_pressed());

		double fps = frame_count/duration_cast<duration<double>>(now - start).count();
		ImGui::Text("FPS: %f", fps);
		ImGui::Text("Built with ImGui (MIT License) and\ncazzwastaken/borderless-imgui-window (MIT License)");

		ImGui::End();
		gui::EndRender();
	}

	if(push_to_talk) {
		microphone.mute(mute_state_before);
	}

	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	return EXIT_SUCCESS;
}
