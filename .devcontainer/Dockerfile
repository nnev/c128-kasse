FROM mcr.microsoft.com/vscode/devcontainers/base:bookworm

RUN sed -i '0,/Components: main/{s/^Components: main/Components: main contrib non-free/}' /etc/apt/sources.list.d/debian.sources

RUN apt-get update && apt-get -y install --no-install-recommends vice cc65 emacs vim && apt-get -y install xpra

ADD setup-vice.sh /setup-vice.sh

USER vscode
RUN bash -x /setup-vice.sh
