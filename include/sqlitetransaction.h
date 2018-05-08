//
// Created by Federico Fuga on 28/11/17.
//

#ifndef SQLITE_SQLITETRANSACTION_H
#define SQLITE_SQLITETRANSACTION_H

#include "msqlitecpp.h"

#include <memory>

namespace sqlite {

class SQLiteStorage;

/** @brief a RAII class to manage transactions
 *
 * This class allow the automatic release of a transaction using a RAII pattern.
 * When instantiated, an object of this class starts a transaction on the linked database,
 * and it releases the transaction (either committing or aborting) when it is explicitly released,
 * using commit() or abort(), or implicitly on destruction.
 * If the transaction is released implicitly, a default action is executed, based on the action passed to the
 * constructor.
 * In this library, nested transactions are not supported. @sa SQLiteStorage::startTransaction
 * So if two SQLiteTransaction objects are instantiated on the same database, only the first will be executed.
 * This also means that if the first transaction is aborted, all the inner transactions will be lost too. same if
 * the first transaction is committed and the inner transactions are aborted. No Exception is thrown !
 */
class EXPORT SQLiteTransaction {
public:
    enum class DestructorAction {
        Commit, Abort
    };

private:
    std::weak_ptr<SQLiteStorage> mDb;

    enum class TransactionStatus {
        Uninitialized, Started, Aborted, Committed
    };
    TransactionStatus mStatus = TransactionStatus::Uninitialized;
    DestructorAction mDestructorAction;
public:
    /// @brief constructs a transaction object with the passed db and the implicit action
    /// @arg db an open database
    /// @arg action the action to execute on destruction, either Commit or Abort (default)
    explicit SQLiteTransaction(std::shared_ptr<SQLiteStorage> db, DestructorAction action = DestructorAction::Abort);

    /// @brief destructor. It executes the default action.
    /// @throws SQLiteException if the transaction release/commit fails.
    ~SQLiteTransaction();

    /// @brief abort the current transaction
    void abort();

    /// @brief commits the current transaction
    void commit();
};

} // ns sqlite

#endif //SQLITE_SQLITETRANSACTION_H
