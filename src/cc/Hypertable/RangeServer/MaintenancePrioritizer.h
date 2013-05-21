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

#ifndef HYPERTABLE_MAINTENANCEPRIORITIZER_H
#define HYPERTABLE_MAINTENANCEPRIORITIZER_H

#include "Common/Logger.h"
#include "Common/ReferenceCount.h"
#include "Common/Time.h"

#include "Global.h"
#include "RSStats.h"

namespace Hypertable {

  class MaintenancePrioritizer : public ReferenceCount {
  public:

    class MemoryState {
    public:
      MemoryState() : limit(0), balance(0), needed(0) { }
      void decrement_needed(int64_t amount) {
        if (amount > needed)
          needed = 0;
        else
          needed -= amount;
      }
      bool need_more() { return needed > 0; }
      int64_t limit;
      int64_t balance;
      int64_t needed;
    };

    MaintenancePrioritizer(RSStatsPtr &server_stats)
      : m_cellstore_minimum_size(0), m_server_stats(server_stats),
        m_initialization_complete(false), m_uninitialized_ranges_seen(false) { }

    virtual void prioritize(RangeDataVector &range_data, MemoryState &memory_state,
                            int32_t priority, String *trace) = 0;

  protected:

    int64_t m_cellstore_minimum_size;
    RSStatsPtr m_server_stats;
    bool m_initialization_complete;
    bool m_uninitialized_ranges_seen;

    void schedule_initialization_operations(RangeDataVector &range_data,
                                            int32_t &priority);

    bool schedule_inprogress_operations(RangeDataVector &range_data,
                                        MemoryState &memory_state,
                                        int32_t &priority, String *trace);

    bool schedule_splits_and_relinquishes(RangeDataVector &range_data,
                                          MemoryState &memory_state,
                                          int32_t &priority, String *trace);

    bool schedule_necessary_compactions(RangeDataVector &range_data,
                            CommitLog *log, int64_t prune_threshold,
                            MemoryState &memory_state,
                            int32_t &priority, String *trace);

    bool purge_shadow_caches(RangeDataVector &range_data,
                             MemoryState &memory_state,
                             int32_t &priority, String *trace);

    bool purge_cellstore_indexes(RangeDataVector &range_data,
                                 MemoryState &memory_state,
                                 int32_t &priority, String *trace);

    bool compact_cellcaches(RangeDataVector &range_data,
                            MemoryState &memory_state,
                            int32_t &priority, String *trace);


  };
  typedef intrusive_ptr<MaintenancePrioritizer> MaintenancePrioritizerPtr;

}

#endif // HYPERTABLE_MAINTENANCEPRIORITIZER_H


