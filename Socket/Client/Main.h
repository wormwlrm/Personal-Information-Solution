#pragma once
#ifndef CLIENT_MAIN_H
#define CLIENT_MAIN_H

#include <WinSock2.h>

#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <thread>
#include <chrono>

#include <string>
using std::string;
#include <stdexcept>

#define SERVER_PORT 11235  // ���� ��Ʈ
#define BUF_SIZE 4096 // ���� ������
#define IPAddress "192.168.100.18" // ���� IP

#endif // !CLIENT_MAIN_H
