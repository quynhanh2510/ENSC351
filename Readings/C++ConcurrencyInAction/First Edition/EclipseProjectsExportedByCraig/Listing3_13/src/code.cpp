#include <map>
#include <string>
#include <mutex>
#include <shared_mutex>

// Update to code from 2nd edition of Williams' textbook:
// Added by Craig Scratchley, Oct. 2019
// Pretend that we have a C++17 compiler instead of our C++14 one
#define shared_mutex shared_timed_mutex

class dns_entry
{};

class dns_cache
{
    std::map<std::string,dns_entry> entries;
    std::shared_mutex entry_mutex;
public:
    dns_entry find_entry(std::string const& domain)
    {
        std::shared_lock<std::shared_mutex> lk(entry_mutex);
        std::map<std::string,dns_entry>::const_iterator const it=
            entries.find(domain);
        return (it==entries.end())?dns_entry():it->second;
    }
    void update_or_add_entry(std::string const& domain,
                             dns_entry const& dns_details)
    {
        std::lock_guard<std::shared_mutex> lk(entry_mutex);
        entries[domain]=dns_details;
    }
};

int main()
{}
