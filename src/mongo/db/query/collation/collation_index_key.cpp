/**
 *    Copyright (C) 2016 MongoDB Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the GNU Affero General Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/platform/basic.h"

#include "mongo/db/query/collation/collation_index_key.h"

#include <stack>

#include "mongo/base/disallow_copying.h"
#include "mongo/bson/bsonobj.h"
#include "mongo/bson/bsonobjbuilder.h"
#include "mongo/db/query/collation/collator_interface.h"
#include "mongo/stdx/memory.h"
#include "mongo/util/assert_util.h"

namespace mongo {

namespace {
template <typename Appendable>
inline void append(StringData fieldName, const Appendable& append, BSONObjBuilder* out) {
    out->append(fieldName, append);
}

template <typename Appendable>
inline void append(StringData fieldName, const Appendable& append, BSONArrayBuilder* out) {
    out->append(append);
}

inline void appendAs(StringData fieldName, BSONElement append, BSONObjBuilder* out) {
    out->appendAs(append, fieldName);
}

inline void appendAs(StringData fieldName, BSONElement append, BSONArrayBuilder* out) {
    out->append(append);
}

inline BufBuilder& subobjStart(StringData fieldName, BSONObjBuilder* bob) {
    return bob->subobjStart(fieldName);
}

inline BufBuilder& subobjStart(StringData fieldName, BSONArrayBuilder* bab) {
    return bab->subobjStart();
}

inline BufBuilder& subarrayStart(StringData fieldName, BSONObjBuilder* bob) {
    return bob->subarrayStart(fieldName);
}

inline BufBuilder& subarrayStart(StringData fieldName, BSONArrayBuilder* bab) {
    return bab->subarrayStart();
}

struct TranslateContext {
    MONGO_DISALLOW_COPYING(TranslateContext);

    TranslateContext(BSONObj iter, BSONObjBuilder* bob)
        : objIter(iter), bob(bob), type(OBJ_CTX), isSubBuilder(false) {}
    TranslateContext(BSONObj iter, std::unique_ptr<BSONObjBuilder>&& bob)
        : objIter(iter), bob(std::move(bob)), type(OBJ_CTX) {}
    TranslateContext(std::vector<BSONElement> vec, std::unique_ptr<BSONArrayBuilder>&& bab)
        : arrIter(vec), bab(std::move(bab)), type(ARRAY_CTX) {}

    ~TranslateContext() {
        if (isSubBuilder) {
            if (isArray()) {
                bab.release();
            } else {
                bob.release();
            }
        } else {
            if (isArray()) {
                bab->doneFast();
            } else {
                bob->doneFast();
            }
        }
    }

    union {
        BSONObj objIter;
        std::vector<BSONElement> arrIter;
    };

    union {
        std::unique_ptr<BSONObjBuilder> bob;
        std::unique_ptr<BSONArrayBuilder> bab;
    };

    enum { ARRAY_CTX, OBJ_CTX } type;
    bool isSubBuilder = true;

    bool isArray() const {
        return type == ARRAY_CTX;
    }
};

using TranslateStack = std::stack<TranslateContext>;

template <typename Builder, typename Iterator>
void _translate(Iterator& iterator,
                const CollatorInterface* collator,
                Builder* out,
                TranslateStack* ctxStack) {
    for (const BSONElement& element : iterator) {
        switch (element.type()) {
            case BSONType::String: {
                append(element.fieldNameStringData(),
                       collator->getComparisonKey(element.valueStringData()).getKeyData(),
                       out);
            }
            case BSONType::Object: {
                auto bob = stdx::make_unique<BSONObjBuilder>(
                    subobjStart(element.fieldNameStringData(), out));
                ctxStack->emplace(element.embeddedObject(), std::move(bob));
                return;
            }
            case BSONType::Array: {
                auto bab = stdx::make_unique<BSONArrayBuilder>(
                    subarrayStart(element.fieldNameStringData(), out));
                ctxStack->emplace(element.Array(), std::move(bab));
                return;
            }
            default:
                out->append(element);
        }
    }

    ctxStack.pop();
}
}

void CollationIndexKey::translate(BSONObj obj,
                                  const CollatorInterface* collator,
                                  BSONObjBuilder* out) {
    invariant(collator);

    TranslateStack ctxStack;
    ctxStack.emplace(obj, out);

    while (!ctxStack.empty()) {
        TranslateContext& ctx = ctxStack.top();

        if (ctx.isArray()) {
            _translate(ctx.arrIter, collator, ctx.bab.get(), &ctxStack);
        } else {
            _translate(ctx.objIter, collator, ctx.bob.get(), &ctxStack);
        }
    }
}

// TODO SERVER-24674: We may want to check that objects and arrays actually do contain strings
// before returning true.
bool CollationIndexKey::shouldUseCollationIndexKey(BSONElement elt,
                                                   const CollatorInterface* collator) {
    return collator && isCollatableType(elt.type());
}

void CollationIndexKey::collationAwareIndexKeyAppend(BSONElement elt,
                                                     const CollatorInterface* collator,
                                                     BSONObjBuilder* out) {
    invariant(out);
    if (!collator) {
        out->appendAs(elt, "");
        return;
    }

    translate(elt.wrap(""), collator, out);
}

}  // namespace mongo
