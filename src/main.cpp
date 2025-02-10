#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#include <emscripten/wasmfs.h>
#endif

int main(int argc, const char **argv)
{
    std::cout << "Hello\n";

#ifdef USE_PROXY_TO_THREAD
    std::cout << "Proxying to pthread\n";
#else
    std::cout << "Running on a separately spawned thread\n";
    std::thread thread([]() {
#endif
    std::cout << "Starting thread\n" << std::flush;
    backend_t opfs_fs = wasmfs_create_opfs_backend();
    std::cout << "backend handle = " << opfs_fs << "\n";

    std::cout << "Created wasm fs backend\n" << std::flush;
    {
        struct stat stat_buf;
        int err = stat("/opfs", &stat_buf);
        if (err != 0) {
            perror("stat /opfs");
        }
    }

    // TODO: will this fail if we already made the dir?
    {
        int err = wasmfs_create_directory("/opfs", 0777, opfs_fs);
        if (err != 0) {
            std::cerr << "Failed to make /opfs dir\n";
        }
    }

    {
        const char *filename = "/opfs/out.txt";

        struct stat stat_buf;
        int err = stat(filename, &stat_buf);
        if (err != 0) {
            perror("stat /opfs/out.txt");

            std::cout << "Making file " << filename << "\n";
            std::ofstream fout(filename);
            fout << "Test output";
        } else {
            std::cout << "Last modification of " << filename << " is "
                      << stat_buf.st_mtim.tv_sec << "s\n";
            std::ifstream fin(filename);
            std::cout << "file contains: ";
            std::string line;
            while (std::getline(fin, line)) {
                std::cout << line << "\n";
            }
        }
    }
#ifndef USE_PROXY_TO_THREAD
});
thread.detach();
#endif

return 0;
}
