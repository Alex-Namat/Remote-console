//
// Created by Namat on 14.10.2017.
//

#include "win_process.hpp"
#include <iostream>

process::process(){
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = true;       //разрешаем наследование дескрипторов
}

void process::run() {
    if (!CreatePipe(&newstdin, &write_stdin, &sa, 0))   //создаем пайп для stdin
    {
        throw std::runtime_error("Fail CreatePipe for stdin!");
    }

    if (!CreatePipe(&read_stdout, &newstdout, &sa, 0)) //создаем пайп для stdout
    {
        CloseHandle(newstdin);
        CloseHandle(write_stdin);
        throw std::runtime_error("Fail CreatePipe for stdout!");
    }

    GetStartupInfo(&si);      //создаем startupinfo для
    // дочернего процесса

    /*

    Параметр dwFlags сообщает функции CreateProcess
    как именно надо создать процесс.

    STARTF_USESTDHANDLES управляет полями hStd*.
    STARTF_USESHOWWINDOW управляет полем wShowWindow.

    */

    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = newstdout;
    si.hStdError = newstdout;   //подменяем дескрипторы для
    si.hStdInput = newstdin;    // дочернего процесса

    if (!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE,
                       NULL, NULL, &si, &pi)) {
        CloseHandle(newstdin);
        CloseHandle(newstdout);
        CloseHandle(read_stdout);
        CloseHandle(write_stdin);
        throw std::runtime_error("Fail CreateProcess!");
    }
}

bool process::is_active() const noexcept{
    WaitForSingleObject(pi.hProcess,1);
    unsigned long exit = 0;
    GetExitCodeProcess(pi.hProcess, &exit);
    if (exit != STILL_ACTIVE) return false;
    return true;
}

std::string process::read(char *buffer, const size_t& length) {
    unsigned long bread = 0;   //кол-во прочитанных байт

    std::string str;
    bool first_word = true;

    do {
        memset(buffer, 0, length);
        ReadFile(read_stdout, buffer, length - 1, &bread, NULL);
        if (first_word) {                           //пропускаем введеную команду
            first_word = false;
            str = std::string(buffer + msg_size);
        } else
            str += std::string(buffer);

        if (str.size() > length - 1){
            buffer_overflow_ = true;
            return std::string(str,0,length-1);
        }

    } while ((str.empty() || str.back() != '>') &&
             !buffer_overflow_ &&
             this->is_active());

    return str;
}

void process::write(std::string str){
    unsigned long bread;
    str += "\n";
    msg_size = str.size();
    WriteFile(write_stdin, str.c_str(), str.size(), &bread, NULL);
}

process::~process() {
    stop();
}

void process::stop() {
    if(this->is_active()) kill_process_tree();
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(newstdin);
    CloseHandle(newstdout);
    CloseHandle(read_stdout);
    CloseHandle(write_stdin);
}

bool process::buffer_overflow() const noexcept {
    return buffer_overflow_;
}

void process::kill_process_tree() const noexcept {
    auto str = "taskkill /PID " + std::to_string(pi.dwProcessId) + " /F /T";
    system(str.c_str());
}
