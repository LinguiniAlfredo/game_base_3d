#include <string>
using namespace std;

template <typename T>
int index_of(const vector<T> &v, const T &value)
{
    int index;
    auto iter = find(v.begin(), v.end(), value);

    if (iter != v.end()) {
        index = std::distance(v.begin(), iter);
    } else {
        index = -1;
    }

    return index;
}

void print_vec3(string s, vec3 v)
{
    printf(s.c_str());
    printf("%f, %f, %f\n", v.x, v.y, v.z);
}

vec3 major_direction(vec3 v)
{
    vec3 abs_v = abs(v);
    if (abs_v.x > abs_v.y && abs_v.x > abs_v.z) {
        return normalize(vec3(v.x, 0.f, 0.f));
    } else if (abs_v.y > abs_v.x && abs_v.y > abs_v.z) {
        return normalize(vec3(0.f, v.y, 0.f));
    } else if (abs_v.z > abs_v.x && abs_v.z > abs_v.y) {
        return normalize(vec3(0.f, 0.f, v.z));
    }
    return v;
}

bool largest_component(vec3 v, float value)
{
    value = abs(value);
    v     = abs(v);
    return value >= v.x && value >= v.y && value >= v.z;
}
