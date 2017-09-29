/* Copyright (c) 2010, 2017, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
  */

#ifndef PFS_USER_H
#define PFS_USER_H

/**
  @file storage/perfschema/pfs_user.h
  Performance schema user (declarations).
*/

#include <sys/types.h>
#include <atomic>

#include "lf.h"
#include "my_inttypes.h"
#include "storage/perfschema/pfs_con_slice.h"
#include "storage/perfschema/pfs_global.h"
#include "storage/perfschema/pfs_lock.h"

struct PFS_global_param;
struct PFS_memory_stat_delta;
struct PFS_thread;

/**
  @addtogroup performance_schema_buffers
  @{
*/

/** Hash key for a user. */
struct PFS_user_key
{
  /**
    Hash search key.
    This has to be a string for @c LF_HASH,
    the format is @c "<username><0x00>"
  */
  char m_hash_key[USERNAME_LENGTH + 1];
  uint m_key_length;
};

/** Per user statistics. */
struct PFS_ALIGNED PFS_user : public PFS_connection_slice
{
public:
  inline void
  init_refcount(void)
  {
    m_refcount.store(1);
  }

  inline int
  get_refcount(void)
  {
    return m_refcount.load();
  }

  inline void
  inc_refcount(void)
  {
    ++m_refcount;
  }

  inline void
  dec_refcount(void)
  {
    --m_refcount;
  }

  void aggregate(bool alive);
  void aggregate_waits(void);
  void aggregate_stages(void);
  void aggregate_statements(void);
  void aggregate_transactions(void);
  void aggregate_errors(void);
  void aggregate_memory(bool alive);
  void aggregate_status(void);
  void aggregate_stats(void);
  void release(void);

  void carry_memory_stat_delta(PFS_memory_stat_delta *delta, uint index);

  /** Internal lock. */
  pfs_lock m_lock;
  PFS_user_key m_key;
  const char *m_username;
  uint m_username_length;

  ulonglong m_disconnected_count;

private:
  std::atomic<int> m_refcount;
};

int init_user(const PFS_global_param *param);
void cleanup_user(void);
int init_user_hash(const PFS_global_param *param);
void cleanup_user_hash(void);

PFS_user *find_or_create_user(PFS_thread *thread,
                              const char *username,
                              uint username_length);

PFS_user *sanitize_user(PFS_user *unsafe);
void purge_all_user(void);

/* For show status. */

extern LF_HASH user_hash;

/** @} */
#endif
