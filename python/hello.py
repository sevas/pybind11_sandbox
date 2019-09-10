import numpy as np
import mylib

print(mylib.add(1, 2))

a = mylib.make_ints(10)
print(a)


v_np = np.ones(shape=(200,), dtype=np.float32)
print("sum(numpy): {}".format(mylib.sum(v_np)))
print("sum_eigen(numpy): {}".format(mylib.sum_eigen(v_np)))


v_l = [1.0, 1.0, 1.0, 1.0]
print("sum(list): {}".format(mylib.sum(v_l)))
print("sum_eigen(list): {}".format(mylib.sum_eigen(v_l)))

a = np.array([
    [120, 0,   320],
    [  0, 120, 240],
    [  0, 0, 1],
])

points = np.array([
    [0, 0, 0], [0, 1, 0], [0, 0, 1]
])

res = mylib.mv_mul(a, points)

print(res)

mylib.array_add_scalar_stl(res.flatten(), 10)
print(res)

print(hex(res.ctypes.data))
mylib.array_add_scalar_eigen(res, 10)
print(res)




dm = mylib.DataManager()

print(dm.get_names())

print(mylib.get_vector_int(dm, "vector_int"))