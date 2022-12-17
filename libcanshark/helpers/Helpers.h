#ifndef CANSHARK_HELPERS_H
#define CANSHARK_HELPERS_H

#include <QList>

namespace dd::libcanshark::helpers {
    class Helpers {
    public:
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
