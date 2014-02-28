#!/usr/bin/python
# -*- coding: utf-8 -*-

from __future__ import with_statement
from fabric.api import *
from fabric.contrib.console import confirm
from fabric.contrib.project import rsync_project
from fabric.contrib import files
# from fabric.api import env, run
import string
from random import choice
import socket
import paramiko


remote_home = '/home/michael'

# def run_rsync(source, target, excludes=[]):
#     excludes = "".join(map(lambda e: " --exclude="+e, excludes))
#     local("expect -c 'spawn rsync -avz"+excludes+" --force -e \"ssh -oStrictHostKeyChecking=no -p "+env.port+"\" "+source+" "+target+"; expect password; send \""+env.password+"\\n\"; interact'")

# fab task:'hello'
# fab task:something='hello'
# fab task:foo=99,bar=True
def deploy_src(loc):
    rsync_project(local_dir=loc, 
        remote_dir='/home/michael', 
        exclude=['.git', "ebin", "*.pyc", 'keys'])

def add_key():
    '''
    Enable ssh login without password
    '''
    # check local ~/.ssh/id_rsa.pub exists
    lfn = "keys/id_rsa"
    pubfn = "keys/id_rsa.pub"
    if not os.path.exists(lfn):
        # need keys/id_rsa do not exists
        local("ssh-keygen -f keys/id_rsa -t rsa -N ''")
    # copy local .ssh/id_rsa.pub content 
    fc = open(pubfn).read()
    append(remote_home + "/.ssh/authorized_keys", fc)

def create_repos():
    if not files.exists('workspace/lgw'):
        run('mkdir -p workspace/lgw')
    with cd('workspace/lgw'):
        if not files.exists('.git'):
            run('git init')


def test():
    run('uname -an')

def host_type():
    run('uname -s')

def upload():
	put('wanda.txt', 'wandax.txt')

def rmfile():
	run('rm wanda.txt')

def install_build_tools():
    """
    Install the tools for smart packages on ubuntu
    """
    sudo('apt-get -y install build-essential checkinstall', pty=True)

def install_fabric():
    """
    Install fabric on ubuntu
    """
    sudo('pip install fabric')

def autoremove():
    """
    Autoremove packages 
    """
    sudo('apt-get -y autoremove', pty=True)

# @task
# @parallel
# def passwd(user, passwd=False):
#     with settings(hide('running', 'stdout', 'stderr'), warn_only=True):
#         if isup(env.host):
#             if not passwd:
#                 passwd = genpass()
#             sudo("echo -e '%s\n%s' | passwd %s" % (passwd, passwd, user))

# def genpass(length=10):
#     return ''.join(choice(string.ascii_letters + string.digits) for _ in range(length))

# def isup(host):
#     print 'connecting host: %s' % host
#     timeout = socket.getdefaulttimeout()
#     socket.setdefaulttimeout(1)
#     up = True
#     try:
#         paramiko.Transport((host, 22))
#     except Exception, e:
#         up = False
#         print '%s down, %s' % (host, e)
#     finally:
#         socket.setdefaulttimeout(timeout)
#         return up

# def test():
#     with settings(warn_only=True):
#         result = local('./manage.py test my_app', capture=True)
#     if result.failed and not confirm("Tests failed. Continue anyway?"):
#         abort("Aborting at user request.")

def commit():
    local("git add -p && git commit")

def push():
    local("git push origin master")

def pull():
    local("git pull origin master")

def clone(user, host, repos):
    local("git clone ssh://%s@%s/%s.git" % (user, host, repos))

def prepare_deploy():
    test()
    commit()
    push()

def deploy():
    code_dir = '/srv/django/myproject'
    with settings(warn_only=True):
        if run("test -d %s" % code_dir).failed:
            run("git clone user@vcshost:/path/to/repo/.git %s" % code_dir)
    with cd(code_dir):
        run("git pull")
        run("touch app.wsgi")


