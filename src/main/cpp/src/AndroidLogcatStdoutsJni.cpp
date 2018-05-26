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

#include <jni.h>

#include "AndroidLogcatStdouts.h"

// https://developer.android.com/training/articles/perf-jni.html#faq_ULE
// http://stackoverflow.com/a/2480564
// http://stackoverflow.com/a/17643762
// if your native file is compiled as C++ you would need extern "C" {...}
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL
Java_ru_nfeotools_alogstd_AndroidLogcatStdoutsJni_init(
    JNIEnv* env, jclass type, jstring appName_, jstring logPrefix_)
{
  const char* appName = env->GetStringUTFChars(appName_, 0);
  const char* logPrefix = env->GetStringUTFChars(logPrefix_, 0);

  return static_cast<jboolean>(
      alogstd::AndroidLogcatStdouts::init(appName, logPrefix));

  env->ReleaseStringUTFChars(appName_, appName);
  env->ReleaseStringUTFChars(logPrefix_, logPrefix);
}

#ifdef __cplusplus
}
#endif
