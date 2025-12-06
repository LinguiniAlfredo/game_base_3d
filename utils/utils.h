template <typename T>
int index_of(const vector<T> &v, const T &value) {
    int index;
    auto iter = find(v.begin(), v.end(), value);

    if (iter != v.end()) {
        index = std::distance(v.begin(), iter);
    } else {
        index = -1;
    }

    return index;
}
