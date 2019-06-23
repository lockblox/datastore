#include <lockblox/blox/datastores/lmdb.h>
#include <cassert>
#include <iostream>
#include <map>
#include <utility>

namespace lockblox::blox {

static void call(int status) {
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
    case MDB_PROBLEM:
      throw std::runtime_error(message);
    default:
      throw std::runtime_error(message);
  }
}

/** datastores::lmdb::configuration *******************************************/

datastores::lmdb::configuration::configuration(std::filesystem::path path,
                                               unsigned int flags,
                                               mdb_mode_t mode)
    : path_(std::move(path)), flags_(flags), mode_(mode) {
  // TODO check if the file exists, if not pass MDB_CREATE as a flag
}

const std::filesystem::path& datastores::lmdb::configuration::path() const {
  return path_;
}

unsigned int datastores::lmdb::configuration::flags() const { return flags_; }

mdb_mode_t datastores::lmdb::configuration::mode() const { return mode_; }

/** datastores::lmdb **********************************************************/

datastores::lmdb::lmdb(const datastores::lmdb::configuration& config)
    : env_(config.path()), db_(env_) {}

datastore::iterator datastores::lmdb::begin() const {
  auto txn = std::make_shared<transaction>(env_);
  auto result = std::make_unique<cursor>(db_, txn);
  try {
    result->first();
    result->key();
    return datastore::iterator(std::move(result));
  } catch (std::out_of_range& e) {
    return end();
  }
}

datastore::iterator datastores::lmdb::end() const {
  auto cursor = std::make_unique<datastores::lmdb::cursor>();
  return iterator(std::move(cursor));
}

datastore::iterator datastores::lmdb::insert(
    datastore::iterator iterator, const datastore::value_type& value) {
  (void)(iterator);
  auto txn = std::make_shared<transaction>(env_, false);
  {
    auto cursor = datastores::lmdb::cursor{db_, txn};
    cursor.put(value);
    txn->commit();
  }
  txn = std::make_shared<transaction>(env_);
  auto result = std::make_unique<datastores::lmdb::cursor>(db_, txn);
  result->seek(value.first);
  return datastore::iterator(std::move(result));
}

datastore::iterator datastores::lmdb::find(datastore::key_type key) const {
  try {
    auto txn = std::make_shared<transaction>(db_.environment());
    auto result = std::make_unique<cursor>(db_, std::move(txn));
    result->seek(key);
    return datastore::iterator(std::move(result));
  } catch (std::out_of_range& e) {
    return end();
  }
}

datastore::iterator datastores::lmdb::erase(datastore::iterator pos) {
  auto txn = std::make_shared<transaction>(env_, false);
  auto cur = cursor(db_, txn);
  auto key = pos->first;
  try {
    ++pos;
  } catch (std::out_of_range& e) {
    pos = end();
  }
  cur.seek(key);
  cur.erase();
  txn->commit();
  return pos;
}

/** datastores::lmdb::environment *********************************************/

datastores::lmdb::environment::environment(
    const std::filesystem::path& directory)
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

datastores::lmdb::environment::~environment() { mdb_env_close(env_); }

datastores::lmdb::environment::operator MDB_env*() const { return env_; }
bool datastores::lmdb::environment::operator==(
    const datastores::lmdb::environment& rhs) {
  return env_ == rhs.env_;
}

/** datastores::lmdb::transaction *********************************************/

datastores::lmdb::transaction::transaction() : txn_(nullptr) {}

datastores::lmdb::transaction::transaction(
    const datastores::lmdb::environment& env, bool readonly)
    : txn_(nullptr) {
  transaction parent;
  unsigned int flags = readonly ? MDB_RDONLY : 0;
  call(mdb_txn_begin(const_cast<datastores::lmdb::environment&>(env), parent,
                     flags, &txn_));
}

datastores::lmdb::transaction::~transaction() {
  if (txn_ != nullptr) {
    abort();
  }
}

void datastores::lmdb::transaction::abort() {
  mdb_txn_abort(txn_);
  txn_ = nullptr;
}

void datastores::lmdb::transaction::commit() {
  call(mdb_txn_commit(txn_));
  txn_ = nullptr;
}

datastores::lmdb::transaction::operator MDB_txn*() { return txn_; }

/** datastores::lmdb::database ************************************************/

datastores::lmdb::database::database() : env_(nullptr), dbi_(0) {}

datastores::lmdb::database::database(const datastores::lmdb::environment& env,
                                     std::string_view name)
    : env_(&env), dbi_(0) {
  unsigned int flags = 0;
  transaction txn(env);
  auto path = name.empty() ? nullptr : name.data();
  call(mdb_dbi_open(txn, path, flags, &dbi_));
  txn.commit();
}

const datastores::lmdb::environment& datastores::lmdb::database::environment()
    const {
  return *env_;
}

datastores::lmdb::database::operator MDB_dbi() const { return dbi_; }

bool datastores::lmdb::database::operator==(
    const datastores::lmdb::database& rhs) {
  return dbi_ == rhs.dbi_;
}

/** datastores::lmdb::buffer **************************************************/

datastores::lmdb::buffer::buffer() : data_{0, nullptr} {}

datastores::lmdb::buffer::buffer(const std::string_view& data)
    : data_{data.size(), (void*)(data.data())} {}

datastores::lmdb::buffer::operator MDB_val*() { return &data_; }

datastores::lmdb::buffer::operator std::string_view() {
  return std::string_view(static_cast<const char*>(data_.mv_data),
                          data_.mv_size);
}

/** datastores::lmdb::cursor **************************************************/

datastores::lmdb::cursor::cursor() : cursor_(nullptr) {}

datastores::lmdb::cursor::cursor(
    datastores::lmdb::database db,
    std::shared_ptr<datastores::lmdb::transaction> txn)
    : database_(db), transaction_(std::move(txn)), cursor_(nullptr) {
  assert(transaction_);
  call(mdb_cursor_open(*transaction_, db, &cursor_));
}

void datastores::lmdb::cursor::seek(const key_type& key) {
  call(mdb_cursor_get(cursor_, buffer(key), buffer(), MDB_SET));
}

void datastores::lmdb::cursor::set(const mapped_type& value) {
  unsigned int flags = MDB_CURRENT;
  call(mdb_cursor_put(cursor_, buffer(key()), buffer(value), flags));
}

void datastores::lmdb::cursor::put(const value_type& value) {
  unsigned int flags = 0;
  call(mdb_cursor_put(cursor_, buffer(value.first), buffer(value.second),
                      flags));
}

std::string_view datastores::lmdb::cursor::key() const {
  buffer key, value;
  call(mdb_cursor_get(cursor_, key, value, MDB_GET_CURRENT));
  return key;
}

std::string_view datastores::lmdb::cursor::value() const {
  buffer key, value;
  call(mdb_cursor_get(cursor_, key, value, MDB_GET_CURRENT));
  return value;
}

bool datastores::lmdb::cursor::equal(const datastore::cursor& rhs) const {
  try {
    auto cursor = dynamic_cast<const datastores::lmdb::cursor&>(rhs);
    return database_ == cursor.database_ &&
           transaction_ == cursor.transaction_ && cursor_ == cursor.cursor_;
  } catch (std::bad_cast& e) {
    return false;
  }
}

void datastores::lmdb::cursor::increment() {
  buffer key, value;
  try {
    call(mdb_cursor_get(cursor_, key, value, MDB_NEXT));
  } catch (std::out_of_range& e) {
    *this = cursor();
  }
}

void datastores::lmdb::cursor::decrement() {
  buffer key, value;
  call(mdb_cursor_get(cursor_, key, value, MDB_PREV));
}

std::unique_ptr<datastore::cursor> datastores::lmdb::cursor::clone() const {
  return std::make_unique<datastores::lmdb::cursor>(*this);
}

void datastores::lmdb::cursor::erase() {
  unsigned int flags = 0;
  call(mdb_cursor_del(cursor_, flags));
}

void datastores::lmdb::cursor::first() {
  call(mdb_cursor_get(cursor_, buffer(), buffer(), MDB_FIRST));
}

}  // namespace lockblox::blox
