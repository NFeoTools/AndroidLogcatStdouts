/*****************************************************************************
 * Project:  AndroidLogcatStdouts
 * Purpose:  Library for the redirection a stdouts
 *           from the native code to the Adroid's logcat.
 * Author:   NikitaFeodonit, nfeodonit@yandex.com
 *****************************************************************************
 *   Copyright (c) 2017-2018 NikitaFeodonit
 *
 *    This file is part of the AndroidLogcatStdouts project.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published
 *    by the Free Software Foundation, either version 3 of the License,
 *    or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *    See the GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "AndroidLogcatStdouts.h"

#include <unistd.h>
#include <iostream>

#include <android/log.h>

#define LOG(fmt, ...) \
  __android_log_print(ANDROID_LOG_DEBUG, "Feographia", fmt, ##__VA_ARGS__)

namespace fgr
{
// Redirect the "stdout" and "stderr" to android logcat.
// https://codelab.wordpress.com/2014/11/03/how-to-use-standard-output-streams-for-logging-in-android-apps/
// http://stackoverflow.com/a/31777050

static int logcatPfd[2];
static pthread_t stdoutsThread;
static const char* pLogcatTag;

/**
 * @author John Tsiombikas
 * @link https://codelab.wordpress.com/2014/11/03/how-to-use-standard-output-streams-for-logging-in-android-apps/
 */
static void* stdoutsThreadFunc(void*)
{
  ssize_t rdsz;
  char buf[256];

  // Workaround for android logcat formatting.
  buf[0] = '-';
  buf[1] = 'F';
  buf[2] = 'g';
  buf[3] = 'r';
  buf[4] = '-';
  buf[5] = ' ';

  int logPrefixSize = 6;

  while ((rdsz = read(logcatPfd[0], buf + logPrefixSize,
              sizeof buf - 1 - logPrefixSize))
      > 0) {
    // if(buf[rdsz + 7 - 1 ] == '\n') --rdsz;
    buf[rdsz + logPrefixSize] = 0; /* add null-terminator */
    __android_log_write(ANDROID_LOG_DEBUG, pLogcatTag, buf);
  }
  return (0);
}

/**
 * @author John Tsiombikas
 * @link https://codelab.wordpress.com/2014/11/03/how-to-use-standard-output-streams-for-logging-in-android-apps/
 */
int redirectStdoutsToLogcat(const char* pAppName)
{
  pLogcatTag = pAppName;

  // Make stdout line-buffered and stderr unbuffered.
  setvbuf(stdout, 0, _IOLBF, 0);
  setvbuf(stderr, 0, _IONBF, 0);

  // Create the pipe and redirect stdout and stderr.
  pipe(logcatPfd);
  dup2(logcatPfd[1], 1);
  dup2(logcatPfd[1], 2);

  // Spawn the logging thread.
  if (pthread_create(&stdoutsThread, 0, stdoutsThreadFunc, 0) == -1) {
    return (-1);
  }

  pthread_detach(stdoutsThread);
  return (0);
}

bool AndroidLogcatStdouts::init()
{
  LOG("-Fgr- AndroidLogcatStdouts init %s", "starting");
  if (redirectStdoutsToLogcat("Feographia")) {
    LOG("-Fgr- AndroidLogcatStdouts init %s", "FAILED");
    return false;
  }
  LOG("-Fgr- AndroidLogcatStdouts init %s", "finished");
  std::cout << "AndroidLogcatStdouts is ready" << std::endl;
  return true;
}
}  // namespace fgr
