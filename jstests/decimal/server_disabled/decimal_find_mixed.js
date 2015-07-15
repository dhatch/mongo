// Tests finding NumberDecimal from the shell in mixed collections when server support is disabled.

(function () {
    "use strict";
    var col = db.decimal_find_mixed;
    col.drop();

    // Insert some sample data.

    assert.writeError(col.insert([
        { 'a' : -1 },
        { 'a' : NumberDecimal("-1") },
        { 'a' : NumberLong("-1") },
        { 'a' : NumberInt("-1") },
        { 'a': NumberDecimal('0') },
        { 'a' :  0 },
        { 'a': NumberDecimal('0.00') },
        { 'a' : NumberDecimal('-0') },
        { 'a' : NumberDecimal("1.0") },
        { 'a' : NumberLong(1) },
        { 'a' : NumberDecimal("1.00") },
        { 'a' : NumberDecimal("2.00") },
        { 'a' : NumberDecimal('12345678901234567890.12345678901234') },
        { 'a' : NumberDecimal("NaN") },
        { 'a' : NumberDecimal("-NaN") },
        { 'a' : NaN },
        { 'a' : NumberDecimal("Infinity") },
        { 'a' : Infinity }
    ]), "Multiple type insertion with mixed in decimals succeeded with decimal server support off.");

    // TODO: Fix these cases when we can successfully verify that a find fails.
    // currently there is no good assertion to call because the error bubbles
    // up from v8_db.cpp
    
    // assert.writeError(col.find({ 'a' : -1 }).count(),
    //                    "Mixed query succeeded with decimal server support off.");
    // assert.writeError(col.find({ 'a' : NumberLong(-1) }).count(),
    //                    "Mixed query succeeded with decimal server support off.");
    // assert.writeError(col.find({ 'a' : NumberInt(-1) }).count(),
    //                    "Mixed query succeeded with decimal server support off.");
    // assert.writeError(col.find({ 'a' : NumberDecimal("-1") }).count(),
    //                    "Mixed query succeeded with decimal server support off.");

    // assert.writeError(col.find({ 'a': NaN }).count(),
    //                    "Mixed query succeeded with decimal server support off.");
    // assert.writeError(col.find({ 'a': NumberDecimal("NaN") }).count(),
    //                    "Mixed query succeeded with decimal server support off.");
    // assert.writeError(col.find({ 'a': Infinity }).count(),
    //                    "Mixed query succeeded with decimal server support off.");
    // assert.writeError(col.find({ 'a': NumberDecimal("Infinity") }).count(),
    //                    "Mixed query succeeded with decimal server support off.");

    // assert.writeError(col.find({ $and : [ { 'a': { $gte : 0 }}, { 'a' : { $lte: 2 }}]}).count(),
    //           "Mixed query succeeded with decimal server support off.");
}());
