// Stub OpenAL context header for basic compilation
#pragma once

#ifndef AL_ALC_H
#define AL_ALC_H

#include "al.h"

typedef struct ALCdevice ALCdevice;
typedef struct ALCcontext ALCcontext;

// Basic constants
#define ALC_FALSE 0
#define ALC_TRUE 1

// Stub functions
extern "C" {
ALCdevice *alcOpenDevice(const ALchar *devicename);
ALboolean alcCloseDevice(ALCdevice *device);
ALCcontext *alcCreateContext(ALCdevice *device, const ALint *attrlist);
void alcDestroyContext(ALCcontext *context);
ALboolean alcMakeContextCurrent(ALCcontext *context);
}

#endif