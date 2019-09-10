#include "pybind11/pybind11.h"
#include "pybind11/stl.h"


#include <vector>
#include <string>
#include <numeric>
#include <functional>

namespace py = pybind11;


int add(int i, int j) {
	return i + j;
}


std::vector<int> make_ints(const size_t size)
{
	return std::vector<int>(size);
}


float sum(const std::vector<float>& values)
{
	return std::accumulate(values.begin(), values.end(), 0.f, std::plus<float>());
}



struct thing
{
	enum class elem_dtype { dt_void, dt_int, dt_float, dt_double, dt_char };
	elem_dtype dtype;
	void* data;
};


// clang-format: off
template<thing::elem_dtype dt> struct dtype_to_type{ typedef void elem_type; };
template<> struct dtype_to_type<thing::elem_dtype::dt_int> { typedef int elem_type; };
template<> struct dtype_to_type<thing::elem_dtype::dt_float> { typedef float elem_type; };
template<> struct dtype_to_type<thing::elem_dtype::dt_double> { typedef double elem_type; };
template<> struct dtype_to_type<thing::elem_dtype::dt_char> { typedef char elem_type; };

template<typename T> struct type_to_dtype { static const thing::elem_dtype dtype=thing::elem_dtype::dt_void; };
template<> struct type_to_dtype<int> { static const thing::elem_dtype dtype = thing::elem_dtype::dt_int; };
template<> struct type_to_dtype<float> { static const thing::elem_dtype dtype = thing::elem_dtype::dt_float; };
template<> struct type_to_dtype<double> { static const thing::elem_dtype dtype = thing::elem_dtype::dt_double; };
template<> struct type_to_dtype<char> { static const thing::elem_dtype dtype = thing::elem_dtype::dt_char; };


// clang-format: on

class data_manager
{


public:
	data_manager()
	{
		things["vector_int"] = thing{ thing::elem_dtype::dt_int, (void*)new std::vector<int>(10) };
		things["vector_float"] = thing{ thing::elem_dtype::dt_float, (void*)new std::vector<float>(10) };
		things["vector_double"] = thing{ thing::elem_dtype::dt_double, (void*)new std::vector<double>(10) };

	}

	~data_manager()
	{
	}

	template<typename T>
	std::vector<T>& get_data(const std::string& name)
	{
		auto item = things[name];

		typedef std::vector<T> ptr_type;
		return *reinterpret_cast<ptr_type*>(item.data);
		
	}

	std::vector<std::string> get_names() const
	{
		std::vector<std::string> names;
		for(auto &thing_entry : things)
		{
			names.push_back(thing_entry.first);
		}
		return names;
	}
	
private:
	std::map<std::string, thing> things;
};


std::vector<int> get_vector_int(data_manager& dm, const std::string& name)
{
	return dm.get_data<int>(name);
}

std::vector<float> get_vector_float(data_manager& dm, const std::string& name)
{
	return dm.get_data<float>(name);
}

std::vector<double> get_vector_double(data_manager& dm, const std::string& name)
{
	return dm.get_data<double>(name);
}





PYBIND11_MODULE(mylib, m) {
	m.doc() = "pybind11 example plugin"; // optional module docstring

	m.def("add", &add, "A function which adds two numbers");
	m.def("make_ints", &make_ints, "A function ");
	m.def("sum", &sum, "Sums the elements in a vector");

	
	py::class_<data_manager>(m, "DataManager")
		.def(py::init())
		.def("get_names", &data_manager::get_names);

	m.def("get_vector_int", &get_vector_int);
	m.def("get_vector_float", &get_vector_float);
	m.def("get_vector_double", &get_vector_double);
}


