// Tests finding NumberDecimal from the shell when server support is disabled.

(function() {
    "use strict";
    var col = db.decimal_find_basic;
    col.drop();

    // Insert some sample data.

    assert.writeError(col.insert([
        { 'decimal': NumberDecimal('0') },
        { 'decimal': NumberDecimal('0.00') },
        { 'decimal' : NumberDecimal('-0') },
        { 'decimal' : NumberDecimal("1.0") },
        { 'decimal' : NumberDecimal("1.00") },
        { 'decimal' : NumberDecimal("2.00") },
        { 'decimal' : NumberDecimal('12345678901234567890.12345678901234') },
        { 'decimal' : NumberDecimal("NaN") },
        { 'decimal' : NumberDecimal("-NaN") },
        { 'decimal' : NumberDecimal("Infinity") },
    ]), "Decimal insertion succeeded with decimal server support off.");

    // TODO: Fix these cases when we can successfully verify that a find fails.
    // currently there is no good assertion to call because the error bubbles
    // up from v8_db.cpp
    
    // // Zeros
    // assert.throws(
    //     function() {
    //         col.find({ 'decimal' : NumberDecimal('0') })
    //     },
    //     [],
    //     "Decimal query succeeded with decimal server support off."
    // );
    // // Numbers
    // assert.throws(
    //     function() {
    //         col.find({ 'decimal' : NumberDecimal('2.00') })
    //     },
    //     [],
    //     "Decimal query succeeded with decimal server support off."
    // );
    // // NaNs
    // assert.throws(
    //     function() {
    //         col.find({ 'decimal' : NumberDecimal('NaN') })
    //     },
    //     [],
    //     "Decimal query succeeded with decimal server support off."
    // );
    // // Infinity
    // assert.throws(
    //     function() {
    //         col.find({ 'decimal' : NumberDecimal('Infinity') })
    //     },
    //     [],
    //     "Decimal query succeeded with decimal server support off."
    // );

    // // Maximum Precision
    // assert.throws(
    //     function() {
    //         col.find({ 'decimal' : NumberDecimal('12345678901234567890.12345678901234') })
    //     },
    //     [],
    //     "Decimal query succeeded with decimal server support off."
    // );

}());
