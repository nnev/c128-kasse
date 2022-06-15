FROM debian:bookworm

RUN sed -i 's,main$,main contrib non-free,g' /etc/apt/sources.list

RUN echo 'deb http://deb.debian.org/debian-debug/ bookworm-debug main contrib non-free' >> /etc/apt/sources.list

RUN apt-get update && apt-get install -y cc65 vice vice-dbgsym wget build-essential git gdb strace procps psmisc

RUN groupadd --gid 1000 kasse \
    && useradd --uid 1000 --gid 1000 -m kasse

ADD . /usr/src/kasse/

USER kasse
WORKDIR /home/kasse

RUN wget -q https://downloads.sourceforge.net/project/vice-emu/releases/vice-3.6.1.tar.gz
RUN mkdir -p ~/.local/share/vice
RUN tar xf vice-3.6.1.tar.gz -C ~/.local/share/vice/ vice-3.6.1/data --strip-components=2

# TODO: why does x128 use 1600% CPU? (only uses 200% outside docker)
