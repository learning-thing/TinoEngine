#ifndef TVEC
#define TVEC

#include <math.h>
#include <iostream>


inline double random_double_d() {
    return rand() / (RAND_MAX+1.0);
}

inline double random_double_d(double min, double max) {
    return min+ (max-min)*random_double_d();
}

class vec3 {
    public:
    double e[3];
    vec3() {};
    vec3(float e0, float e1, float e2) {e[0]=e0; e[1]=e1; e[2]=e2;};//convert initializer arguments into vec
    vec3(float e0) {e[0]=e0; e[1]=e0; e[2]=e0;};//convert initializer arguments into vec

    static vec3 random() {
        return vec3(random_double_d(), random_double_d(), random_double_d());
    }

    static vec3 random(double min, double max) {
        return vec3(random_double_d(min, max), random_double_d(min, max), random_double_d(min, max));
    }

    inline float x() const {return e[0]; }
    inline float y() const {return e[1]; }
    inline float z() const {return e[2]; }

    inline float r() const {return e[0]; }
    inline float g() const {return e[1]; }
    inline float b() const {return e[2]; }

    inline const vec3& operator+() const {return * this; }
    inline vec3 operator-() const {return vec3(-e[0], -e[1],-e[2]); }
    inline float operator[] (int i) const {return e[i];}
    inline double& operator[](int i) {return e[i];};

    inline vec3& operator+=(const vec3 &v2);
    inline vec3& operator-=(const vec3 &v2);
    inline vec3& operator*=(const vec3 &v2);
    inline vec3& operator/=(const vec3 &v2);
    inline vec3& operator*=(const float t);
    inline vec3& operator/=(const float t);

    inline double length() const { return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]); }
    inline double squared_length() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }

    bool near_zero() const {
        auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1])<s) && (fabs(e[2])<s);
    }

    inline void make_unit_vector();
};


inline std::istream& operator>>(std::istream &is, vec3 &t) {
    is >> t.e[0] >> t.e[1] >> t.e[2];
    return is;
}

inline std::ostream& operator<<(std::ostream &os, const vec3 &t) {
    os << t.e[0] << " " << t.e[1] << " " << t.e[2];
    return os;
}

inline void vec3::make_unit_vector() {
    float k= 1.0/sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
    e[0]*=k; e[1]*=k; e[2]*=k;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {//Vector+Vector
    return vec3(v1.e[0]+v2.e[0], v1.e[1]+v2.e[1], v1.e[2]+v2.e[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {//Vector-Vector
    return vec3(v1.e[0]-v2.e[0], v1.e[1]-v2.e[1], v1.e[2]-v2.e[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {//Vector * Vector
    return vec3(v1.e[0]*v2.e[0], v1.e[1]*v2.e[1], v1.e[2]*v2.e[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {//Vector * Vector
    return vec3(v1.e[0]/v2.e[0], v1.e[1]/v2.e[1], v1.e[2]/v2.e[2]);
}

inline vec3 operator*(float t, const vec3 &v1) {//float * Vector
    return vec3(v1.e[0]*t, v1.e[1]*t, v1.e[2]*t);
}

inline vec3 operator/(const vec3 &v1, float t) {
    return vec3(v1.e[0]/t, v1.e[1]/t, v1.e[2]/t);
}

inline vec3 operator*(const vec3 &v1, float t) {//Vector * Vector
    return vec3(v1.e[0]*t, v1.e[1]*t, v1.e[2]*t);
}

inline float dot(const vec3 &v1, const vec3 v2) {
    return v1.e[0]*v2.e[0] + v1.e[1]*v2.e[1] +v1.e[2]*v2.e[2];
}

inline float dot(const vec3 &v1) {
    return v1.e[0]*v1.e[0] + v1.e[1]*v1.e[1] +v1.e[2]*v1.e[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
return vec3( (v1.e[1]*v2.e[2] - v1.e[2]*v2.e[1]), 
             (-(v1.e[0]*v2.e[2] - v1.e[2]*v2.e[0])),
             (v1.e[0]*v2.e[1] - v1.e[1]*v2.e[0]));
}

inline vec3& vec3::operator+=(const vec3 &v) {
    e[0]+=v[0];
    e[1]+=v[1];
    e[2]+=v[2];
    return *this;
}
inline vec3& vec3::operator*=(const vec3 &v) {
    e[0]*=v[0];
    e[1]*=v[1];
    e[2]*=v[2];
    return *this;
}
inline vec3& vec3::operator/=(const vec3 &v) {
    e[0]/=v[0];
    e[1]/=v[1];
    e[2]/=v[2];
    return *this;
}
inline vec3& vec3::operator-=(const vec3 &v) {
    e[0]-=v[0];
    e[1]-=v[1];
    e[2]-=v[2];
    return *this;
}
inline vec3& vec3::operator*=(const float t) {
    e[0]*=t;
    e[1]*=t;
    e[2]*=t;
    return *this;
}
inline vec3& vec3::operator/=(const float t) {
    float k = 1.0/t;
    e[0]*=k;
    e[1]*=k;
    e[2]*=k;
    return *this;
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

inline vec3 random_in_unit_sphere() {
    while (true) {
        vec3 p = vec3::random(-1,1);
        //std::cout << p.x() << " " << p.y() << " " << p.z() << "\n";
        if (p.squared_length()<1) {
            return p;
        }
    }
}

inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline vec3 random_on_hemisphere( const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0) return on_unit_sphere;
    else return -on_unit_sphere;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv,n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv+cos_theta*n);
    vec3 r_out_parralel = -sqrt(fabs(1.0 - r_out_perp.squared_length())) * n;
    return r_out_perp + r_out_parralel;
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double_d(-1,1), random_double_d(-1,1), 0);
        if (p.squared_length() < 1) return p;
    }
}

#endif
