import numpy as np
import mylib

print(mylib.add(1, 2))

a = mylib.make_ints(10)
print(a)


v_np = np.ones(shape=(200,), dtype=np.float32)
print("sum(numpy): {}".format(mylib.sum(v_np)))

v_l = [1.0, 1.0, 1.0, 1.0]
print("sum(list): {}".format(mylib.sum(v_l)))


# dm = mylib.DataManager()

# print(dm.get_names())

# print(mylib.get_vector_int(dm, "vector_int"))