/*
  Data Structures Log
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __J_LOG_H
#define __J_LOG_H

typedef enum {
    LOG_TEST,
    LOG_ERROR,
    LOG_LEVEL,
    LOG_ATTENTION,
    LOG_WARNING,
    LOG_NOTICE,
    LOG_INFO,
    LOG_DEBUG,
} _log_level_t;

#include <stdio.h>
#define _NAME_CURR_PRO "libj_ds"
#define pr_test(fmt, ...)   do { if (LOG_TEST      < LOG_LEVEL) fprintf(stderr, "[T][%s]%s " fmt "\n", _NAME_CURR_PRO, TAG, ##__VA_ARGS__); } while(0)
#define pr_err(fmt, ...)    do { if (LOG_ERROR     < LOG_LEVEL) fprintf(stderr, "[E][%s]%s " fmt "\n", _NAME_CURR_PRO, TAG, ##__VA_ARGS__); } while(0)
#define pr_attn(fmt, ...)   do { if (LOG_ATTENTION < LOG_LEVEL) fprintf(stderr, "[A][%s]%s " fmt "\n", _NAME_CURR_PRO, TAG, ##__VA_ARGS__); } while(0)
#define pr_warn(fmt, ...)   do { if (LOG_WARNING   < LOG_LEVEL) fprintf(stderr, "[W][%s]%s " fmt "\n", _NAME_CURR_PRO, TAG, ##__VA_ARGS__); } while(0)
#define pr_notice(fmt, ...) do { if (LOG_NOTICE    < LOG_LEVEL) fprintf(stderr, "[N][%s]%s " fmt "\n", _NAME_CURR_PRO, TAG, ##__VA_ARGS__); } while(0)
#define pr_info(fmt, ...)   do { if (LOG_INFO      < LOG_LEVEL) fprintf(stdout, "[I][%s]%s " fmt "\n", _NAME_CURR_PRO, TAG, ##__VA_ARGS__); } while(0)
#define pr_debug(fmt, ...)  do { if (LOG_DEBUG     < LOG_LEVEL) fprintf(stdout, "[D][%s]%s " fmt "\n", _NAME_CURR_PRO, TAG, ##__VA_ARGS__); } while(0)

#endif /* __J_LOG_H */
