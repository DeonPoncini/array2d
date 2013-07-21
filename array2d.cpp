#include <algorithm>
#include <iostream>
#include <iterator>

template <typename T, typename I>
class Stripe;

template <typename T>
struct ptr_traits
{
    typedef T* ptr_type;
};

template <>
template <typename T, typename I>
struct ptr_traits<Stripe<T,I> >
{
    typedef Stripe<T,I> ptr_type;
};

template<typename T>
class DataIterator : public std::iterator<std::random_access_iterator_tag,T>
{
public:
    // typedefs
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename std::iterator<iterator_category, T>::value_type value_type;
    typedef typename std::iterator<iterator_category, T>::difference_type difference_type;
    typedef typename std::iterator<iterator_category, T>::reference reference;
    typedef typename std::iterator<iterator_category, T>::pointer pointer;

    typedef typename ptr_traits<T>::ptr_type ptr_type;

    template<typename T2>
    DataIterator(const DataIterator<T2>& other) : data(other.data) {}
    DataIterator(const ptr_type pData) : data(pData) {}

    template<typename T2>
    DataIterator& operator=(const DataIterator<T2>& r) { data = r; return *this; }

    DataIterator& operator++() { ++data; return *this; }
    DataIterator& operator--() { --data; return *this; }
    DataIterator operator++(int) { return DataIterator(data++); }
    DataIterator operator--(int) { return DataIterator(data--); }

    DataIterator operator+(const difference_type& n) const
        { return DataIterator(data + n); }
    DataIterator& operator+=(const difference_type& n)
        { data += n; return *this; }
    DataIterator operator-(const difference_type& n) const
        { return DataIterator(ptr_type(data - n)); }
    DataIterator& operator-=(const difference_type& n)
        { data -= n; return *this; }

    reference operator*()  const { return *data; }
    const pointer   operator->()  const { return &data; }
    pointer   operator->()  { return &data; }

    reference operator[](const difference_type& n) const { return data[n]; }

    template<typename T2>
    friend bool operator==(const DataIterator<T2>& r1, const DataIterator<T2>& r2);

    template<typename T2>
    friend bool operator!=(const DataIterator<T2>& r1, const DataIterator<T2>& r2);

    template<typename T2>
    friend bool operator<(const DataIterator<T2>& r1, const DataIterator<T2>& r2);

    template<typename T2>
    friend bool operator>(const DataIterator<T2>& r1, const DataIterator<T2>& r2);

    template<typename T2>
    friend bool operator<=(const DataIterator<T2>& r1, const DataIterator<T2>& r2);

    template<typename T2>
    friend bool operator>=(const DataIterator<T2>& r1, const DataIterator<T2>& r2);

    template<typename T2>
    friend typename DataIterator<T2>::difference_type
        operator+(const DataIterator<T2>& r1, const DataIterator<T2>& r2);

    template<typename T2>
    friend typename DataIterator<T2>::difference_type
        operator-(const DataIterator<T2>& r1, const DataIterator<T2>& r2);

private:
    ptr_type data;
};

template<typename T>
bool operator==(const DataIterator<T>& r1, const DataIterator<T>& r2)
    { return (r1.data == r2.data); }

template<typename T>
bool operator!=(const DataIterator<T>& r1, const DataIterator<T>& r2)
    { return (r1.data != r2.data); }

template<typename T>
bool operator<(const DataIterator<T>& r1, const DataIterator<T>& r2)
    { return (r1.data < r2.data); }

template<typename T>
bool operator>(const DataIterator<T>& r1, const DataIterator<T>& r2)
    { return (r1.data > r2.data); }

template<typename T>
bool operator<=(const DataIterator<T>& r1, const DataIterator<T>& r2)
    { return (r1.data <= r2.data); }

template<typename T>
bool operator>=(const DataIterator<T>& r1, const DataIterator<T>& r2)
    { return (r1.data >= r2.data); }

template<typename T>
typename DataIterator<T>::difference_type
operator+(const DataIterator<T>& r1,
          const DataIterator<T>& r2)
{
    return DataIterator<T>(r1.data + r2.data);
}

template<typename T>
typename DataIterator<T>::difference_type
operator-(const DataIterator<T>& r1, const DataIterator<T>& r2)
{
    return r1.data - r2.data;
}

template<typename T>
class Array2D
{
public:

    // iterators
    typedef DataIterator<const T> const_iterator;
    typedef DataIterator<T>       iterator;

    Array2D(size_t _width, size_t _height) :
        width(_width),
        height(_height),
        size(width*height),
        data(new T[size])
    {
    }

    ~Array2D()
    {
        delete[] data;
    }

    Array2D(const Array2D& other) :
        width(other.width),
        height(other.height),
        size(other.size),
        data(new T[size])
    {
        std::copy(other.data, other.data + other.size, data);
    }

    Array2D& operator=(const Array2D<T>& other)
    {
        if(this != other)
        {
            T* new_data = new T[other.size];
            width = other.width;
            height = other.height;
            size = other.size;

            std::copy(other.data, other.data + other.size, new_data);

            delete[] data;
            data = new_data;
        }
        return *this;
    }

    const_iterator begin() const { return const_iterator(data); }
    const_iterator end()   const { return const_iterator(data + size); }

    iterator begin() { return iterator(data); }
    iterator end()   { return iterator(data + size); }

    Stripe<T,iterator> operator[](size_t row)
    {
        return Stripe<T, iterator>(begin() + row*width,width);
    }

    const Stripe<T,const_iterator> operator[](size_t row) const
    {
        return Stripe<T,const_iterator>(begin() + row*width,width);
    }

    typedef DataIterator<const Stripe<T,const_iterator> > const_stripe_iterator;
    typedef DataIterator<Stripe<T,iterator> >       stripe_iterator;

    const_stripe_iterator stripe_begin() const
        { return const_stripe_iterator(Stripe<T, const_iterator>(begin(),width)); }

    const_stripe_iterator stripe_end() const
        { return const_stripe_iterator(Stripe<T, const_iterator>(end(),width)); }

    stripe_iterator stripe_begin()
        { return stripe_iterator(Stripe<T,iterator>(begin(),width)); }

    stripe_iterator stripe_end()
        { return stripe_iterator(Stripe<T,iterator>(end(),width)); }

private:
    size_t width;
    size_t height;
    size_t size;
    T* data;
};

template <typename T, typename I>
class Stripe
{
public:
    typedef typename DataIterator<T>::value_type value_type;
    typedef typename DataIterator<T>::difference_type difference_type;
    typedef typename DataIterator<T>::reference reference;
    typedef typename DataIterator<T>::pointer pointer;

    typedef I iterator;
    typedef const I const_iterator;

    Stripe(I _start, size_t _length) :
        start(_start),
        length(_length)
    {
    }

    I begin()
    {
        return start;
    }

    I end()
    {
        return start+length;
    }

    reference operator[](size_t index)
    {
        return (start + index)[0];
    }

    const reference operator[](size_t index) const
    {
        return (start + index)[0];
    }

    Stripe& operator++()
    {
        start += length;
        return *this;
    }

    Stripe operator++(int)
    {
        Stripe result(*this);
        ++(*this);
        return result;
    }

    Stripe& operator--()
    {
        start -= length;
        return *this;
    }

    Stripe operator--(int)
    {
        Stripe<T,I> result(*this);
        --(*this);
        return result;
    }

    bool operator==(const Stripe& other) const
    {
        return (other.start == start);
    }

    bool operator!=(const Stripe& other) const
    {
        return !(*this == other);
    }

    Stripe& operator+=(const difference_type& n)
    {
        start += n*length;
        return *this;
    }

    Stripe operator+(const difference_type& n) const
    {
        Stripe<T,I> result(*this);
        this += n;
        return result;
    }

    Stripe& operator-=(const difference_type& n)
    {
        start -= n*length;
        return *this;
    }

    Stripe operator-(const difference_type& n) const
    {
        Stripe result(*this);
        this -= n;
        return result;
    }

    reference operator*()  const
    {
        return *start;
    }

    pointer operator->() const
    {
        return &start;
    }

    bool operator<(const Stripe<T,I>& rhs)
    {
        return start < rhs.start;
    }

    bool operator>(const Stripe<T,I>& rhs)
    {
        return start > rhs.start;
    }

    bool operator<=(const Stripe<T,I>& rhs)
    {
        return start <= rhs.start;
    }

    bool operator>=(const Stripe<T,I>& rhs)
    {
        return start >= rhs.start;
    }

private:
    I start;
    size_t   length;
};


int main()
{
    Array2D<int> array(10,10);
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            array[i][j] = i*j;
        }
    }

    std::cout << "Print out by operator[][]" << std::endl;
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            std::cout << array[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;

    // print out the matrix
    std::cout << "Print out by linear iterator" << std::endl;
    for(Array2D<int>::iterator it = array.begin(); it != array.end(); it++)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl << std::endl;

    // per stripe
    std::cout << "Print out by stripe iterator" << std::endl;
    for(Array2D<int>::stripe_iterator it = array.stripe_begin(); it != array.stripe_end(); it++)
    {
        for(Stripe<int,Array2D<int>::iterator>::iterator it2 = it->begin(); it2 != it->end(); it2++)
        {
            std::cout << *it2 << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    // try sorting
    Array2D<int> sortedArray = array;
    std::cout << "Sorted output" << std::endl;
    std::sort(sortedArray.begin(),sortedArray.end());
    for(Array2D<int>::iterator it = sortedArray.begin(); it != sortedArray.end(); it++)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl << std::endl;
    for(Array2D<int>::stripe_iterator it = array.stripe_begin(); it != array.stripe_end(); it++)
    {
        for(Stripe<int,Array2D<int>::iterator>::iterator it2 = it->begin(); it2 != it->end(); it2++)
        {
            std::cout << *it2 << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}

