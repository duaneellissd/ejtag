#pragma once

#include <stdint.h>

struct cmd_callback_info;

int parse_struct(struct cmd_callback_info *pInfo, const char *fmt, void *struct_ptr);
int parse_base64_start(struct cmd_callback_info *pInfo);
int parse_base64_stop(struct cmd_callback_info *pInfo);


int reply_integer(struct cmd_callback_info *pInfo, int integer_value);
int reply_string(struct cmd_callback_info *pInfo, const char *str);
int reply_struct(struct cmd_callback_info *pInfo, const char *fmt, const void *struct_ptr);
int reply_buffer(struct cmd_callback_info *pInfo, uint8_t *buffer, size_t length);


	int data_struct_pack(struct cmd_callback_info *pInfo, const char *fmt, const void *struct_ptr);
