# Kickstart Definition for Client Chroot for i386

# we are going to install into a chroot, such as /opt/ltps/i386
install

# rev #2 will be configurable (i.e. http or ftp or cdrom/dvd or nfs, etc, etc)
repo --name=released-9 --mirrorlist=http://mirrors.fedoraproject.org/mirrorlist?repo=fedora-9&arch=i386
repo --name=updates-9 --mirrorlist=http://mirrors.fedoraproject.org/mirrorlist?repo=updates-released-f9&arch=i386
repo --name=temporary-9 --baseurl=http://togami.com/~k12linux-temporary/fedora/9/i386/

%include ../common.ks
# TODO: This is a temporary hack, it should be a dep from initscripts instead.
event-compat-sysv
%end
