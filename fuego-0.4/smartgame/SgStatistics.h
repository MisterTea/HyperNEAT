//----------------------------------------------------------------------------
/** @file SgStatistics.h
    Classes for incremental computation of statistical properties.
    The classes SgStatisticsBase (mean), SgStatistics (mean, variance) and
    SgStatisticsExt (mean, variance, min, max) are extensions of each other,
    but don't derive from each other, to avoid the overhead of virtual
    functions and because a base class has can (or could in the future) have
    functions that are not supported in derived classes (like removing a
    sample, which is easy in SgStatisticsBase, but not possible in
    SgStatisticsExt). However, member functions with the same meaning have the
    same name, so that the classes are easily replacable in user code and can
    be used as template arguments.
*/
//----------------------------------------------------------------------------

#ifndef SG_STATISTICS_H
#define SG_STATISTICS_H

#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "SgException.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

/** Computes mean of a statistical variable.
    The template parameters are the floating point type and the counter type,
    depending on the precision-memory tradeoff.
*/
template<typename VALUE, typename COUNT>
class SgStatisticsBase
{
public:
    SgStatisticsBase();

    /** Create statistics initialized with values.
        Note that value must be initialized to 0 if count is 0.
        Equivalent to creating a statistics and calling @c count times
        Add(val)
    */
    SgStatisticsBase(VALUE val, COUNT count);

    void Add(VALUE val);

    void Remove(VALUE val);

    /** Add a value n times */
    void Add(VALUE val, COUNT n);

    /** Remove a value n times. */
    void Remove(VALUE val, COUNT n);

    void Clear();

    const COUNT& Count() const;

    /** Initialize with values.
        Equivalent to calling Clear() and calling @c count times
        Add(val)
    */
    void Initialize(VALUE val, COUNT count);

    /** Check if the mean value is defined.
        The mean value is defined, if the count if greater than zero. The
        result of this function is equivalent to <tt>Count() > 0</tt>, for
        integer count types and <tt>Count() > epsilon()</tt> for floating
        point count types.
    */
    bool IsDefined() const;

    const VALUE& Mean() const;

    /** Write in human readable format. */
    void Write(std::ostream& out) const;

    /** Save in a compact platform-independent text format.
        The data is written in a single line, without trailing newline.
    */
    void SaveAsText(std::ostream& out) const;

    /** Load from text format.
        See SaveAsText()
    */
    void LoadFromText(std::istream& in);

private:
    COUNT m_count;

    VALUE m_mean;
};

template<typename VALUE, typename COUNT>
inline SgStatisticsBase<VALUE,COUNT>::SgStatisticsBase()
{
    Clear();
}

template<typename VALUE, typename COUNT>
inline SgStatisticsBase<VALUE,COUNT>::SgStatisticsBase(VALUE val, COUNT count)
    : m_count(count),
      m_mean(val)
{
}

template<typename VALUE, typename COUNT>
void SgStatisticsBase<VALUE,COUNT>::Add(VALUE val)
{
    // Write order dependency: at least one class (SgUctSearch in lock-free
    // mode) uses SgStatisticsBase concurrently without locking and assumes
    // that m_mean is valid, if m_count is greater zero
    COUNT count = m_count;
    ++count;
    SG_ASSERT(! std::numeric_limits<COUNT>::is_exact
              || count > 0); // overflow
    val -= m_mean;
    m_mean +=  val / count;
    m_count = count;
}

template<typename VALUE, typename COUNT>
void SgStatisticsBase<VALUE,COUNT>::Remove(VALUE val)
{
    // Write order dependency: at least on class (SgUctSearch in lock-free
    // mode) uses SgStatisticsBase concurrently without locking and assumes
    // that m_mean is valid, if m_count is greater zero
    COUNT count = m_count;
    if (count > 1) 
    {
        --count;
        m_mean += (m_mean - val) / count;
        m_count = count;
    }
    else
        Clear();
}

template<typename VALUE, typename COUNT>
void SgStatisticsBase<VALUE,COUNT>::Remove(VALUE val, COUNT n)
{
    SG_ASSERT(m_count >= n);
    // Write order dependency: at least on class (SgUctSearch in lock-free
    // mode) uses SgStatisticsBase concurrently without locking and assumes
    // that m_mean is valid, if m_count is greater zero
    COUNT count = m_count;
    if (count > n) 
    {
        count -= n;
        m_mean += n * (m_mean - val) / count;
        m_count = count;
    }
    else
        Clear();
}

template<typename VALUE, typename COUNT>
void SgStatisticsBase<VALUE,COUNT>::Add(VALUE val, COUNT n)
{
    // Write order dependency: at least one class (SgUctSearch in lock-free
    // mode) uses SgStatisticsBase concurrently without locking and assumes
    // that m_mean is valid, if m_count is greater zero
    COUNT count = m_count;
    count += n;
    SG_ASSERT(! std::numeric_limits<COUNT>::is_exact
              || count > 0); // overflow
    val -= m_mean;
    m_mean +=  n * val / count;
    m_count = count;
}

template<typename VALUE, typename COUNT>
inline void SgStatisticsBase<VALUE,COUNT>::Clear()
{
    m_count = 0;
    m_mean = 0;
}

template<typename VALUE, typename COUNT>
inline const COUNT& SgStatisticsBase<VALUE,COUNT>::Count() const
{
    return m_count;
}

template<typename VALUE, typename COUNT>
inline void SgStatisticsBase<VALUE,COUNT>::Initialize(VALUE val, COUNT count)
{
    SG_ASSERT(count > 0);
    m_count = count;
    m_mean = val;
}

template<typename VALUE, typename COUNT>
inline bool SgStatisticsBase<VALUE,COUNT>::IsDefined() const
{
    if (std::numeric_limits<COUNT>::is_exact)
        return m_count > 0;
    else
        return m_count > std::numeric_limits<COUNT>::epsilon();
}

template<typename VALUE, typename COUNT>
void SgStatisticsBase<VALUE,COUNT>::LoadFromText(std::istream& in)
{
    in >> m_count >> m_mean;
}

template<typename VALUE, typename COUNT>
inline const VALUE& SgStatisticsBase<VALUE,COUNT>::Mean() const
{
    SG_ASSERT(IsDefined());
    return m_mean;
}

template<typename VALUE, typename COUNT>
void SgStatisticsBase<VALUE,COUNT>::Write(std::ostream& out) const
{
    if (IsDefined())
        out << Mean();
    else
        out << '-';
}

template<typename VALUE, typename COUNT>
void SgStatisticsBase<VALUE,COUNT>::SaveAsText(std::ostream& out) const
{
    out << m_count << ' ' << m_mean;
}

//----------------------------------------------------------------------------

/** Computes mean and variance of a statistical variable.
    The template parameters are the floating point type and the counter type,
    depending on the precision-memory tradeoff.
*/
template<typename VALUE, typename COUNT>
class SgStatistics
{
public:
    SgStatistics();

    /** Create statistics initialized with values.
        Equivalent to creating a statistics and calling @c count times
        Add(val)
    */
    SgStatistics(VALUE val, COUNT count);

    void Add(VALUE val);

    void Clear();

    bool IsDefined() const;

    const VALUE& Mean() const;

    const COUNT& Count() const;

    VALUE Deviation() const;

    VALUE Variance() const;

    /** Write in human readable format. */
    void Write(std::ostream& out) const;

    /** Save in a compact platform-independent text format.
        The data is written in a single line, without trailing newline.
    */
    void SaveAsText(std::ostream& out) const;

    /** Load from text format.
        See SaveAsText()
    */
    void LoadFromText(std::istream& in);

private:
    SgStatisticsBase<VALUE,COUNT> m_statisticsBase;

    VALUE m_variance;
};

template<typename VALUE, typename COUNT>
inline SgStatistics<VALUE,COUNT>::SgStatistics()
{
    Clear();
}

template<typename VALUE, typename COUNT>
inline SgStatistics<VALUE,COUNT>::SgStatistics(VALUE val, COUNT count)
    : m_statisticsBase(val, count)
{
    m_variance = 0;
}

template<typename VALUE, typename COUNT>
void SgStatistics<VALUE,COUNT>::Add(VALUE val)
{
    if (IsDefined())
    {
        COUNT countOld = Count();
        VALUE meanOld = Mean();
        m_statisticsBase.Add(val);
        VALUE mean = Mean();
        COUNT count = Count();
        m_variance = (countOld * (m_variance + meanOld * meanOld)
                      + val * val) / count  - mean * mean;
    }
    else
    {
        m_statisticsBase.Add(val);
        m_variance = 0;
    }
}

template<typename VALUE, typename COUNT>
inline void SgStatistics<VALUE,COUNT>::Clear()
{
    m_statisticsBase.Clear();
    m_variance = 0;
}

template<typename VALUE, typename COUNT>
inline const COUNT& SgStatistics<VALUE,COUNT>::Count() const
{
    return m_statisticsBase.Count();
}

template<typename VALUE, typename COUNT>
inline VALUE SgStatistics<VALUE,COUNT>::Deviation() const
{
    return std::sqrt(m_variance);
}

template<typename VALUE, typename COUNT>
inline bool SgStatistics<VALUE,COUNT>::IsDefined() const
{
    return m_statisticsBase.IsDefined();
}

template<typename VALUE, typename COUNT>
void SgStatistics<VALUE,COUNT>::LoadFromText(std::istream& in)
{
    m_statisticsBase.LoadFromText(in);
    in >> m_variance;
}

template<typename VALUE, typename COUNT>
inline const VALUE& SgStatistics<VALUE,COUNT>::Mean() const
{
    return m_statisticsBase.Mean();
}

template<typename VALUE, typename COUNT>
inline VALUE SgStatistics<VALUE,COUNT>::Variance() const
{
    return m_variance;
}

template<typename VALUE, typename COUNT>
void SgStatistics<VALUE,COUNT>::Write(std::ostream& out) const
{
    if (IsDefined())
        out << Mean() << " dev=" << Deviation();
    else
        out << '-';
}

template<typename VALUE, typename COUNT>
void SgStatistics<VALUE,COUNT>::SaveAsText(std::ostream& out) const
{
    m_statisticsBase.SaveAsText(out);
    out << ' ' << m_variance;
}

//----------------------------------------------------------------------------

/** Extended version of SgStatistics.
    Also stores minimum and maximum values.
    The template parameters are the floating point type and the counter type,
    depending on the precision-memory tradeoff.
*/
template<typename VALUE, typename COUNT>
class SgStatisticsExt
{
public:
    SgStatisticsExt();

    void Add(VALUE val);

    void Clear();

    bool IsDefined() const;

    const VALUE& Mean() const;

    const COUNT& Count() const;

    VALUE Max() const;

    VALUE Min() const;

    VALUE Deviation() const;

    VALUE Variance() const;

    void Write(std::ostream& out) const;

private:
    SgStatistics<VALUE,COUNT> m_statistics;

    VALUE m_max;

    VALUE m_min;
};

template<typename VALUE, typename COUNT>
inline SgStatisticsExt<VALUE,COUNT>::SgStatisticsExt()
{
    Clear();
}

template<typename VALUE, typename COUNT>
void SgStatisticsExt<VALUE,COUNT>::Add(VALUE val)
{
    m_statistics.Add(val);
    if (val > m_max)
        m_max = val;
    if (val < m_min)
        m_min = val;
}

template<typename VALUE, typename COUNT>
inline void SgStatisticsExt<VALUE,COUNT>::Clear()
{
    m_statistics.Clear();
    m_min = std::numeric_limits<VALUE>::max();
    m_max = -std::numeric_limits<VALUE>::max();
}

template<typename VALUE, typename COUNT>
inline const COUNT& SgStatisticsExt<VALUE,COUNT>::Count() const
{
    return m_statistics.Count();
}

template<typename VALUE, typename COUNT>
inline VALUE SgStatisticsExt<VALUE,COUNT>::Deviation() const
{
    return m_statistics.Deviation();
}

template<typename VALUE, typename COUNT>
inline bool SgStatisticsExt<VALUE,COUNT>::IsDefined() const
{
    return m_statistics.IsDefined();
}

template<typename VALUE, typename COUNT>
inline VALUE SgStatisticsExt<VALUE,COUNT>::Max() const
{
    return m_max;
}

template<typename VALUE, typename COUNT>
inline const VALUE& SgStatisticsExt<VALUE,COUNT>::Mean() const
{
    return m_statistics.Mean();
}

template<typename VALUE, typename COUNT>
inline VALUE SgStatisticsExt<VALUE,COUNT>::Min() const
{
    return m_min;
}

template<typename VALUE, typename COUNT>
inline VALUE SgStatisticsExt<VALUE,COUNT>::Variance() const
{
    return m_statistics.Variance();
}

template<typename VALUE, typename COUNT>
void SgStatisticsExt<VALUE,COUNT>::Write(std::ostream& out) const
{
    if (IsDefined())
    {
        m_statistics.Write(out);
        out << " min=" << m_min << " max=" << m_max;
    }
    else
        out << '-';
}

//----------------------------------------------------------------------------

/** Set of named statistical variables.
    The template parameters are the floating point type and the counter type,
    depending on the precision-memory tradeoff.
*/
template<typename VALUE, typename COUNT>
class SgStatisticsCollection
{
public:
    /** Add the statistics of another collection.
        The collections must contain the same entries.
    */
    void Add(const SgStatisticsCollection<VALUE,COUNT>& collection);

    void Clear();

    bool Contains(const std::string& name) const;

    /** Create a new variable. */
    void Create(const std::string& name);

    const SgStatistics<VALUE,COUNT>& Get(const std::string& name) const;

    SgStatistics<VALUE,COUNT>& Get(const std::string& name);

    void Write(std::ostream& o) const;

private:
    typedef std::map<std::string,SgStatistics<VALUE,COUNT> > Map;

    typedef typename Map::iterator Iterator;

    typedef typename Map::const_iterator ConstIterator;

    Map m_map;
};

template<typename VALUE, typename COUNT>
void
SgStatisticsCollection<VALUE,COUNT>
::Add(const SgStatisticsCollection<VALUE,COUNT>& collection)
{
    if (m_map.size() != collection.m_map.size())
        throw SgException("Incompatible statistics collections");
    for (Iterator p = m_map.begin(); p != m_map.end(); ++p)
    {
        ConstIterator k = collection.m_map.find(p->first);
        if (k == collection.m_map.end())
            throw SgException("Incompatible statistics collections");
        p->second.Add(k->second);
    }
}

template<typename VALUE, typename COUNT>
void SgStatisticsCollection<VALUE,COUNT>::Clear()
{
    for (Iterator p = m_map.begin(); p != m_map.end(); ++p)
        p->second.Clear();
}

template<typename VALUE, typename COUNT>
bool SgStatisticsCollection<VALUE,COUNT>::Contains(const std::string& name)
    const
{
    return (m_map.find(name) != m_map.end());
}

template<typename VALUE, typename COUNT>
void SgStatisticsCollection<VALUE,COUNT>::Create(const std::string& name)
{
    m_map[name] = SgStatistics<VALUE,COUNT>();
}

template<typename VALUE, typename COUNT>
const SgStatistics<VALUE,COUNT>&
SgStatisticsCollection<VALUE,COUNT>::Get(const std::string& name) const
{
    ConstIterator p = m_map.find(name);
    if (p == m_map.end())
    {
        std::ostringstream o;
        o << "Unknown statistics name " << name << '.';
        throw SgException(o.str());
    }
    return p->second;
}

template<typename VALUE, typename COUNT>
SgStatistics<VALUE,COUNT>&
SgStatisticsCollection<VALUE,COUNT>::Get(const std::string& name)
{
    Iterator p = m_map.find(name);
    if (p == m_map.end())
    {
        std::ostringstream o;
        o << "Unknown statistics name " << name << '.';
        throw SgException(o.str());
    }
    return p->second;
}

template<typename VALUE, typename COUNT>
void SgStatisticsCollection<VALUE,COUNT>::Write(std::ostream& o) const
{
    for (ConstIterator p = m_map.begin(); p != m_map.end(); ++p)
        o << p->first << ": " << p->second.Write(o) << '\n';
}

//----------------------------------------------------------------------------

/** Histogram.
    The template parameters are the floating point type and the counter type,
    depending on the precision-memory tradeoff.
*/
template<typename VALUE, typename COUNT>
class SgHistogram
{
public:
    SgHistogram();

    SgHistogram(VALUE min, VALUE max, int bins);

    /** Reinitialize and clear histogram. */
    void Init(VALUE min, VALUE max, int bins);

    void Add(VALUE value);

    void Clear();

    int Bins() const;

    COUNT Count() const;

    /** Get count in a certain bin. */
    COUNT Count(int i) const;

    /** Write as x,y-table.
        Writes the historgram in a format that likely can be used by other
        programs. Writes one x,y pair per line. The separator is TAB.
        The x-values are the left border values of the bins, the y-values
        are the counts of the bins.
    */
    void Write(std::ostream& out) const;

    /** Write with labels.
        Example output with label "Value", the numbers in brackets are the
        left border of each bin:
        @verbatim
        Value[0]  100
        Value[10] 2000
        Value[20] 500
        @endverbatim
    */
    void WriteWithLabels(std::ostream& out, const std::string& label) const;

private:
    typedef std::vector<COUNT> Vector;

    int m_bins;

    COUNT m_count;

    VALUE m_binSize;

    VALUE m_min;

    VALUE m_max;

    Vector m_array;
};

template<typename VALUE, typename COUNT>
SgHistogram<VALUE,COUNT>::SgHistogram()
{
    Init(0, 1, 1);
}

template<typename VALUE, typename COUNT>
SgHistogram<VALUE,COUNT>::SgHistogram(VALUE min, VALUE max, int bins)
{
    Init(min, max, bins);
}

template<typename VALUE, typename COUNT>
void SgHistogram<VALUE,COUNT>::Add(VALUE value)
{
    ++m_count;
    int i = static_cast<int>((value - m_min) / m_binSize);
    if (i < 0)
        i = 0;
    if (i >= m_bins)
        i = m_bins - 1;
    ++m_array[i];
}

template<typename VALUE, typename COUNT>
int SgHistogram<VALUE,COUNT>::Bins() const
{
    return m_bins;
}

template<typename VALUE, typename COUNT>
void SgHistogram<VALUE,COUNT>::Clear()
{
    m_count = 0;
    for (typename Vector::iterator it = m_array.begin(); it != m_array.end();
         ++ it)
        *it = 0;
}

template<typename VALUE, typename COUNT>
COUNT SgHistogram<VALUE,COUNT>::Count() const
{
    return m_count;
}

template<typename VALUE, typename COUNT>
COUNT SgHistogram<VALUE,COUNT>::Count(int i) const
{
    SG_ASSERT(i >= 0);
    SG_ASSERT(i < m_bins);
    return m_array[i];
}

template<typename VALUE, typename COUNT>
void SgHistogram<VALUE,COUNT>::Init(VALUE min, VALUE max, int bins)
{
    m_array.resize(bins);
    m_min = min;
    m_max = max;
    m_bins = bins;
    m_binSize = (m_max - m_min) / m_bins;
    Clear();
}

template<typename VALUE, typename COUNT>
void SgHistogram<VALUE,COUNT>::Write(std::ostream& out) const
{
    for (int i = 0; i < m_bins; ++i)
        out << (m_min + i * m_binSize) << '\t' << m_array[i] << '\n';

}

template<typename VALUE, typename COUNT>
void SgHistogram<VALUE,COUNT>::WriteWithLabels(std::ostream& out,
                                             const std::string& label) const
{
    for (int i = 0; i < m_bins; ++i)
    {
        std::ostringstream binLabel;
        binLabel << label << '[' << (m_min + i * m_binSize) << ']';
        out << SgWriteLabel(binLabel.str()) << m_array[i] << '\n';
    }
}

//----------------------------------------------------------------------------

#endif // SG_STATISTICS_H
