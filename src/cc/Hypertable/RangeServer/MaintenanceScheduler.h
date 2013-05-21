/** -*- c++ -*-
 * Copyright (C) 2007-2012 Hypertable, Inc.
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 3 of the
 * License, or any later version.
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

#ifndef HYPERTABLE_MAINTENANCESCHEDULER_H
#define HYPERTABLE_MAINTENANCESCHEDULER_H

#include "Common/ReferenceCount.h"

#include "MaintenancePrioritizerLogCleanup.h"
#include "MaintenancePrioritizerLowMemory.h"
#include "RSStats.h"
#include "TableInfoMap.h"

namespace Hypertable {

  class MaintenanceScheduler : public ReferenceCount {
  public:
    MaintenanceScheduler(MaintenanceQueuePtr &queue, RSStatsPtr &server_stats,
                         TableInfoMapPtr &live_map);

    void schedule();

    void set_low_memory_mode(bool on) {
      if (on) {
        if (!m_low_memory_mode && m_low_memory_prioritization)
          m_prioritizer = &m_prioritizer_low_memory;
      }
      else {
        if (m_low_memory_mode) {
          m_prioritizer = &m_prioritizer_log_cleanup;
          boost::xtime_get(&m_last_low_memory, TIME_UTC_);
        }
      }
      m_low_memory_mode = on;
    }

  private:

    int get_level(RangeData &rd);

    /** Returns <i>true</i> if in low memory mode
     * @return <i>true</i> if in low memory mode, <i>false</i> otherwise
     */
    bool low_memory_mode() { return m_low_memory_mode; }

    /** Checks to see if scheduler debug signal file exists.
     */
    bool debug_signal_file_exists(boost::xtime now);

    /** Writes debugging output and removes signal file.
     */
    void write_debug_output(boost::xtime now, RangeDataVector &range_data,
                            const String &header_str);

    MaintenanceQueuePtr m_queue;
    RSStatsPtr m_server_stats;
    TableInfoMapPtr m_live_map;
    MaintenancePrioritizer *m_prioritizer;
    MaintenancePrioritizerLogCleanup m_prioritizer_log_cleanup;
    MaintenancePrioritizerLowMemory  m_prioritizer_low_memory;
    int32_t m_maintenance_interval;
    boost::xtime m_last_low_memory;
    boost::xtime m_last_check;
    int64_t m_query_cache_memory;
    int32_t m_low_memory_limit_percentage;
    int32_t m_merging_delay;
    int32_t m_merges_per_interval;
    int32_t m_move_compactions_per_interval;
    int32_t m_maintenance_queue_worker_count;
    int32_t m_start_offset;
    bool m_initialized;
    bool m_low_memory_prioritization;
    bool m_low_memory_mode;
  };

  typedef intrusive_ptr<MaintenanceScheduler> MaintenanceSchedulerPtr;


}

#endif // HYPERTABLE_MAINTENANCESCHEDULER_H


