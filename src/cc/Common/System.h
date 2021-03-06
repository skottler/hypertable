/*
 * Copyright (C) 2007-2012 Hypertable, Inc.
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or any later version.
 *
 * Hypertable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/** @file
 * Retrieves system information (hardware, installation directory, etc)
 */

#ifndef HYPERTABLE_SYSTEM_H
#define HYPERTABLE_SYSTEM_H

#include <boost/random.hpp>
#include "Common/Version.h"
#include "Common/Mutex.h"
#include "Common/String.h"

namespace Hypertable {

  /** @addtogroup Common
   *  @{
   */

  struct CpuInfo;
  struct CpuStat;
  struct LoadAvgStat;
  struct MemStat;
  struct DiskStat;
  struct OsInfo;
  struct SwapStat;
  struct NetInfo;
  struct NetStat;
  struct ProcInfo;
  struct ProcStat;
  struct FsStat;
  struct TermInfo;

  /** @file
   * Retrieves system information (hardware, installation directory, etc)
   */
  class System {
  public:
    /**
     * Initializes the static class members; checks header version vs. library
     * version, initializes Logging and other stuff.
     *
     * Must be inlined to do proper version check
     *
     * @param install_directory Path of the installation directory. If empty
     *      then will try to find the installation directory by using argv[0]
     *      in combination with some heuristics
     */
    static inline void initialize(const String &install_directory = String()) {
      ScopedLock lock(ms_mutex);

      if (ms_initialized)
        return;

      check_version();
      _init(install_directory);
      ms_initialized = true;
    }


    /** Returns the installation directory
     *
     * @param argv0 argv[0] of the main method
     * @return The (presumed) installation directory
     */
    static String locate_install_dir(const char *argv0);

    /** Returns the installation directory; same as %locate_install_dir but
     * does not lock %ms_mutex.
     *
     * @param argv0 argv[0] of the main method
     * @return The (presumed) installation directory
     */
    static String _locate_install_dir(const char *argv0);

    /** The installation directory */
    static String install_dir;

    /** The exe file name */
    static String exe_name;

    /** The processor count */
    static int32_t get_processor_count();

    /** The pid of the current process */
    static int32_t get_pid();

    /** Sets the random seed of the random number generator */
    static void seed(uint32_t seed) { ms_rng.seed(seed); }

    /** Returns a random 32bit number */
    static uint32_t rand32() { return ms_rng(); }

    /** Returns a random 64bit number */
    static uint64_t rand64() { return (uint64_t)rand32() << 32 | rand32(); }

    /** Returns the number of drives */
    static int32_t get_drive_count();

    /** Retrieves updated CPU information (see SystemInfo.h) */
    static const CpuInfo &cpu_info();

    /** Retrieves updated CPU statistics (see SystemInfo.h) */
    static const CpuStat &cpu_stat();

    /** Retrieves updated Memory statistics (see SystemInfo.h) */
    static const MemStat &mem_stat();

    /** Retrieves updated Disk statistics (see SystemInfo.h) */
    static const DiskStat &disk_stat();

    /** Retrieves updated Operating system information (see SystemInfo.h) */
    static const OsInfo &os_info();

    /** Retrieves updated Swap statistics (see SystemInfo.h) */
    static const SwapStat &swap_stat();

    /** Retrieves updated Network information (see SystemInfo.h) */
    static const NetInfo &net_info();

    /** Retrieves updated Network statistics (see SystemInfo.h) */
    static const NetStat &net_stat();

    /** Retrieves updated Process information (see SystemInfo.h) */
    static const ProcInfo &proc_info();

    /** Retrieves updated Process statistics (see SystemInfo.h) */
    static const ProcStat &proc_stat();

    /** Retrieves updated Filesystem statistics (see SystemInfo.h) */
    static const FsStat &fs_stat();

    /** Retrieves updated Terminal information (see SystemInfo.h) */
    static const TermInfo &term_info();

    /** Retrieves updated Load average statistics (see SystemInfo.h) */
    static const LoadAvgStat &loadavg_stat();

  private:
    /** Internal initialization helper */
    static void _init(const String &install_directory);

    /** true if %initialize was already called */
    static bool ms_initialized;

    /** a %Mutex to protect the static members */
    static Mutex ms_mutex;

    /** Random number generator */
    static boost::mt19937 ms_rng;
  };

  /** @} */

} // namespace Hypertable

#endif // HYPERTABLE_SYSTEM_H
