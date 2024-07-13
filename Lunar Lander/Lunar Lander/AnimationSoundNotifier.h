/**
* Author: [Hexun Zhang]
* Assignment: Lunar Lander
* Date due: 2024-07-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#pragma once
#include <SDL_mixer.h>
#include <SDL.h>

void FadeIn(Mix_Chunk* sound_wav, int& active_channel, int play_times, int duration) {
    // Fade in (from 0 to full volume) over 1 second
    active_channel = Mix_FadeInChannel(
        -1,              // using the first channel that is not currently in use...
        sound_wav,  // ...fade in this chunk of audio from volume 0 to max volume...
        play_times,       // ...once...
        duration      // ...over 1000 milliseconds.
    );
}

void FadeOut(int active_channel, int duration) {
    // Fade out (from full volume to 0) over 1 second
    Mix_FadeOutChannel(
        active_channel,  // using the specific channel where the sound is playing...
        duration      // ...over 1000 milliseconds.
    );
}