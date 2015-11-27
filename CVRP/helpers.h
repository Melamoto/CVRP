#include <vector>
#include <algorithm>
#include <random>

// Removes the first instance of the given value from vec if one is present
template<typename T>
inline void removeFromVector(vector<T>& vec, T& value)
{
    auto pos = find(vec.begin(), vec.end(), value);
    if (pos != vec.end()) vec.erase(pos);
}

// Removes the first instance of the given value from the sorted vector vec if one is present
template<typename T>
inline void removeFromSortedVector(vector<T>& vec, T& value)
{
    auto pos = equal_range(vec.begin(), vec.end(), value);
    if (get<0>(pos) != get<1>(pos)) vec.erase(get<0>(pos));
}

// Returns a random element iterator from vec
template<typename T>
inline typename vector<T>::iterator randomElement(vector<T>& vec, default_random_engine& rng)
{
    uniform_int_distribution<size_t> eltSelect(0, vec.size()-1);
    vector<T>::iterator eltIt = vec.begin() + eltSelect(rng);
    return eltIt;
}

bool cycleAdjacent(size_t vecSize, size_t i, size_t j){
    size_t min;
    size_t max;
    if (i < j){
        min = i;
        max = j;
    }
    else if (j < i){
        min = j;
        max = i;
    }
    else return false;
    if (min + 1 == max) return true;
    if (min == 0 && max == vecSize - 1) return true;
    return false;
}

inline size_t cycleNext(size_t vecSize, size_t i){
    return i + 1 % vecSize;
}
inline size_t cyclePrev(size_t vecSize, size_t i){
    return i == 0 ? vecSize - 1 : i - 1;
}

bool cycleBetween(size_t vecSize, size_t elt, size_t first, size_t last){
    size_t next = elt;
    while (next != first && next != last) next = cycleNext(vecSize, next);
    return next == last;
}
