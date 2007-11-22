#!/bin/sh

NAME=$(basename $(pwd))
VERSION="5.1" # FIXME needs proper automation
RELEASE=$NAME-$VERSION
BUILDPATH="/tmp/$RELEASE"

usage() {
    echo "usage:\n
    $(basename $0) (this help screen)
    $(basename $0) --release
      Create a versioned tarball
    $(basename $0) --force
      Create a snapshot tarball
    $(basename $0) --tag (check; tag)
      bzr tag NAME-VERSION
    $(basename $0) --clean (clean)
      Clean up tarballs\n"
}

clean() {
        echo "INFO: cleaning workdir in $BUILDPATH"
        if [ -d $BUILDPATH ]; then
            rm -rf $BUILDPATH/*
            rmdir $BUILDPATH
        fi
        echo "INFO: removing bzipped source $BUILDPATH.tar.bz2"
        rm -rf $BUILDPATH.tar.bz2
        echo "INFO: removing gzipped source $BUILDPATH.tar.gz"
        rm -rf $BUILDPATH.tar.gz
    }

check() {
        if [ -n "`bzr st`" ]; then \
                echo "ERROR: Uncommitted changes, please commit first !"; \
                bzr st; \
                exit 1; \
        fi
    }

tag() {
    bzr tag $RELEASE
}

autogen() {
    if [ -x $BUILDPATH/autogen.sh ]; then
        cd $BUILDPATH
        ./autogen.sh
        cd -
    fi
}

export_tree() {
    echo "INFO: exporting tree to workdir $BUILDPATH"
    bzr export $BUILDPATH
}

mktgz() {
    cd /tmp
    tar czvf $BUILDPATH.tar.gz $RELEASE
    cd -
    echo "INFO: created gzipped source tarball in $BUILDPATH.tar.gz"
}

mktbz() {
    cd /tmp
    tar cjvf $BUILDPATH.tar.bz2 $RELEASE
    cd -
    echo "INFO: created bzipped source tarball in $BUILDPATH.tar.bz2"
}

mktemp -d $BUILDPATH

case $1 in
    --release)
        clean
        check
        tag
        export_tree
        autogen
        mktgz
        mktbz
        ;;
     --force)
        clean
        check
        export_tree
        autogen
        mktgz
        mktbz
        ;;
     --tag)
        check
        tag
        ;;
     --clean)
        clean
        ;;
     *)
        usage
        ;;
     esac

rm -rf $BUILDPATH
