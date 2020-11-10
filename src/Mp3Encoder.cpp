#include "Mp3Encoder.h"

#include <lame.h>

namespace converter {

Mp3Encoder::Mp3Encoder(const WavFileReader::WavHeader &wavHeader,
                       const std::string &file)
    : _wavHeader(wavHeader), _file(file) {
  _mp3File.exceptions(std::ifstream::failbit);
  try {
    _mp3File.open(_file.c_str(), std::ios_base::binary);
  } catch (std::ifstream::failure e) {
    std::runtime_error("Error opening input/output file: ");
  }

  gfp = lame_init();

  lame_set_in_samplerate(gfp, _wavHeader.SamplesPerSec);
  lame_set_VBR(gfp, vbr_default);
  lame_set_VBR_q(gfp, 5);
  // Setting Channels
  switch (_wavHeader.NumOfChan) {
  case 1:
    lame_set_mode(gfp, MONO);
    lame_set_num_channels(gfp, 1);
    break;
  case 2:
    lame_set_mode(gfp, JOINT_STEREO);
    lame_set_num_channels(gfp, 2);
    break;
  }

  lame_set_bWriteVbrTag(gfp, 1);

  lame_set_out_samplerate(gfp, _wavHeader.SamplesPerSec);

  lame_set_findReplayGain(gfp, 1);
  lame_set_write_id3tag_automatic(gfp, 0);

  int lResult = 0;
  if (lame_init_params(gfp) == -1) {
    _mp3File.close();
    std::runtime_error("FATAL ERROR: parameters failed to initialize properly "
                       "in lame. Aborting!");
  }
}

int Mp3Encoder::encode(short int *pcm_in, const size_t pcm_size) {
  int write = 0;
  const auto bytes_per_sample = _wavHeader.bitsPerSample / 8;
  const auto mp3_size = (size_t)(1.25 * (pcm_size / bytes_per_sample) + 7200);
  unsigned char mp3_buffer[mp3_size];
  if (_wavHeader.NumOfChan == 1) {
    write = lame_encode_buffer(gfp, pcm_in, pcm_in, pcm_size / bytes_per_sample,
                               mp3_buffer, mp3_size);
  } else {
    write = lame_encode_buffer_interleaved(
        gfp, pcm_in, pcm_size / bytes_per_sample / _wavHeader.NumOfChan,
        mp3_buffer, mp3_size);
  }
  _mp3File.write(reinterpret_cast<char *>(mp3_buffer), write);
  return write;
}

void Mp3Encoder::finish() {
  unsigned char mp3_buffer[8192];
  auto write = lame_encode_flush(gfp, mp3_buffer, 8192);
  _mp3File.write(reinterpret_cast<char *>(mp3_buffer), write);
  _mp3File.close();
}

} // namespace converter
