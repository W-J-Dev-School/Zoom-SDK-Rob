#pragma once

#include "file_transfer.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <thread>
#include "httplib.h"

bool send_file(const char *addr, const wchar_t* path)
{
    std::cout << "[CLIENT] " << "Sending started" << std::endl;

    std::wstring video_path = path;
    video_path.append(L"\\meetingrec_0.mp4");

    httplib::Client http_client(addr);

    // Send file

    std::fstream file;
    file.open(video_path, std::ios::binary | std::ios::in | std::ios::ate);
    assert(file.is_open() && "Input file failed to open!");

    size_t file_size = file.tellg();
    file.seekg(0);

    httplib::Result response = http_client.Post(
        "/", [&](size_t offset, httplib::DataSink& sink) {
            char data[8192] = { 0 };

            file.seekg(offset);
            file.read(data, sizeof(data));

            assert(!file.bad() && "File is bad!");

            size_t data_length = file.gcount();

            std::cout << "[CLIENT] " << "Sending " << data_length << " bytes" << std::endl;
            sink.write(data, data_length);

            if (file.eof())
            {
                std::cout << "[CLIENT] "<< "Sending done" << std::endl;
                sink.done();
            }

            return true;
        },
        "application/octet-stream");

    file.close();

    bool ok = response.error() == httplib::Error::Success;

    if (ok)
        std::cout << "[CLIENT] " << "Response: " << response.value().body << std::endl;
    else
        std::cout << "[CLIENT] " << "Failure" << std::endl;

    return ok;
}

void send_file_in_thread(const char *addr, const wchar_t *path)
{
    std::thread(send_file, addr, path);
}