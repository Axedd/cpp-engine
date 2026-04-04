#pragma once
#include "anim/AnimationClip.h"

struct Animator {
    const AnimationClip* clip = nullptr;

    float time = 0.0f;      // seconds since clip start
    int frame = 0;          // local frame index [0..frameCount-1]
    bool finished = false;

    void play(const AnimationClip* newClip, bool restart = true) {
        clip = newClip;
        if (restart) {
            time = 0.0f;
            frame = 0;
            finished = false;
        }
    }

    void update(float dt) {
        if (!clip) return;
        if (clip->frameCount <= 1) return;
        if (clip->fps <= 0.0f) return;
        if (finished) return;

        time += dt;

        // "how many frames have passed"
        int f = (int)(time * clip->fps);

        if (clip->loop) {
            frame = f % clip->frameCount;
        }
        else {
            if (f >= clip->frameCount) {
                frame = clip->frameCount - 1;
                finished = true;
            }
            else {
                frame = f;
            }
        }
    }

    int absoluteFrame() const {
        if (!clip) return 0;
        return clip->startFrame + frame;
    }
};
