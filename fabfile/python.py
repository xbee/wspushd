from fabric.api import *
import cuisine

def ensure():
    if not is_installed():
        puts("Installing python support...")
        install()
    else:
        puts("Python and tools already installed")

def is_installed():
    return cuisine.file_exists("/usr/local/bin/virtualenv")

def install():
    install_prerequisites()
    install_pip()
    install_virtualenv()

def install_prerequisites():
    cuisine.package_ensure("python-dev")

def install_pip():
    cuisine.package_ensure("python-pip")

def install_virtualenv():
    sudo("pip install virtualenv")