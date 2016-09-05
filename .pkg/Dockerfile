FROM centos:7

WORKDIR /root

RUN yum -y upgrade 
COPY harden.sh /root
RUN bash /root/harden.sh && rm /root/harden.sh
RUN yum -y clean all

ADD minit/minit /sbin/minit
VOLUME /app/python-3
