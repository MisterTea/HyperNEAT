//----------------------------------------------------------------------------
/** @file SgHash.h
    Hash codes and Zobrist tables.

    See A.L. Zobrist "A New Hashing Method with Application for Game Playing",
    Techn. Rep. #88, Univ. of Wisconsin, Madison, WI 53706, April 1970.
    (Reprinted in ICCA Journal, Spring 1990?.)
*/
//----------------------------------------------------------------------------

#ifndef SG_HASH_H
#define SG_HASH_H

#include <algorithm>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "SgArray.h"
#include "SgException.h"
#include "SgRandom.h"

//----------------------------------------------------------------------------

/** N-bit hash codes */
template<int N>
class SgHash
{
 public:
  /** Costruct hash code initialized with zero. */
  SgHash() {}

  /** Construct hash code from integer index */
  SgHash(unsigned int key);

  ~SgHash() {}

  /** Reinitialize the hash code.
      Caution: after Clear() hash code matches code of empty board.
  */
  void Clear();

  /** Set to random value. */
  void Invalidate();

  bool operator<(const SgHash& code) const;

  bool operator==(const SgHash& code) const;

  bool operator!=(const SgHash& code) const;

  bool IsZero() const;

  /** Combine this hash code with the given hash code. */
  void Xor(const SgHash& code);

  /** Use this hash code to hash into a table with 'max' elements. */
  unsigned int Hash(int max) const;

  /** First integer (deprecated).
      @deprecated Don't use this function; it exposes implementation
      details.
  */
  unsigned int Code1() const;

  /** Second integer (deprecated).
      @deprecated Don't use this function; it exposes implementation
      details.
  */
  unsigned int Code2() const;

  /** Return a random hash code.
      @return A random hash code, which is not zero.
  */
  static SgHash Random();

  /** Roll bits n places to the left */
  void RollLeft(int n);

  /** Roll bits n places to the right */
  void RollRight(int n);

  /** Convert hash code to string */
  std::string ToString() const;

  /** Convert string to hash code */
  void FromString(const std::string& str);

  static int Size();

 private:
  /** Thomas Wang's 32 bit mix function */
  unsigned int Mix32(int key) const;

  unsigned int GetWord() const;

  std::bitset<N> m_code;
};

/** For backwards compatibility */
typedef SgHash<64> SgHashCode;

template<int N>
SgHash<N>::SgHash(unsigned int key)
: m_code(Mix32(key))
{
  // Use Thomas Wang's 32 bit mix function, cyclically
  for (int i = 1; i < (N / 32); ++i)
    {
      unsigned int mix = Mix32(GetWord());
      m_code <<= 32;
      m_code |= mix;
    }
}

template<int N>
bool SgHash<N>::operator<(const SgHash& code) const
{
  // std::bitset does not define operator<, so we have to do it (less
  // efficiently) ourselves
  for (int i = N - 1; i >= 0; --i)
    {
      bool c1 = m_code[i];
      bool c2 = code.m_code[i];
      if (! c1 && c2)
        return true;
      if (c1 && ! c2)
        return false;
    }
  return false;
}

template<int N>
bool SgHash<N>::operator==(const SgHash& code) const
{
  return code.m_code == m_code;
}

template<int N>
bool SgHash<N>::operator!=(const SgHash& code) const
{
  return code.m_code != m_code;
}

template<int N>
void SgHash<N>::Clear()
{
  m_code.reset();
}

template<int N>
unsigned int SgHash<N>::Code1() const
{
  return GetWord();
}

template<int N>
unsigned int SgHash<N>::Code2() const
{
  return (m_code >> 32).to_ulong();
}

template<int N>
void SgHash<N>::FromString(const std::string& str)
{
  Clear();
  for (std::string::const_iterator i_str = str.begin();
       i_str != str.end(); ++i_str)
    {
      m_code <<= 4;
      char c = *i_str;
      if (c >= '0' && c <= '9')
        m_code |= std::bitset<N>(c - '0');
      else if (c >= 'A' && c <= 'F')
        m_code |= std::bitset<N>(10 + c - 'A');
      else if (c >= 'a' && c <= 'f')
        m_code |= std::bitset<N>(10 + c - 'a');
      else throw SgException("Bad hex in hash string");
    }
}

template<int N>
unsigned int SgHash<N>::GetWord() const
{
  static const std::bitset<N> mask(0xffffffffUL);
  return (m_code & mask).to_ulong();
}

template<int N>
unsigned int SgHash<N>::Hash(int max) const
{
  return GetWord() % max;
}

template<int N>
void SgHash<N>::Invalidate()
{
  *this = Random();
}

template<int N>
bool SgHash<N>::IsZero() const
{
  return m_code.none();
}

template<int N>
unsigned int SgHash<N>::Mix32(int key) const
{
  key += ~(key << 15);
  key ^=  (key >> 10);
  key +=  (key << 3);
  key ^=  (key >> 6);
  key += ~(key << 11);
  key ^=  (key >> 16);
  return key;
}

template<int N>
SgHash<N> SgHash<N>::Random()
{
  SgHash hashcode;
  hashcode.m_code = SgRandom::Global().Int();
  for (int i = 1; i < (N / 32); ++i)
    {
      hashcode.m_code <<= 32;
      hashcode.m_code |= SgRandom::Global().Int();
    }

  return hashcode;
}

template<int N>
void SgHash<N>::RollLeft(int n)
{
  m_code = (m_code << n) ^ (m_code >> (N - n));
}

template<int N>
void SgHash<N>::RollRight(int n)
{
  m_code = (m_code >> n) ^ (m_code << (N - n));
}

template<int N>
int SgHash<N>::Size()
{
  return N;
}

template<int N>
std::string SgHash<N>::ToString() const
{
  std::ostringstream buffer;
  buffer.fill('0');
  std::bitset<N> mask(0xff);
  for (int i = N / 8; i >= 0; --i)
    {
      std::bitset<N> b = ((m_code >> (i * 8)) & mask);
      buffer << std::hex << std::setw(2) << b.to_ulong();
    }
  return buffer.str();
}

template<int N>
void SgHash<N>::Xor(const SgHash& code)
{
  m_code ^= code.m_code;
}

template<int N>
std::ostream& operator<<(std::ostream& out, const SgHash<N>& hash)
{
  out << hash.ToString();
  return out;
}

template<int N>
std::istream& operator>>(std::istream& in, const SgHash<N>& hash)
{
  std::string str;
  in >> str;
  hash.FromString(str);
  return in;
}

//----------------------------------------------------------------------------

/** Provides random hash codes for Zobrist hashing. */
template<int N>
class SgHashZobrist
{
 public:
  /** Hash index.
      The hash index ranges from [0..MAX_HASH_INDEX-1]. For board games with
      black and white pieces, MAX_HASH_INDEX needs to be bigger than twice
      the number of points on the board. It's up to the client to map points
      to this range.
      2 * SG_MAXPOINT, plus capture hash
  */
  static const int MAX_HASH_INDEX = 1500;

  SgHashZobrist();

  const SgHash<N>& Get(int index) const { return m_hash[index]; }

  /** Global table for this size of hash code. */
  static const SgHashZobrist& GetTable();

 private:
  static SgHashZobrist m_globalTable;

  SgArray<SgHash<N>,MAX_HASH_INDEX> m_hash;
};

/** For backwards compatibility */
typedef SgHashZobrist<64> SgHashZobristTable;

template<int N>
SgHashZobrist<N> SgHashZobrist<N>::m_globalTable;

template<int N>
SgHashZobrist<N>::SgHashZobrist()
{
  for (int i = 0; i < MAX_HASH_INDEX; ++i)
    m_hash[i] = SgHash<N>::Random();
}

template<int N>
const SgHashZobrist<N>& SgHashZobrist<N>::GetTable()
{
  return m_globalTable;
}

//----------------------------------------------------------------------------

namespace SgHashUtil
{
  /** Xor hash code with code from the global Zobrist table.
      @deprecated Use your own zobrist table to avoid bugs due to
      overlapping usage of indices
  */
  template<int N>
    inline SgHash<N> GetZobrist(int index)
    {
      return SgHashZobrist<N>::GetTable().Get(index);
    }

  /** Xor hash code with code from the global Zobrist table.
      @deprecated Use your own zobrist table to avoid bugs due to
      overlapping usage of indices
  */
  template<int N>
    inline void XorZobrist(SgHash<N>& hash, int index)
    {
      hash.Xor(GetZobrist<N>(index));
    }

  /** Xor hash code with integer index (no max value) */
  template<int N>
    inline void XorInteger(SgHash<N>& hash, int index)
    {
      hash.Xor(SgHash<N>(index));
    }
}

//----------------------------------------------------------------------------

#endif // SG_HASH_H
