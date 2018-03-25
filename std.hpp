#include <thread>
#include <future>
#include <vector>
#include <algorithm>

using namespace std;

template<class InputIterator, class Function>
void for_each_fast(InputIterator b, InputIterator e, Function f)
{
    typedef decltype(std::async(std::launch::async, std::for_each<InputIterator, Function>,
                                std::declval<InputIterator>(), std::declval<InputIterator>(),
                                std::declval<Function>())) Future;

    const auto hc = thread::hardware_concurrency();
    const auto dst = distance(b, e);
    const auto per_thr = dst / hc;
    const auto mod = dst % hc;

    auto last = b;
    vector<Future> handle;
    for (unsigned i = 0; i < hc - 1; ++i)
    {
        advance(last, per_thr + ((mod > i) ? 1 : 0));
        handle.push_back(async(std::launch::async, for_each<InputIterator, Function>, b, last, f));
        b = last;
    }
    for_each<InputIterator, Function>(b, e, f);
    for (auto& h : handle)
        h.wait();
}
