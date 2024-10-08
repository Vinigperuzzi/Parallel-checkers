# IPPD (Introdução ao processamento paralelo e distribuído)

This repository has been created to contain the project of the IPPD class in the course of computing science in UFPel. <br>
This project is proposed by professor Gerson Cavalheiro and developed by the students Allan N. Schuch and Vinícius G Peruzzi.

It is a game of checkers parallel with openmp and distributed with MPI, enjoy.

# How to run the project

### By yourself:

All you need is python in a version that has pip, or at least have pip installed aside.<br>
For more compatibility we recommend set the python version for 3.11.0, although python is backwards compatible.

Assuming you already have this setup, and previous contact with the language:<br>
You have to run :
>pip install -r requirements.txt

Maybe you want to create a venv before, but that's entirelly up to you...<br>

After you have done with the dependencies, you can compile the shared lib with the MAKEFILE included in project's folder:
>make

And then run the Chekers.py:
>python Checkers.py

# ------------------------------------------------------------

### Automatically:
If you do not have python installed and do not have de knowledge to setup the environment, we have a bash.sh file that do it for you. <br>

All you have to do is two simple commands:

>chmod +x SetupProjectEnv.sh<br>
>source SetupProjectenv.sh

The first one is to set permission to run as program and the second is to run it, just like ./bash.sh, but with source you force the changes to occur in the open terminal.<br>

##### What this bash.sh does?
This bash will install a lot of stuff.<br>
The .sh itself will explain what it does, but simply:<br>
1 - Check if pyenv is installed. If not, update/upgrade apt, install dependencies for pyenv and install pyenv;<br>
2 - Check if python version is 3.11.0 (even if you previously has pyenv installed), if not, install and set 3.11.0 version as global for system;<br>
3 - Try to update pip;<br>
4 - Check if already exists a venv with the name Checkers, otherwise create it and activate it;<br>
5 - Install the dependencies with pip in the Checkers environment;<br>
6 - Run makefile to compile the shared lib .so.<br>

Now the setup is ready for the command >python Checkers.py<br><br>

ATENTION: The command deactivate set venv back to global;<br>
    If you need to remove the .so lib, you can run >make clean.<br>
    If you want to go back to a specified python version you can run >pyenv global [$version]


# ------------------------------------------------------------

### With docker

There's a Dockerfile to be build with docker compose, so you can run it using it.
Note that it uses an Graphics interface, so it will not be a simple comando to run the container in the right way. But, there's is two bashes that run the project using docker. You can simple run these two commands:

>chmod +x bin/run<br>
>bin/run

To stop you can do the same for bin/stop bash

Note that if you are using docker in a version older than 2.0, the syntax will not work with docker compose, so you may alter it to docker-compose, or simple copy/paste from bin/stop directly to you terminal.