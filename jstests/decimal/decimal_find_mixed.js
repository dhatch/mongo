// Tests finding NumberDecimal from the shell in mixed collections.

(function () {
    "use strict";
    var col = db.decimal_find_mixed;
    col.drop();

    // Insert some sample data.

    assert.writeOK(col.insert([
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
    ]), "Initial insertion failed");

    assert.eq(col.find({ 'a' : -1 }).count(), 4, "A1");
    assert.eq(col.find({ 'a' : NumberLong(-1) }).count(), 4, "A2");
    assert.eq(col.find({ 'a' : NumberInt(-1) }).count(), 4, "A3");
    assert.eq(col.find({ 'a' : NumberDecimal("-1") }).count(), 4, "A4");

    assert.eq(col.find({ 'a': NaN }).count(), 3, "B1");
    assert.eq(col.find({ 'a': NumberDecimal("NaN") }).count(), 3, "B2");
    assert.eq(col.find({ 'a': Infinity }).count(), 2, "B3");
    assert.eq(col.find({ 'a': NumberDecimal("Infinity") }).count(), 2, "B4");

    assert.eq(col.find({ $and : [ { 'a': { $gte : 0 }}, { 'a' : { $lte: 2 }}]}).count(),
              8, "C1");
}());
