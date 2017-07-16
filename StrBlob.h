#ifndef STR_BLOB_H
#define STR_BLOB_H

#include <vector>
#include <memory>
#include <string>
#include <initializer_list>
#include <stdexcept>

using namespace std;

//必须要提前声明StrBlobPtr，才能声明为StrBlob类的友元类
class StrBlobPtr;

class StrBlob
{
    friend class StrBlobPtr;
public:
    typedef std::vector<std::string>::size_type size_type;
    // using size_type = std::vector<std::string>::size_type;
    StrBlob();
    StrBlob(std::initializer_list<std::string>);
    size_type size() const { return data->size(); }
    bool empty() const { return data->empty(); }

    // 添加和删除元素
    void push_back(const std::string& str) { data->push_back(str); }
    void pop_back();

    // 元素访问
    std::string& front();
    const std::string& front() const;
    std::string& back();
    const std::string& back() const;

    // 提供给StrBlobPtr的接口
    StrBlobPtr begin();     //定义Blob后才能定义这两个函数，因为用到了StrBlobPtr的成员（函数）
    StrBlobPtr end();
private:
    // 定义数据成员data
    std::shared_ptr<std::vector<std::string>> data;
    // 若data[i]不合法，抛出一个异常
    void check(size_type, const std::string&) const;
};

inline StrBlob::StrBlob() : data(std::make_shared<std::vector<std::string>>()) {}
inline StrBlob::StrBlob(std::initializer_list<std::string> il) :
    data(std::make_shared<std::vector<std::string>>(il)){}

// 若data[i]不合法，抛出一个异常
inline void StrBlob::check(size_type i, const std::string &msg) const
{
    if (i >= data->size())
        throw std::out_of_range(msg);
}

// 添加和删除元素
inline void StrBlob::pop_back()
{
    check(0, "pop_back on empty StrBlob....");
    data->pop_back();
}

// 元素访问
inline std::string& StrBlob::front()
{
    check(0, "front on empty StrBlob....");
    return data->front();
}
inline const std::string& StrBlob::front() const
{
    check(0, "front on empty StrBlob....");
    return data->front();
}
inline std::string& StrBlob::back()
{
    check(0, "back on empty StrBlob...");
    return data->back();
}
inline const std::string& StrBlob::back() const
{
    check(0, "back on empty StrBlob...");
    return data->back();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////StrBlobPtr类的定义/////////////////////////////////////////////////////////
class StrBlobPtr
{
    friend bool eq(const StrBlobPtr&, const StrBlobPtr&);
public:
    StrBlobPtr() : curr(0) {}
    StrBlobPtr(StrBlob &a, size_t sz = 0) : wptr(a.data), curr(sz) {}
    std::string& deref() const;
    StrBlobPtr& incur();       //前缀递增
    StrBlobPtr& decur();       //前缀递减

private:
    // 若检查成功，check返回一个指向vector的shared_ptr
    std::shared_ptr<std::vector<std::string>> check(size_t, const std::string&) const;
    // 保存一个weak_ptr, 意味着底层vector可能会被销毁
    std::weak_ptr<std::vector<std::string>> wptr;
    // 在数组中的当前位置
    size_t curr;
};
extern neq(const StrBlobPtr &lhs, const StrBlobPtr &rhs);
inline std::shared_ptr<std::vector<std::string>> StrBlobPtr::check(size_t i, const std::string& msg) const
{
    auto ret = wptr.lock(); //判断vector是否还存在
    if (!ret)
        throw std::runtime_error("Unbound StrBlobPtr....");
    if (i >= ret->size())
        throw std::out_of_range(msg);
    return ret;
}
inline std::string& StrBlobPtr::deref() const
{
    std::shared_ptr<std::vector<std::string>> p = check(curr, "dereference past end...");
    return (*p)[curr];  // (*p)是对象所指的vector
}
inline StrBlobPtr& StrBlobPtr::incur()       //前缀递增, 返回递增后的对象的引用
{
    // 若curr已经指向了容器的尾后位置， 则不能递增它
    std::shared_ptr<std::vector<std::string>> p = check(curr, "increment past end of StrBlobPtr....");
    ++curr;
    return *this;
}
inline StrBlobPtr& StrBlobPtr::decur()       //前缀递减, 返回递减后的对象的引用
{
    // 若curr已经是0，则不能递减它，否则产生一个非法下标
    --curr;
    check(-1, "decrement past begin of StrBlobPtr....");
    return *this;
}

// 提供给StrBlobPtr的接口
StrBlobPtr StrBlob::begin()     //定义Blob后才能定义这两个函数，因为用到了StrBlobPtr的成员（函数）
{
    return StrBlobPtr(*this);
}
StrBlobPtr StrBlob::end()
{
    auto ret = StrBlobPtr(*this, data->size());
    return ret;
}

inline bool eq(const StrBlobPtr &i, const StrBlobPtr &j)
{
    auto m = i.wptr.lock(), n = j.wptr.lock();
    //若底层的vector是同一个
    if (m == n)
        // 则两个指针都为空， 或者指向相同的元素
        return (!n || i.curr == j.curr);
    else return false;
}

inline neq(const StrBlobPtr &lhs, const StrBlobPtr &rhs)
{
    return !eq(lhs, rhs);
}

#endif // STR_BLOB_H
