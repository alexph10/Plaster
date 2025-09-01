#include "AudioSystem.h"
#include <spdlog/spdlog.h>
#include <sndfile.h>
#include <vector>

AudioSystem::AudioSystem() 
    : m_device(nullptr)
    , m_context(nullptr) {
}

AudioSystem::~AudioSystem() {
    Shutdown();
}

bool AudioSystem::Initialize() {
    spdlog::info("Initializing Audio System...");

    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        spdlog::error("Failed to open audio device");
        return false;
    }

    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context) {
        spdlog::error("Failed to create audio context");
        alcCloseDevice(m_device);
        return false;
    }

    if (!alcMakeContextCurrent(m_context)) {
        spdlog::error("Failed to make audio context current");
        alcDestroyContext(m_context);
        alcCloseDevice(m_device);
        return false;
    }

    SetListenerPosition(0.0f, 0.0f, 0.0f);
    SetListenerVelocity(0.0f, 0.0f, 0.0f);
    SetListenerOrientation(0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

    spdlog::info("Audio System initialization complete");
    return true;
}

void AudioSystem::Update(float deltaTime) {

}

void AudioSystem::Shutdown() {
    // Clean up all sounds
    m_sounds.clear();

    // Clean up OpenAL
    if (m_context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_context);
        m_context = nullptr;
    }

    if (m_device) {
        alcCloseDevice(m_device);
        m_device = nullptr;
    }

    spdlog::info("Audio System shutdown complete");
}

bool AudioSystem::LoadSound(const std::string& name, const std::string& filename) {
    // Check if sound already exists
    if (m_sounds.find(name) != m_sounds.end()) {
        spdlog::warn("Sound '{}' already loaded", name);
        return true;
    }

    auto sound = std::make_unique<AudioSource>();
    sound->filename = filename;

    // Load audio file using libsndfile
    SF_INFO sfInfo;
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfInfo);
    if (!file) {
        spdlog::error("Failed to open audio file: {}", filename);
        return false;
    }


    std::vector<short> samples(sfInfo.frames * sfInfo.channels);
    sf_count_t framesRead = sf_readf_short(file, samples.data(), sfInfo.frames);
    sf_close(file);

    if (framesRead != sfInfo.frames) {
        spdlog::error("Failed to read all audio data from: {}", filename);
        return false;
    }


    ALenum format;
    if (sfInfo.channels == 1) {
        format = AL_FORMAT_MONO16;
    } else if (sfInfo.channels == 2) {
        format = AL_FORMAT_STEREO16;
    } else {
        spdlog::error("Unsupported audio format in: {}", filename);
        return false;
    }


    alGenBuffers(1, &sound->buffer);
    if (!CheckALError("Generate buffer")) {
        return false;
    }


    alBufferData(sound->buffer, format, samples.data(), 
                 static_cast<ALsizei>(samples.size() * sizeof(short)), 
                 sfInfo.samplerate);
    if (!CheckALError("Buffer data")) {
        alDeleteBuffers(1, &sound->buffer);
        return false;
    }

    alGenSources(1, &sound->source);
    if (!CheckALError("Generate source")) {
        alDeleteBuffers(1, &sound->buffer);
        return false;
    }

    alSourcei(sound->source, AL_BUFFER, sound->buffer);
    if (!CheckALError("Attach buffer to source")) {
        alDeleteSources(1, &sound->source);
        alDeleteBuffers(1, &sound->buffer);
        return false;
    }

    m_sounds[name] = std::move(sound);
    spdlog::info("Loaded sound: {} from {}", name, filename);
    return true;
}

void AudioSystem::PlaySound(const std::string& name, bool loop) {
    auto it = m_sounds.find(name);
    if (it == m_sounds.end()) {
        spdlog::warn("Sound '{}' not found", name);
        return;
    }

    alSourcei(it->second->source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcePlay(it->second->source);
    CheckALError("Play sound");
}

void AudioSystem::StopSound(const std::string& name) {
    auto it = m_sounds.find(name);
    if (it == m_sounds.end()) {
        spdlog::warn("Sound '{}' not found", name);
        return;
    }

    alSourceStop(it->second->source);
    CheckALError("Stop sound");
}

void AudioSystem::SetVolume(const std::string& name, float volume) {
    auto it = m_sounds.find(name);
    if (it == m_sounds.end()) {
        spdlog::warn("Sound '{}' not found", name);
        return;
    }

    alSourcef(it->second->source, AL_GAIN, volume);
    CheckALError("Set volume");
}

void AudioSystem::SetListenerPosition(float x, float y, float z) {
    alListener3f(AL_POSITION, x, y, z);
    CheckALError("Set listener position");
}

void AudioSystem::SetListenerVelocity(float x, float y, float z) {
    alListener3f(AL_VELOCITY, x, y, z);
    CheckALError("Set listener velocity");
}

void AudioSystem::SetListenerOrientation(float forwardX, float forwardY, float forwardZ,
                                        float upX, float upY, float upZ) {
    ALfloat orientation[] = { forwardX, forwardY, forwardZ, upX, upY, upZ };
    alListenerfv(AL_ORIENTATION, orientation);
    CheckALError("Set listener orientation");
}

void AudioSystem::SetSourcePosition(const std::string& name, float x, float y, float z) {
    auto it = m_sounds.find(name);
    if (it == m_sounds.end()) {
        spdlog::warn("Sound '{}' not found", name);
        return;
    }

    alSource3f(it->second->source, AL_POSITION, x, y, z);
    CheckALError("Set source position");
}

void AudioSystem::SetSourceVelocity(const std::string& name, float x, float y, float z) {
    auto it = m_sounds.find(name);
    if (it == m_sounds.end()) {
        spdlog::warn("Sound '{}' not found", name);
        return;
    }

    alSource3f(it->second->source, AL_VELOCITY, x, y, z);
    CheckALError("Set source velocity");
}

bool AudioSystem::CheckALError(const std::string& operation) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        spdlog::error("OpenAL error in {}: {}", operation, error);
        return false;
    }
    return true;
}


