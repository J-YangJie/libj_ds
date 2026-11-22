/*
  libj_ds permanent tests death helper
  Copyright (C) 2026  YangJie <yangjie98765@yeah.net>

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

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <signal.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test.h"

void test_death_expect_abort(void (*fn)(void* arg), void* arg, const char* what)
{
    pid_t pid;
    int status;

    fflush(NULL);
    pid = fork();
    if (pid < 0) {
        TEST_FAIL("Could not fork for death test `%s`.", what);
        return;
    }
    if (pid == 0) {
        struct rlimit core_limit = { 0, 0 };

        setrlimit(RLIMIT_CORE, &core_limit);
        fn(arg);
        _exit(0);
    }

    if (waitpid(pid, &status, 0) != pid) {
        TEST_FAIL("Could not wait for death test `%s`.", what);
        return;
    }
    if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGABRT)
        TEST_FAIL("Death test `%s` did not abort.", what);
}
