// Stub OpenAL header for basic compilation
#pragma once

#ifndef AL_AL_H
#define AL_AL_H

typedef char ALboolean;
typedef char ALchar;
typedef char ALbyte;
typedef unsigned char ALubyte;
typedef short ALshort;
typedef unsigned short ALushort;
typedef int ALint;
typedef unsigned int ALuint;
typedef int ALsizei;
typedef int ALenum;
typedef float ALfloat;
typedef double ALdouble;
typedef void ALvoid;

// Basic constants
#define AL_FALSE 0
#define AL_TRUE 1
#define AL_SOURCE_RELATIVE 0x202
#define AL_BUFFER 0x1009
#define AL_LOOPING 0x1007
#define AL_GAIN 0x100A
#define AL_POSITION 0x1004
#define AL_VELOCITY 0x1006
#define AL_ORIENTATION 0x100F
#define AL_NO_ERROR 0
#define AL_FORMAT_MONO16 0x1101
#define AL_FORMAT_STEREO16 0x1103

// Stub functions
extern "C" {
void alGenBuffers(ALsizei n, ALuint *buffers);
void alDeleteBuffers(ALsizei n, const ALuint *buffers);
void alGenSources(ALsizei n, ALuint *sources);
void alDeleteSources(ALsizei n, const ALuint *sources);
void alBufferData(ALuint buffer, ALenum format, const ALvoid *data,
                  ALsizei size, ALsizei freq);
void alSourcei(ALuint source, ALenum param, ALint value);
void alSourcef(ALuint source, ALenum param, ALfloat value);
void alSource3f(ALuint source, ALenum param, ALfloat value1, ALfloat value2,
                ALfloat value3);
void alSourcePlay(ALuint source);
void alSourceStop(ALuint source);
void alListener3f(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
void alListenerfv(ALenum param, const ALfloat *values);
ALenum alGetError(void);
}

#endif