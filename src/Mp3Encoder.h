#pragma once

#include "WavFileReader.h"

#include <lame.h>

#include <fstream>
#include <iostream>
#include <string>

namespace converter {

class Mp3Encoder {
public:
  Mp3Encoder(const WavFileReader::WavHeader &wavHeader,
             const std::string &file);
  virtual ~Mp3Encoder() = default;

  int encode(short int *pcm_in, const size_t pcm_size);
  void finish();

private:
  const WavFileReader::WavHeader _wavHeader;
  const std::string _file;
  lame_global_flags *gfp = nullptr;
  char errMsg[1000];
  std::ofstream _mp3File;
};

} // namespace converter
