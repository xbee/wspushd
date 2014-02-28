
from __future__ import with_statement
import os
import sys
from fabric.api import *
from fabric.contrib.console import confirm
from fabric.contrib.project import rsync_project
from fabric.contrib import files
import cuisine

POST_RECEIVE = '''#!/bin/sh
GIT_WORK_TREE=/home/michael/prod/lgw git checkout -f
'''

def ensure():
    if not is_installed():
        puts("Installing git support...")
        install_git()
        create_deploy()
        enable_deploy_by_git()
    else:
        puts("Deploy by git push ok")

def is_installed():
    return cuisine.file_exists("/home/michael/prod/rebar.config")

def install_git():
    cuisine.package_ensure(["git", 'git-core', 'gitweb'])

def install_rabbitmq():
	cuisine.package_ensure(['rabbitmq-server', 'rabbitmq-stomp'])

def create_deploy():
	'''
	Create a deploy repos on remote server
	'''
	cuisine.dir_ensure("deploy/lgw.git")
	with cd("deploy/lgw.git"):
		run("git init --bare")
		cuisine.file_ensure("hooks/post-receive")
		if not files.contains("hooks/post-receive", POST_RECEIVE):
			files.append("hooks/post-receive", POST_RECEIVE)
		run('chmod +x hooks/post-receive')


def enable_deploy_by_git():
	'''
	Enable to deploy to remote by git 
	'''
	local('git remote add deploy ssh://115.28.171.71/home/michael/deploy/lgw.git')
	local('git push deploy +master:refs/heads/master')

def push_deploy():
	local('git push deploy')
