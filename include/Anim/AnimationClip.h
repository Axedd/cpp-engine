#pragma once

struct AnimationClip {
    int startFrame = 0;     // first frame index in the row
    int frameCount = 1;     
    float fps = 0.0f;      
    bool loop = true;
};