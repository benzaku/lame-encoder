#include "WavFileReader.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

namespace converter {

WavFileReader::WavFileReader(const std::filesystem::path &wavFilePath)
    : _wav(wavFilePath.c_str(), std::ios_base::binary),
      _filePath(wavFilePath.string()) {
  if (!_wav.read(reinterpret_cast<char *>(&_header), sizeof(WavHeader))) {
    std::runtime_error("Cannot load wav file header");
  }
  _pcmSize = _header.Subchunk2Size;
}

WavFileReader::~WavFileReader() {}

size_t WavFileReader::read(short int *buffer, const size_t bufferSize) {
  const auto dataLeft = _header.Subchunk2Size - _dataRead;
  const auto sizeToRead = dataLeft >= bufferSize ? bufferSize : dataLeft;
  if (!_wav.read(reinterpret_cast<char *>(buffer), sizeToRead)) {
    return 0;
  }
  _dataRead += sizeToRead;
  return sizeToRead;
}

const size_t WavFileReader::dataRead() const { return _dataRead; }

const size_t WavFileReader::pcmSize() const { return _pcmSize; }

const WavFileReader::WavHeader &WavFileReader::getHeader() const {
  return _header;
}

} // namespace converter
