// Find the decimal in the haystack using query operators!

col = db.decimal_find_query;
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
    { 'decimal' : NumberDecimal("-Infinity") },
]), "Initial insertion failed");

assert.eq(col.find({ $eq : { 'decimal' : NumberDecimal('1') }}).count(), '2');
assert.eq(col.find({ $lt : { 'decimal': NumberDecimal('1.00000000000001')}}).count(),
                   4);
assert.eq(col.find({ $gt : { 'decimal': NumberDecimal('1.5')}}).count(), 3);

assert.eq(col.find({ $gte : { 'decimal' : NumberDecimal('2.000') }}).count(), 3);
assert.eq(col.find({ $lte : { 'decimal' : NumberDecimal('0.9999999999999999')}}).count(),
                   4);

assert.eq(
    col.find({ $nin : { 'decimal': [NumberDecimal('Infinity'),
                                    NumberDecimal('-Infinity')]}}).count(), 9,
    "Not enough infinity");


assert.eq.automsg("col.find({ $mod : { 'decimal': '2' })", "5");
