// Tests finding NumberDecimal from the shell.

col = db.decimal_find_basic;
col.drop();

// Insert some sample data.

assert.writeOK(col.insert([
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
]), "Initial insertion failed");

// Zeros

assert.eq.automsg("col.find({ 'decimal' : NumberDecimal('0') }).count()", "3");

// NaNs


assert.eq(col.find({ 'decimal' : NumberDecimal('NaN') }).count(), 2, 'nan count wrong.');

var theNaNs = col.find({ 'decimal': NumberDecimal('NaN')});
assert(bsonWoCompare(theNaNs[0].decimal, NumberDecimal('NaN')) == 0, 'the nan is not nan.');
assert(bsonWoCompare(theNaNs[1].decimal, NumberDecimal('NaN')) == 0, 'the nan is not nan.');

// Infinity

assert.writeOK(col.insert({ 'decimal' : NumberDecimal('-Infinity')}), 'inf write faied.');
assert.eq(col.find({ 'decimal' : NumberDecimal('Infinity') }).count(), 1, 'infinity count wrong.');
assert.eq(col.find({ 'decimal' : NumberDecimal('-Infinity') }).count(), 1, 'infinity count wrong.');

// Maximum Precision

assert.eq(
    col.find({ 'decimal' : NumberDecimal('12345678901234567890.12345678901234') }).count(), 1,
    'precision missing');
