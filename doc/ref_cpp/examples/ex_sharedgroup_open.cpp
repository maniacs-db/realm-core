// @@Example: ex_cpp_shared_group_open @@
// @@Fold@@
#include <cassert>
#include <tightdb.hpp>
#include <tightdb/file.hpp>

using namespace tightdb;

// Define schema for main table
TIGHTDB_TABLE_3(PeopleTable,
                  name,   String,
                  age,    Int,
                  hired,  Bool)

// @@EndFold@@
void func()
{
    // Create a new shared group in unattached state
    SharedGroup::unattached_tag tag;
    SharedGroup db(tag);
    db.open("shared_db.tightdb");

    // @@Fold@@

    // Do a write transaction
    {
        WriteTransaction trx(db);

        // Get table (creating it if it does not exist)
        PeopleTable::Ref employees = trx.get_table<PeopleTable>("employees");

        // Add initial rows (with sub-tables)
        if (employees->is_empty()) {
            employees->add("joe", 42, false);
            employees->add("jessica", 22, true);
        }

        trx.commit();
    }

    // Verify changes in read-only transaction
    {
        ReadTransaction trx(db);
        PeopleTable::ConstRef employees = trx.get_table<PeopleTable>("employees");

        // Do a query
        PeopleTable::Query q = employees->where().hired.equal(true);
        PeopleTable::View view = q.find_all();

        // Verify result
        assert(view.size() == 1 && view[0].name == "jessica");
    }
// @@EndFold@@
}
// @@Fold@@

int main()
{
    func();
    File::remove("shared_db.tightdb");
}
// @@EndFold@@
// @@EndExample@@