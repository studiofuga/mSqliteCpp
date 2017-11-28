//
// Created by Federico Fuga on 28/11/17.
//

#include "sqlitetransaction.h"

#include "sqlitestorage.h"

using namespace sqlite;

SQLiteTransaction::SQLiteTransaction(std::shared_ptr<SQLiteStorage> db, DestructorAction action)
        : mDb(db),
          mDestructorAction(action)
{
    auto d = db;
    if (db->startTransaction()) {
        mStatus = TransactionStatus::Started;
    }
}

SQLiteTransaction::~SQLiteTransaction()
{
    auto d = mDb.lock();
    if (mStatus == TransactionStatus::Started && d != nullptr) {
        if (mDestructorAction == DestructorAction::Commit)
            d->commitTransaction();
        else
            d->abortTransaction();
    }
}

void SQLiteTransaction::abort()
{
    auto d = mDb.lock();
    if (mStatus == TransactionStatus::Started && d != nullptr) {
        d->abortTransaction();
        mStatus = TransactionStatus::Aborted;
    }
}

void SQLiteTransaction::commit()
{
    auto d = mDb.lock();
    if (mStatus == TransactionStatus::Started && d != nullptr) {
        d->commitTransaction();
        mStatus = TransactionStatus::Committed;
    }
}
