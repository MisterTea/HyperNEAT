//----------------------------------------------------------------------------
/** @file SgStack.h
    Stack class.
*/
//----------------------------------------------------------------------------

#ifndef SG_STACK_H
#define SG_STACK_H

#include <algorithm>

//----------------------------------------------------------------------------

/** Stack with up to size objects of class T. Stack does not assume ownership.
	Memory management of objects on stack is the user's responsibility.
*/
template <class T, int SIZE>
class SgStack
{
public:
    SgStack()
        : m_sp(0)
    { }

    ~SgStack() {}

    /** Empty the stack */
    void Clear();
        
    /** Make this stack a copy of other*/
    void CopyFrom(const SgStack<T,SIZE>& other);
    
    bool IsEmpty() const;

    bool NonEmpty() const;

    /** remove and return top element. Must be NonEmpty. */
    T Pop();

    void Push(T data);

    /** Push all elements from other stack onto this stack */
    void PushAll(const SgStack<T,SIZE>& other);
    
    /** Number of elements on stack */
    int Size() const;

    /** Exchange contents of this and other stack */
    void SwapWith(SgStack<T,SIZE>& other);
    
    const T& Top() const;

    const T& operator[](int index) const;

private:
    int m_sp;

    T m_stack[SIZE];

    /** not implemented */
    SgStack(const SgStack&);

    /** not implemented */
    SgStack& operator=(const SgStack&);
};

//----------------------------------------------------------------------------

template<typename T, int SIZE>
void SgStack<T,SIZE>::Clear()
{
    m_sp = 0;
}

template<typename T, int SIZE>
void SgStack<T,SIZE>::CopyFrom(const SgStack<T,SIZE>& other)
{
    for(int i=0; i < other.Size(); ++i)
    	m_stack[i] = other.m_stack[i];
    m_sp = other.m_sp;
}

template<typename T, int SIZE>
bool SgStack<T,SIZE>::IsEmpty() const
{
    return m_sp == 0;
}

template<typename T, int SIZE>
bool SgStack<T,SIZE>::NonEmpty() const
{
    return m_sp != 0;
}

template<typename T, int SIZE>
T SgStack<T,SIZE>::Pop()
{
    SG_ASSERT(0 < m_sp);
    return m_stack[--m_sp];
}

template<typename T, int SIZE>
void SgStack<T,SIZE>::Push(T data)
{
    SG_ASSERT(m_sp < SIZE);
    m_stack[m_sp++] = data;
}

template<typename T, int SIZE>
void SgStack<T,SIZE>::PushAll(const SgStack<T,SIZE>& other)
{
    for(int i=0; i < other.Size(); ++i)
    	Push(other.m_stack[i]);
}

template<typename T, int SIZE>
int SgStack<T,SIZE>::Size() const
{
    return m_sp;
}

template<typename T, int SIZE>
void SgStack<T,SIZE>::SwapWith(SgStack<T,SIZE>& other)
{
	int nuSwap = std::min(Size(), other.Size());
    for(int i = 0; i < nuSwap; ++i)
    	std::swap(m_stack[i], other.m_stack[i]);
    if (Size() < other.Size())
    	for(int i = Size(); i < other.Size(); ++i)
        	m_stack[i] = other.m_stack[i];
    else if (other.Size() < Size())
    	for(int i = other.Size(); i < Size(); ++i)
        	other.m_stack[i] = m_stack[i];
    std::swap(m_sp, other.m_sp);
}

template<typename T, int SIZE>
const T& SgStack<T,SIZE>::Top() const
{
    SG_ASSERT(0 < m_sp);
    return m_stack[m_sp-1];
}

template<typename T, int SIZE>
const T& SgStack<T,SIZE>::operator[](int index) const
{
    SG_ASSERT(index >= 0);
    SG_ASSERT(index < m_sp);
    return m_stack[index];
}

//----------------------------------------------------------------------------

#endif // SG_STACK_H
