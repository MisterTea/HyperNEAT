#!/bin/bash

# Script to do daily tasks for Fuego.
#
# This script should be run as a nightly cron job by one of the Fuego admins.
# Since SourceForge discourages auto-updaters, it should *not* be run on the
# SourceForge shell server, but on a private machine.
# To run it, check out a version of fuego to a dedicated directory, for
# example ~/fuego_daily (don't use this checked out version for other tasks).
# Then register ~/fuego_daily/fuego/tools/cronjob/fuego_daily.sh as a
# cron job to run once a day.
# You need to have set up password-less ssh to fuego.sourceforge.net
# (see http://alexandria.wiki.sourceforge.net/SSH+Key+Generation)

FUEGO_DIR=$(dirname $0)/../..
FUEGO_WEB_SERVER=games.cs.ualberta.ca
FUEGO_WEB_DIR=/usr/brazeau/misc/games/go/fuego
SVN_BACKUP_DIR=../fuego_svn_backup

# Update the checked out sources.

cd $FUEGO_DIR
svn update || exit 1

# Generate documentation and put it on the UofA web server. Don't use
# fuego.sourceforge.net, because it allows only 100 MB webspace

cd doc/doxygen
make || exit 1
rsync -r fuego-doc $FUEGO_WEB_SERVER:$FUEGO_WEB_DIR

# Backup subversion repository

cd ..
rsync -av fuego.svn.sourceforge.net::svn/fuego/* $SVN_BACKUP_DIR
