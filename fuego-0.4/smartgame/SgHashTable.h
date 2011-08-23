//----------------------------------------------------------------------------
/** @file SgHashTable.h
    Hash table.
*/
//----------------------------------------------------------------------------

#ifndef SG_HASHTABLE_H
#define SG_HASHTABLE_H

#include "SgHash.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

/** Entry in a HashTable: code and data */
template <class DATA>
struct SgHashEntry
{
    SgHashEntry()
        : m_hash(),
          m_data()
    { }

    SgHashEntry(const SgHashCode& code, const DATA& data)
        : m_hash(code),
          m_data(data)
    { }

    SgHashCode m_hash;

    DATA m_data;
};

//----------------------------------------------------------------------------

/** HashTable implements an array of DATA */
template <class DATA>
class SgHashTable
{
public:
    /** Create a hash table with 'maxHash' entries. */
    explicit SgHashTable(int maxHash);

    ~SgHashTable();

    /** Leaves the positions in the hash table, but set all depths to zero, so
        that only the best move is valid, not the value. The hash entries will
        easily be replaced by fresh information.
    */
    void Age();

    /** Clear the hash table by marking all entries as invalid. */
    void Clear();

    /** Return true and the data stored under that code, or false if
        none stored.
    */
    bool Lookup(const SgHashCode& code, DATA* data) const;

    /** Size of hash table. */
    int MaxHash() const;

    /** Try to store 'data' under the hash code 'code'.
        Return whether the data was stored. The only reason for not storing
        it would be some 'better' data already hashing to the same hash code.
    */
    bool Store(const SgHashCode& code, const DATA& data);

    /** number of collisions on store */
    int NuCollisions() const
    {
        return m_nuCollisions;
    }

    /** total number of stores attempted */
    int NuStores() const
    {
        return m_nuStores;
    }

    /** total number of lookups attempted */
    int NuLookups() const
    {
        return m_nuLookups;
    }

    /** number of successful lookups */
    int NuFound() const
    {
        return m_nuFound;
    }

private:
    /** complete hash code for each entry */
    SgHashEntry<DATA>* m_entry;

    /** size of hash table */
    int m_maxHash;

    // AR: the following statistics can be made debug only
    // AR: pass a HashStatistics class to the HashTable when constructed

    /** number of collisions on store */
    mutable int m_nuCollisions;

    /** total number of stores attempted */
    mutable int m_nuStores;

    /** total number of lookups attempted */
    mutable int m_nuLookups;

    /** number of successful lookups */
    mutable int m_nuFound;

    /** not implemented */
    SgHashTable(const SgHashTable&);

    /** not implemented */
    SgHashTable& operator=(const SgHashTable&);
};

template <class DATA>
SgHashTable<DATA>::SgHashTable(int maxHash)
 : m_entry(0),
   m_maxHash(maxHash),
   m_nuCollisions(0),
   m_nuStores(0),
   m_nuLookups(0),
   m_nuFound(0)
{
    m_entry = new SgHashEntry<DATA>[m_maxHash];
    Clear();
}

template <class DATA>
SgHashTable<DATA>::~SgHashTable()
{
    delete[] m_entry;
}

template <class DATA>
void SgHashTable<DATA>::Age()
{
    for (int i = m_maxHash-1; i >= 0; --i)
    {
        m_entry[i].m_data.AgeData();
    }
}

template <class DATA>
void SgHashTable<DATA>::Clear()
{
    for (int i = m_maxHash-1; i >= 0; --i)
    {
        m_entry[i].m_data.Invalidate();
    }
}

template <class DATA>
int SgHashTable<DATA>::MaxHash() const
{
    return m_maxHash;
}

template <class DATA>
bool SgHashTable<DATA>::Store(const SgHashCode& code, const DATA& data)
{
    ++m_nuStores;
    int h = code.Hash(m_maxHash);
    SgHashEntry<DATA>& entry = m_entry[h];
    if (entry.m_data.IsValid() && code != entry.m_hash)
        ++m_nuCollisions;
    if (! entry.m_data.IsValid() || data.IsBetterThan(entry.m_data))
    {
        entry.m_hash = code;
        entry.m_data = data;
        return true;
    }
    return false;
}

template <class DATA>
bool SgHashTable<DATA>::Lookup(const SgHashCode& code, DATA* data) const
{
    ++m_nuLookups;
    int h = code.Hash(m_maxHash);
    const SgHashEntry<DATA>& entry = m_entry[h];
    if (entry.m_data.IsValid() && entry.m_hash == code)
    {
        *data = entry.m_data;
        ++m_nuFound;
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------

/** Writes statistics on hash table use (not the content) */
template <class DATA>
std::ostream& operator<<(std::ostream& out, const SgHashTable<DATA>& hash)
{    
    out << "HashTableStatistics:\n"
        << SgWriteLabel("Stores") << hash.NuStores() << '\n'
        << SgWriteLabel("LookupAttempt") << hash.NuLookups() << '\n'
        << SgWriteLabel("LookupSuccess") << hash.NuFound() << '\n'
        << SgWriteLabel("Collisions") << hash.NuCollisions() << '\n';
    return out;
}

#endif // SG_HASHTABLE_H
