/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#ifndef OCEANBASE_LOGSERVICE_LOG_ENTRY_HEADER_
#define OCEANBASE_LOGSERVICE_LOG_ENTRY_HEADER_

#include "lib/ob_define.h"
#include "lib/utility/ob_print_utils.h"
#include "share/scn.h"

namespace oceanbase
{
namespace palf
{
class LogEntryHeader
{
public:
  LogEntryHeader();
  ~LogEntryHeader();
public:
  int generate_header(const char *log_data,
                      const int64_t data_len,
                      const share::SCN &scn);
  LogEntryHeader& operator=(const LogEntryHeader &header);
  void reset();
  bool is_valid() const;
  bool check_integrity(const char *buf, const int64_t buf_len) const;
  int32_t get_data_len() const { return log_size_; }
  const share::SCN get_scn() const { return scn_; }
  int64_t get_data_checksum() const { return data_checksum_; }
  bool check_header_integrity() const;
  NEED_SERIALIZE_AND_DESERIALIZE;
  TO_STRING_KV("magic", magic_,
               "version", version_,
               "log_size", log_size_,
               "scn_", scn_,
               "data_checksum", data_checksum_,
               "flag", flag_);
public:
  static constexpr int16_t MAGIC = 0x4C48;  // 'LH' means LOG ENTRY HEADER
  static const int64_t HEADER_SER_SIZE;
private:
  bool get_header_parity_check_res_() const;
  void update_header_checksum_();
  bool check_header_checksum_() const;
private:
  static constexpr int16_t LOG_ENTRY_HEADER_VERSION = 1;
private:
  int16_t magic_;
  int16_t version_;
  int32_t log_size_;
  share::SCN scn_;
  int64_t data_checksum_;
  // The lowest bit is used for parity check.
  int64_t flag_;
};
}
}
#endif // OCEANBASE_LOGSERVICE_LOG_ENTRY_HEADER_
