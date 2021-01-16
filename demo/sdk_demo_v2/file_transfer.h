#pragma once

bool send_file(const char *addr, const wchar_t *path);
void send_file_in_thread(const char *addr, const wchar_t *path);