#include "Executor.h"
#include "Mp3Encoder.h"
#include "WavFileReader.h"

#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <string>
#include <thread>

namespace fs = std::filesystem;
namespace {

const auto USAGE = "Usage: converter <path contains wav files>";
static auto files = std::vector<fs::path>{};

const std::string ext = "mp3";
} // namespace

int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cerr << USAGE << std::endl;
    return 1;
  }

  const fs::path wav_directory = std::string(argv[1]);

  if (!fs::exists(wav_directory)) {
    std::cerr << "Input directory of wav files: " << wav_directory
              << " does not exist" << std::endl;
    return 1;
  }

  if (!fs::is_directory(wav_directory)) {
    std::cerr << "Input directory of wav files: " << wav_directory
              << " is not a directory" << std::endl;
    return 1;
  }

  for (const auto file : fs::directory_iterator(wav_directory)) {
    if (file.path().extension() == ".wav") {
      std::cout << "Adding " << file.path() << " to queue" << std::endl;
      files.emplace_back(file);
    }
  }

  if (files.empty()) {
    std::cerr << wav_directory << " does not contain wave files." << std::endl;
    return 1;
  }

  std::cout << "There are " << files.size() << " .wav files to be converted."
            << std::endl;

  auto readers =
      std::vector<std::tuple<std::shared_ptr<converter::WavFileReader>,
                             std::shared_ptr<converter::Executor>,
                             std::shared_ptr<converter::Mp3Encoder>>>{};
  for (auto p : files) {
    const auto executor = std::make_shared<converter::Executor>();
    const auto pReader = std::make_shared<converter::WavFileReader>(p);
    const auto pEncoder = std::make_shared<converter::Mp3Encoder>(
        pReader->getHeader(), p.replace_extension(".mp3").string());
    readers.emplace_back(std::make_tuple(pReader, executor, pEncoder));
  }

  for (const auto &t : readers) {
    const auto reader = std::get<0>(t);
    const auto executor = std::get<1>(t);
    const auto encoder = std::get<2>(t);
    executor->start();
    executor->post(
        std::make_shared<converter::Task>([reader, executor, encoder]() {
          short int buffer[8192];
          while (reader->dataRead() < reader->pcmSize()) {
            const auto read = reader->read(buffer, 8192);
            encoder->encode(buffer, read);
          }
          encoder->finish();
        }));
  }

  for (const auto &t : readers) {
    std::get<1>(t)->finish();
  }

  return 0;
}
