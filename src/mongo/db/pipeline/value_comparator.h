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

#pragma once

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "mongo/base/string_data.h"
#include "mongo/db/pipeline/value.h"

namespace mongo {

class ValueComparator {
public:
    /**
     * Functor compatible for use with unordered STL containers.
     */
    class EqualTo {
    public:
        explicit EqualTo(const ValueComparator* comparator) : _comparator(comparator) {}

        bool operator()(const Value& lhs, const Value& rhs) const {
            return _comparator->compare(lhs, rhs) == 0;
        }

    private:
        const ValueComparator* _comparator;
    };

    /**
     * Functor compatible for use with ordered STL containers.
     */
    class LessThan {
    public:
        explicit LessThan(const ValueComparator* comparator) : _comparator(comparator) {}

        bool operator()(const Value& lhs, const Value& rhs) const {
            return _comparator->compare(lhs, rhs) < 0;
        }

    private:
        const ValueComparator* _comparator;
    };

    /**
     * Functor for computing the hash of a Value, compatible for use with unordered STL containers.
     */
    class Hasher {
    public:
        explicit Hasher(const ValueComparator* comparator) : _comparator(comparator) {}

        size_t operator()(const Value& val) const {
            return _comparator->hash(val);
        }

    private:
        const ValueComparator* _comparator;
    };

    /**
     * Constructs a value comparator with simple comparison semantics.
     */
    ValueComparator() = default;

    /**
     * Constructs a value comparator with special string comparison semantics.
     */
    ValueComparator(const StringData::ComparatorInterface* stringComparator)
        : _stringComparator(stringComparator) {}

    /**
     * Returns <0 if 'lhs' is less than 'rhs', 0 if 'lhs' is equal to 'rhs', and >0 if 'lhs' is
     * greater than 'rhs'.
     */
    int compare(const Value& lhs, const Value& rhs) const {
        return Value::compare(lhs, rhs, _stringComparator);
    }

    /**
     * Computes a hash of 'val' since that Values which compare equal under this comparator also
     * have equal hashes.
     */
    size_t hash(const Value& val) const {
        size_t seed = 0xf0afbeef;
        val.hash_combine(seed, _stringComparator);
        return seed;
    }

    /**
     * Evaluates a deferred comparison object that was generated by invoking one of the comparison
     * operators on the Value class.
     */
    bool evaluate(Value::DeferredComparison deferredComparison) const;

    /**
     * Returns a function object which computes whether one Value is equal to another under this
     * comparator. This comparator must outlive the returned function object.
     */
    EqualTo getEqualTo() const {
        return EqualTo(this);
    }

    /**
     * Returns a function object which computes whether one Value is less than another under this
     * comparator. This comparator must outlive the returned function object.
     */
    LessThan getLessThan() const {
        return LessThan(this);
    }

    /**
     * Returns a function object which computes the hash of a Value such that equal Values under
     * this comparator have equal hashes.
     */
    Hasher getHasher() const {
        return Hasher(this);
    }

    /**
     * Construct an empty ordered set of Value whose ordering and equivalence classes are given by
     * this comparator. This comparator must outlive the returned set.
     */
    std::set<Value, LessThan> makeOrderedValueSet() const {
        return std::set<Value, LessThan>(LessThan(this));
    }

    /**
     * Construct an empty unordered set of Value whose equivalence classes are given by this
     * comparator. This comparator must outlive the returned set.
     */
    std::unordered_set<Value, Hasher, EqualTo> makeUnorderedValueSet() const {
        return std::unordered_set<Value, Hasher, EqualTo>(0, Hasher(this), EqualTo(this));
    }

    /**
     * Construct an empty ordered map from Value to type T whose ordering and equivalence classes
     * are given by this comparator. This comparator must outlive the returned set.
     */
    template <typename T>
    std::map<Value, T, LessThan> makeOrderedValueMap() const {
        return std::map<Value, T, LessThan>(LessThan(this));
    }

    /**
     * Construct an empty unordered map from Value to type T whose equivalence classes are given by
     * this comparator. This comparator must outlive the returned set.
     */
    template <typename T>
    std::unordered_map<Value, T, Hasher, EqualTo> makeUnorderedValueMap() const {
        return std::unordered_map<Value, T, Hasher, EqualTo>(0, Hasher(this), EqualTo(this));
    }

private:
    const StringData::ComparatorInterface* _stringComparator = nullptr;
};

//
// Type aliases for sets and maps of Value for use by clients of the Document/Value library.
//

using ValueSet = std::set<Value, ValueComparator::LessThan>;

using ValueUnorderedSet =
    std::unordered_set<Value, ValueComparator::Hasher, ValueComparator::EqualTo>;

template <typename T>
using ValueMap = std::map<Value, T, ValueComparator::LessThan>;

template <typename T>
using ValueUnorderedMap =
    std::unordered_map<Value, T, ValueComparator::Hasher, ValueComparator::EqualTo>;

}  // namespace mongo
