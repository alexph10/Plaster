// Stub sndfile header for basic compilation
#pragma once

typedef struct SNDFILE_tag SNDFILE;
typedef long long sf_count_t;

typedef struct {
  sf_count_t frames;
  int samplerate;
  int channels;
  int format;
  int sections;
  int seekable;
} SF_INFO;

// Basic formats
#define SF_FORMAT_WAV 0x010000
#define SF_FORMAT_PCM_16 0x0002
#define SFM_READ 0x10
#define SFM_WRITE 0x20
#define SFM_RDWR 0x30

// Stub functions
extern "C" {
SNDFILE *sf_open(const char *path, int mode, SF_INFO *sfinfo);
long sf_readf_float(SNDFILE *sndfile, float *ptr, long frames);
sf_count_t sf_readf_short(SNDFILE *sndfile, short *ptr, sf_count_t frames);
int sf_close(SNDFILE *sndfile);
const char *sf_strerror(SNDFILE *sndfile);
}