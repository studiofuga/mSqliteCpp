/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 19/06/19
 */

#include "msqlitecpp/v2/fields.h"
#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/insertstatement.h"
#include "msqlitecpp/v2/createstatement.h"


int main(int argc, char *argv[])
{

    auto db = msqlitecpp::v2::Storage::inMemory();

    db.open();

    msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fKey("key");
    msqlitecpp::v2::Column<msqlitecpp::v2::ColumnTypes::Text> fValue("value");

    auto create = msqlitecpp::v2::makeCreateStatement(db, "sample", fKey, fValue);
    create.execute();

    return 0;
}