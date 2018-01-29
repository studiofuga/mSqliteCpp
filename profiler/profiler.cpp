//
// Created by Federico Fuga on 26/01/18.
//

#include "sqlitetable.h"

#include <iostream>
#include <chrono>
#include <cstdio>
#include <tuple>
#include <condition_variable>

using namespace sqlite;

const FieldDef<FieldType::Integer> fldId = makeFieldDef("id", FieldType::Integer()).primaryKey();
const FieldDef<FieldType::Text> fldName = makeFieldDef("name", FieldType::Text());
const FieldDef<FieldType::Real> fldReal = makeFieldDef("value", FieldType::Real());

const int numRows = 120000;
const int numThreads = 3;

using milli = std::chrono::milliseconds;

void profile_st()
{
    std::cout << "Running Single Thread...\n";
    const std::string dbname = "st.db";

    std::remove(dbname.c_str());
    auto db = std::make_shared<SQLiteStorage>(dbname);
    db->open();

    auto table = SQLiteTable::create(db, "profiler", fldId, fldName, fldReal);

    auto start = std::chrono::high_resolution_clock::now();
    db->startTransaction();

    for (int i = 0; i < numRows; ++i) {
        table.insert(fldId.assign(i), fldName.assign("sample"), fldReal.assign(i));
    }
    db->commitTransaction();
    auto finish = std::chrono::high_resolution_clock::now();


    std::cout << "Single Thread: insert of " << numRows << " rows: "
              << std::chrono::duration_cast<milli>(finish - start).count()
              << " ms\n";
}

void insert (int numRows, int firstId, SQLiteTable &table) {
    for (int i = 0; i < numRows; ++i) {
        table.insert(fldId.assign(firstId + i), fldName.assign("sample"), fldReal.assign(i));
    }
}

void profile_mt()
{
    std::cout << "Running Multi Thread...\n";
    const std::string dbname = "mt.db";

    std::remove(dbname.c_str());
    auto db = std::make_shared<SQLiteStorage>(dbname);
    db->open();

    auto table = SQLiteTable::create(db, "profiler", fldId, fldName, fldReal);

    auto start = std::chrono::high_resolution_clock::now();
    db->startTransaction();

    std::vector<std::thread> thr;
    for (int t = 0; t < numThreads; ++t) {
        auto nt = std::thread (std::bind(insert, numRows/numThreads, t * numRows/numThreads, table));
        thr.push_back(std::move(nt));
    }

    for (int t = 0; t < numThreads; ++t) {
        if (thr[t].joinable())
            thr[t].join();
    }

    for (int t = 0; t < numThreads; ++t) {
        //delete thr[t];
    }

    db->commitTransaction();
    auto finish = std::chrono::high_resolution_clock::now();

    std::cout << "Multi Thread (" << numThreads << "): insert of " << numRows << " rows: "
              << std::chrono::duration_cast<milli>(finish - start).count()
              << " ms\n";

}

using QEntry = std::tuple<int,std::string,float>;
using Queue = std::list<QEntry>;

void insert_queued (int numRows, int firstId, Queue &queue, std::mutex &m, std::condition_variable &cv ) {
    for (int i = 0; i < numRows; ++i) {
        std::unique_lock<std::mutex> lk(m);
        queue.push_back(std::make_tuple(i + firstId, std::string{"sample"}, (float)i));
        cv.notify_one();
    }
}


void profile_qmt()
{
    std::cout << "Running Queued Multi Thread...\n";
    const std::string dbname = "qmt.db";

    std::remove(dbname.c_str());
    auto db = std::make_shared<SQLiteStorage>(dbname);
    db->open();

    auto table = SQLiteTable::create(db, "profiler", fldId, fldName, fldReal);

    auto start = std::chrono::high_resolution_clock::now();
    db->startTransaction();

    Queue queue;
    std::mutex m;
    std::condition_variable cv;

    std::thread qthr([&table, &queue, &m, &cv](){
        int n = 0;
        do {
            std::unique_lock<std::mutex> lk(m);
            while (queue.empty()) {
                cv.wait(lk);
            }
            auto x = queue.front();
            queue.pop_front();
            lk.unlock();

            table.insert(fldId.assign(std::get<0>(x)), fldName.assign(std::get<1>(x)), fldReal.assign(std::get<2>(x)));
            ++n;
        } while (n < numRows);
    });

    std::vector<std::thread> thr;
    for (int t = 0; t < numThreads; ++t) {
        int firstId = t * numRows/numThreads;
        int rd = numRows/numThreads;

        auto nt = std::thread([rd, firstId, &cv, &m, &queue]() {
            for (int i = 0; i < rd; ++i) {
                std::unique_lock<std::mutex> lk(m);
                queue.push_back(std::make_tuple(i + firstId, std::string{"sample"}, (float)i));
                cv.notify_one();
            }
        }) ;
        thr.push_back(std::move(nt));
    }

    for (int t = 0; t < numThreads; ++t) {
        if (thr[t].joinable())
            thr[t].join();
    }

    qthr.join();

    for (int t = 0; t < numThreads; ++t) {
        //delete thr[t];
    }

    db->commitTransaction();
    auto finish = std::chrono::high_resolution_clock::now();

    std::cout << "Queued Multi Thread (" << numThreads << "): insert of " << numRows << " rows: "
              << std::chrono::duration_cast<milli>(finish - start).count()
              << " ms\n";

}


int main(int argc, char *argv[])
{
    profile_st();
    profile_mt();
    profile_qmt();

    return 0;
}