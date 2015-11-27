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
