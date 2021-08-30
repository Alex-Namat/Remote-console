//
// Created by Namat on 14.10.2017.
//

#ifndef SERVER_WIN_PROCESS_HPP
#define SERVER_WIN_PROCESS_HPP

#include <windows.h>
#include <string>

class process {
public:
    process();
    ~process();
    void run();
    void stop();
    bool is_active() const noexcept;
    std::string read(char *buffer, const size_t &length);
    void write(std::string str);

    bool buffer_overflow() const noexcept;
private:
    void kill_process_tree() const noexcept;

    STARTUPINFO si;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;

    HANDLE newstdin,newstdout,read_stdout,write_stdin;  //дескрипторы пайпов

    size_t msg_size {0};

    bool buffer_overflow_ {false};
};


#endif //SERVER_WIN_PROCESS_HPP
