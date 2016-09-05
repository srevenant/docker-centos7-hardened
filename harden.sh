#!/bin/bash

cat >> /etc/security/limits.conf <<END
## CFG0110: DISABLE core dumps
* soft core 0
* hard core 0
END

cat <<END > /etc/bashrc.local
umask 022

## include SYSV ''opt'' directories
for d in \`echo {/,/usr/}opt/*/*bin\`; do
    if [ -d \$d ]; then
        PATH=\${PATH}:\$d
    fi
done
for d in \`echo {/,/usr/}opt/*/man\`; do
    if [ -d \$d ]; then
        MANPATH=\${MANPATH}:\$d
    fi
done

## and our 'app' directories, only if symlink
for d in \`echo /app/*\`; do
    if [ -L \$d ]; then
        if [ -d \$d/bin ]; then
            PATH=\${PATH}:\$d/bin
        fi
        if [ -d \$d/sbin ]; then
            PATH=\${PATH}:\$d/sbin
        fi
        if [ -d \$d/man ]; then
            MANPATH=\${MANPATH}:\$d/man
        fi
    fi
done

export PATH MANPATH
if [ "\$PS1" ]; then
    case "\$TERM" in
        linux*|xterm*)
            # root gets red
            if [ "\`/usr/bin/id -u\`" -eq 0 ]; then
                c=31
            # user not eq logname is yeller
            elif [ "\`/usr/bin/id -u -n\`" != "\`logname\`" ]; then
                c=33
            # then blue
            else
                c=34
            fi
            ;;
        *)
            c=34
            ;;
    esac
    PS1="\\[\\e[1m\\]\\@|\\[\\e[\${c}m\\]\\u@\\h\\[\\e[0m\\]:\\w\\\\\$ "

    HISTSIZE=1000  # 1000 lines of history
    alias ll='ls -lFba'
    alias m=more
fi
END

chmod 0644 /etc/bashrc.local
cat <<END >> /etc/skel/.bashrc
if [ -f /etc/bashrc.local ]; then
    . /etc/bashrc.local
fi
END

cat <<END >> /root/.bashrc
if [ -f /etc/bashrc.local ]; then
    . /etc/bashrc.local
fi
END
unalias cp 2>/dev/null

for f in `find /home -name .bashrc`; do
	cp -f /etc/skel/.bashrc $f
done

# remove root alias stupidity
sed -i -e "/^alias /d" /root/.bashrc

###########
#for service in \
#    apmd atd autofs avahi-daemon avahi-dnsconfd bluetooth cpuspeed \
#    cups cups-config-daemon firstboot gpm hidd hplip ip6tables iscsi \
#    iscsid isdn kudzu lm_sensors mcstrans microcode_ctl nfs \
#    pcmcia pcscd rawdevices readahead_early readahead_later rhnsd \
#    rpcgssd rpcidmapd rpcsvcgssd smartd xfs xinetd yum-updatesd
#do
#    chkconfig $service --list 2>/dev/null |grep :on > /dev/null
#    if [ "$?" -eq 0 ]; then
#        echo "Disabling service $service"
#        echo -n "    "
#        chkconfig $service off && chkconfig $service --list
#    fi
#done
##############
