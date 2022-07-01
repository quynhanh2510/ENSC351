#include <exception>
#include <stack>
#include <mutex>
#include <memory>
#include <iostream>

struct empty_stack: std::exception
{
    const char* what() const throw()
    {
        return "empty stack";
    }

};

template<typename T>
class threadsafe_stack
{
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack(){}
    threadsafe_stack(const threadsafe_stack& other)
    {
        std::lock_guard lock{other.m};
        data=other.data;
    }
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;

    void push(T new_value)
    {
        std::lock_guard lock{m};
        data.push(new_value);
    }
    std::shared_ptr<T> pop()
    {
        std::lock_guard lock{m};
        if(data.empty()) throw empty_stack();
        auto const res(std::make_shared<T>(data.top()));
        data.pop();
        return res;
    }
    void pop(T& value)
    {
        std::lock_guard lock{m};
        if(data.empty()) throw empty_stack();
        value=data.top();
        data.pop();
    }
    bool empty() const
    {
        std::lock_guard lock{m};
        return data.empty();
    }
};

int main()
{
    try
    {
        threadsafe_stack<int> si;
        si.push(5);
        std::cout << *si.pop() << std::endl;
        // if(!si.empty())
        {
            int x;
            si.pop(x);
        }
    }
    catch(std::exception const& e)
    {  /* LOG */
       throw;
    }
    catch(...) // Catch anything else.
    {  /* LOG */
       throw;
    }
}
