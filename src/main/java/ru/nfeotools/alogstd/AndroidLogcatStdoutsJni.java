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

package ru.nfeotools.alogstd;

public class AndroidLogcatStdoutsJni
{
  public native static boolean init(String appName, String logPrefix);

  static {
    try {
      // Needed for shared linking on Android < 4.3 (API < 18).
      // See also:
      // https://github.com/KeepSafe/ReLinker
      // https://github.com/ikonst/android-dl
      if (BuildConfig.BUILD_SHARED_LIBS) {
        System.loadLibrary("c++_shared");
      }

      System.loadLibrary("alogstd");

    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load.\n" + e);
      System.exit(1);
    }
  }
}
