#include <iostream>

#include <chrono>
#include <functional>
#include <numeric>
#include <string>
#include <vector>

#include "Eigen/Core"

#include "pybind11/eigen.h"
#include "pybind11/numpy.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace py = pybind11;

int add(int i, int j) { return i + j; }

std::vector<int> make_ints(const size_t size) { return std::vector<int>(size); }

float sum(const std::vector<float>& values) { return std::accumulate(values.begin(), values.end(), 0.f, std::plus<float>()); }

float sum_eigen(Eigen::MatrixXf& m)
{
    std::cout << m.rows() << "  " << m.cols() << std::endl;
    return m.sum();
}

static std::map<std::string, uint64_t> timings;

Eigen::MatrixX3f mv_mul(const Eigen::Matrix3f& M, const Eigen::MatrixX3f& points) { return M * points; }

void array_add_scalar_stl(std::vector<float>& in, const float val)
{
    auto before = std::chrono::high_resolution_clock::now();
    for (auto i = 0u; i < in.size(); ++i)
    {
        in[i] += val;
    }
    auto after = std::chrono::high_resolution_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before).count();
    timings["array_add_scalar_stl"] = elapsed;
}

void array_add_scalar_eigen(Eigen::MatrixXf& in, const float val)
{
    std::cout << "ptr: " << &in << std::endl;

    Eigen::MatrixXf values(in.rows(), in.cols());
    values.setConstant(val);
    in += values;
}

struct thing
{
    enum class elem_dtype
    {
        dt_void,
        dt_int,
        dt_float,
        dt_double,
        dt_char,
        dt_vertex,
    };
    elem_dtype dtype;
    void* data;
    void (*deleter)(const void*);
};

namespace depthsense
{

struct vertex
{
    float x, y, z;
    vertex() : x(0.f), y(0.f), z(12.f){};
};

} // namespace depthsense

// clang-format off
template<thing::elem_dtype dt> struct dtype_to_type{ typedef void elem_type;  };
template<> struct dtype_to_type<thing::elem_dtype::dt_int> { typedef int elem_type; };
template<> struct dtype_to_type<thing::elem_dtype::dt_float> { typedef float elem_type; };
template<> struct dtype_to_type<thing::elem_dtype::dt_double> { typedef double elem_type; };
template<> struct dtype_to_type<thing::elem_dtype::dt_char> { typedef char elem_type;  };

template<> struct dtype_to_type<thing::elem_dtype::dt_vertex> { typedef depthsense::vertex elem_type;  };


template<typename T> struct type_to_dtype {     static const thing::elem_dtype dtype=thing::elem_dtype::dt_void;   static constexpr char* name = "void";};
template<> struct type_to_dtype<int>    { static const thing::elem_dtype dtype = thing::elem_dtype::dt_int;     static constexpr char* name = "int";};
template<> struct type_to_dtype<float>  { static const thing::elem_dtype dtype = thing::elem_dtype::dt_float;   static constexpr char* name = "float";};
template<> struct type_to_dtype<double> { static const thing::elem_dtype dtype = thing::elem_dtype::dt_double;  static constexpr char* name = "double";};
template<> struct type_to_dtype<char>   { static const thing::elem_dtype dtype = thing::elem_dtype::dt_char;    static constexpr char* name = "char";};

template<> struct type_to_dtype<depthsense::vertex> { static const thing::elem_dtype dtype = thing::elem_dtype::dt_vertex;   static constexpr char* name = "vertex";};

// clang-format on

class data_store
{

  public:
    data_store()
    {
        add("vector_int", std::vector<int>(10));
        add("vector_float", std::vector<float>(10));
        add("vector_double", std::vector<double>(10));
        add("vector_vertex", std::vector<depthsense::vertex>(10));
    }

    virtual ~data_store() { clear(); }

    template <typename T> void add(const std::string& name, const std::vector<T>& data)
    {
        auto* copied = new std::vector<T>(data);
        auto deleter = [](const void* obj_ptr) {
            using std::vector;

            void* x = const_cast<void*>(obj_ptr);
            reinterpret_cast<std::vector<T>*>(x)->~vector<T>();
        };

        things[name] = thing{type_to_dtype<T>::dtype, reinterpret_cast<void*>(copied), deleter};
    }

    template <typename T> void remove(const std::string& name)
    {
        if (things.find(name) == things.end())
        {
            throw py::key_error(std::string("no data named: ") + name);
        }

        auto item = things[name];
        typedef std::vector<T> ptr_type;
        delete reinterpret_cast<ptr_type*>(item.data);
    }

    void clear()
    {
        const auto names = get_names();
        for (auto& name : names)
        {
            auto& item = things[name];
            try
            {
                item.deleter(item.data);
                things.erase(things.find(name));
            }
            catch (const std::exception&e)
            {
                std::cerr << "could not delete item " << name << ". Reason: " << e.what() << std::endl;
            }
        }

    }

    template <typename T> std::vector<T>& get_data(const std::string& name)
    {
        if (things.find(name) == things.end())
        {
            throw py::key_error(std::string("no data named: ") + name);
        }

        auto item = things[name];

        if (type_to_dtype<T>::dtype == item.dtype)
        {
            typedef std::vector<T> ptr_type;
            return *reinterpret_cast<ptr_type*>(item.data);
        }

        throw py::key_error(std::string("wrong type '") + type_to_dtype<T>::name + "' for data named " + name);
    }

    std::vector<std::string> get_names() const
    {
        std::vector<std::string> names;
        for (auto& thing_entry : things)
        {
            names.push_back(thing_entry.first);
        }
        return names;
    }

  private:
    std::map<std::string, thing> things;
};

// specializations needed for the bindings
template <typename T> struct ds_vector_getter
{
    static std::vector<T>& get(data_store& ds, const std::string& name) { return ds.get_data<T>(name); }
};

#define MYLIB_MAKE_SPECIALIZATION(Type, TypeSuffix) typedef ds_vector_getter<Type> get_vector##TypeSuffix;

MYLIB_MAKE_SPECIALIZATION(int, i)
MYLIB_MAKE_SPECIALIZATION(char, c)
MYLIB_MAKE_SPECIALIZATION(float, f)
MYLIB_MAKE_SPECIALIZATION(double, d)

// MYLIB_MAKE_SPECIALIZATION(depthsense::vertex, vertex)
template <> struct ds_vector_getter<depthsense::vertex>
{
    static py::array_t<float> get(data_store& ds, const std::string& name)
    {
        py::array_t<float> out;
        const auto& data = ds.get_data<depthsense::vertex>(name);
        out.resize(py::array_t<double>::ShapeContainer{10, 3});
        for (auto i = 0u; i < data.size(); ++i)
        {
            out.mutable_at(i, 0) = data[i].x;
            out.mutable_at(i, 1) = data[i].y;
            out.mutable_at(i, 2) = data[i].z;
        }

        return out;
    }
};
typedef ds_vector_getter<depthsense::vertex> get_vectorvertex;

uint64_t get_timing(const std::string& name)
{
    if (timings.find(name) != timings.end())
    {
        return timings[name];
    }
    else
    {
        return std::numeric_limits<uint64_t>::max();
    }
}

PYBIND11_MODULE(mylib, m)
{

    m.doc() = "pybind11 example plugin"; // optional module docstring

    m.def("add", &add, "A function which adds two numbers");
    m.def("make_ints", &make_ints, "A function ");
    m.def("sum", &sum, "Sums the elements in a vector");
    m.def("sum_eigen", &sum_eigen, "Sums the elements in an arbitrary matrix");

    m.def("array_add_scalar_stl", &array_add_scalar_stl, "Sums the elements in a vector");
    m.def("array_add_scalar_eigen", &array_add_scalar_eigen, "Sums the elements in an arbitrary matrix");

    m.def("mv_mul", &mv_mul, "Mat x Vec product");
    m.def("get_timing", &get_timing, "get time for key");

    // clang-format off
    py::class_<data_store>(m, "DataStore")
	    .def(py::init())
	    .def("get_names", &data_store::get_names);
    // clang-format on

    m.def("get_vector_char", &get_vectorc::get);
    m.def("get_vector_int", &get_vectori::get);
    m.def("get_vector_float", &get_vectorf::get);
    m.def("get_vector_double", &get_vectord::get);
    m.def("get_vector_vertex", &get_vectorvertex::get);
}
