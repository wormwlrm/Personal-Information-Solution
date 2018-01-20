#pragma once
#ifndef SERVER_MAIN_H
#define SERVER_MAIN_H


#include <WinSock2.h>
#include "Log.h"

#include <iostream>
using std::cout;
using std::endl;

#include <thread>
#include <chrono>
#include <vector>

#include <string>
using std::string;
#include <stdexcept>


#define SERVER_PORT 11235  // 서버 포트
#define BUF_SIZE 4096 // 버퍼 사이즈
#define IPAddress "192.168.100.18" // 서버 IP
#define BAN_TIME 20

#endif // !SERVER_MAIN_H
