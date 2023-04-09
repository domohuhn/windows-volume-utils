/*
* SPDX-License-Identifier: 0BSD
*/

#ifndef KEYBOARD_LISTENER_HPP_INCLUDED
#define KEYBOARD_LISTENER_HPP_INCLUDED

#include <functional>
#include <windows.h>


/** Installs a keyboard event listener hook in windows.
 */
class keyboard_listener {
public:
    /** Loads the hook library and gets the function addresses.
     * Terminates the program on error.
     */
    keyboard_listener();

    /** Stops listening and unloads the hook lirbary. */
    ~keyboard_listener();

    /** Installs the hook in the windows API an begins to listen to events. */
    void listen();

    /** Stops listenting to events */
    void stop();

    /** Number of events recorded */
    int event_count() const;

    /** If we are currently listenning */
    bool active() const noexcept {
        return is_listening_;
    }

    /** Checks if the control key is pressed */
    bool ctrl_pressed() const;

private:
    using InstallFunc = HHOOK (*)();
    using IntFunc = int (*)();
    using BoolFunc = bool (*)();
    HINSTANCE library_ = nullptr;

	InstallFunc install_ = nullptr;
	BoolFunc uninstall_ = nullptr;
	IntFunc keypress_count_ = nullptr;
    BoolFunc ctrl_pressed_ = nullptr;
    bool is_listening_ = false;
};

#endif /* KEYBOARD_LISTENER_HPP_INCLUDED */
