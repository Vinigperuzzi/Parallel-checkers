# IPPD (Introdução ao processamento paralelo e distribuído)

This repository has been created to contain the project of the IPPD class in the course of computing science in UFPel.
This project is proposed by professor Gerson Cavalheiro and developed by the students Allan N. Schuch andf Vinícius G Peruzzi

#How to run the project

All you need is python in a version that has pip, or at least have pip installed aside.
For more compatibility we recommend set the python version for 3.11.0, although python is backwards compatible.

Assuming you already have this setup, and previous contact with the language:
You have to run :
>pip install -r requirements.txt

Maybe you want to create a venv before, but that's entirelly up to you

After you have done with the dependencies, you can compile the shared lib with the MAKEFILE included in project's folder:
>make

And then run the Chekers.py:
>python Checkers.py

# ------------------------------------------------------------

If you do not have python installed and do not have de knowledge to setup the environment, we have a bash.sh file that do it for you
All you have to do is two simple commands:

>chmod +x SetupProjectEnv.sh
>source SetupProjectenv.sh

The first one is to set permission to run as program and the second is to run it, just like ./bash.sh, but wit source you force the changes to occur in the open terminal.

# What this bash.sh does?
This bash will install a lot of stuff
The .sh itself will explain what it does, but simply:
1 - Check if pyenv is installed. If not, update/upgrade apt, install dependencies for pyenv and install pyenv;
2 - Check if python version is 3.11.0 (even if you previously has pyenv installed), if not, install and set 3.11.0 version as global for system;
3 - Try to update pip;
4 - Check if already exists a venv with the name Checkers, otherwise create it and activate it;
5 - Install the dependencies with pip in the Checkers environment;
6 - Run makefile to compile the shared lib .so.

Now the setup is ready for the command >python Checkers.py

ATENTION: The command deactivate set venv back to global;
    If you need to remove the .so lib, you can run >make clean.
    If you want to go back to a especified python version you can run >pyenv global [$version]