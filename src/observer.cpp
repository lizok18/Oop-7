#include "observer.hpp"
#include <iostream>

void ConsoleObserver::on_fight(std::shared_ptr<NPC> attacker, 
                              std::shared_ptr<NPC> defender, 
                              bool win) {
    if (win) {
        std::cout << "[БОЙ] " << attacker->getName() 
                  << " убил " << defender->getName() << std::endl;
    }
}

std::shared_ptr<ConsoleObserver> ConsoleObserver::get() {
    static auto instance = std::make_shared<ConsoleObserver>();
    return instance;
}

FileObserver::FileObserver(const std::string& filename) {
    log_file.open(filename, std::ios::app);
}

FileObserver::~FileObserver() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

void FileObserver::on_fight(std::shared_ptr<NPC> attacker, 
                           std::shared_ptr<NPC> defender, 
                           bool win) {
    if (win && log_file.is_open()) {
        log_file << "[БОЙ] " << attacker->getName() 
                 << " убил " << defender->getName() << std::endl;
    }
}

std::shared_ptr<FileObserver> FileObserver::get() {
    static auto instance = std::make_shared<FileObserver>("battle_log.txt");
    return instance;
}