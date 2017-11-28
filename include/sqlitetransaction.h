//
// Created by Federico Fuga on 28/11/17.
//

#ifndef SQLITE_SQLITETRANSACTION_H
#define SQLITE_SQLITETRANSACTION_H

#include <memory>

namespace sqlite {

    class SQLiteStorage;

    class SQLiteTransaction {
    public:
        enum class DestructorAction { Commit, Abort };

    private:
        std::weak_ptr<SQLiteStorage> mDb;

        enum class TransactionStatus {
            Uninitialized, Started, Aborted, Committed
        };
        TransactionStatus mStatus = TransactionStatus::Uninitialized;
        DestructorAction mDestructorAction;
    public:
        explicit SQLiteTransaction(std::shared_ptr<SQLiteStorage> db, DestructorAction action = DestructorAction::Abort);
        ~SQLiteTransaction();

        void abort();
        void commit();
    };

} // ns sqlite

#endif //SQLITE_SQLITETRANSACTION_H
