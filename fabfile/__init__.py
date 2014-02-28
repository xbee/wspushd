from fabric.api import *
from cuisine import *
from . import python
from . import deploy

# env.use_ssh_config = True
env.user = 'michael'
# env.password = 'rtmap2013'
env.hosts = ['115.28.171.71', ]
env.key_filename = 'keys/id_rsa.pub'
env.roledefs = {
    'test': ['115.28.171.71']
}

remote_home = '/home/michael'

@roles("test")
def configure_host():
    python.ensure()

@roles("test")
def deploy_host():
    deploy.ensure()

@roles("test")
def push():
	deploy.push_deploy()

def setup():
	package_ensure("cuisine")


