/**
 * Tests that the $graphLookup stage respects the collation when matching between the
 * 'connectFromField' and the 'connectToField'.  $graphLookup should use the collation
 * set on the aggregation, or the default collation of the collection.
 */
(function() {
    const caseInsensitive = {locale: "en", strength: 2};
    var coll = db.collation_graphLookup;

    // $graphLookup respects the collation set on the aggregation pipeline.
    coll.drop();
    coll.insert({username: "tom", friends: ["jeremy", "jimmy"]});
    coll.insert([{username: "JEREMY"}, {username: "JIMMY"}]);

    var res = coll.aggregate([{
                      $graphLookup: {
                          from: coll.getName(),
                          startWith: "$friends",
                          connectFromField: "friends",
                          connectToField: "username",
                          as: "friendUsers"
                      }
                  }], {collation: caseInsensitive})
                  .toArray();
    print(tojson(res));
})();
