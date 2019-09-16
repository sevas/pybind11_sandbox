import pytest
import mylib
import numpy as np

@pytest.fixture
def ds():
    yield mylib.DataStore()


def test_get_data_by_type(ds):
    assert mylib.get_vector_int(ds, "vector_int") == [0] * 10
    assert mylib.get_vector_float(ds, "vector_float") == [0.0] * 10

    vertices = mylib.get_vector_vertex(ds, "vector_vertex")
    assert vertices.shape == (10, 3)
    expected_values = [0.0, 0.0, 12.0] * 10  
    assert np.all(vertices.flatten() == expected_values )



def test_type_mismatch_raises_keyerror(ds):
    with pytest.raises(KeyError):
        mylib.get_vector_int(ds, "vector_float")
