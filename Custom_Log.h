#ifndef CUSTOM_LOG_H
#define CUSTOM_LOG_H

#include <stdio.h>

#define title(msg, ...) printf("[*] " msg "\n", ##__VA_ARGS__)
#define succ(msg, ...) printf("[+] " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[i] " msg "\n", ##__VA_ARGS__)
#define warn(msg, ...) printf("[!] " msg "\n", ##__VA_ARGS__)

#endif