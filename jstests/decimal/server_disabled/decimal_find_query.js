// Find the decimal using query operators

(function () {
    "use strict";
    var col = db.decimal_find_query;
    col.drop();

    // Insert some sample data.

    assert.writeError(col.insert([
        { 'decimal': NumberDecimal('0') },
        { 'decimal': NumberDecimal('0.00') },
        { 'decimal' : NumberDecimal('-0') },
        { 'decimal' : NumberDecimal("1.0") },
        { 'decimal' : NumberDecimal("1.00") },
        { 'decimal' : NumberDecimal("2.00") },
        { 'decimal' : NumberDecimal('1234567890123456789012.12345678901234') },
        { 'decimal' : NumberDecimal("NaN") },
        { 'decimal' : NumberDecimal("-NaN") },
        { 'decimal' : NumberDecimal("Infinity") },
        { 'decimal' : NumberDecimal("-Infinity") },
    ]), "Decimal insertion succeeded with decimal server support off.");

    // TODO: Fix these cases when we can successfully verify that a find fails.
    // currently there is no good assertion to call because the error bubbles
    // up from v8_db.cpp
    
    // assert.writeError(col.find({ 'decimal' : { $eq: NumberDecimal('1') }}).count(),
    //                    "Complex decimal query succeeded with decimal server support off.");
    // assert.writeError(col.find({ 'decimal': { $lt: NumberDecimal('1.00000000000001') }}).count(),
    //                    "Complex decimal query succeeded with decimal server support off.");
    // assert.writeError(col.find({ 'decimal': { $gt: NumberDecimal('1.5')}}).count(),
    //                    "Complex decimal query succeeded with decimal server support off.");

    // assert.writeError(col.find({ 'decimal' : { $gte: NumberDecimal('2.000') }}).count(),
    //                    "Complex decimal query succeeded with decimal server support off.");
    // assert.writeError(col.find({ 'decimal' : { $lte : NumberDecimal('0.9999999999999999')}}).count(),
    //                    "Complex decimal query succeeded with decimal server support off.");

    // assert.writeError(
    //     col.find({ 'decimal': { $nin: [NumberDecimal('Infinity'),
    //                                    NumberDecimal('-Infinity')]}}).count(),
    //     "Complex decimal query succeeded with decimal server support off.");

    // TODO test $mod after repair for number long.
}());
