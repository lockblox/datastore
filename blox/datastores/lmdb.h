#pragma once
#include <lmdb.h>
#include <blox/datastore.h>
#include <filesystem>

namespace blox::datastores {

class lmdb final : public datastore {
 public:
  class configuration {
   public:
    explicit configuration(std::filesystem::path path, unsigned int flags = 0,
                           mdb_mode_t mode = 0644);
    const std::filesystem::path& path() const;
    unsigned int flags() const;
    mdb_mode_t mode() const;

   private:
    std::filesystem::path path_;
    unsigned int flags_;
    mdb_mode_t mode_;
  };

  explicit lmdb(const configuration& config);
  ~lmdb() final = default;
  iterator begin() const override;
  iterator end() const override;
  iterator insert(const_iterator iterator, const value_type& value) override;
  iterator find(key_type key) const override;
  iterator erase(iterator pos) override;

 private:

  class buffer {
   public:
    /** Creates an empty buffer */
    buffer();

    /** Creates a buffer from reference data */
    buffer(const std::string_view& data);

    operator std::string_view();

    /** Converts to the underlying LMDB datatype */
    operator MDB_val*();

   private:
    MDB_val data_;
  };

  class environment {
   public:
    environment(const std::filesystem::path& directory);
    ~environment();

    operator MDB_env*() const;

    bool operator==(const environment& rhs);

   private:
    MDB_env* env_;
  };


  /** Encapsulates an LMDB transaction.
   *
   * A transaction may span multiple databases.
   */
  class transaction {
   public:
    transaction();
    explicit transaction(const environment& env, bool readonly=true);
    ~transaction();
    transaction(transaction&&) = delete;
    transaction(transaction&) = delete;
    transaction& operator=(transaction&&) = delete;
    transaction& operator=(transaction&) = delete;

    void abort();
    void commit();

    operator MDB_txn*();

   private:
    MDB_txn* txn_;
  };

  /** Encapsulates an LMDB database.
   *
   * Databases should only be opened once, by the first transaction in
   * the process. After the first transaction completes, the database handles
   * can freely be used by all subsequent transactions.
   */
  class database {
   public:
    database();
    explicit database(const environment& env,
                      std::string_view name = std::string_view());

    const datastores::lmdb::environment& environment() const;

    database(const database&) = default;
    database(database&&) = default;
    database& operator=(const database&) = default;
    database& operator=(database&&) = default;

    operator MDB_dbi() const;

    bool operator==(const database& rhs);

   private:
    const datastores::lmdb::environment* env_;
    MDB_dbi dbi_;
  };

  class cursor : public datastore::cursor {
   public:
    using value_type = datastore::value_type;
    using key_type = datastore::key_type;
    using mapped_type = datastore::mapped_type;

    /** Creates a sentinel cursor */
    cursor();

    /** Creates a cursor */
    explicit cursor(database db, std::shared_ptr<transaction> txn);

    /** Seeks to the given key */
    void seek(const key_type& key);

    /** Seeks to the first key */
    void first();

    /** Sets the mapped value at the current position */
    void set(const mapped_type& value);

    /** Inserts the given value at the given key */
    void put(const value_type& value);

    /** Erases the value at the given key */
    void erase();

    /** Returns the key at the current position */
    key_type key() const override;

    /** Returns the value at the current position */
    mapped_type value() const override;

    /** Compares with another cursor */
    bool equal(const datastore::cursor& rhs) const override;

    void increment() override;
    void decrement() override;

    std::unique_ptr<datastore::cursor> clone() const override;

   private:
    database database_;
    std::shared_ptr<transaction> transaction_;
    MDB_cursor* cursor_;
  };

  environment env_;
  database db_;
};

}  // namespace blox::datastores
