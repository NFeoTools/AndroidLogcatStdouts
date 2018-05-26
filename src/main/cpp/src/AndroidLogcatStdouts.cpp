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
  __android_log_print(ANDROID_LOG_DEBUG, "ALogStd", fmt, ##__VA_ARGS__)

namespace alogstd
{
// Redirect the "stdout" and "stderr" to android logcat.
// https://codelab.wordpress.com/2014/11/03/how-to-use-standard-output-streams-for-logging-in-android-apps/
// http://stackoverflow.com/a/31777050

static int gLogcatPfd[2];
static pthread_t gStdoutsThread;
static std::string gLogcatTag;
static std::string gLogPrefix;

/**
 * @author John Tsiombikas
 * @link https://codelab.wordpress.com/2014/11/03/how-to-use-standard-output-streams-for-logging-in-android-apps/
 */
static void* stdoutsThreadFunc(void*)
{
  ssize_t rdsz;
  char buf[256];

  // Workaround for android logcat formatting.
  unsigned int logPrefixSize = gLogPrefix.size();
  gLogPrefix.copy(buf, logPrefixSize, 0);

  while((rdsz = read(gLogcatPfd[0], buf + logPrefixSize,
             sizeof buf - 1 - logPrefixSize))
      > 0) {
    // if(buf[rdsz + 7 - 1 ] == '\n') --rdsz;
    buf[rdsz + logPrefixSize] = 0;  // add null-terminator
    __android_log_write(ANDROID_LOG_DEBUG, gLogcatTag.c_str(), buf);
  }
  return (0);
}

/**
 * @author John Tsiombikas
 * @link https://codelab.wordpress.com/2014/11/03/how-to-use-standard-output-streams-for-logging-in-android-apps/
 */
int redirectStdoutsToLogcat(
    const std::string& appName, const std::string& logPrefix)
{
  gLogcatTag = appName;
  gLogPrefix = logPrefix;

  // Make stdout line-buffered and stderr unbuffered.
  setvbuf(stdout, 0, _IOLBF, 0);
  setvbuf(stderr, 0, _IONBF, 0);

  // Create the pipe and redirect stdout and stderr.
  pipe(gLogcatPfd);
  dup2(gLogcatPfd[1], 1);
  dup2(gLogcatPfd[1], 2);

  // Spawn the logging thread.
  if(pthread_create(&gStdoutsThread, 0, stdoutsThreadFunc, 0) == -1) {
    return (-1);
  }

  pthread_detach(gStdoutsThread);
  return (0);
}

bool AndroidLogcatStdouts::init(
    const std::string& appName, const std::string& logPrefix)
{
  std::string prefStr(logPrefix);
  prefStr.resize(5, '-');
  prefStr += ' ';

  std::string initMsgStr = prefStr + "AndroidLogcatStdouts init %s";
  const char* initMsg = initMsgStr.c_str();

  LOG(initMsg, "starting");
  if(redirectStdoutsToLogcat(appName, prefStr)) {
    LOG(initMsg, "FAILED");
    return false;
  }
  LOG(initMsg, "finished");
  std::cout << "AndroidLogcatStdouts is ready" << std::endl;
  return true;
}

}  // namespace alogstd
