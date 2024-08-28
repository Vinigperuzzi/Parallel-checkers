import os
from pathlib import Path

def obtain_proj_path():
    path_script_parent = os.path.abspath(Path(__file__).resolve().parent)
    path_project = os.path.dirname(path_script_parent)
    return path_project