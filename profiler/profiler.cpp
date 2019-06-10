//
// Created by Federico Fuga on 26/01/18.
//

#include "msqlitecpp/v1/sqlitetable.h"

#include <iostream>
#include <chrono>
#include <cstdio>
#include <tuple>
#include <condition_variable>
#include <functional>

using namespace sqlite;

FieldDef<FieldType::Integer> fldId = makeFieldDef("id", FieldType::Integer()).primaryKey();
FieldDef<FieldType::Text> fldName = makeFieldDef("name", FieldType::Text());
FieldDef<FieldType::Real> fldReal = makeFieldDef("value", FieldType::Real());

const int numRows = 120000;
const int numThreads = 3;

using milli = std::chrono::milliseconds;

void profile_st(std::string dbname, bool usePreparedStatements)
{
    std::cout << "Running Single Thread " << (usePreparedStatements ? "(Prepared Statements)" : "") << "\n";

    std::remove(dbname.c_str());
    auto db = std::make_shared<SQLiteStorage>(dbname);
    db->open();

    auto table = SQLiteTable::make(db, "profiler", fldId, fldName, fldReal);

    auto start = std::chrono::high_resolution_clock::now();
    db->startTransaction();

    if (usePreparedStatements) {
        auto stmt = table.prepareInsert(std::make_tuple(fldId, fldName, fldReal));
        for (int i = 0; i < numRows; ++i) {
            table.insert(stmt, std::make_tuple(i, std::string{"sample"}, (double)i));
        }
    } else {
        for (int i = 0; i < numRows; ++i) {
            table.insert(fldId.assign(i), fldName.assign("sample"), fldReal.assign(i));
        }
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

void insert_prepared (SQLiteTable::PreparedInsert<decltype(fldId), decltype(fldName), decltype(fldReal)> stmt, int numRows, int firstId, SQLiteTable &table) {
    for (int i = 0; i < numRows; ++i) {
        table.insert(stmt, std::make_tuple(firstId + i, std::string{"sample"}, (double)i));
    }
}

void profile_mt(std::string dbname, bool usePreparedStatements)
{
    std::cout << "Running Multi Thread..." << (usePreparedStatements ? "(Prepared Statements)" : "") << "\n";

    std::remove(dbname.c_str());
    auto db = std::make_shared<SQLiteStorage>(dbname);
    db->open();

    auto table = SQLiteTable::make(db, "profiler", fldId, fldName, fldReal);

    auto start = std::chrono::high_resolution_clock::now();
    db->startTransaction();

    std::vector<std::thread> thr;
    if (usePreparedStatements) {
        for (int t = 0; t < numThreads; ++t) {
            SQLiteTable::PreparedInsert<decltype(fldId), decltype(fldName), decltype(fldReal)> stmt = table.prepareInsert(std::make_tuple(fldId, fldName, fldReal));
            auto nt = std::thread(std::bind(insert_prepared, stmt, numRows / numThreads, t * numRows / numThreads, table));
            thr.push_back(std::move(nt));
        }
    } else {
        for (int t = 0; t < numThreads; ++t) {
            auto nt = std::thread(std::bind(insert, numRows / numThreads, t * numRows / numThreads, table));
            thr.push_back(std::move(nt));
        }
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

using QEntry = std::tuple<int,std::string,double>;
using Queue = std::list<QEntry>;

void insert_queued (int numRows, int firstId, Queue &queue, std::mutex &m, std::condition_variable &cv ) {
    for (int i = 0; i < numRows; ++i) {
        std::unique_lock<std::mutex> lk(m);
        queue.push_back(std::make_tuple(i + firstId, std::string{"sample"}, (float)i));
        cv.notify_one();
    }
}


void profile_qmt(std::string dbname, bool usePreparedStatements)
{
    std::cout << "Running Queued Multi Thread... " << (usePreparedStatements ? "(Prepared Statements)" : "") << "\n";

    std::remove(dbname.c_str());
    auto db = std::make_shared<SQLiteStorage>(dbname);
    db->open();

    auto table = SQLiteTable::make(db, "profiler", fldId, fldName, fldReal);

    auto start = std::chrono::high_resolution_clock::now();
    db->startTransaction();

    Queue queue;
    std::mutex m;
    std::condition_variable cv;

    std::thread qthr([&table, &queue, &m, &cv, usePreparedStatements](){
        int n = 0;

        SQLiteTable::PreparedInsert<decltype(fldId), decltype(fldName), decltype(fldReal)> stmt = table.prepareInsert(std::make_tuple(fldId, fldName, fldReal));

        do {
            std::unique_lock<std::mutex> lk(m);
            while (queue.empty()) {
                cv.wait(lk);
            }
            auto x = queue.front();
            queue.pop_front();
            lk.unlock();

            if (usePreparedStatements) {
                table.insert(stmt, std::make_tuple(std::get<0>(x), std::get<1>(x), std::get<2>(x)));
            } else {
                table.insert(fldId.assign(std::get<0>(x)), fldName.assign(std::get<1>(x)),
                             fldReal.assign(std::get<2>(x)));
            }
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
    profile_st("st.db",false);
	try {
		profile_st("st_pr.db", true);
	}
	catch (SQLiteException &x) {
		std::cerr << "Failed profile_st(true): " << x.what() << "\n";
	}

	profile_mt("mt.db", false);
    profile_mt("mt_pr.db", true);
    profile_qmt("q.db", false);
    profile_qmt("q_ptr.db", true);

    return 0;
}