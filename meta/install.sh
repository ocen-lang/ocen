#!/bin/bash

# This is meant to be a one-liner to clone, build and install `ocen` on your system.

BASE=${BASE:=$HOME/}
CLONE_DIR=${CLONE_DIR:=$BASE/ocen}

function show_env_vars() {
    echo
    echo "export OCEN_ROOT=$CLONE_DIR"
    echo "export PATH=\$OCEN_ROOT/bin:\$PATH"
    echo
}

function clone_ocen() {
    # Clone the repository and build ocen if it doesn't exist
    if [ ! -d $CLONE_DIR ]; then
        echo "[+] Cloning ocen-lang/ocen to $CLONE_DIR"
        git config --global core.autocrlf false
        git clone https://github.com/ocen-lang/ocen $CLONE_DIR
        echo "[+] Building ocen"
        cd $CLONE_DIR
        ./meta/bootstrap.sh
    else
        echo "[-] Directory $CLONE_DIR already exists. Please remove it or specify BASE or CLONE_DIR."
        exit 1
    fi
}

function update_shell() {
    # Find correct .rc file depending on the shell
    if [ -f $HOME/.bashrc ]; then
        RC_FILE=$HOME/.bashrc
    elif [ -f $HOME/.zshrc ]; then
        RC_FILE=$HOME/.zshrc
    else
        echo "-----------------------------------------------------------"
        echo "[-] Unknown shell. Add these to your shell config file:"
        show_env_vars
        return 1
    fi

    # Check if RC_FILE already contains OCEN_ROOT
    if grep -q "OCEN_ROOT" $RC_FILE; then
        echo "[+] Shell config already contains OCEN_ROOT. Trying compiler..."

        # Check if `ocen --help` works, but ignore errors
        OUTPUT=$(source $RC_FILE && ocen --help 2>&1)
        # If exit code is 0, ocen is installed correctly
        if [ $? -eq 0 ]; then
            echo "[+] ocen is already installed and working."
            return 0
        else
            echo "-----------------------------------------------------------"
            echo $OUTPUT
            echo "-----------------------------------------------------------"
            echo "[-] ocen is not installed correctly. Please check your shell config file for:"
            show_env_vars
            return 1
        fi

    else

        read -p "[+] Add OCEN_ROOT to $RC_FILE? [Y/n] " -n 1 -r
        echo    # (optional) move to a new line
        if [[ $REPLY =~ ^[Nn]$ ]]
        then
            echo "-----------------------------------------------------------"
            echo "[-] Okay. Add these to your shell config file:"
            show_env_vars
            return 0
        fi

        echo "export OCEN_ROOT=$CLONE_DIR" >> $RC_FILE
        echo "export PATH=\$OCEN_ROOT/bin:\$PATH" >> $RC_FILE
    fi
}

function install_vscode_extension() {
    # If VSCode is not installed, skip this step
    if ! command -v code &> /dev/null; then
        echo "[-] VSCode is not installed. Skipping extension installation."
        return 0
    fi

    # Check if `mustafaquraish.ocen` extension is already installed
    if code --list-extensions | grep -q "mustafaquraish.ocen"; then
        echo "[+] VSCode Ocen extension is already installed."
        return 0
    fi

    read -p "[+] Install VSCode Ocen extension? [Y/n] " -n 1 -r
    echo    # (optional) move to a new line
    if [[ $REPLY =~ ^[Nn]$ ]]
    then
        echo "[-] Okay. You can install the extension manually from the VSCode marketplace."
        return 0
    fi

    code --install-extension mustafaquraish.ocen
}

clone_ocen
update_shell
install_vscode_extension