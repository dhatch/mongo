// Test decimal updates

(function () {
    "use strict";
    var col = db.decimal_updates;
    col.drop();

    // Insert some sample data.

    assert.writeError(col.insert([
        { 'a' : NumberDecimal("1.0") },
        { 'a' : NumberDecimal("0.0") },
        { 'a' : NumberDecimal("1.00") },
        { 'a' : NumberLong("1") },
        { 'a' : 1 },
    ]), "Decimal insertion succeeded with decimal server support off.");

    // TODO: Fix these cases when we can successfully verify that a find fails.
    // currently there is no good assertion to call because the error bubbles
    // up from v8_db.cpp
    
    assert.writeError(col.update({}, { $inc: { 'a' : NumberDecimal("10") }}, { multi : true }),
                   "Decimal $inc 10 update succeeded with decimal server support off.");
    // assert.writeOK(col.find({ a : 11 }).count(), 0,
    //           "Count after $inc modified decimals with decimal server support off.");
    assert.writeError(col.update({}, { $inc: { 'a' : NumberDecimal("0") }}, { multi : true }),
                   "Decimal $inc 0 update succeeded with decimal server support off.");
    // assert.writeOK(col.find({ a : 11 }).count(), 0,
    //           "Count after $inc modified decimals with decimal server support off.");

    assert.writeError(col.update({}, { $mul: { 'a' : NumberDecimal("1") }}, { multi : true }),
                   "Decimal $mul 10 update succeeded with decimal server support off.");
    // assert.writeOK(col.find({ a : 1 }).count(), 0,
    //           "Count after $mul modified decimals with decimal server support off.");
    // assert.writeError(col.update({}, { $mul: { 'a' : NumberDecimal("0") }}, { multi : true }),
    //                "Decimal $mul 0 update succeeded with decimal server support off.");
    // assert.writeOK(col.find({ a : 0 }).count(), 0,
    //           "Count after $inc modified decimals with decimal server support off.");

    assert.writeError(col.update({}, { $bit: { 'a': { and : 1 }}}, { multi : true }),
                      "Count after $bit modified decimals with decimal server support off.");
}());
