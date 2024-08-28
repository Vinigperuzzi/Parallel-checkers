#!/bin/bash

echo "Welcome to the bash.sh for Allan's and Vinícius's Checkers project Setup.
At the end of the script you will have installed in you operational system the pyenv (in case you do not already have), with all it's
depedencies and with python global version set to 3.11.0 and setting the aproprietated environment variables.
It will also set the venv for the project folder and install all requirements for the project to run.
And at the end, it will call a make file to compile the file.c to a Shared Lib and then you are able to run checkers.py.
In case you do not want to install pyenv cause you already have python and wish to change version to 3.11.0 manually, you
do not need to run this bash.sh, you can just run >pip install -r requirements.txt\n\n"

echo "You want to procced? (y/n)" 
read instChoice

if [ "$instChoice" = "y" ] || [ "$instChoice" = "Y" ]; then
    echo "Starting installation proccess..."

        # Install pyenv in case that it isn't
    if ! command -v pyenv &>/dev/null; then
        echo "You do not have pyenv installed in your machine."

        echo "Updatting system..."
        sudo apt update -y
        sudo apt upgrade -y

        #Intall the pyenv dependencies
        echo "Dependencies"
        sudo apt install git curl build-essential dkms perl wget -y
        sudo apt install gcc make default-libmysqlclient-dev libssl-dev -y
        sudo apt install -y zlib1g-dev libbz2-dev libreadline-dev libsqlite3-dev llvm \
        libncurses5-dev libncursesw5-dev \
        xz-utils tk-dev libffi-dev liblzma-dev python3-openssl git

        #effectivelly install pyenv
        curl -L https://github.com/pyenv/pyenv-installer/raw/master/bin/pyenv-installer | bash

        # setting the environment variables to use pyenv in bash
        echo "Setting env var"
        export PYENV_ROOT="$HOME/.pyenv"
        export PATH="$PYENV_ROOT/bin:$PATH"
        eval "$(pyenv init -)"
    
    else
        echo "You already have pyenv installed"
    fi

    # Install the 3.11.0 python version
    python_version="3.11.0"
    if ! pyenv versions | grep -q "$python_version"; then
        echo "Instalando o Python $python_version..."
        pyenv install "$python_version"
        pyenv global "$python_version"
        echo "Install and set python version"
    else
        echo "You already have the right python version set as default"
    fi

    #Try to update pip
    pip install --upgrade pip

    venv_name="Checkers"
    # Verify if venv already exists and create otherwise
    if [ ! -d "$venv_name" ]; then
        echo "Creating env"
        python -m venv "$venv_name"
    fi

    # Activate venv
    source "$venv_name"/bin/activate

    # Installing projects dependencies
    if [ "$(basename "$VIRTUAL_ENV")" = "$venv_name" ]; then
    echo "Installing the project's dependencies..."
    pip install -r requirements.txt
    else
        echo "The venv set as active is not the "$venv_name", if for some reason this bash is not able to set it as active, run >"$venv_name"/bin/activate"
    fi


    #Run project's compiler
    make

    echo "All right! You can now run the checkers.py with the command >python checkers.py
        In case you want to set the env back to global, run: >deactivate."

else
    echo "Operation cancelled."
fi