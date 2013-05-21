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

#ifndef HYPERTABLE_COMMITLOGREADER_H
#define HYPERTABLE_COMMITLOGREADER_H

#include <stack>
#include <vector>

#include <boost/thread/mutex.hpp>

#include "Common/ReferenceCount.h"
#include "Common/String.h"
#include "Common/HashMap.h"
#include "Common/Filesystem.h"

#include "BlockCompressionCodec.h"
#include "BlockCompressionHeaderCommitLog.h"
#include "CommitLogBase.h"
#include "CommitLogBlockStream.h"
#include "Key.h"


namespace Hypertable {

  class CommitLogReader : public CommitLogBase {

  public:
    CommitLogReader(FilesystemPtr &fs, const String &log_dir);

    CommitLogReader(FilesystemPtr &fs, const String &log_dir,
            const std::vector<uint32_t> &fragment_filter);

    virtual ~CommitLogReader() { }

    void get_init_fragment_ids(std::vector<uint32_t> &ids);

    bool next_raw_block(CommitLogBlockInfo *,
                        BlockCompressionHeaderCommitLog *);
    bool next(const uint8_t **blockp, size_t *lenp,
              BlockCompressionHeaderCommitLog *);

    void reset() {
      m_fragment_queue_offset = 0;
      m_block_buffer.clear();
      m_revision = TIMESTAMP_MIN;
      m_latest_revision = TIMESTAMP_MIN;
      m_error_map.clear();
    }

    void get_linked_logs(StringSet &linked_logs) {
      linked_logs.insert(m_linked_logs.begin(), m_linked_logs.end());
    }

    const std::map<uint32_t, uint32_t> &get_error_map() { return m_error_map; }

    String last_fragment_fname() { return m_last_fragment_fname; }

    int32_t last_fragment_id() { return m_last_fragment_id; }

  private:

    void load_fragments(String log_dir, CommitLogFileInfo *parent);
    void load_compressor(uint16_t ztype);

    FilesystemPtr     m_fs;
    uint64_t          m_fragment_queue_offset;
    DynamicBuffer     m_block_buffer;
    int64_t           m_revision;

    typedef hash_map<uint16_t, BlockCompressionCodecPtr> CompressorMap;

    CompressorMap          m_compressor_map;
    uint16_t               m_compressor_type;
    BlockCompressionCodec *m_compressor;
    std::set<uint64_t>     m_fragment_filter;
    std::vector<uint32_t>  m_init_fragments;
    std::map<uint32_t, uint32_t> m_error_map;
    StringSet              m_linked_logs;
    String                 m_last_fragment_fname;
    int32_t                m_last_fragment_id;
    bool                   m_verbose;
  };

  typedef intrusive_ptr<CommitLogReader> CommitLogReaderPtr;

} // namespace Hypertable

#endif // HYPERTABLE_COMMITLOGREADER_H

