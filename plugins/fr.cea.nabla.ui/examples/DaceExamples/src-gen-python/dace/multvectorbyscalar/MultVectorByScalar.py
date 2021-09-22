import numpy as np
import dace as dp

from dace.sdfg import SDFG
from dace.memlet import Memlet
from dace.sdfg.validation import validate, validate_sdfg

# Importing these outside creates an import loop
from dace.codegen import codegen, compiler
from dace.codegen.compiled_sdfg import CompiledSDFG
import dace.config

array1 = dp.ndarray([5], dp.float64)
array1 = [0.0] * 5
array2 = dp.ndarray([5], dp.float64)
array2 = [1.0] * 5

mysdfg = SDFG('MultVectorByScalar')


AddJob = mysdfg.add_state("AddJob", is_start_state=True)

AddJob_tasklet = AddJob.add_tasklet('AddJob', {'array2'}, {'array1'}, 'array1=array2*3.0')

AddJob_array2 = mysdfg.add_array('AddJob_array2', [5], dp.float64)
AddJob_array1 = mysdfg.add_array('AddJob_array1', [5], dp.float64)

map_entry, map_exit = AddJob.add_map('AddJob_map', dict(i0='0:5'))
AddJob.add_memlet_path(AddJob.add_read('AddJob_array2'),map_entry, AddJob_tasklet, dst_conn='array2',memlet=dace.Memlet('AddJob_array2[i0]'))
AddJob.add_memlet_path(AddJob_tasklet, map_exit, AddJob.add_write('AddJob_array1'), src_conn='array1',memlet=dace.Memlet('AddJob_array1[i0]'))



mysdfg(AddJob_array2=array2,AddJob_array1=array1)


mysdfg.view('MultVectorByScalar.html')