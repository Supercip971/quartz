#pragma once
#include <fstream>

#include "result.hpp"
#include "utils/traits.hpp"
namespace plt {

class File : public NoCopy {

    std::string path;

    bool is_open = false;
    bool is_binary = false;
    bool is_read = false;

    std::fstream file;

    File(){};

public:
    File(File &&f) {
        std::swap(path, f.path);
        std::swap(is_open, f.is_open);
        std::swap(is_binary, f.is_binary);
        std::swap(is_read, f.is_read);
        std::swap(file, f.file);
    }

    File &operator=(File &&f) {
        std::swap(path, f.path);
        std::swap(is_open, f.is_open);
        std::swap(is_binary, f.is_binary);
        std::swap(is_read, f.is_read);
        std::swap(file, f.file);
        return (*this);
    }
    static Result<File> open(std::string const &path) {

        File file;
        file.path = path;

        file.file.open(path, std::fstream::in);
        if (!file.file.is_open()) {
            return Result<File>::err("Could not open file");
        }
        file.is_open = true;
        file.is_read = true;
        return Result<File>::ok(std::move(file));
    }

    static Result<File> openBinary(std::string const &path) {

        File file;
        file.path = path;
        file.file.open(path, std::fstream::binary | std::fstream::in);
        if (!file.file.is_open()) {
            return Result<File>::err("Could not open file: {}", path);
        }
        file.is_open = true;
        file.is_read = true;
        file.is_binary = true;
        return Result<File>::ok(std::move(file));
    }

    size_t size() {
        if (!is_open) {
            return 0;
        }
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        return size;
    }

    Result<std::vector<uint8_t>> readWhole() {
        if (!is_open) {
            return std::string("File is not open");
        }
        auto size = this->size();
        std::vector<uint8_t> buffer(size);
        file.read((char *)buffer.data(), size);
        return buffer;
    }

    void close() {

        if (is_open) {

            debug$("closing file: {}", path);
            file.close();
            is_open = false;
        }
    }

    ~File() {
        close();
    }
};

} // namespace plt
