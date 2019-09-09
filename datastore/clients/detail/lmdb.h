#pragma once
#include <datastore/clients/lmdb.h>
#include <lmdb.h>
#include <set>

namespace datastore::clients::detail {

class lmdb final : public client {
 public:
  explicit lmdb(const lmdb_configuration& config);
  ~lmdb() final = default;

  [[nodiscard]] size_type size() const override;

 protected:
  [[nodiscard]] std::unique_ptr<cursor> first() const override;
  [[nodiscard]] std::unique_ptr<cursor> last() const override;
  std::unique_ptr<client::cursor> insert_or_assign(
      std::unique_ptr<client::cursor> pos, const value_type& value) override;
  [[nodiscard]] std::unique_ptr<cursor> lookup(key_type key) const override;
  std::unique_ptr<cursor> erase(std::unique_ptr<cursor> pos) override;
  [[nodiscard]] size_type capacity() const override;

 private:
  class buffer {
   public:
    /** Creates an empty buffer */
    buffer();

    /** Creates a buffer from reference data */
    buffer(const std::string_view& data);

    operator std::string_view() const;

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
    transaction() = default;
    explicit transaction(const environment& env, bool readonly = true);
    ~transaction();
    transaction(transaction&&) = delete;
    transaction(transaction&) = delete;
    transaction& operator=(transaction&&) = delete;
    transaction& operator=(transaction&) = delete;

    void abort();
    void commit();

    [[nodiscard]] bool readonly() const;

    operator MDB_txn*();

   private:
    MDB_txn* txn_ = nullptr;
    bool readonly_ = true;
    bool committed_ = false;
    bool aborted_ = false;
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

    [[nodiscard]] const lmdb::environment& environment() const;

    database(const database&) = default;
    database(database&&) = default;
    database& operator=(const database&) = default;
    database& operator=(database&&) = default;

    operator MDB_dbi() const;

    bool operator==(const database& rhs);

   private:
    const lmdb::environment* env_;
    MDB_dbi dbi_;
  };

  class cursor final : public client::cursor {
   public:
    using value_type = client::value_type;
    using key_type = client::key_type;
    using mapped_type = client::mapped_type;

    /** Creates a sentinel cursor */
    cursor() = default;

    /** Destroys a cursor */
    ~cursor() override;

    cursor(const cursor&) = delete;
    cursor& operator=(const cursor&) = delete;
    cursor(cursor&&) noexcept;
    cursor& operator=(cursor&&) noexcept;

    /** Creates a cursor */
    explicit cursor(database db, std::shared_ptr<transaction> txn);

    /** Creates a cursor */
    explicit cursor(database db);

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
    [[nodiscard]] key_type key() const override;

    /** Returns the value at the current position */
    [[nodiscard]] mapped_type value() const override;

    /** Compares with another cursor */
    [[nodiscard]] bool equal(const client::cursor& rhs) const override;

    /** Moves the cursor forward by one position */
    void increment() override;

    /** Moves the cursor backwards by one position */
    void decrement() override;

    /** Creates a copy of the cursor */
    [[nodiscard]] std::unique_ptr<client::cursor> clone() const override;

    /** Returns the transaction for this cursor */
    [[nodiscard]] const std::shared_ptr<lmdb::transaction>& transaction() const;

    /** Closes the cursor */
    void close();

    /** Compares with another lmdb cursor */
    bool operator==(const cursor& rhs) const;

    /** Compares with another lmdb cursor */
    bool operator!=(const cursor& rhs) const;

    /** Returns a default constructed cursor */
    static const cursor& default_instance();

   private:
    void reset();
    buffer key_, value_;
    database database_;
    std::shared_ptr<lmdb::transaction> transaction_ = nullptr;
    MDB_cursor* cursor_ = nullptr;
  };

  environment env_;
  database db_;
};

}  // namespace datastore::clients::detail
