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

#ifndef HYPERTABLE_RANGESTATE_H
#define HYPERTABLE_RANGESTATE_H

#include "Common/PageArenaAllocator.h"
#include "Common/String.h"

namespace Hypertable {

  /**
   * Holds the persistent state of a Range.
   */
  class RangeState {
  public:
    //
    // !!
    // this enum is a mixture of an enumeration and a bitfield; the last bit
    // is always reserved for the PHANTOM flag!
    //
    enum StateType {
      STEADY,
      SPLIT_LOG_INSTALLED,
      SPLIT_SHRUNK,
      RELINQUISH_LOG_INSTALLED,
      PHANTOM =  0x80
      };
    RangeState() : state(STEADY), timestamp(0), soft_limit(0), transfer_log(0),
                   split_point(0), old_boundary_row(0), source(0) { }
    RangeState(CharArena &arena, const RangeState &other) {
      state = other.state;
      timestamp = other.timestamp;
      soft_limit = other.soft_limit;
      transfer_log = other.transfer_log ? arena.dup(other.transfer_log) : 0;
      split_point = other.split_point ? arena.dup(other.split_point) : 0;
      old_boundary_row = other.old_boundary_row ? arena.dup(other.old_boundary_row) : 0;
      source = other.source ? arena.dup(other.source) : 0;
    }
    virtual ~RangeState() {}

    virtual void clear();

    size_t encoded_length() const;
    void encode(uint8_t **bufp) const;
    virtual void decode(const uint8_t **bufp, size_t *remainp);

    static String get_text(uint8_t state);

    uint8_t state;
    int64_t timestamp;
    uint64_t soft_limit;
    const char *transfer_log;
    const char *split_point;
    const char *old_boundary_row;
    const char *source;
  };

  std::ostream &operator<<(std::ostream &, const RangeState &);

  /**
   * Holds the storage the persistent state of a Range.
   */
  class RangeStateManaged : public RangeState {
  public:
    RangeStateManaged() { }
    RangeStateManaged(const RangeStateManaged &rs) {
      operator=(rs);
    }
    RangeStateManaged(const RangeState &rs) {
      operator=(rs);
    }
    virtual ~RangeStateManaged() {}
    virtual void clear();

    RangeStateManaged& operator=(const RangeStateManaged &other) {
      const RangeState *otherp = &other;
      return operator=(*otherp);
    }
    RangeStateManaged& operator=(const RangeState &rs) {
      state = rs.state;
      timestamp = rs.timestamp;
      soft_limit = rs.soft_limit;

      if (rs.transfer_log) {
        m_transfer_log = rs.transfer_log;
        transfer_log = m_transfer_log.c_str();
      }
      else
        clear_transfer_log();

      if (rs.split_point) {
        m_split_point = rs.split_point;
        split_point = m_split_point.c_str();
      }
      else
        clear_split_point();

      if (rs.old_boundary_row) {
        m_old_boundary_row = rs.old_boundary_row;
        old_boundary_row = m_old_boundary_row.c_str();
      }
      else
        clear_old_boundary_row();

      if (rs.source) {
        m_source = rs.source;
        source = m_source.c_str();
      }
      else
        clear_source();

      return *this;
    }

    void set_transfer_log(const String &tl) {
      m_transfer_log = tl;
      transfer_log = m_transfer_log.c_str();
    }

    void clear_transfer_log() {
      m_transfer_log = "";
      transfer_log = 0;
    }

    void set_split_point(const String &sp) {
      m_split_point = sp;
      split_point = m_split_point.c_str();
    }

    void clear_split_point() {
      m_split_point = "";
      split_point = 0;
    }

    void set_old_boundary_row(const String &osr) {
      m_old_boundary_row = osr;
      old_boundary_row = m_old_boundary_row.c_str();
    }

    void clear_old_boundary_row() {
      m_old_boundary_row = "";
      old_boundary_row = 0;
    }

    void set_source(const String &osr) {
      m_source = osr;
      source = m_source.c_str();
    }

    void clear_source() {
      m_source = "";
      source = 0;
    }

    virtual void decode(const uint8_t **bufp, size_t *remainp);

  private:
    String m_transfer_log;
    String m_split_point;
    String m_old_boundary_row;
    String m_source;
  };

} // namespace Hypertable

#endif // HYPERTABLE_RANGESTATE_H
