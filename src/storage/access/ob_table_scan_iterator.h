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

#ifndef OCEANBASE_STORAGE_OB_TABLE_SCAN_ITERATOR_
#define OCEANBASE_STORAGE_OB_TABLE_SCAN_ITERATOR_

#include "common/row/ob_row_iterator.h"
#include "common/rowkey/ob_rowkey.h"
#include "lib/allocator/ob_fifo_allocator.h"
#include "lib/container/ob_se_array.h"
#include "share/schema/ob_schema_getter_guard.h"
#include "storage/blocksstable/ob_block_sstable_struct.h"
#include "ob_block_sample_iterator.h"
#include "storage/ob_col_map.h"
#include "storage/ob_i_store.h"
#include "ob_multiple_get_merge.h"
#include "ob_multiple_merge.h"
#include "ob_multiple_multi_scan_merge.h"
#include "ob_multiple_scan_merge.h"
#include "ob_multiple_skip_scan_merge.h"
#include "ob_multiple_multi_skip_scan_merge.h"
#include "ob_row_sample_iterator.h"
#include "ob_single_merge.h"
#include "storage/tx_storage/ob_access_service.h"
#include "storage/tx_storage/ob_ls_map.h"
#include "storage/tx/ob_trans_service.h"
#include "ob_table_scan_range.h"

namespace oceanbase
{
namespace storage
{
class ObTableScanParam;
class ObTableReadInfo;

class ObTableScanIterator : public common::ObNewRowIterator
{
public:
  ObTableScanIterator();
  virtual ~ObTableScanIterator();
  int init( ObTableScanParam &scan_param, const ObTabletHandle &tablet_handle);
  int switch_param( ObTableScanParam &scan_param, const ObTabletHandle &tablet_handle);
  virtual int get_next_row(common::ObNewRow *&row) override;
  virtual int get_next_row() override { common::ObNewRow *r = NULL; return get_next_row(r); }
  virtual int get_next_rows(int64_t &count, int64_t capacity) override;
  int rescan(ObTableScanParam &scan_param);
  void reuse();
  void reset_for_switch();
  virtual void reset();
  ObAccessService::ObStoreCtxGuard &get_ctx_guard() { return ctx_guard_; }
public:
  static constexpr int64_t RP_MAX_FREE_LIST_NUM = 1024;
  static constexpr const char LABEL[] = "RPTableScanIter";
private:
  static const int64_t LOOP_RESCAN_BUFFER_SIZE = 8 * 1024; // 8K
  int prepare_table_param(const ObTabletHandle &tablet_handle);
  int prepare_table_context();
  template<typename T> int init_scan_iter(T *&iter);
  template<typename T> void reset_scan_iter(T *&iter);
  int switch_scan_param(ObMultipleMerge &iter);
  void reuse_row_iters();
  int switch_param_for_iter();
  int open_iter();
  int can_retire_to_row_sample(bool &retire);  // try to retire to row sample
  // for read uncommitted data, txn possible rollbacked before iterate
  // check txn status after read rows to ensure read result is correct
  int check_txn_status_if_read_uncommitted_();
  bool is_inited_;
  ObSingleMerge *single_merge_;
  ObMultipleGetMerge *get_merge_;
  ObMultipleScanMerge *scan_merge_;
  ObMultipleMultiScanMerge *multi_scan_merge_;
  ObMultipleSkipScanMerge *skip_scan_merge_;
  ObRowSampleIterator *row_sample_iterator_;
  ObBlockSampleIterator *block_sample_iterator_; // TODO: refactor
  // we should consider the constructor cost
  ObTableAccessParam main_table_param_;
  ObTableAccessContext main_table_ctx_;
  ObGetTableParam get_table_param_;

  ObAccessService::ObStoreCtxGuard ctx_guard_;
  ObTableScanParam *scan_param_;
  ObTableScanRange table_scan_range_;
  ObQueryRowIterator *main_iter_;
private:
  DISALLOW_COPY_AND_ASSIGN(ObTableScanIterator);
};

} // end namespace storage
} // end namespace oceanbase

#endif // OCEANBASE_STORAGE_OB_TABLE_SCAN_ITERATOR_
