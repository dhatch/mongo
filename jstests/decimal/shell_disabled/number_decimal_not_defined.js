// Tests using NumberDecimal from the shell when shell support is disabled.

(function() {
    "use strict";
    var col = db.number_decimal_defined;
    col.drop();

    // Test shell commands and assert that they all throw

    assert.throws(
        function() {
            var d = NumberDecimal("50.2");
        },
        [],
        "Decimal type was created when shell support was off.")

    assert.throws(
        function() {
            col.insert([{ 'decimal': NumberDecimal('25.2') }]);
        },
        [],
        "Decimal insertion was executed when shell support was off."
    );

    assert.throws(
        function() {
            col.update({}, { $inc: { 'a' : NumberDecimal("10") }}, { multi : true });
        },
        [],
        "Decimal update command was executed when shell support was off."
    );

    assert.throws(
        function() {
            col.find({ 'decimal' : NumberDecimal('0') }).count();
        },
        [],
        "Decimal find command was executed when shell support was off."
    );

}());
