#ifndef CANSHARK_HELPERS_H
#define CANSHARK_HELPERS_H

#include <QList>

namespace dd::libcanshark::helpers {
    class Helpers {
    public:
        template<typename T>
        static std::vector<T> hex2bytes(const std::string& s)
        {
            constexpr size_t width = sizeof(T) * 2;
            std::vector<T> v;
            v.reserve((s.size() + width - 1) / width);
            for (auto it = s.crbegin(); it < s.crend(); it += width)
            {
                try {
                    auto begin = std::min(s.crend(), it + width).base();
                    auto end = it.base();
                    std::string slice(begin, end);
                    T value = std::stoul(slice, 0, 16);
                    v.push_back(value);
                } catch(std::exception& e) {
                    return {};
                }
            }
            return v;
        }

        template<typename T>
        static QList<T> split(const T& a, QList<T>& l, int n)
        {
            for (int i = 0; i < a.size(); i += n)
                l.push_back(a.mid(i, n));
            return l;
        }
    };

} // helpers

#endif //CANSHARK_HELPERS_H
