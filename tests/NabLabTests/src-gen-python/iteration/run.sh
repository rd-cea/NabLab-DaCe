# DO NOT EDIT THIS FILE - it is machine generated
#
#!/bin/sh
#
#
# Excute iteration.py with the installed python3.
# Numpy and plyvel modules must be installed.
# To reproduce results, use runvenv.sh.
#
export N_WS_PATH=$HOME/workspaces/NabLab/tests
export PYTHONPATH=$N_WS_PATH/.nablab/mesh/cartesianmesh2dnumpy:$N_WS_PATH/.nablab/linearalgebra/linearalgebranumpy
python3 iteration.py $*