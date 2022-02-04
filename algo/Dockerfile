FROM ubuntu:18.04

RUN apt update
RUN apt upgrade -y
RUN apt install -y curl zsh git vim wget software-properties-common

RUN apt-get install -y direnv;
RUN sh -c "$(curl -fsSL https://raw.github.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
RUN echo 'eval "$(direnv hook zsh)"' >> ~/.zshrc
COPY zsh_history zsh_history

RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt install -y wget cmake g++-10 
RUN apt install -y build-essential
ENTRYPOINT usr/bin/zsh