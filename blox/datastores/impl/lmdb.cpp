#include "lmdb.h"
#include <iostream>
#include <map>
#include <utility>

namespace {
void call(int status) {
  std::string message = mdb_strerror(status);
  switch (status) {
    case MDB_SUCCESS:
      break;
    case EINVAL:
      throw std::system_error(std::make_error_code(std::errc::invalid_argument),
                              message);
    case ENOSPC:
      throw std::system_error(
          std::make_error_code(std::errc::no_space_on_device), message);
    case EIO:
      throw std::system_error(std::make_error_code(std::errc::io_error),
                              message);
    case ENOMEM:
      throw std::system_error(
          std::make_error_code(std::errc::not_enough_memory), message);
    case ENOENT:
      throw std::system_error(
          std::make_error_code(std::errc::no_such_file_or_directory), message);
    case EACCES:
      throw std::system_error(
          std::make_error_code(std::errc::permission_denied), message);
    case MDB_KEYEXIST:
      throw std::runtime_error(message);
    case MDB_NOTFOUND:
      throw std::out_of_range(message);
    case MDB_PAGE_NOTFOUND:
      throw std::runtime_error(message);
    case MDB_CORRUPTED:
      throw std::runtime_error(message);
    case MDB_PANIC:
      throw std::runtime_error(message);
    case MDB_VERSION_MISMATCH:
      throw std::runtime_error(message);
    case MDB_INVALID:
      throw std::runtime_error(message);
    case MDB_MAP_FULL:
      throw std::length_error(message);
    case MDB_DBS_FULL:
      throw std::length_error(message);
    case MDB_READERS_FULL:
      throw std::length_error(message);
    case MDB_TLS_FULL:
      throw std::length_error(message);
    case MDB_PAGE_FULL:
      throw std::length_error(message);
    case MDB_MAP_RESIZED:
      throw std::length_error(message);
    case MDB_INCOMPATIBLE:
      throw std::domain_error(message);
    case MDB_BAD_RSLOT:
      throw std::invalid_argument(message);
    case MDB_BAD_TXN:
      throw std::invalid_argument(message);
    case MDB_BAD_VALSIZE:
      throw std::invalid_argument(message);
    case MDB_BAD_DBI:
      throw std::invalid_argument(message);
    default:
      throw std::runtime_error(message);
  }
}
}  // namespace

namespace blox::datastores::impl {

/** lmdb **********************************************************/

lmdb::lmdb(const lmdb_configuration& config) : env_(config.path()), db_(env_) {
  // TODO check if the file exists, if not pass MDB_CREATE as a flag
}

std::unique_ptr<datastore::cursor> lmdb::first() const {
  auto txn = std::make_shared<transaction>(env_);
  auto result = std::make_unique<cursor>(db_, txn);
  try {
    result->first();
    return result;
  } catch (std::out_of_range&) {
    return last();
  }
}

std::unique_ptr<datastore::cursor> lmdb::last() const {
  return std::make_unique<lmdb::cursor>();
}

std::unique_ptr<datastore::cursor> lmdb::insert(
    std::unique_ptr<datastore::cursor> pos,
    const datastore::value_type& value) {
  {
    auto cursor = lmdb::cursor{db_, std::make_shared<transaction>(env_, false)};
    cursor.put(value);
    cursor.transaction()->commit();
  }
  auto cursor = dynamic_cast<lmdb::cursor*>(pos.get());
  if (!cursor || *cursor == lmdb::cursor::default_instance()) {
    pos = std::make_unique<lmdb::cursor>(db_);
    cursor = dynamic_cast<lmdb::cursor*>(pos.get());
  }
  cursor->seek(value.first);
  return pos;
}

std::unique_ptr<datastore::cursor> lmdb::lookup(datastore::key_type key) const {
  try {
    auto result = std::make_unique<cursor>(db_);
    result->seek(key);
    return result;
  } catch (std::out_of_range&) {
    return last();
  } catch (std::invalid_argument&) {
    return last();
  }
}

std::unique_ptr<datastore::cursor> lmdb::erase(
    std::unique_ptr<datastore::cursor> pos) {
  auto key = pos->key();
  try {
    pos->increment();
  } catch (std::out_of_range&) {
    pos = last();
  }
  {
    lmdb::cursor cur{db_, std::make_shared<transaction>(env_, false)};
    cur.seek(key);
    cur.erase();
    cur.transaction()->commit();
  }
  return pos;
}

datastore::size_type lmdb::capacity() const {
  MDB_envinfo envinfo;
  call(mdb_env_info(env_, &envinfo));
  return envinfo.me_mapsize;
}

datastore::size_type lmdb::size() const {
  MDB_stat stat;
  call(mdb_env_stat(env_, &stat));
  return stat.ms_entries;
}

/** lmdb::environment *********************************************/

lmdb::environment::environment(const std::filesystem::path& directory)
    : env_(nullptr) {
  // TODO ensure that each environment directory is only opened once
  call(mdb_env_create(&env_));
  // Allow only a single database
  call(mdb_env_set_maxdbs(env_, 1));
  // TODO validate that the path is a directory
  auto path = std::string(directory.native().begin(), directory.native().end());
  unsigned int flags = MDB_NOTLS;
  unsigned int mode = 0;
  call(mdb_env_open(env_, path.c_str(), flags, mode));
}

lmdb::environment::~environment() { mdb_env_close(env_); }

lmdb::environment::operator MDB_env*() const { return env_; }

bool lmdb::environment::operator==(const lmdb::environment& rhs) {
  return env_ == rhs.env_;
}

/** lmdb::transaction *********************************************/

lmdb::transaction::transaction(const lmdb::environment& env, bool readonly)
    : txn_(nullptr), readonly_(readonly) {
  transaction parent;
  unsigned int flags = readonly ? MDB_RDONLY : 0;
  call(
      mdb_txn_begin(const_cast<lmdb::environment&>(env), parent, flags, &txn_));
}

lmdb::transaction::~transaction() {
  if (txn_ != nullptr) {
    abort();
  }
}

void lmdb::transaction::abort() {
  mdb_txn_abort(txn_);
  txn_ = nullptr;
  aborted_ = true;
}

void lmdb::transaction::commit() {
  if (txn_) {
    call(mdb_txn_commit(txn_));
    txn_ = nullptr;
    committed_ = true;
  }
}

lmdb::transaction::operator MDB_txn*() { return txn_; }

bool lmdb::transaction::readonly() const { return readonly_; }

/** lmdb::database ************************************************/

lmdb::database::database() : env_(nullptr), dbi_(0) {}

lmdb::database::database(const lmdb::environment& env, std::string_view name)
    : env_(&env), dbi_(0) {
  unsigned int flags = 0;
  transaction txn(env);
  auto path = name.empty() ? nullptr : name.data();
  call(mdb_dbi_open(txn, path, flags, &dbi_));
  txn.commit();
}

const lmdb::environment& lmdb::database::environment() const { return *env_; }

lmdb::database::operator MDB_dbi() const { return dbi_; }

bool lmdb::database::operator==(const lmdb::database& rhs) {
  return dbi_ == rhs.dbi_;
}

/** lmdb::buffer **************************************************/

lmdb::buffer::buffer() : data_{0, nullptr} {}

lmdb::buffer::buffer(const std::string_view& data)
    : data_{data.size(), (void*)(data.data())} {}

lmdb::buffer::operator MDB_val*() { return &data_; }

lmdb::buffer::operator std::string_view() const {
  return std::string_view(static_cast<const char*>(data_.mv_data),
                          data_.mv_size);
}

/** lmdb::cursor **************************************************/

lmdb::cursor::cursor(lmdb::database db, std::shared_ptr<lmdb::transaction> txn)
    : database_(db), transaction_(std::move(txn)), cursor_(nullptr) {
  if (transaction_) {
    call(mdb_cursor_open(*transaction_, db, &cursor_));
  }
}

lmdb::cursor::cursor(lmdb::database db)
    : cursor(db, std::make_shared<lmdb::transaction>(db.environment())) {}

lmdb::cursor::cursor(lmdb::cursor&& rhs) noexcept
    : database_(rhs.database_),
      transaction_(std::move(rhs.transaction_)),
      cursor_(rhs.cursor_) {
  rhs.database_ = lmdb::database();
  rhs.cursor_ = nullptr;
}

lmdb::cursor& lmdb::cursor::operator=(lmdb::cursor&& rhs) noexcept {
  close();
  database_ = rhs.database_;
  transaction_ = std::move(rhs.transaction_);
  cursor_ = rhs.cursor_;

  rhs.database_ = lmdb::database();
  rhs.transaction_ = nullptr;
  rhs.cursor_ = nullptr;
  return *this;
}

void lmdb::cursor::seek(const key_type& key) {
  key_ = key;
  call(mdb_cursor_get(cursor_, key_, value_, MDB_SET));
}

void lmdb::cursor::set(const mapped_type& value) {
  value_ = value;
  unsigned int flags = MDB_CURRENT;
  call(mdb_cursor_put(cursor_, key_, value_, flags));
}

void lmdb::cursor::put(const value_type& value) {
  unsigned int flags = 0;
  key_ = value.first;
  value_ = value.second;
  call(mdb_cursor_put(cursor_, key_, value_, flags));
}

std::string_view lmdb::cursor::key() const { return key_; }

std::string_view lmdb::cursor::value() const { return value_; }

bool lmdb::cursor::equal(const datastore::cursor& rhs) const {
  try {
    const auto& cursor = dynamic_cast<const lmdb::cursor&>(rhs);
    return database_ == cursor.database_ &&
           transaction_ == cursor.transaction_ && cursor_ == cursor.cursor_;
  } catch (std::bad_cast&) {
    return false;
  }
}

void lmdb::cursor::increment() {
  try {
    call(mdb_cursor_get(cursor_, key_, value_, MDB_NEXT));
  } catch (std::out_of_range&) {
    *this = cursor();
  }
}

void lmdb::cursor::decrement() {
  try {
    call(mdb_cursor_get(cursor_, key_, value_, MDB_PREV));
  } catch (std::out_of_range&) {
    *this = cursor();
  }
}

std::unique_ptr<datastore::cursor> lmdb::cursor::clone() const {
  auto result = std::make_unique<lmdb::cursor>(database_, transaction_);
  if (database_ && transaction_ && key() != buffer()) {
    result->seek(key());
  }
  return result;
}

void lmdb::cursor::erase() {
  unsigned int flags = 0;
  call(mdb_cursor_del(cursor_, flags));
}

void lmdb::cursor::first() {
  call(mdb_cursor_get(cursor_, key_, value_, MDB_FIRST));
}

void lmdb::cursor::close() {
  if (cursor_ && transaction_) {
    if (transaction_->readonly()) {
      // lmdb automatically closes cursors on write transactions
      mdb_cursor_close(cursor_);
      cursor_ = nullptr;
    }
  }
}

lmdb::cursor::~cursor() { close(); }

const std::shared_ptr<lmdb::transaction>& lmdb::cursor::transaction() const {
  return transaction_;
}

bool lmdb::cursor::operator==(const lmdb::cursor& rhs) const {
  return database_ == rhs.database_ && transaction_ == rhs.transaction_ &&
         cursor_ == rhs.cursor_;
}

bool lmdb::cursor::operator!=(const lmdb::cursor& rhs) const {
  return !(*this == rhs);
}

const lmdb::cursor& lmdb::cursor::default_instance() {
  static lmdb::cursor result;
  return result;
}

}  // namespace blox::datastores::impl
