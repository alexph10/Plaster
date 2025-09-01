#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <unordered_map>
#include <memory>

struct AudioSource {
    ALuint buffer;
    ALuint source;
    std::string filename;
};

class AudioSystem {
public:
    AudioSystem();
    ~AudioSystem();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();


    bool LoadSound(const std::string& name, const std::string& filename);
    void PlaySound(const std::string& name, bool loop = false);
    void StopSound(const std::string& name);
    void SetVolume(const std::string& name, float volume);


    void SetListenerPosition(float x, float y, float z);
    void SetListenerVelocity(float x, float y, float z);
    void SetListenerOrientation(float forwardX, float forwardY, float forwardZ,
                               float upX, float upY, float upZ);

    void SetSourcePosition(const std::string& name, float x, float y, float z);
    void SetSourceVelocity(const std::string& name, float x, float y, float z);

private:
    ALCdevice* m_device;
    ALCcontext* m_context;
    
    std::unordered_map<std::string, std::unique_ptr<AudioSource>> m_sounds;

    bool CheckALError(const std::string& operation);
    ALuint LoadWAV(const std::string& filename);
};
